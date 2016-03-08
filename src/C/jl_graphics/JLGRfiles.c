#include "jl_pr.h"
#include "JLGRinternal.h"

char* _jl_fl_full_fname(jl_gr_t* jl_gr,char *selecteddir,char *selecteditem);
str_t jl_fl_convert__(jl_t* jlc, str_t filename);

static void _jl_fl_user_select_check_extradir(char *dirname) {
	if(dirname[strlen(dirname)-1] == '/' && strlen(dirname) > 1) {
		if(dirname[strlen(dirname)-2] == '/')
			dirname[strlen(dirname)-1] = '\0';
	}
}

// Return 1 on success
// Return 0 if directory not available
static uint8_t _jl_fl_user_select_open_dir(jl_gr_t* jl_gr, char *dirname) {
	DIR *dir;
	struct dirent *ent;
	jvct_t * _jlc = jl_gr->jlc->_jlc;
	str_t converted_filename;

	_jl_fl_user_select_check_extradir(dirname);
	if(dirname[1] == '\0') {
		jl_me_alloc(jl_gr->jlc, (void**)&dirname, 0, 0);
		dirname = SDL_GetPrefPath("JL_Lib", "\0");
		_jl_fl_user_select_check_extradir(dirname);
	}
	_jlc->fl.dirname = dirname;
	_jlc->fl.cursor = 0;
	_jlc->fl.cpage = 0;
	converted_filename = jl_fl_convert__(jl_gr->jlc, _jlc->fl.dirname);
	cl_list_clear(_jlc->fl.filelist);
//UnComment to test file system conversion code.
	printf("dirname=%s:%s\n", _jlc->fl.dirname, converted_filename);
	if ((dir = opendir (converted_filename)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			char *element = malloc(strlen(ent->d_name) + 1);
			memcpy(element, ent->d_name, strlen(ent->d_name));
			element[strlen(ent->d_name)] = '\0';
			cl_list_add(_jlc->fl.filelist, element);
		}
		closedir(dir);
		jl_cl_list_alphabetize(_jlc->fl.filelist);
	} else {
		//Couldn't open Directory
		int errsv = errno;
		if(errsv == ENOTDIR) { //Not a directory - is a file
			_jlc->fl.returnit = 1;
			_jlc->fl.dirname[strlen(dirname)-1] = '\0';
			_jlc->fl.inloop = 0;
			jl_gr->jlc->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else if(errsv == ENOENT) { // Directory Doesn't Exist
			return 0;
		}else if(errsv == EACCES) { // Doesn't have permission
			return 0;
		}else if((errsv == EMFILE) || (errsv == ENFILE) ||
			(errsv == ENOMEM)) //Not enough memory!
		{
			jl_io_print(jl_gr->jlc, "FileViewer Can't Open Directory:"
				"Not Enough Memory!");
			jl_sg_kill(jl_gr->jlc);
		}else{ //Unknown error
			jl_io_print(jl_gr->jlc, "FileViewer Can't Open Directory:"
				"Unknown Error!");
			jl_sg_kill(jl_gr->jlc);
		}
	}
	return 1;
}

/**
 * Open directory for file viewer.
 * If '!' is put at the beginning of "program_name", then it's treated as a
 *	relative path instead of a program name.
 * @param jlc: The library context
 * @param program_name: program name or '!'+relative path
 * @param newfiledata: any new files created with the fileviewer will
 *	automatically be saved with this data.
 * @param newfilesize: size of "newfiledata"
 * @returns 0: if can't open the directory. ( Doesn't exist, Bad permissions )
 * @returns 1: on success.
**/
uint8_t jl_fl_user_select_init(jl_gr_t* jl_gr, const char *program_name,
	void *newfiledata, uint64_t newfilesize)
{
	jvct_t * _jlc = jl_gr->jlc->_jlc;
	jl_gr->jlc->loop = JL_SG_WM_LOOP;
	_jlc->fl.returnit = 0;
	_jlc->fl.inloop = 1;
	_jlc->fl.newfiledata = newfiledata;
	_jlc->fl.newfilesize = newfilesize;
	_jlc->fl.prompt = 0;
	_jlc->fl.promptstring = NULL;
	if(program_name[0] == '!') {
		char *path = jl_me_copy(jl_gr->jlc,program_name,
			strlen(program_name));
		return _jl_fl_user_select_open_dir(jl_gr, path);
	}else{
		return _jl_fl_user_select_open_dir(jl_gr,
			SDL_GetPrefPath("JL_Lib",program_name));
	}
}

static void _jl_fl_user_select_up(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;

	if(jl_gr->jlc->ctrl.h == 1) {
		jvct_t * _jlc = jl_gr->jlc->_jlc;
		if((_jlc->fl.cursor > 0) || _jlc->fl.cpage) _jlc->fl.cursor--;
	}
}

static void _jl_fl_user_select_dn(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;

	if(jl_gr->jlc->ctrl.h == 1) {
		jvct_t * _jlc = jl_gr->jlc->_jlc;
		if(_jlc->fl.cursor + (_jlc->fl.cpage * (_jlc->fl.drawupto+1)) <
			cl_list_count(_jlc->fl.filelist) - 1)
		{
			_jlc->fl.cursor++;
		}
	}
}

static void _jl_fl_user_select_rt(jl_t* jl) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_dn(jl);
	}
}

static void _jl_fl_user_select_lt(jl_t* jl) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_up(jl);
	}
}

static void _jl_fl_open_file(jl_t*jlc, char *selecteditem) {
	jvct_t * _jlc = jlc->_jlc;
	char *newdir = _jl_fl_full_fname(jlc->jl_gr,
		_jlc->fl.dirname, selecteditem);

	free(_jlc->fl.dirname);
	_jlc->fl.dirname = NULL;
	_jl_fl_user_select_open_dir(jlc->jl_gr,newdir);
}

static void _jl_fl_user_select_do(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;

	if(jl_gr->jlc->ctrl.h == 1) {
		jvct_t * _jlc = jl_gr->jlc->_jlc;
		struct cl_list_iterator *iterator;
		int i;
		char *stringtoprint;

		iterator = cl_list_iterator_create(_jlc->fl.filelist);
		for(i = 0; i < cl_list_count(_jlc->fl.filelist); i++) {
			stringtoprint = cl_list_iterator_next(iterator);
			if(i ==
				_jlc->fl.cursor + //ON PAGE
				(_jlc->fl.cpage * (_jlc->fl.drawupto+1))) //PAGE
			{
				_jlc->fl.selecteditem = stringtoprint;
				cl_list_iterator_destroy(iterator);
				break;
			}
		}
		if(strcmp(_jlc->fl.selecteditem, "..") == 0) {
			for(i = strlen(_jlc->fl.dirname)-2; i > 0; i--) {
				if(_jlc->fl.dirname[i] == '/') break;
				else _jlc->fl.dirname[i] = '\0';
			}
			_jl_fl_user_select_open_dir(jl_gr,_jlc->fl.dirname);
		}else if(strcmp(_jlc->fl.selecteditem, ".") == 0) {
			_jlc->fl.inloop = 0;
			jl_gr->jlc->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else{
			_jl_fl_open_file(jl_gr->jlc, _jlc->fl.selecteditem);
		}
	}
}

/**
 * Run the file viewer.
 * @param jl_gr: The jl_gr library Context.
**/
void jl_fl_user_select_loop(jl_gr_t* jl_gr) {
	jvct_t * _jlc = jl_gr->jlc->_jlc;
	struct cl_list_iterator *iterator;
	int i;
	char *stringtoprint;

	_jlc->fl.drawupto = ((int)(20.f * jl_gl_ar(jl_gr))) - 1;

	iterator = cl_list_iterator_create(_jlc->fl.filelist);

	jl_gr_fill_image_set(jl_gr, 0, JL_IMGI_ICON, 1, 255);
	jl_gr_fill_image_draw(jl_gr);
	jl_gr_draw_text(jl_gr, "Select File", (jl_vec3_t) { .02, .02, 0. },
		jl_gr->jlc->font);

	jl_ct_run_event(jl_gr,JL_CT_MAINUP, _jl_fl_user_select_up, jl_dont);
	jl_ct_run_event(jl_gr,JL_CT_MAINDN, _jl_fl_user_select_dn, jl_dont);
	jl_ct_run_event(jl_gr,JL_CT_MAINRT, _jl_fl_user_select_rt, jl_dont);
	jl_ct_run_event(jl_gr,JL_CT_MAINLT, _jl_fl_user_select_lt, jl_dont);
	//Draw files
	for(i = 0; i < cl_list_count(_jlc->fl.filelist); i++) {
		stringtoprint = cl_list_iterator_next(iterator);
		if(strcmp(stringtoprint, "..") == 0) {
			stringtoprint = "//containing folder//";
		}else if(strcmp(stringtoprint, ".") == 0) {
			stringtoprint = "//this folder//";
		}
		if(i - (_jlc->fl.cpage * (_jlc->fl.drawupto+1)) >= 0)
			jl_gr_draw_text(jl_gr, stringtoprint, (jl_vec3_t) {
				.06,
				.08 + (jl_gr->jlc->font.size *
					(i - (_jlc->fl.cpage * (
					_jlc->fl.drawupto+1)))), 0. },
				jl_gr->jlc->font);
		if(i - (_jlc->fl.cpage * (_jlc->fl.drawupto+1)) >
			_jlc->fl.drawupto - 1)
		{
			break;
		 	cl_list_iterator_destroy(iterator);
	 	}
	}
	if(_jlc->fl.cursor > _jlc->fl.drawupto) {
		_jlc->fl.cursor = 0;
		_jlc->fl.cpage++;
	}
	if(_jlc->fl.cursor < 0) {
		_jlc->fl.cursor = _jlc->fl.drawupto;
		_jlc->fl.cpage--;
	}
	if(_jlc->fl.prompt) {
		if(jl_gr_draw_textbox(jl_gr, .02, jl_gl_ar(jl_gr) - .06, .94, .02,
			&_jlc->fl.promptstring))
		{
			char *name = _jl_fl_full_fname(jl_gr,
				_jlc->fl.dirname,
				(char*)_jlc->fl.promptstring->data);
			name[strlen(name) - 1] = '\0';
			jl_fl_save(jl_gr->jlc, _jlc->fl.newfiledata,
				name, _jlc->fl.newfilesize);
			_jl_fl_user_select_open_dir(jl_gr, _jlc->fl.dirname);
			_jlc->fl.prompt = 0;
		}
	}else{
		jl_gr_draw_text(jl_gr, ">", (jl_vec3_t) {
			.02, .08 + (.04 * _jlc->fl.cursor), 0. },
			jl_gr->jlc->font);
		jl_gr_draw_text(jl_gr, _jlc->fl.dirname,
			(jl_vec3_t) { .02, jl_gl_ar(jl_gr) - .02, 0. },
			(jl_font_t) { 0, JL_IMGI_ICON, 0,
				jl_gr->jlc->fontcolor, .02});
		jl_ct_run_event(jl_gr, JL_CT_SELECT, _jl_fl_user_select_do,
			jl_dont);
	}
	jl_gr_sp_rnl(jl_gr, _jlc->fl.btns[0]);
	jl_gr_sp_rnl(jl_gr, _jlc->fl.btns[1]);
}

/**
 * Get the results from the file viewer.
 * @param jlc: Library Context.
 * @returns: If done, name of selected file.  If not done, NULL is returned.
**/
char *jl_fl_user_select_get(jl_gr_t* jl_gr) {
	jvct_t * _jlc = jl_gr->jlc->_jlc;
	
	if(_jlc->fl.returnit)
		return _jlc->fl.dirname;
	else
		return NULL;
}

static void _jl_fl_btn_makefile_press(jl_t* jl) {
	jvct_t *_jlc = jl->_jlc;
	_jlc->fl.prompt = 1;
}

static void _jl_fl_btn_makefile_loop(jl_t* jlc, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jvct_t *_jlc = jl_gr->jlc->_jlc;

	//TODO: make graphic: 0, 1, 1, 255
	if(!_jlc->fl.newfiledata) return;
	jl_gr_draw_glow_button(jl_gr, _jlc->fl.btns[0], "+ New File",
		_jl_fl_btn_makefile_press);
}

static void _jl_fl_btn_makefile_draw(jl_t* jlc, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jl_rect_t rc = { 0., 0., jl_gl_ar(jl_gr), jl_gl_ar(jl_gr) };
	jl_vec3_t tr = { 0., 0., 0. };

	jl_gr_vos_image(jl_gr, jl_gr->gl.temp_vo, rc, 0, JL_IMGI_ICON, 9, 255);
	jl_gr_draw_vo(jl_gr, jl_gr->gl.temp_vo, &tr);
}

static void _jl_fl_btn_makefolder_loop(jl_t* jlc, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jvct_t *_jlc = jl_gr->jlc->_jlc;
	
	//TODO: make graphic: 0, 1, 2, 255,
	jl_gr_draw_glow_button(jl_gr, _jlc->fl.btns[1], "+ New Folder",
		_jl_fl_btn_makefile_press);
}

static void _jl_fl_btn_makefolder_draw(jl_t* jlc, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jl_rect_t rc = { 0., 0., jl_gl_ar(jl_gr), jl_gl_ar(jl_gr) };
	jl_vec3_t tr = { 0., 0., 0. };

	jl_gr_vos_image(jl_gr, jl_gr->gl.temp_vo, rc, 0, JL_IMGI_ICON, 10, 255);
	jl_gr_draw_vo(jl_gr, jl_gr->gl.temp_vo, &tr);
}

void jl_gr_fl_init(jl_gr_t* jl_gr) {
	jvct_t* _jlc = jl_gr->jlc->_jlc;
	jl_rect_t rc1 = { .8, 0., .1, .1 };
	jl_rect_t rc2 = { .9, 0., .1, .1 };

	//Create the variables
	_jlc->fl.filelist = cl_list_create();
	_jlc->fl.inloop = 0;
	_jlc->fl.btns[0] = jl_gr_sp_new(jl_gr, rc1,
		_jl_fl_btn_makefile_draw, _jl_fl_btn_makefile_loop, 0);
	_jlc->fl.btns[1] = jl_gr_sp_new(jl_gr, rc2,
		_jl_fl_btn_makefolder_draw, _jl_fl_btn_makefolder_loop, 0);
	_jlc->has.fileviewer = 1;
}
