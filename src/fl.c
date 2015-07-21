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

/** @cond **/
//Static Functions 
static void _jl_fl_save(jl_t* jlc, const void *file, const char *name,
	uint32_t bytes)
{
	jvct_t * _jlc = jlc->_jlc;
	int errsv;
	ssize_t n_bytes;
	int fd;
	char * bytecount;
	char * atplace;
	
	if(name == NULL) {
		jl_sg_kill(jlc, "Save: Name is Null");
		return;
	}else if(strlen(name) == 0) {
		jl_sg_kill(jlc, "Save: Name is Empty String");
		return;
	}

	jl_io_offset(jlc, JL_IO_SIMPLE, "FLSV"); // { : FLSV
	jl_io_printc(jlc, "SAVING....");
	jl_io_offset(jlc, JL_IO_INTENSE, "FLSV"); // =
	bytecount = jl_me_string_fnum(jlc, bytes);
	jl_io_printc(jlc, bytecount);
	free(bytecount);

	uint8_t offs = (name[0] == '!');
	fd = open(name + offs, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

	if(fd <= 0) {
		errsv = errno;

		jl_io_offset(jlc, JL_IO_MINIMAL, "FLSV"); // =
		jl_io_printc(jlc, "jl_fl_save: Failed to open file: \"");
		jl_io_printc(jlc, name);
		jl_io_printc(jlc, "\" Write failed: ");
		jl_io_printc(jlc, strerror(errsv));
		jl_io_printc(jlc, "\n");
		exit(-1);
	}
	int at = lseek(fd, 0, SEEK_END);
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLSV");
	jl_io_printc(jlc, "Writing....");
	n_bytes = write(fd, file, bytes);
	if(n_bytes <= 0) {
		errsv = errno;
		close(fd);
		_jl_fl_errf(_jlc, ":Write failed: ");
		_jl_fl_errf(_jlc, strerror(errsv));
		jl_sg_kill(jlc, "\n");
	}
	bytecount = jl_me_string_fnum(jlc, (int)n_bytes);
	atplace = jl_me_string_fnum(jlc, at);
	jl_io_printc(jlc, "Wrote ");
	jl_io_printc(jlc, bytecount);
	jl_io_printc(jlc, " bytes @");
	jl_io_printc(jlc, atplace);
	jl_io_printc(jlc, "\n");
	free(bytecount);
	free(atplace);
	close(fd);
	jl_io_printc(jlc, "Saved!");
	jl_io_close_block(_jlc->jlc); // } : Close Block "FLSV"
	return;
}

static inline void _jl_fl_reset_cursor(char *file_name) {
	int fd = open(file_name, O_RDWR);
	lseek(fd, 0, SEEK_SET);
	close(fd);
}

//NON-STATIC Library Dependent Functions
void _jl_fl_errf(jvct_t * _jlc, const char *msg) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "ERRF");
	jl_io_printc(_jlc->jlc, "saving to errf: ");
	jl_io_printc(_jlc->jlc, _jlc->fl.errf_filename);
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "ERRF");
	jl_io_printc(_jlc->jlc, msg);
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "ERRF");
	//Write to the errf file
	_jl_fl_save(_jlc->jlc, msg, _jlc->fl.errf_filename, strlen(msg));
	jl_io_printc(_jlc->jlc, "saved to errf!\n");
	
	jl_io_close_block(_jlc->jlc); //Close Block "ERRF"
}

/** @endcond **/

/**
 * Save A File To The File System.  Save Data of "bytes" bytes in "file" to
 * file "name"
 * @param jlc: Library Context
 * @param file: Data To Save To File
 * @param name: The Name Of The File to save to
 * @param bytes: Size of "File"
 */
void jl_fl_save(jl_t* jlc, const void *file, const char *name, uint32_t bytes) {
	truncate(name, 0);
	_jl_fl_save(jlc, file, name, bytes);
}

/**
 * Load a File from the file system.  Returns bytes loaded from "file_name"
 * @param jlc: Library Context
 * @param file_name: file to load
 * @returns A readable "strt" containing the bytes from the file.
 */
strt jl_fl_load(jl_t* jlc, char *file_name) {
	_jl_fl_reset_cursor(file_name);
	u32t i;
	unsigned char *file = malloc(MAXFILELEN);
	uint8_t offs = (file_name[0] == '!');
	int fd = open(file_name + offs, O_RDWR);
	
	//Open Block FLLD
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLLD");
	
	if(fd <= 0) {
		jl_io_printc(jlc, "failed to open: \"");
		jl_io_printc(jlc, file_name);
		jl_io_printc(jlc, "\"\n");
		jl_sg_kill(jlc, "file_file_load: Failed to open file");
	}
	int Read = read(fd, file, MAXFILELEN + 1);
	jlc->info = Read;

	jl_io_printc(jlc, "read ");
	jl_io_printi(jlc, jlc->info);
	jl_io_printc(jlc, "bytes\n");
	close(fd);
	strt frtn = jl_me_strt_make(jlc->info, STRT_KEEP);
	for( i = 0; i < jlc->info; i++) {
		frtn->data[i] = file[i];
	}
	frtn->data = file;
	
	jl_io_close_block(jlc); //Close Block "FLLD"
	return frtn;
}

/**
 * Save file "filename" with contents "data" of size "dataSize" to package
 * "packageFileName"
 * @param jlc: Library Context
 * @param packageFileName: Name of package to Save to
 * @param fileName: the file to Save to within the package.
 * @param data: the data to save to the file
 * @param dataSize: the # of bytes to save from the data to the file.
 * @returns 0: On success
 * @returns 1: If File is unable to be made.
 */
char jl_fl_pk_save(jl_t* jlc, char *packageFileName, char *fileName,
	void *data, uint64_t dataSize)
{
	jl_io_offset(jlc, JL_IO_SIMPLE, "PKSV"); // {
	jl_io_printc(jlc, "opening \"");
	jl_io_printc(jlc, packageFileName);
	jl_io_printc(jlc, "\"....\n");
	struct zip *archive = zip_open(packageFileName, ZIP_CREATE 
		/*| ZIP_CHECKCONS*/, NULL);
	if(archive == NULL) {
		jl_io_close_block(jlc); // !}
		return 1;
	}else{
		jl_io_printc(jlc, "opened file system:\"");
		jl_io_printc(jlc, packageFileName);
		jl_io_printc(jlc, "\".\n");
	}

	struct zip_source *s;
	if ((s=zip_source_buffer(archive, (void *)data, dataSize, 0)) == NULL) {
		zip_source_free(s);
		_jl_fl_errf(jlc->_jlc, "[JLVM/FILE/PKDJ/SAVE] src null error[replace]: ");
		_jl_fl_errf(jlc->_jlc, (char *)zip_strerror(archive));
		jl_sg_kill(jlc, "\n");
	}
//	printf("%d,%d,%d\n",archive,sb.index,s);
	if(zip_file_add(archive, fileName, s, ZIP_FL_OVERWRITE)) {
		jl_io_printc(jlc, "add/err: \"");
		jl_io_printc(jlc, zip_strerror(archive));
		jl_io_printc(jlc, "\"\n");
	}else{
		jl_io_printc(jlc, "added \"");
		jl_io_printc(jlc, fileName);
		jl_io_printc(jlc, "\" to file sys.\n");
	}
	zip_close(archive);
	jl_io_printc(jlc, "DONE!\n");
	jl_io_close_block(jlc); // }
	return 0;
}

static void _jl_fl_pk_load_quit(jl_t* jlc) {
	jl_io_close_block(jlc); //Close Block "PKLD"
}

/**
 * Load file "filename" in package "packageFileName" & Return contents
 * May return NULL.  If it does jlc->errf will be set.
 * -ERR:
 *	-ERR_NERR:	File is empty.
 *	-ERR_NONE:	Can't find filename in packageFileName. [ DNE ]
 *	-ERR_FIND:	Can't find packageFileName. [ DNE ]
 * @param jlc: Library Context
 * @param packageFileName: Package to load file from
 * @param filename: file within package to load
 * @returns: contents of file ( "filename" ) in package ( "packageFileName" )
*/
uint8_t *jl_fl_pk_load(jl_t* jlc, const char *packageFileName,
	const char *filename)
{
	jlc->errf = JL_ERR_NERR;
	jl_io_offset(jlc, JL_IO_SIMPLE, "PKLD"); // { : PKLD
	int zerror = 0;
	jl_io_printc(jlc, "loading package:\"");
	jl_io_printc(jlc, packageFileName);
	jl_io_printc(jlc, "\"...\n");
	jl_io_printc(jlc, "error check 1.\n");
	struct zip *zipfile = zip_open(packageFileName, ZIP_CHECKCONS, &zerror);
	jl_io_printc(jlc, "error check 2.\n");
	if(zerror == ZIP_ER_OPEN) {
		jl_io_printc(jlc, " NO EXIST!");
		jlc->errf = JL_ERR_FIND;
		_jl_fl_pk_load_quit(jlc);
		return NULL;
	}
	jl_io_printc(jlc, "error check 3.\n");
	if(zipfile == NULL) {
		jl_sg_kill(jlc, ":couldn't load pckg!\n");
	}
	jl_io_printc(jlc, "error check 4.\n");
	jl_io_printc(jlc, (char *)zip_strerror(zipfile));
	jl_io_printc(jlc, "loaded package.\n");
	unsigned char *fileToLoad = malloc(PKFMAX);
	jl_io_printc(jlc, "opening file in package....\n");
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	jl_io_printc(jlc, "call pass.");
	if(file == NULL) {
		jl_io_printc(jlc, "couldn't open up file: \"");
		jl_io_printc(jlc, filename);
		jl_io_printc(jlc, "\" in ");
		jl_io_printc(jlc, packageFileName);
		jl_io_printc(jlc, "because: ");
		jl_io_printc(jlc, (void *)zip_strerror(zipfile));
		jl_io_printc(jlc, "\n");
		jlc->errf = JL_ERR_NONE;
		_jl_fl_pk_load_quit(jlc);
		return NULL;
	}
	jl_io_printc(jlc, "opened file in package\n");
	jl_io_printc(jlc, "reading opened file....\n");
	if((jlc->info = zip_fread(file, fileToLoad, PKFMAX)) == -1) {
		jl_sg_kill(jlc, "file reading failed");
	}
	jl_io_offset(jlc, JL_IO_MINIMAL, "PKLD"); // = : PKLD TODO: REMOVE
	if(jlc->info == 0) {
		jl_io_printc(jlc, "empty file, returning NULL.");
		return NULL;
	}
	jl_io_printc(jlc, "read ");
	jl_io_printi(jlc, jlc->info);
	jl_io_printc(jlc, " bytes\n");
	jl_io_printc(jlc, "read opened file.\n");
	zip_close(zipfile);
	jl_io_printc(jlc, "done.\n");
	jlc->errf = JL_ERR_NERR;
	_jl_fl_pk_load_quit(jlc);
	return fileToLoad;
}

/**
 * Create a folder (directory)
 * @param jlc: library context
 * @param pfilebase: name of directory to create
*/
void jl_fl_mkdir(jl_t* jlc, strt pfilebase) {
	jl_io_offset(jlc, JL_IO_SIMPLE, "MDIR"); // {
	if(mkdir((void *)pfilebase->data, 0)) {
		int errsv = errno;
		if(errsv == EEXIST) {
			jl_io_printc(jlc, "Directory Exist! Continue...\n");
		}else{
			_jl_fl_errf(jlc->_jlc, ":Directory: Uh oh...:\n:");
			_jl_fl_errf(jlc->_jlc, strerror(errsv));
			jl_sg_kill(jlc, "\n");
		}
	}else{
		jl_io_printc(jlc, "Created Directory!\n");
	}
	jl_io_close_block(jlc); // } : MDIR
}

/**
 * Create the media package file & load a file from it.
 * @param jlc: library context
 * @param pfilebase: name of file to load from package
 * @param pzipfile: the name of the zipfile to create.
 * @param contents: the contents to put in the file
 * @param size: the size (in bytes) of the contents.
 * @return x: the data contents of the file.
*/
uint8_t * jl_fl_mkfile(jl_t* jlc, char *pzipfile, char *pfilebase,
	char *contents, uint32_t size)
{

//	if(!pfilebase) { return; }
	uint8_t *freturn;

	//Create Block "MKFL"
	jl_io_offset(jlc, JL_IO_SIMPLE, "MKFL"); // {

	jl_io_printc(jlc, "Creating File...\n");
	jl_fl_save(jlc, contents, pzipfile, size);
	jl_io_printc(jlc, "Attempt Complete!\n");
	SDL_Delay(1000); //give file system time to update
	jl_io_printc(jlc, "Try loading....\n");
	if(
		((freturn = jl_fl_pk_load(jlc, pzipfile, pfilebase))== NULL) &&
		(jlc->errf == JL_ERR_FIND) )//Package still doesn't exist!!
	{
		jl_sg_kill(jlc, ":Failed To Create file\n");
	}
	jl_io_printc(jlc, "Good loading!\n");
	//Close Block "MKFL"
	jl_io_close_block(jlc); // }
	jl_io_printc(jlc, "File Made!\n");
	return freturn;
}

/**
 * Load media package, create it if it doesn't exist.
 * @param jlc: Library Context
 * @param Fname: File in Media Package to load.
 * @param pzipfile: Where to make the package.
 * @param pdata: Media Package Data to save if it doesn't exist.
 * @param psize: Size of "pdata" 
*/
uint8_t *jl_fl_media(jl_t* jlc, char *Fname, char *pzipfile,
	void *pdata, uint64_t psize)
{
	uint8_t *freturn = jl_fl_pk_load(jlc, pzipfile, Fname);
	jl_io_printc(jlc, "JL_FL_MEDIA Returning\n");
	//If Package doesn't exist!! - create
	if( (freturn == NULL) && (jlc->errf == JL_ERR_FIND) )
		return jl_fl_mkfile(jlc, pzipfile, Fname, pdata, psize);
	else
		return freturn;
}

/**
 * Get the designated location for a resource pack. Resloc = Resource Location
 * @param jlc: Library Context.
 * @param pprg_name: Program Name
 * @param pfilename: Name Of Resource Pack
 * @returns: The designated location for a resouce pack
*/
char * jl_fl_get_resloc(jl_t* jlc, char* pprg_name, char* pfilename) {
	jvct_t * _jlc = jlc->_jlc;
	int i;
	char *filebase;
	char *location;
	strt filebases = jl_me_strt_make(0, STRT_KEEP);
	strt errfs = jl_me_strt_make(0, STRT_KEEP);
	
	//Open Block "FLBS"
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLBS");
	
	jl_io_printc(jlc, "Getting FileBase....\n");
	
	#if JL_PLAT == JL_PLAT_PHONE
	jl_me_strt_merg(jlc, filebases, Strt(JLVM_FILEBASE));
	jl_me_strt_merg(jlc, filebases, Strt(pprg_name));
	jl_me_strt_merg(jlc, filebases, Strt("/"));
	filebase = (void *)filebases->data;
	//Setting errf
	jl_me_strt_merg(jlc, errfs, Strt(JLVM_FILEBASE));
	jl_me_strt_merg(jlc, errfs, Strt("errf.txt"));

	#elif JL_PLAT == JL_PLAT_COMPUTER
	filebase = SDL_GetPrefPath("JLVM", pprg_name);
	if(filebase == NULL) {
		jl_me_strt_merg(jlc, filebases, Strt("JLVM/"));
		jl_me_strt_merg(jlc, filebases, Strt(pprg_name));
		filebase = (void *)filebases->data;
	}else{
		filebases = Strt(filebase);
	}
	jl_io_printc(jlc, "Setting Errf....\n");
	jl_me_strt_merg(jlc, errfs, filebases);
	jl_me_strt_trunc(jlc, errfs, filebases->size - 5);
	jl_me_strt_merg(jlc, errfs, Strt("errf.txt"));
	jl_io_printc(jlc, "Set Errf....\n");
	#else //OTHER
	#endif
	
	_jlc->fl.errf_filename = (void *)errfs->data;
	jl_io_printc(jlc, "errf_filename:");
	jl_io_printc(jlc, _jlc->fl.errf_filename);
	jl_io_printc(jlc, "\n");

	jl_io_printc(jlc, filebase);
	
	//filebase maker
	#if JL_PLAT == JL_PLAT_PHONE
	jl_fl_mkdir(jlc, Strt(JLVM_FILEBASE));
	#else
	jl_io_printc(jlc, "\nFB2....\n");
	int fsize = strlen(filebase)+strlen(JLVM_FILEBASE) + 1;
	char *filebase2 = malloc(fsize);
	jl_io_printc(jlc, "Clear....\n");
	for(i = 0; i < fsize; i++) {
		filebase2[i] = '\0';
	}
	jl_io_printc(jlc, "Add....\n");
	strncat(filebase2,filebase,strlen(filebase));
	strncat(filebase2,JLVM_FILEBASE,strlen(JLVM_FILEBASE));
	jl_io_printc(jlc, "Make....\n");
	jl_fl_mkdir(jlc, Strt(filebase2));
	#endif
	jl_fl_mkdir(jlc, Strt(filebase));

	jl_io_printc(jlc, "Merge....\n");
	strt pvar_pkfl = jl_me_strt_make(0, STRT_KEEP);
	jl_me_strt_merg(jlc, pvar_pkfl, Strt(filebase));
	jl_me_strt_merg(jlc, pvar_pkfl, Strt(pfilename));

	location = jl_me_string_fstrt(jlc, pvar_pkfl);

	jl_io_printc(jlc, "filebase: ");
	jl_io_printc(jlc, filebase);
	jl_io_printc(jlc, "\nlocation: ");
	jl_io_printc(jlc, location);
	
	jl_io_close_block(jlc); //Close Block "FLBS"
	jl_io_printc(jlc, "finished resloc w/ \"");
	jl_io_printc(jlc, location);
	jl_io_printc(jlc, "\"\n");
	return location;
}

static void _jl_fl_user_select_check_extradir(char *dirname) {
	if(dirname[strlen(dirname)-1] == '/' &&
		dirname[strlen(dirname)-2] == '/')
	{
		dirname[strlen(dirname)-1] = '\0';
	}
}

// Return 1 on success
// Return 0 if directory not available
static uint8_t _jl_fl_user_select_open_dir(jl_t* jlc, char *dirname) {
	DIR *dir;
	struct dirent *ent;
	jvct_t * _jlc = jlc->_jlc;
	uint8_t offset = 0;

	_jl_fl_user_select_check_extradir(dirname);
	if(dirname[1] == '\0') {
		jl_me_alloc(jlc, (void**)&dirname, 0, 0);
		dirname = SDL_GetPrefPath("JLVM", "\0");
		_jl_fl_user_select_check_extradir(dirname);
	}
	offset = dirname[0] == '!';
	_jlc->fl.dirname = dirname;
	_jlc->fl.cursor = 0;
	_jlc->fl.cpage = 0;
	cl_list_clear(_jlc->fl.filelist);
	printf("dirname=%s\n", _jlc->fl.dirname);
	if ((dir = opendir (dirname + offset)) != NULL) {
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
			jlc->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else if(errsv == ENOENT) { // Directory Doesn't Exist
			return 0;
		}else if(errsv == EACCES) { // Doesn't have permission
			return 0;
		}else if((errsv == EMFILE) || (errsv == ENFILE) ||
			(errsv == ENOMEM)) //Not enough memory!
		{
			jl_sg_kill(jlc, "FileViewer Can't Open Directory:"
				"Not Enough Memory!");
		}else{ //Unknown error
			jl_sg_kill(jlc, "FileViewer Can't Open Directory:"
				"Unknown Error!");
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
uint8_t jl_fl_user_select_init(jl_t* jlc, const char *program_name,
	void *newfiledata, uint64_t newfilesize)
{
	jvct_t * _jlc = jlc->_jlc;
	jlc->loop = JL_SG_WM_DN;
	_jlc->fl.returnit = 0;
	_jlc->fl.inloop = 1;
	_jlc->fl.newfiledata = newfiledata;
	_jlc->fl.newfilesize = newfilesize;
	_jlc->fl.prompt = 0;
	_jlc->fl.promptstring = NULL;
	if(program_name[0] == '!') {
		char *path = jl_me_copy(jlc,program_name,strlen(program_name));
		return _jl_fl_user_select_open_dir(jlc, path);
	}else{
		return _jl_fl_user_select_open_dir(jlc,
			SDL_GetPrefPath("JLVM",program_name));
	}
}

static void _jl_fl_user_select_up(jl_t* jlc) {
	if(jlc->ctrl.h == 1) {
		jvct_t * _jlc = jlc->_jlc;
		if((_jlc->fl.cursor > 0) || _jlc->fl.cpage) _jlc->fl.cursor--;
	}
}

static void _jl_fl_user_select_dn(jl_t* jlc) {
	if(jlc->ctrl.h == 1) {
		jvct_t * _jlc = jlc->_jlc;
		if(_jlc->fl.cursor + (_jlc->fl.cpage * (_jlc->fl.drawupto+1)) <
			cl_list_count(_jlc->fl.filelist) - 1)
		{
			_jlc->fl.cursor++;
		}
	}
}

static void _jl_fl_user_select_rt(jl_t* jlc) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_dn(jlc);
	}
}

static void _jl_fl_user_select_lt(jl_t* jlc) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_up(jlc);
	}
}

static char* _jl_fl_full_fname(jl_t* jlc,char *selecteddir,char *selecteditem){
	jvct_t * _jlc = jlc->_jlc;

	char *newdir = malloc(
		strlen(_jlc->fl.dirname) +
		strlen(selecteditem) + 2);
	memcpy(newdir, _jlc->fl.dirname,
		strlen(_jlc->fl.dirname));
	memcpy(newdir + strlen(_jlc->fl.dirname),
		selecteditem,
		strlen(selecteditem));
	newdir[strlen(_jlc->fl.dirname) +
		strlen(selecteditem)] = '/';
	newdir[strlen(_jlc->fl.dirname) +
		strlen(selecteditem) + 1] = '\0';
	return newdir;
}

static void _jl_fl_open_file(jl_t*jlc, char *selecteditem) {
	jvct_t * _jlc = jlc->_jlc;
	char *newdir = _jl_fl_full_fname(jlc, _jlc->fl.dirname, selecteditem);

	free(_jlc->fl.dirname);
	_jlc->fl.dirname = NULL;
	_jl_fl_user_select_open_dir(jlc,newdir);
}

static void _jl_fl_user_select_do(jl_t* jlc) {
	if(jlc->ctrl.h == 1) {
		jvct_t * _jlc = jlc->_jlc;
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
			_jl_fl_user_select_open_dir(jlc,_jlc->fl.dirname);
		}else if(strcmp(_jlc->fl.selecteditem, ".") == 0) {
			_jlc->fl.inloop = 0;
			jlc->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else{
			_jl_fl_open_file(jlc, _jlc->fl.selecteditem);
		}
	}
}

/**
 * Run the file viewer.
 * @param jlc: Library Context.
**/
void jl_fl_user_select_loop(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	struct cl_list_iterator *iterator;
	int i;
	char *stringtoprint;
	_jlc->fl.drawupto = ((int)(20.f * jl_dl_p(jlc))) - 1;

	iterator = cl_list_iterator_create(_jlc->fl.filelist);

	jl_gr_draw_image(jlc, 0, 1, 0., 0., 1., jl_dl_p(jlc), 1, 127);
	jl_gr_draw_text(jlc, "Select File", .02, .02, .04,255);

	jl_ct_run_event(jlc,JL_CT_MAINUP, _jl_fl_user_select_up, jl_dont);
	jl_ct_run_event(jlc,JL_CT_MAINDN, _jl_fl_user_select_dn, jl_dont);
	jl_ct_run_event(jlc,JL_CT_MAINRT, _jl_fl_user_select_rt, jl_dont);
	jl_ct_run_event(jlc,JL_CT_MAINLT, _jl_fl_user_select_lt, jl_dont);
	//Draw files
	for(i = 0; i < cl_list_count(_jlc->fl.filelist); i++) {
		stringtoprint = cl_list_iterator_next(iterator);
		if(strcmp(stringtoprint, "..") == 0) {
			stringtoprint = "//containing folder//";
		}else if(strcmp(stringtoprint, ".") == 0) {
			stringtoprint = "//this folder//";
		}
		if(i - (_jlc->fl.cpage * (_jlc->fl.drawupto+1)) >= 0)
			jl_gr_draw_text(jlc, stringtoprint,
				.06, .08 + (.04 *
				(i - (_jlc->fl.cpage * (_jlc->fl.drawupto+1)))),
				.04,255);
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
		if(jl_gr_draw_textbox(jlc, .02, jl_dl_p(jlc) - .06, .94, .02,
			&_jlc->fl.promptstring))
		{
			char *name = _jl_fl_full_fname(jlc,
				_jlc->fl.dirname,
				(char*)_jlc->fl.promptstring->data);
			name[strlen(name) - 1] = '\0';
			jl_fl_save(jlc, _jlc->fl.newfiledata,
				name, _jlc->fl.newfilesize);
			_jl_fl_user_select_open_dir(jlc, _jlc->fl.dirname);
			_jlc->fl.prompt = 0;
		}
	}else{
		jl_gr_draw_text(jlc, ">", .02, .08 + (.04 * _jlc->fl.cursor),
			.04,255);
		jl_gr_draw_text(jlc, _jlc->fl.dirname, .02, jl_dl_p(jlc) - .02, .02, 255);
		jl_ct_run_event(jlc, JL_CT_SELECT, _jl_fl_user_select_do,
			jl_dont);
	}
	_jlc->fl.btns[0]->loop(jlc);
	_jlc->fl.btns[1]->loop(jlc);
}

/**
 * Get the results from the file viewer.
 * @param jlc: Library Context.
 * @returns: If done, name of selected file.  If not done, NULL is returned.
**/
char *jl_fl_user_select_get(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	
	if(_jlc->fl.returnit)
		return _jlc->fl.dirname;
	else
		return NULL;
}

static void _jl_fl_btn_makefile_action(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	_jlc->fl.prompt = 1;
}

static void _jl_fl_btn_makefile(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	if(_jlc->fl.newfiledata == NULL) return;
	jl_gr_draw_glow_button(jlc, _jlc->fl.btns[0], "+ New File",
		_jl_fl_btn_makefile_action);
}

static void _jl_fl_btn_makefolder(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	jl_gr_draw_glow_button(jlc, _jlc->fl.btns[1], "+ New Folder",
		_jl_fl_btn_makefile_action);
}

void _jl_fl_kill(jvct_t * _jlc) {
	if(_jlc->has.fileviewer) {
		jl_io_printc(_jlc->jlc, "killing fl...\n");
		cl_list_destroy(_jlc->fl.filelist);
		jl_io_printc(_jlc->jlc, "killed fl!\n");
	}
}

void _jl_fl_initb(jvct_t * _jlc) {
	//Create the variables
	_jlc->fl.filelist = cl_list_create();
	_jlc->fl.inloop = 0;
	_jlc->fl.btns[0] = jl_gr_sprite_make(_jlc->jlc, 0, 1, 1, 255,
		.8, 0., .1, .1, _jl_fl_btn_makefile, 0);
	_jlc->fl.btns[1] = jl_gr_sprite_make(_jlc->jlc, 0, 1, 2, 255,
		.9, 0., .1, .1, _jl_fl_btn_makefolder, 0);
	_jlc->has.fileviewer = 1;
}

void _jl_fl_inita(jvct_t * _jlc) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "FILE"); // {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "INIT"); // {

	jl_io_printc(_jlc->jlc, "program name:");
	jl_dl_progname(_jlc->jlc, Strt("JLVM"));

	char * pkfl =
		jl_fl_get_resloc(_jlc->jlc, "JLLB", "media.zip");
	remove(pkfl);

	truncate(_jlc->fl.errf_filename, 0);
	_jl_fl_errf(_jlc, ":Starting...\n");
	jl_io_close_block(_jlc->jlc); // } Close Block "INIT"
	jl_io_close_block(_jlc->jlc); // } Close Block "FILE"
	jl_io_printc(_jlc->jlc, "finished file init\n");
}
