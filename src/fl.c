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
static void _jl_fl_save(jl_t* jlc, const void *file_data, const char *file_name,
	uint32_t bytes)
{
	jvct_t * _jlc = jlc->_jlc;
	int errsv;
	ssize_t n_bytes;
	int fd;
	char * bytecount;
	char * atplace;
	
	if(file_name == NULL) {
		_jl_fl_errf(_jlc, "Save[==]: file_name is Null\n");
		jl_sg_kill(jlc);
		return;
	}else if(strlen(file_name) == 0) {
		_jl_fl_errf(_jlc, "Save[strlen]: file_name is Empty String\n");
		jl_sg_kill(jlc);
		return;
	}else if(!file_data) {
		_jl_fl_errf(_jlc, "Save[file_data]: file_data is NULL\n");
		jl_sg_kill(jlc);
		return;	
	}

	jl_io_offset(jlc, JL_IO_SIMPLE, "FLSV"); // { : FLSV
	jl_io_printc(jlc, "SAVING....");
	jl_io_offset(jlc, JL_IO_INTENSE, "FLSV"); // =
	bytecount = jl_me_string_fnum(jlc, bytes);
	jl_io_printc(jlc, bytecount);
	free(bytecount);

	uint8_t offs = (file_name[0] == '!');
	fd = open(file_name + offs, O_RDWR | O_CREAT, JL_FL_PERMISSIONS);

	if(fd <= 0) {
		errsv = errno;

		jl_io_offset(jlc, JL_IO_MINIMAL, "FLSV"); // =
		_jl_fl_errf(_jlc, "Save[open]: Failed to open file: \"");
		_jl_fl_errf(_jlc, file_name);
		_jl_fl_errf(_jlc, "\" Write failed: ");
		_jl_fl_errf(_jlc, strerror(errsv));
		_jl_fl_errf(_jlc, "\n");
		jl_io_close_block(jlc); // !}
		jl_sg_kill(jlc);
	}
	int at = lseek(fd, 0, SEEK_END);
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLSV");
	jl_io_printc(jlc, "Writing....");
	n_bytes = write(fd, file_data, bytes);
	if(n_bytes <= 0) {
		errsv = errno;
		close(fd);
		_jl_fl_errf(_jlc, ":Save[write]: Write to \"");
		_jl_fl_errf(_jlc, file_name);
		_jl_fl_errf(_jlc, "\" failed:\n:\"");
		_jl_fl_errf(_jlc, strerror(errsv));
		_jl_fl_errf(_jlc, "\"\n");
		jl_io_close_block(jlc); // !}
		jl_sg_kill(jlc);
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
	jl_io_close_block(jlc); // } : Close Block "FLSV"
	return;
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

	jl_io_printc(_jlc->jlc, "Get external storage directory.\n");
	root_path = jl_me_strt_mkfrom_str(JL_FL_BASE);
	jl_io_printc(_jlc->jlc, "Append JL_ROOT_DIR.\n");
	root_dir = jl_me_strt_mkfrom_str(JL_ROOT_DIR);
	jl_io_printc(_jlc->jlc, "Merging root_path and root_dir.\n");
	jl_me_strt_merg(_jlc->jlc, root_path, root_dir);
	jl_io_printc(_jlc->jlc, "Free root_dir.\n");
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
		_jl_fl_errf(_jlc, "This platform has no pref path!\n");
		jl_sg_kill(_jlc->jlc);
	}
#endif
	// Make "-- JL_ROOT_DIR"
	if(jl_fl_mkdir(_jlc->jlc, (str_t) root_path->data) == 2) {
		jl_io_printc(_jlc->jlc, (str_t) root_path->data);
		jl_io_printc(_jlc->jlc, ": mkdir : Permission Denied\n");
		jl_sg_kill(_jlc->jlc);
	}
	// Set paths.root & free root_path
	_jlc->fl.paths.root = jl_me_string_fstrt(_jlc->jlc, root_path);
	jl_io_printc(_jlc->jlc, "Root Path=\"");
	jl_io_printc(_jlc->jlc, _jlc->fl.paths.root);
	jl_io_printc(_jlc->jlc, "\"\n");
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
void _jl_fl_errf(jvct_t * _jlc, const char *msg) {
	if(!_jlc->has.filesys) {
		printf("JL-Lib: \"%s\"\n", msg);
		return;
	}
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "ERRF");
	jl_io_printc(_jlc->jlc, "saving to errf: ");
	jl_io_printc(_jlc->jlc, _jlc->fl.paths.errf);
	jl_io_printc(_jlc->jlc, "\n");
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "ERRF");
	jl_io_printc(_jlc->jlc, msg);
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "ERRF");
	// Write to the errf file
	_jl_fl_save(_jlc->jlc, msg, _jlc->fl.paths.errf, strlen(msg));
	jl_io_printc(_jlc->jlc, "saved to errf!\n");
	
	jl_io_close_block(_jlc->jlc); //Close Block "ERRF"
}

/** @endcond **/

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
			_jl_fl_errf(jlc->_jlc, "jl_fl_exist: Out of Memory!\n");
			jl_sg_kill(jlc);
		}else{ //Unknown error
			_jl_fl_errf(jlc->_jlc, "jl_fl_exist: Unknown Error!\n");
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
	truncate(name, 0);
	_jl_fl_save(jlc, file, name, bytes);
}

/**
 * Load a File from the file system.  Returns bytes loaded from "file_name"
 * @param jlc: Library Context
 * @param file_name: file to load
 * @returns A readable "strt" containing the bytes from the file.
 */
strt jl_fl_load(jl_t* jlc, str_t file_name) {
	jl_fl_reset_cursor__(file_name);
	m_u32_t i;
	unsigned char *file = malloc(MAXFILELEN);
	uint8_t offs = (file_name[0] == '!');
	int fd = open(file_name + offs, O_RDWR);
	
	//Open Block FLLD
	jl_io_offset(jlc, JL_IO_SIMPLE, "FLLD");
	
	if(fd <= 0) {
		jl_io_printc(jlc, "failed to open: \"");
		jl_io_printc(jlc, file_name);
		jl_io_printc(jlc, "\"\n");
		jl_io_printc(jlc, "file_file_load: Failed to open file\n");
		jl_sg_kill(jlc);
	}
	int Read = read(fd, file, MAXFILELEN + 1);
	jlc->info = Read;

	jl_io_printc(jlc, "jl_fl_load(): read ");
	jl_io_printi(jlc, jlc->info);
	jl_io_printc(jlc, " bytes\n");
	close(fd);
	strt frtn = jl_me_strt_make(jlc->info);
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
char jl_fl_pk_save(jl_t* jlc, str_t packageFileName, str_t fileName,
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
		_jl_fl_errf(jlc->_jlc, "[JL_FL_PK_SAVE] src null error[replace]: ");
		_jl_fl_errf(jlc->_jlc, (char *)zip_strerror(archive));
		_jl_fl_errf(jlc->_jlc, "\n");
		jl_sg_kill(jlc);
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
	if(zerror == ZIP_ER_NOENT) {
		jl_io_printc(jlc, " NO EXIST!");
		jlc->errf = JL_ERR_FIND;
		_jl_fl_pk_load_quit(jlc);
		return NULL;
	}
	jl_io_printc(jlc, "error check 3.\n");
	if(zipfile == NULL) {
		_jl_fl_errf(jlc->_jlc, ":couldn't load pckg!\n");
		jl_sg_kill(jlc);
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
		_jl_fl_errf(jlc->_jlc, "file reading failed");
		jl_sg_kill(jlc);
	}
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
			jl_io_printc(jlc, strerror(errsv));
			_jl_fl_errf(jlc->_jlc, ":Directory: Uh oh...:\n:");
			_jl_fl_errf(jlc->_jlc, strerror(errsv));
			_jl_fl_errf(jlc->_jlc, "\n");
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
uint8_t * jl_fl_mkfile(jl_t* jlc, str_t pzipfile, str_t pfilebase,
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
		_jl_fl_errf(jlc->_jlc, ":Failed To Create file\n");
		jl_sg_kill(jlc);
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
uint8_t *jl_fl_media(jl_t* jlc, str_t Fname, str_t pzipfile,
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
	
	jl_io_printc(jlc, "Getting Resource Location....\n");
	// Append 'prg_folder' onto 'resloc'
	jl_me_strt_merg(jlc, resloc, pfstrt);
	// Append 'filesr' onto 'resloc'
	jl_me_strt_merg(jlc, resloc, filesr);
	// Make 'prg_folder' if it doesn't already exist.
	if( jl_fl_mkdir(jlc, (str_t) resloc->data) == 2 ) {
		jl_io_printc(jlc, "jl_fl_get_resloc: couldn't make \"");
		jl_io_printc(jlc, (str_t) resloc->data);
		jl_io_printc(jlc, "\"\n");
		jl_io_printc(jlc, "mkdir : Permission Denied\n");
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
	jl_io_printc(jlc, "finished resloc w/ \""); 
	jl_io_printc(jlc, rtn);
	jl_io_printc(jlc, "\"\n");
	return rtn;
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
		dirname = SDL_GetPrefPath("JL_Lib", "\0");
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
			_jl_fl_errf(_jlc, "FileViewer Can't Open Directory:"
				"Not Enough Memory!\n");
			jl_sg_kill(jlc);
		}else{ //Unknown error
			_jl_fl_errf(_jlc, "FileViewer Can't Open Directory:"
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

	jl_gr_fill_image_set(jlc, 0, 1, 1, 255);
	jl_gr_fill_image_draw(jlc);
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
		jl_gr_draw_text(jlc, ">", .02, .08 + (.04 * _jlc->fl.cursor),
			.04,255);
		jl_gr_draw_text(jlc, _jlc->fl.dirname, .02, jl_gl_ar(jlc) - .02, .02, 255);
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

	jl_gr_vos_image(jlc, _jlc->gl.temp_vo, rc, 0, 1, 9, 255);
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

	jl_gr_vos_image(jlc, _jlc->gl.temp_vo, rc, 0, 1, 10, 255);
	jl_gr_draw_vo(jlc, _jlc->gl.temp_vo, &tr);
}

void _jl_fl_kill(jvct_t * _jlc) {
	if(_jlc->has.fileviewer) {
		jl_io_printc(_jlc->jlc, "killing fl...\n");
		cl_list_destroy(_jlc->fl.filelist);
		jl_io_printc(_jlc->jlc, "killed fl!\n");
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
	// Get ( and if need be, make ) the directory for everything.
	jl_io_printc(_jlc->jlc, "Get/Make directory for everything...\n");
	jl_fl_get_root__(_jlc);
	jl_io_printc(_jlc->jlc, "Complete!\n");
	// Get ( and if need be, make ) the error file.
	jl_io_printc(_jlc->jlc, "Get/Make directory error logfile...\n");
	jl_fl_get_errf__(_jlc);
	jl_io_printc(_jlc->jlc, "Complete!\n");
	//
	jl_io_tag(_jlc->jlc, JL_IO_SIMPLE);
	_jlc->has.filesys = 1;
	jl_io_printc(_jlc->jlc, "program name:");
	jl_dl_progname(_jlc->jlc, Strt("JL_Lib"));

	str_t pkfl = jl_fl_get_resloc(_jlc->jlc, JL_MAIN_DIR, JL_MAIN_MEF);
	remove(pkfl);

	truncate(_jlc->fl.paths.errf, 0);
	_jl_fl_errf(_jlc, ":Starting...\n");
	jl_io_printc(_jlc->jlc, "finished file init\n");
	jl_io_close_block(_jlc->jlc);
}
