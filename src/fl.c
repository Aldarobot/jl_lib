/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * fl.c
 * 	This allows you to modify the file system.  It uses libzip.
 */
#include "header/jl_pr.h"
#if JL_PLAT == JL_PLAT_PHONE
	#include <sys/stat.h>
#endif

#define PKFMAX 10000000
#define JL_FL_PERMISSIONS ( S_IRWXU | S_IRWXG | S_IRWXO )

#if JL_PLAT == JL_PLAT_PHONE
	extern str_t JL_FL_BASE;
#endif

/** @cond **/
// Static Functions

// This function converts linux filenames to native filnames
static str_t jl_fl_convert__(jl_t* jlc, str_t filename) {
	jvct_t * _jlc = jlc->_jlc;

	strt src = jl_me_strt_mkfrom_str(filename);
	strt converted = jl_me_strt_make(0);

	if(jl_me_test_next(src, "!")) {
		src->curs++; // ignore
	}else{
		src->curs++; // ignore
		jl_me_strt_merg(_jlc->jlc, converted, _jlc->fl.separator);
	}
	while(1) {
		strt append = jl_me_read_upto(jlc, src, '/', 300);
		if(append->data[0] == '\0') break;
		jl_me_strt_merg(_jlc->jlc, converted, append);
		if(jl_me_strt_byte(src) == '/')
			jl_me_strt_merg(_jlc->jlc,converted,_jlc->fl.separator);
		src->curs++; // Skip '/'
		jl_me_strt_free(append);
	}
	jl_me_strt_free(src);
	return jl_me_string_fstrt(jlc, converted);
}

static int jl_fl_save_(jl_t* jlc, const void *file_data, const char *file_name,
	uint32_t bytes)
{
	int errsv;
	ssize_t n_bytes;
	int fd;
	
	if(file_name == NULL) {
		jl_io_print(jlc, "Save[==]: file_name is Null");
		jl_sg_kill(jlc);
		return -1;
	}else if(strlen(file_name) == 0) {
		jl_io_print(jlc, "Save[strlen]: file_name is Empty String");
		jl_sg_kill(jlc);
		return -1;
	}else if(!file_data) {
		jl_io_print(jlc, "Save[file_data]: file_data is NULL");
		jl_sg_kill(jlc);
		return -1;	
	}

	str_t converted_filename = jl_fl_convert__(jlc, file_name);
	fd = open(converted_filename, O_RDWR | O_CREAT, JL_FL_PERMISSIONS);

	if(fd <= 0) {
		errsv = errno;

		jl_io_print(jlc, "Save[open]:");
		jl_io_print(jlc, " Failed to open file:");
		jl_io_print(jlc, "\t%s", converted_filename);
		jl_io_print(jlc, " Write failed: %s", strerror(errsv));
		jl_sg_kill(jlc);
	}
	int at = lseek(fd, 0, SEEK_END);
	n_bytes = write(fd, file_data, bytes);
	if(n_bytes <= 0) {
		errsv = errno;
		close(fd);
		jl_io_print(jlc, ":Save[write]: Write to \"%s\" failed:");
		jl_io_print(jlc, "\"%s\"", strerror(errsv));
		jl_sg_kill(jlc);
	}
	close(fd);
	return at;
}

static inline void jl_fl_reset_cursor__(str_t file_name) {
	int fd = open(file_name, O_RDWR);
	lseek(fd, 0, SEEK_SET);
	close(fd);
}

static inline void jl_fl_get_root__(jvct_t * _jlc) {
	strt root_path;

#if JL_PLAT == JL_PLAT_PHONE
	strt root_dir;

	jl_io_print(_jlc->jlc, "Get external storage directory.");
	root_path = jl_me_strt_mkfrom_str(JL_FL_BASE);
	jl_io_print(_jlc->jlc, "Append JL_ROOT_DIR.");
	root_dir = jl_me_strt_mkfrom_str(JL_ROOT_DIR);
	jl_io_print(_jlc->jlc, "Merging root_path and root_dir.");
	jl_me_strt_merg(_jlc->jlc, root_path, root_dir);
	jl_io_print(_jlc->jlc, "Free root_dir.");
	jl_me_strt_free(root_dir);
#else
	// Get the operating systems prefered path
	m_str_t pref_path = SDL_GetPrefPath(JL_ROOT_DIRNAME, "\0");

	if(pref_path) {
		// Erase extra non-needed '/'s
		pref_path[strlen(pref_path) - 1] = '\0';
		// Set root path to pref path
		root_path = jl_me_strt_mkfrom_str(pref_path);
		// Free the pointer to pref path
		SDL_free(pref_path);
	}else{
		jl_io_print(_jlc->jlc, "This platform has no pref path!");
		jl_sg_kill(_jlc->jlc);
	}
#endif
	// Make "-- JL_ROOT_DIR"
	if(jl_fl_mkdir(_jlc->jlc, (str_t) root_path->data) == 2) {
		jl_io_print(_jlc->jlc, (str_t) root_path->data);
		jl_io_print(_jlc->jlc, ": mkdir : Permission Denied");
		jl_sg_kill(_jlc->jlc);
	}
	// Set paths.root & free root_path
	_jlc->fl.paths.root = jl_me_string_fstrt(_jlc->jlc, root_path);
	jl_io_print(_jlc->jlc, "Root Path=\"%s\"", _jlc->fl.paths.root);
}

static inline void jl_fl_get_errf__(jvct_t * _jlc) {
	strt fname = jl_me_strt_mkfrom_str("errf.txt");
	// Add the root path
	strt errfs = jl_me_strt_mkfrom_str(_jlc->fl.paths.root);

	// Add the file name
	jl_me_strt_merg(_jlc->jlc, errfs, fname);
	// Free fname
	jl_me_strt_free(fname);
	// Set paths.errf & free errfs
	_jlc->fl.paths.errf = jl_me_string_fstrt(_jlc->jlc, errfs);
}

// NON-STATIC Library Dependent Functions

/** @endcond **/

/**
 * Print text to a file.
 * @param jlc: The library context.
 * @param fname: The name of the file to print to.
 * @param msg: The text to print.
**/
void jl_fl_print(jl_t* jlc, str_t fname, str_t msg) {
	jvct_t * _jlc = jlc->_jlc;

	// Write to the errf file
	if(_jlc->has.filesys)
		jl_fl_save_(_jlc->jlc, msg, fname, strlen(msg));
}

/**
 * Check whether a file or directory exists.
 * @param jlc: The library context.
 * @param path: The path to the file to check.
 * @returns 0: If the file doesn't exist.
 * @returns 1: If the file does exist and is a directory.
 * @returns 2: If the file does exist and isn't a directory.
 * @returns 3: If the file exists and the user doesn't have permissions to open.
 * @returns 255: This should never happen.
**/
u8_t jl_fl_exist(jl_t* jlc, str_t path) {
	DIR *dir;
	if ((dir = opendir (path)) == NULL) {
		//Couldn't open Directory
		int errsv = errno;
		if(errsv == ENOTDIR) { //Not a directory - is a file
			return 2;
		}else if(errsv == ENOENT) { // Directory Doesn't Exist
			return 0;
		}else if(errsv == EACCES) { // Doesn't have permission
			return 3;
		}else if((errsv == EMFILE) || (errsv == ENFILE) ||
			(errsv == ENOMEM)) //Not enough memory!
		{
			jl_io_print(jlc, "jl_fl_exist: Out of Memory!");
			jl_sg_kill(jlc);
		}else{ //Unknown error
			jl_io_print(jlc, "jl_fl_exist: Unknown Error!");
			jl_sg_kill(jlc);
		}
	}else{
		return 1; // Directory Does exist
	}
	return 255;
}

/**
 * Save A File To The File System.  Save Data of "bytes" bytes in "file" to
 * file "name"
 * @param jlc: Library Context
 * @param file: Data To Save To File
 * @param name: The Name Of The File to save to
 * @param bytes: Size of "File"
 */
void jl_fl_save(jl_t* jlc, const void *file, const char *name, uint32_t bytes) {
	str_t converted_filename = jl_fl_convert__(jlc, name);
	// delete file
	unlink(converted_filename);
	// make file
	jl_fl_save_(jlc, file, name, bytes);
}

/**
 * Load a File from the file system.  Returns bytes loaded from "file_name"
 * @param jlc: Library Context
 * @param file_name: file to load
 * @returns A readable "strt" containing the bytes from the file.
 */
strt jl_fl_load(jl_t* jlc, str_t file_name) {
	jl_fl_reset_cursor__(file_name);
	unsigned char *file = malloc(MAXFILELEN);
	str_t converted_filename = jl_fl_convert__(jlc, file_name);
	int fd = open(converted_filename, O_RDWR);
	
	//Open Block FLLD
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLLD");
	
	if(fd <= 0) {
		jl_io_print(jlc, "failed to open: \"%s\"", file_name);
		jl_io_print(jlc, "file_file_load: Failed to open file.");
		jl_sg_kill(jlc);
	}
	int Read = read(fd, file, MAXFILELEN);
	jlc->info = Read;

	jl_io_print(jlc, "jl_fl_load(): read %d bytes", jlc->info);
	close(fd);

	strt rtn = jlc->info ?
		jl_me_strt_mkfrom_data(jlc, jlc->info, file) : NULL;
	
	jl_io_close_block(jlc); //Close Block "FLLD"
	return rtn;
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
char jl_fl_pk_save(jl_t* jlc, str_t packageFileName, str_t fileName,
	void *data, uint64_t dataSize)
{
	str_t converted = jl_fl_convert__(jlc, packageFileName);

	jl_io_offset(jlc, JL_IO_SIMPLE, "PKSV"); // {
	jl_io_print(jlc, "opening \"%s\"....", converted);
	struct zip *archive = zip_open(converted, ZIP_CREATE 
		/*| ZIP_CHECKCONS*/, NULL);
	if(archive == NULL) {
		jl_io_close_block(jlc); // !}
		return 1;
	}else{
		jl_io_print(jlc, "opened package, \"%d\".", converted);
	}

	struct zip_source *s;
	if ((s=zip_source_buffer(archive, (void *)data, dataSize, 0)) == NULL) {
		zip_source_free(s);
		jl_io_print(jlc, "[JL_FL_PK_SAVE] src null error[replace]: %s",
			(char *)zip_strerror(archive));
		jl_sg_kill(jlc);
	}
//	printf("%d,%d,%d\n",archive,sb.index,s);
	if(zip_file_add(archive, fileName, s, ZIP_FL_OVERWRITE)) {
		jl_io_print(jlc, "add/err: \"%s\"", zip_strerror(archive));
	}else{
		jl_io_print(jlc, "added \"%s\" to file sys.", fileName);
	}
	zip_close(archive);
	jl_io_print(jlc, "DONE!");
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
strt jl_fl_pk_load(jl_t* jlc, const char *packageFileName,
	const char *filename)
{
	str_t converted = jl_fl_convert__(jlc, packageFileName);
	int zerror = 0;

	jlc->errf = JL_ERR_NERR;
	jl_io_offset(jlc, JL_IO_SIMPLE, "PKLD"); // { : PKLD

	jl_io_print(jlc, "loading package:\"%s\"...", converted);
	jl_io_print(jlc, "error check 1.");
	struct zip *zipfile = zip_open(converted, ZIP_CHECKCONS, &zerror);
	jl_io_print(jlc, "error check 2.");
	if(zerror == ZIP_ER_NOENT) {
		jl_io_print(jlc, " NO EXIST!");
		jlc->errf = JL_ERR_FIND;
		_jl_fl_pk_load_quit(jlc);
		return NULL;
	}
	jl_io_print(jlc, "error check 3.");
	if(zipfile == NULL) {
		jl_io_print(jlc, "couldn't load pckg \"%s\"", packageFileName);
		jl_sg_kill(jlc);
	}
	jl_io_print(jlc, "error check 4.");
	jl_io_print(jlc, (char *)zip_strerror(zipfile));
	jl_io_print(jlc, "loaded package.");
	unsigned char *fileToLoad = malloc(PKFMAX);
	jl_io_print(jlc, "opening file in package....");
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	jl_io_print(jlc, "call pass.");
	if(file == NULL) {
		jl_io_print(jlc, "couldn't open up file: \"%s\" in \"%s\":",
			filename, converted);
		jl_io_print(jlc, "because: %s", (void *)zip_strerror(zipfile));
		jlc->errf = JL_ERR_NONE;
		_jl_fl_pk_load_quit(jlc);
		return NULL;
	}
	jl_io_print(jlc, "opened file in package / reading opened file....");
	if((jlc->info = zip_fread(file, fileToLoad, PKFMAX)) == -1) {
		jl_io_print(jlc, "file reading failed");
		jl_sg_kill(jlc);
	}
	if(jlc->info == 0) {
		jl_io_print(jlc, "empty file, returning NULL.");
		return NULL;
	}
	jl_io_print(jlc, "jl_fl_pk_load: read %d bytes", jlc->info);
	zip_close(zipfile);
	jl_io_print(jlc, "closed file.");
	// Make a strt from the data.
	strt rtn = jlc->info ?
		jl_me_strt_mkfrom_data(jlc, jlc->info, fileToLoad) : NULL;
	jl_io_print(jlc, "done.");
	jlc->errf = JL_ERR_NERR;
	_jl_fl_pk_load_quit(jlc);
	return rtn;
}

/**
 * Create a folder (directory)
 * @param jlc: library context
 * @param pfilebase: name of directory to create
 * @returns 0: Success
 * @returns 1: If the directory already exists.
 * @returns 2: Permission Error
 * @returns 255: Never.
*/
u8_t jl_fl_mkdir(jl_t* jlc, str_t path) {
	m_u8_t rtn = 255;

	jl_io_offset(jlc, JL_IO_SIMPLE, "MDIR"); // {
	if(mkdir(path, JL_FL_PERMISSIONS)) {
		int errsv = errno;
		if(errsv == EEXIST) {
			rtn = 1;
		}else if((errsv == EACCES) || (errsv == EROFS)) {
			rtn = 2;
		}else{
			jl_io_print(jlc, "couldn't mkdir:%s", strerror(errsv));
			jl_sg_kill(jlc);
		}
	}else{
		rtn = 0;
	}
	jl_io_close_block(jlc); // } : MDIR
	// Return
	return rtn;
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
strt jl_fl_mkfile(jl_t* jlc, str_t pzipfile, str_t pfilebase,
	char *contents, uint32_t size)
{
//	if(!pfilebase) { return; }
	strt rtn;

	//Create Block "MKFL"
	jl_io_offset(jlc, JL_IO_SIMPLE, "MKFL"); // {

	jl_io_print(jlc, "Creating File....");
	jl_fl_save(jlc, contents, pzipfile, size);
	jl_io_print(jlc, "Attempt Complete!");
	SDL_Delay(1000); //give file system time to update
	jl_io_print(jlc, "Try loading....");
	if(
		((rtn = jl_fl_pk_load(jlc, pzipfile, pfilebase))== NULL) &&
		(jlc->errf == JL_ERR_FIND) )//Package still doesn't exist!!
	{
		jl_io_print(jlc, "Failed To Create file");
		jl_sg_kill(jlc);
	}
	jl_io_print(jlc, "Good loading!");
	//Close Block "MKFL"
	jl_io_close_block(jlc); // }
	jl_io_print(jlc, "File Made!");
	return rtn;
}

/**
 * Load media package, create it if it doesn't exist.
 * @param jlc: Library Context
 * @param Fname: File in Media Package to load.
 * @param pzipfile: Where to make the package.
 * @param pdata: Media Package Data to save if it doesn't exist.
 * @param psize: Size of "pdata" 
*/
strt jl_fl_media(jl_t* jlc, str_t Fname, str_t pzipfile,
	void *pdata, uint64_t psize)
{
	strt rtn = jl_fl_pk_load(jlc, pzipfile, Fname);
	jl_io_print(jlc, "JL_FL_MEDIA Returning");
	//If Package doesn't exist!! - create
	if( (rtn == NULL) && (jlc->errf == JL_ERR_FIND) )
		return jl_fl_mkfile(jlc, pzipfile, Fname, pdata, psize);
	else
		return rtn;
}

/**
 * Get the designated location for a resource file. Resloc = Resource Location
 * @param jlc: Library Context.
 * @param prg_folder: The name of the folder for all of the program's resources.
 *	For a company "PlopGrizzly" with game "Super Game":
 *		Pass: "PlopGrizzly_SG"
 *	For an individual game developer "Jeron Lau" with game "Cool Game":
 *		Pass: "JeronLau_CG"
 * @param fname: Name Of Resource Pack
 * @returns: The designated location for a resouce pack
*/
str_t jl_fl_get_resloc(jl_t* jlc, str_t prg_folder, str_t fname) {
	jvct_t * _jlc = jlc->_jlc;
	strt filesr = jl_me_strt_mkfrom_str(JL_FILE_SEPARATOR);
	strt pfstrt = jl_me_strt_mkfrom_str(prg_folder);
	strt fnstrt = jl_me_strt_mkfrom_str(fname);
	strt resloc = jl_me_strt_mkfrom_str(_jlc->fl.paths.root);
	str_t rtn = NULL;
	
	//Open Block "FLBS"
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLBS"); // {
	
	jl_io_print(jlc, "Getting Resource Location....");
	// Append 'prg_folder' onto 'resloc'
	jl_me_strt_merg(jlc, resloc, pfstrt);
	// Append 'filesr' onto 'resloc'
	jl_me_strt_merg(jlc, resloc, filesr);
	// Make 'prg_folder' if it doesn't already exist.
	if( jl_fl_mkdir(jlc, (str_t) resloc->data) == 2 ) {
		jl_io_print(jlc, "jl_fl_get_resloc: couldn't make \"%s\"",
			(str_t) resloc->data);
		jl_io_print(jlc, "mkdir : Permission Denied");
		jl_sg_kill(jlc);
	}
	// Append 'fname' onto 'resloc'
	jl_me_strt_merg(jlc, resloc, fnstrt);
	// Set 'rtn' to 'resloc' and free 'resloc'
	rtn = jl_me_string_fstrt(jlc, resloc);
	// Free pfstrt & fnstrt & filesr
	jl_me_strt_free(pfstrt),jl_me_strt_free(fnstrt),jl_me_strt_free(filesr);
	// Close Block "FLBS"
	jl_io_close_block(jlc); // }
	jl_io_print(jlc, "finished resloc w/ \"%s\"", rtn); 
	return rtn;
}

static void _jl_fl_user_select_check_extradir(char *dirname) {
	if(dirname[strlen(dirname)-1] == '/' && strlen(dirname) > 1) {
		if(dirname[strlen(dirname)-2] == '/')
			dirname[strlen(dirname)-1] = '\0';
	}
}

// Return 1 on success
// Return 0 if directory not available
static uint8_t _jl_fl_user_select_open_dir(jl_t* jlc, char *dirname) {
	DIR *dir;
	struct dirent *ent;
	jvct_t * _jlc = jlc->_jlc;
	str_t converted_filename;

	_jl_fl_user_select_check_extradir(dirname);
	if(dirname[1] == '\0') {
		jl_me_alloc(jlc, (void**)&dirname, 0, 0);
		dirname = SDL_GetPrefPath("JL_Lib", "\0");
		_jl_fl_user_select_check_extradir(dirname);
	}
	_jlc->fl.dirname = dirname;
	_jlc->fl.cursor = 0;
	_jlc->fl.cpage = 0;
	converted_filename = jl_fl_convert__(jlc, _jlc->fl.dirname);
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
			jlc->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else if(errsv == ENOENT) { // Directory Doesn't Exist
			return 0;
		}else if(errsv == EACCES) { // Doesn't have permission
			return 0;
		}else if((errsv == EMFILE) || (errsv == ENFILE) ||
			(errsv == ENOMEM)) //Not enough memory!
		{
			jl_io_print(jlc, "FileViewer Can't Open Directory:"
				"Not Enough Memory!");
			jl_sg_kill(jlc);
		}else{ //Unknown error
			jl_io_print(jlc, "FileViewer Can't Open Directory:"
				"Unknown Error!");
			jl_sg_kill(jlc);
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
			SDL_GetPrefPath("JL_Lib",program_name));
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

	_jlc->fl.drawupto = ((int)(20.f * jl_gl_ar(jlc))) - 1;

	iterator = cl_list_iterator_create(_jlc->fl.filelist);

	jl_gr_fill_image_set(jlc, 0, JL_IMGI_ICON, 1, 255);
	jl_gr_fill_image_draw(jlc);
	jl_gr_draw_text(jlc, "Select File", (jl_vec3_t) { .02, .02, 0. },
		jlc->font);

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
			jl_gr_draw_text(jlc, stringtoprint, (jl_vec3_t) {
				.06,
				.08 + (jlc->font.size * (i - (_jlc->fl.cpage * (
					_jlc->fl.drawupto+1)))), 0. },
				jlc->font);
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
		if(jl_gr_draw_textbox(jlc, .02, jl_gl_ar(jlc) - .06, .94, .02,
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
		jl_gr_draw_text(jlc, ">", (jl_vec3_t) {
			.02, .08 + (.04 * _jlc->fl.cursor), 0. },
			jlc->font);
		jl_gr_draw_text(jlc, _jlc->fl.dirname,
			(jl_vec3_t) { .02, jl_gl_ar(jlc) - .02, 0. },
			(jl_font_t) { 0, JL_IMGI_ICON, 0, jlc->fontcolor, .02});
		jl_ct_run_event(jlc, JL_CT_SELECT, _jl_fl_user_select_do,
			jl_dont);
	}
	jl_gr_sp_rnl(jlc, _jlc->fl.btns[0]);
	jl_gr_sp_rnl(jlc, _jlc->fl.btns[1]);
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

static void _jl_fl_btn_makefile_press(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	_jlc->fl.prompt = 1;
}

static void _jl_fl_btn_makefile_loop(jl_t* jlc, jl_sprd_t* sprd) {
	jvct_t *_jlc = jlc->_jlc;

	//TODO: make graphic: 0, 1, 1, 255
	if(!_jlc->fl.newfiledata) return;
	jl_gr_draw_glow_button(jlc, _jlc->fl.btns[0], "+ New File",
		_jl_fl_btn_makefile_press);
}

static void _jl_fl_btn_makefile_draw(jl_t* jlc, jl_sprd_t* sprd) {
	jvct_t* _jlc = jlc->_jlc;
	jl_rect_t rc = { 0., 0., jl_gl_ar(jlc), jl_gl_ar(jlc) };
	jl_vec3_t tr = { 0., 0., 0. };

	jl_gr_vos_image(jlc, _jlc->gl.temp_vo, rc, 0, JL_IMGI_ICON, 9, 255);
	jl_gr_draw_vo(jlc, _jlc->gl.temp_vo, &tr);
}

static void _jl_fl_btn_makefolder_loop(jl_t* jlc, jl_sprd_t* sprd) {
	jvct_t *_jlc = jlc->_jlc;
	
	//TODO: make graphic: 0, 1, 2, 255,
	jl_gr_draw_glow_button(jlc, _jlc->fl.btns[1], "+ New Folder",
		_jl_fl_btn_makefile_press);
}

static void _jl_fl_btn_makefolder_draw(jl_t* jlc, jl_sprd_t* sprd) {
	jvct_t* _jlc = jlc->_jlc;
	jl_rect_t rc = { 0., 0., jl_gl_ar(jlc), jl_gl_ar(jlc) };
	jl_vec3_t tr = { 0., 0., 0. };

	jl_gr_vos_image(jlc, _jlc->gl.temp_vo, rc, 0, JL_IMGI_ICON, 10, 255);
	jl_gr_draw_vo(jlc, _jlc->gl.temp_vo, &tr);
}

void _jl_fl_kill(jvct_t * _jlc) {
	if(_jlc->has.fileviewer) {
		jl_io_print(_jlc->jlc, "killing fl....");
		cl_list_destroy(_jlc->fl.filelist);
		jl_io_print(_jlc->jlc, "killed fl!");
	}
}

void _jl_fl_initb(jvct_t * _jlc) {
	jl_rect_t rc1 = { .8, 0., .1, .1 };
	jl_rect_t rc2 = { .9, 0., .1, .1 };

	//Create the variables
	_jlc->fl.filelist = cl_list_create();
	_jlc->fl.inloop = 0;
	_jlc->fl.btns[0] = jl_gr_sp_new(_jlc->jlc, rc1,
		_jl_fl_btn_makefile_draw, _jl_fl_btn_makefile_loop, 0);
	_jlc->fl.btns[1] = jl_gr_sp_new(_jlc->jlc, rc2,
		_jl_fl_btn_makefolder_draw, _jl_fl_btn_makefolder_loop, 0);
	_jlc->has.fileviewer = 1;
}

void _jl_fl_inita(jvct_t * _jlc) {
	jl_io_offset(_jlc->jlc, JL_IO_INTENSE, "flIa");
	// Find out the native file separator.
	_jlc->fl.separator = jl_me_strt_mkfrom_str("/");
	// Get ( and if need be, make ) the directory for everything.
	jl_io_print(_jlc->jlc, "Get/Make directory for everything....");
	jl_fl_get_root__(_jlc);
	jl_io_print(_jlc->jlc, "Complete!");
	// Get ( and if need be, make ) the error file.
	jl_io_print(_jlc->jlc, "Get/Make directory error logfile....");
	jl_fl_get_errf__(_jlc);
	jl_io_print(_jlc->jlc, "Complete!");
	//
	jl_io_tag(_jlc->jlc, JL_IO_SIMPLE);
	_jlc->has.filesys = 1;
	jl_io_print(_jlc->jlc, "program name:");
	jl_dl_progname(_jlc->jlc, Strt("JL_Lib"));

	str_t pkfl = jl_fl_get_resloc(_jlc->jlc, JL_MAIN_DIR, JL_MAIN_MEF);
	remove(pkfl);

	truncate(_jlc->fl.paths.errf, 0);
	jl_io_print(_jlc->jlc, "Starting....");
	jl_io_print(_jlc->jlc, "finished file init");
	jl_io_close_block(_jlc->jlc);
}
