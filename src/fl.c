/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * fl.c
 * 	This allows you to modify the file system.  It uses libzip.
 */
#include "header/jl_pr.h"

#define PKFMAX 10000000

strt gvar_pkfl;
char *errf;

void jl_fl_save(jl_t* pusr, void *file, char *name, uint32_t bytes) {
	jvct_t * pjlc = pusr->pjlc;
	
	int errsv;
	ssize_t n_bytes;
	int fd;

	jl_io_tag(pusr, JL_IO_TAG_SIMPLE - JL_IO_TAG_MAX);
	jl_io_print_lowc(pusr, "SAVING....");
	jl_io_tag(pusr, JL_IO_TAG_INTENSE - JL_IO_TAG_MAX);
	jl_io_print_lows(pusr, jl_me_strt_fnum(bytes));

	fd = open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	truncate(name, 0);
	if(fd <= 0) {
		errsv = errno;

		jl_io_tag(pusr, JL_IO_TAG_MINIMAL - JL_IO_TAG_MAX);
		jl_io_print_lows(pusr,
			jl_me_strt_merg(
				jl_me_strt_merg(
					Strt("jl_fl_save: Failed to open file: "),
					Strt(name),STRT_TEMP),
				jl_me_strt_merg(
					Strt(" Write failed: "),
					Strt(strerror(errsv)),STRT_TEMP),
				STRT_TEMP
			)
		);
		exit(-1);
	}
	int at = lseek(fd, 0, SEEK_END);
	jl_io_tag(pusr, JL_IO_TAG_SIMPLE - JL_IO_TAG_MAX);
	jl_io_print_lowc(pusr, "Writing....");
	n_bytes = write(fd, file, bytes);
	if(n_bytes <= 0) {
		errsv = errno;
		close(fd);
		jlvm_dies(pjlc,
			jl_me_strt_merg(Strt("Write failed: "),Strt(strerror(errsv)),STRT_TEMP));
	}
	printf("Wrote %d bytes @%d!\n", (int)n_bytes, at);
/*	jl_io_print_lows(0,
		jl_me_strt_merg(
			jl_me_strt_merg(Strt("Wrote "),
				jl_me_strt_fnum((int)n_bytes), STRT_TEMP),
			jl_me_strt_merg(Strt(" bytes @ "),
				jl_me_strt_fnum(at), STRT_TEMP), STRT_TEMP
		)
	);*/
	jl_io_print_lowc(pusr, "Wrote....");
	close(fd);
	return;
}

void _jl_fl_errf(jvct_t * pjlc, char *msg) {
	jl_io_tag(pjlc->sg.usrd, JL_IO_TAG_SIMPLE - JL_IO_TAG_MAX);
	jl_io_print_lowc(pjlc->sg.usrd, "[JLVM] saving to errf: ");
	jl_io_print_lowc(pjlc->sg.usrd, errf);
	jl_io_print_lowc(pjlc->sg.usrd, msg);
	jl_fl_save(pjlc->sg.usrd, msg,errf,strlen(msg));
}

static inline void _jl_fl_reset_cursor(char *file_name) {
	int fd = open(file_name, O_RDWR);
	lseek(fd, 0, SEEK_SET);
	close(fd);
}

strt jl_fl_load(jl_t* pusr, char *file_name) {
	_jl_fl_reset_cursor(file_name);
	u32t i;
	unsigned char *file = malloc(MAXFILELEN);
	int fd = open(file_name, O_RDWR);
	if(fd <= 0) {
		printf("failed to open %s\n", file_name);
		jlvm_dies(pusr->pjlc, Strt("file_file_load: Failed to open file"));
	}
	int Read = read(fd, file, MAXFILELEN + 1);
	pusr->info = Read;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/FILE/LOAD] read %d bytes\n", pusr->info);
	#endif
	close(fd);
	strt frtn = jl_me_strt_make(pusr->info, STRT_KEEP);
	for( i = 0; i < pusr->info; i++) {
		frtn->data[i] = file[i];
	}
	frtn->data = file;
	return frtn;
}

char jl_fl_pk_save(jl_t* pusr, char *packageFileName, char *fileName,
	void *data, uint64_t dataSize)
{
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/PKDJ/SAVE] opening \"%s\"....\n", packageFileName);
	#endif
	struct zip *archive = zip_open(packageFileName, ZIP_CREATE 
		/*| ZIP_CHECKCONS*/, NULL);
	if(archive == NULL) {
		return 1;
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("[JLVM/FILE/PKDJ/SAVE] opened file system:\"%s\".\n",
			packageFileName);
		#endif
	}

	struct zip_source *s;
	if ((s=zip_source_buffer(archive, (void *)data, dataSize, 0)) == NULL) {
		zip_source_free(s);
		jlvm_dies(pusr->pjlc, jl_me_strt_merg(
			Strt("[JLVM/FILE/PKDJ/SAVE] src null error[replace]: "),
			Strt(zip_strerror(archive)), STRT_TEMP
		));
	}
//	printf("%d,%d,%d\n",archive,sb.index,s);
	if(zip_file_add(archive, fileName, s, ZIP_FL_OVERWRITE)) {
		printf("[JLVM/FILE/PKDJ/SAVE] add/err: %s\n", zip_strerror(archive));
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("[JLVM/FILE/PKDJ/SAVE] added %s to file sys.\n", fileName);
		#endif
	}
	zip_close(archive);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/PKDJ/SAVE] DONE!\n");
	#endif
	return 0;
}

uint8_t *jl_fl_pk_load(jl_t* pusr, char *packageFileName, char *filename)
{
	jl_io_offset(pusr, "FILE");
	jl_io_offset(pusr, "LOAD");
	int zerror;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lows(pusr, jl_me_strt_merg(
		jl_me_strt_merg(
			Strt("loading package:\""), Strt(packageFileName),
			STRT_TEMP),
		Strt("\"..."), STRT_TEMP));
	#endif
	struct zip *zipfile = zip_open(packageFileName, ZIP_CHECKCONS, &zerror);
	if(zerror == ZIP_ER_OPEN) {
		jl_io_print_lowc(pusr, " NO EXIST!");
		pusr->errf = JL_ERR_FIND;
		return NULL;
	}
	if(zipfile == NULL) {
		jlvm_dies(pusr->pjlc, Strt("couldn't load pckg!"));
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, (char *)zip_strerror(zipfile));
	jl_io_print_lowc(pusr, "loaded package.\n");
	#endif
	unsigned char *fileToLoad = malloc(PKFMAX);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "opening file in package....\n");
	#endif
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "call pass.");
	#endif
	if(file == NULL) {
		jl_io_print_lows(pusr,
			jl_me_strt_merg(
			jl_me_strt_merg(Strt("couldn't open up file: \""),
				Strt(filename), STRT_TEMP),
			jl_me_strt_merg(Strt("\" in "),
				Strt(packageFileName), STRT_TEMP),STRT_TEMP));
		jl_io_print_lows(pusr,
			jl_me_strt_merg(Strt("because: "),
				Strt(zip_strerror(zipfile)), STRT_TEMP));
		pusr->errf = JL_ERR_NONE;
		return NULL;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "opened file in package\n");
	jl_io_print_lowc(pusr, "reading opened file....\n");
	#endif
	if((pusr->info = zip_fread(file, fileToLoad, PKFMAX)) == -1) {
		jlvm_dies(pusr->pjlc, Strt("file reading failed"));
	}
	char * readbytes = malloc(30);
	sprintf(readbytes, "read %d bytes\n", pusr->info);
	jl_io_print_lowc(pusr, readbytes);
	free(readbytes);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "read opened file.\n");
	#endif
	zip_close(zipfile);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "done.\n");
	#endif
	pusr->errf = JL_ERR_NERR;
	jl_io_offset(pusr, "JLVM");
	return fileToLoad;
}

/**
 * Create a folder (directory)
 * @param pusr: library context
 * @param pfilebase: name of directory to create
*/
void jl_fl_mkdir(jl_t* pusr, strt pfilebase) {
	if(mkdir((void *)pfilebase->data, 0)) {
		int errsv = errno;
		if(errsv == EEXIST) {
			#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
			jl_io_print_lowc(pusr, "Directory Exist! Continue...\n");
			#endif
		}else{
			jlvm_dies(pusr->pjlc,
				jl_me_strt_merg(
					Strt("Directory: Uh oh..."),
					Strt(strerror(errsv)), STRT_TEMP
				)
			);
		}
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pusr, "Created Directory!\n");
		#endif
	}
}

/**
 * Create a file & load it.
 * @param pusr: library context
 * @param pfile: name of file to create
 * @param contents: the contents to put in the file
 * @param size: the size (in bytes) of the contents.
 * @return x: the data contents of the file.
*/
uint8_t * jl_fl_mkfile(jl_t* pusr, char *pfilebase, char *contents, uint32_t size){
	uint8_t *freturn;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "Creating File...\n");
	#endif
	jl_fl_save(pusr, contents, (void *)gvar_pkfl->data, size);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "Attempt Complete!\n");
	#endif
	SDL_Delay(1000); //give file system time to update
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "Try loading....\n");
	#endif
	if(
		((freturn =
			jl_fl_pk_load(pusr,(void *)gvar_pkfl->data,pfilebase))
		== NULL) &&
		(pusr->errf == JL_ERR_FIND) )//Package still doesn't exist!!
	{
		jlvm_dies(pusr->pjlc, Strt("Failed To Create file"));
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "Good loading!\n");
	#endif
	return freturn;
}

//
uint8_t *jl_fl_pk_mnld(jl_t* pusr, char *Fname) {
	uint8_t *freturn;
	if(
		((freturn = jl_fl_pk_load(pusr,(void *)gvar_pkfl->data,Fname)) == NULL) &&
		pusr->errf == JL_ERR_FIND ) //Package doesn't exist!! - create
	{
		jl_fl_mkfile(pusr, Fname, jal5_head_jlvm(), jal5_head_size());
	}
	return freturn;
}

/*
 * Return the location of the resource pack for program with name
 * "pprg_name"
*/
strt jl_fl_get_resloc(jl_t* pusr, strt pprg_name, strt pfilename) {
	int i;
	
	#if PLATFORM == 1 //PHONE
	char *filebase = (void*)jl_me_strt_merg(
		Strt(JLVM_FILEBASE),
		jl_me_strt_merg(pprg_name, Strt("/"), STRT_TEMP),
		STRT_KEEP
	)->data;
	errf = jl_me_strt_merg(Strt(JLVM_FILEBASE), Strt("errf.txt"), STRT_KEEP)
		->data;
	#elif PLATFORM == 0 //COMPUTER
	char *filebase = SDL_GetPrefPath("JLVM",(char *)pprg_name->data);
	if(filebase == NULL) {
		filebase = (void*) jl_me_strt_merg(
			Strt("JLVM/"),pprg_name,STRT_KEEP)->data;
	}
	errf = malloc(strlen(filebase)+3);
	for(i = 0; i < strlen(filebase)-5; i++) {
		errf[i] = filebase[i];
	}
	errf[strlen(filebase)-5] = 'e';
	errf[strlen(filebase)-4] = 'r';
	errf[strlen(filebase)-3] = 'r';
	errf[strlen(filebase)-2] = 'f';
	errf[strlen(filebase)-1] = '.';
	errf[strlen(filebase)+0] = 't';
	errf[strlen(filebase)+1] = 'x';
	errf[strlen(filebase)+2] = 't';
	errf[strlen(filebase)+3] = '\0';
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("errf:%s\n",errf);
		#endif
	#else //OTHER
	#endif
	
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_offset(pusr, "FLBS");
	jl_io_print_lowc(pusr, filebase);
	#endif
	
	//filebase maker
	#if PLATFORM == 1
	jl_fl_mkdir(pusr, Strt(JLVM_FILEBASE));
	#else
	int fsize = strlen(filebase)+strlen(JLVM_FILEBASE);
	char *filebase2 = malloc(fsize);
	for(i = 0; i < fsize; i++) {
		filebase2[i] = 0;
	}
	strcat(filebase2,filebase);
	strcat(filebase2,JLVM_FILEBASE);
	jl_fl_mkdir(pusr, Strt(filebase2));
	#endif
	jl_fl_mkdir(pusr, Strt(filebase));

	strt pvar_pkfl = jl_me_strt_merg(Strt(filebase), pfilename, STRT_KEEP);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "filebase: ");
	jl_io_print_lowc(pusr, filebase);
	jl_io_print_lows(pusr, pvar_pkfl);

	jl_io_offset(pusr, "INIT");
	jl_io_offset(pusr, "PKFL");

	jl_io_print_lows(pusr, pvar_pkfl);
	#endif
	return pvar_pkfl;
}

static void _jl_fl_user_select_open_dir(jl_t* pusr, char *dirname) {
	DIR *dir;
	struct dirent *ent;
	jvct_t * pjlc = pusr->pjlc;

	if(dirname[strlen(dirname)-1] == '/' &&
		dirname[strlen(dirname)-2] == '/')
	{
		dirname[strlen(dirname)-1] = '\0';
	}
	pjlc->fl.dirname = dirname;
	pjlc->fl.cursor = 0;
	pjlc->fl.cpage = 0;
	cl_list_clear(pjlc->fl.filelist);
	if ((dir = opendir (dirname)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			char *element = malloc(strlen(ent->d_name) + 1);
			memcpy(element, ent->d_name, strlen(ent->d_name));
			element[strlen(ent->d_name)] = '\0';
			cl_list_add(pjlc->fl.filelist, element);
		}
		closedir(dir);
		jl_cl_list_alphabetize(pjlc->fl.filelist);
	} else {
		//Couldn't open Directory
		int errsv = errno;
		if(errsv == ENOTDIR) { //Not a directory - is a file
			pjlc->fl.dirname[strlen(dirname)-1] = '\0';
			pusr->loop = JL_SG_WM_EXIT; //Go into exit loop
		}
	}
}

void jl_fl_user_select_init(jl_t* pusr, char *program_name) {
	pusr->loop = JL_SG_WM_DN;
	_jl_fl_user_select_open_dir(pusr, SDL_GetPrefPath("JLVM",program_name));
}

static void _jl_fl_user_select_up(jl_t* pusr, float x, float y) {
	if((int)y == 1) {
		jvct_t * pjlc = pusr->pjlc;
		if((pjlc->fl.cursor > 0) || pjlc->fl.cpage) pjlc->fl.cursor--;
	}
}

static void _jl_fl_user_select_dn(jl_t* pusr, float x, float y) {
	if((int)y == 1) {
		jvct_t * pjlc = pusr->pjlc;
		if(pjlc->fl.cursor + (pjlc->fl.cpage * 21) <
			cl_list_count(pjlc->fl.filelist) - 1)
		{
			pjlc->fl.cursor++;
		}
	}
}

static void _jl_fl_user_select_rt(jl_t* pusr, float x, float y) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_dn(pusr, x, y);
	}
}

static void _jl_fl_user_select_lt(jl_t* pusr, float x, float y) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_up(pusr, x, y);
	}
}

static void _jl_fl_user_select_do(jl_t* pusr, float x, float y) {
	if((int)y == 1) {
		jvct_t * pjlc = pusr->pjlc;
		struct cl_list_iterator *iterator;
		int i;
		char *stringtoprint;

		iterator = cl_list_iterator_create(pjlc->fl.filelist);
		for(i = 0; i < cl_list_count(pjlc->fl.filelist); i++) {
			stringtoprint = cl_list_iterator_next(iterator);
			if(i == pjlc->fl.cursor) {
				pjlc->fl.selecteditem = stringtoprint;
				cl_list_iterator_destroy(iterator);
				break;
			}
		}
		if(strcmp(pjlc->fl.selecteditem, "..") == 0) {
			for(i = strlen(pjlc->fl.dirname)-2; i > 0; i--) {
				if(pjlc->fl.dirname[i] == '/') break;
				else pjlc->fl.dirname[i] = '\0';
			}
			_jl_fl_user_select_open_dir(pusr,pjlc->fl.dirname);
		}else if(strcmp(pjlc->fl.selecteditem, ".") == 0) {
			pusr->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else{
			char *newdir = malloc(
				strlen(pjlc->fl.dirname) +
				strlen(pjlc->fl.selecteditem) + 2);
			memcpy(newdir, pjlc->fl.dirname,
				strlen(pjlc->fl.dirname));
			memcpy(newdir + strlen(pjlc->fl.dirname),
				pjlc->fl.selecteditem,
				strlen(pjlc->fl.selecteditem));
			newdir[strlen(pjlc->fl.dirname) +
				strlen(pjlc->fl.selecteditem)] = '/';
			newdir[strlen(pjlc->fl.dirname) +
				strlen(pjlc->fl.selecteditem) + 1] = '\0';
			free(pjlc->fl.dirname);
			_jl_fl_user_select_open_dir(pusr,newdir);
		}
	}
}

void jl_fl_user_select_loop(jl_t* pusr) {
	jvct_t * pjlc = pusr->pjlc;
	struct cl_list_iterator *iterator;
	int i;
	char *stringtoprint;

	iterator = cl_list_iterator_create(pjlc->fl.filelist);

	jl_gr_draw_image(pusr, 0, 1, 0., 0., 1., 1., 1, 127);
	jl_gr_draw_text(pusr, "Select File", .02, .02, .04,255);

	jl_ct_run_event(pusr,JL_CT_MAINUP, _jl_fl_user_select_up, jl_ct_dont);
	jl_ct_run_event(pusr,JL_CT_MAINDN, _jl_fl_user_select_dn, jl_ct_dont);
	jl_ct_run_event(pusr,JL_CT_MAINRT, _jl_fl_user_select_rt, jl_ct_dont);
	jl_ct_run_event(pusr,JL_CT_MAINLT, _jl_fl_user_select_lt, jl_ct_dont);
	//Draw up to 20
	for(i = 0; i < cl_list_count(pjlc->fl.filelist); i++) {
		stringtoprint = cl_list_iterator_next(iterator);
		if(strcmp(stringtoprint, "..") == 0) {
			stringtoprint = "//containing folder//";
		}else if(strcmp(stringtoprint, ".") == 0) {
			stringtoprint = "//this folder//";
		}
		if(i - (pjlc->fl.cpage * 21) >= 0)
			jl_gr_draw_text(pusr, stringtoprint,
				.06, .08 + (.04 * (i - (pjlc->fl.cpage * 21))),
				.04,255);
		if(i - (pjlc->fl.cpage * 21) > 19) {
			break;
		 	cl_list_iterator_destroy(iterator);
	 	}
	}
	if(pjlc->fl.cursor > 20) {
		pjlc->fl.cursor = 0;
		pjlc->fl.cpage++;
	}
	if(pjlc->fl.cursor < 0) {
		pjlc->fl.cursor = 20;
		pjlc->fl.cpage--;
	}
	jl_gr_draw_text(pusr, ">", .02, .08 + (.04 * pjlc->fl.cursor), .04,255);
	jl_gr_draw_text(pusr, pjlc->fl.dirname, .02, .94, .02, 255);
	jl_ct_run_event(pusr, JL_CT_SELECT, _jl_fl_user_select_do, jl_ct_dont);
}

char *jl_fl_user_select_get(jl_t* pusr) {
	jvct_t * pjlc = pusr->pjlc;
	return pjlc->fl.dirname;
}

void _jl_fl_kill(jvct_t * pjlc) {
	cl_list_destroy(pjlc->fl.filelist);
}

void _jl_fl_init(jvct_t * pjlc) {

	jl_io_offset(pjlc->sg.usrd, "FILE");
	jl_io_offset(pjlc->sg.usrd, "INIT");

	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "program name:");
	#endif
	lsdl_prog_name(Strt("JLVM"));

	gvar_pkfl = jl_fl_get_resloc(pjlc->sg.usrd,
		Strt("JLVM"), Strt("jlvm.zip"));
	remove((void*)gvar_pkfl->data);

	_jl_fl_errf(pjlc, "Segmentation Fault / Floatation Exception etc.");
	jl_io_offset(pjlc->sg.usrd, "JLVM");
	
	//Create the variables
	pjlc->fl.filelist = cl_list_create();
}
