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
static void _jl_fl_save(jl_t* pusr, void *file, const char *name, uint32_t bytes) {
	jvct_t * pjlc = pusr->pjlc;
	int errsv;
	ssize_t n_bytes;
	int fd;
	char * bytecount;
	char * atplace;
	
	if(name == NULL) {
		jl_sg_kill(pusr, "Save: Name is Null");
		return;
	}else if(strlen(name) == 0) {
		jl_sg_kill(pusr, "Save: Name is Empty String");
		return;
	}

	jl_io_offset(pusr, JL_IO_SIMPLE, "FLSV");
	jl_io_printc(pusr, "SAVING....");
	jl_io_offset(pusr, JL_IO_INTENSE, "FLSV");
	bytecount = jl_me_string_fnum(pusr, bytes);
	jl_io_printc(pusr, bytecount);
	free(bytecount);

	uint8_t offs = (name[0] == '!');
	fd = open(name + offs, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

	if(fd <= 0) {
		errsv = errno;

		jl_io_offset(pusr, JL_IO_MINIMAL, "FLSV");
		jl_io_printc(pusr, "jl_fl_save: Failed to open file: \"");
		jl_io_printc(pusr, name);
		jl_io_printc(pusr, "\" Write failed: ");
		jl_io_printc(pusr, strerror(errsv));
		jl_io_printc(pusr, "\n");
		exit(-1);
	}
	int at = lseek(fd, 0, SEEK_END);
	jl_io_offset(pusr, JL_IO_SIMPLE, "FLSV");
	jl_io_printc(pusr, "Writing....");
	n_bytes = write(fd, file, bytes);
	if(n_bytes <= 0) {
		errsv = errno;
		close(fd);
		_jl_fl_errf(pjlc, ":Write failed: ");
		_jl_fl_errf(pjlc, strerror(errsv));
		jl_sg_kill(pusr, "\n");
	}
	bytecount = jl_me_string_fnum(pusr, (int)n_bytes);
	atplace = jl_me_string_fnum(pusr, at);
	jl_io_printc(pusr, "Wrote ");
	jl_io_printc(pusr, bytecount);
	jl_io_printc(pusr, "bytes @");
	jl_io_printc(pusr, atplace);
	jl_io_printc(pusr, "\n");
	free(bytecount);
	free(atplace);
/*	jl_io_print_lows(0,
		jl_me_strt_merg(
			jl_me_strt_merg(Strt("Wrote "),
				jl_me_strt_fnum((int)n_bytes), STRT_TEMP),
			jl_me_strt_merg(Strt(" bytes @ "),
				jl_me_strt_fnum(at), STRT_TEMP), STRT_TEMP
		)
	);*/
	jl_io_printc(pusr, "Wrote....");
	close(fd);
	
	jl_io_close_block(pjlc->sg.usrd); //Close Block "FLSV"
	return;
}

static inline void _jl_fl_reset_cursor(char *file_name) {
	int fd = open(file_name, O_RDWR);
	lseek(fd, 0, SEEK_SET);
	close(fd);
}

//NON-STATIC Library Dependent Functions
void _jl_fl_errf(jvct_t * pjlc, char *msg) {
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "ERRF");
	jl_io_printc(pjlc->sg.usrd, "saving to errf: ");
	jl_io_printc(pjlc->sg.usrd, pjlc->fl.errf_filename);
	jl_io_offset(pjlc->sg.usrd, JL_IO_MINIMAL, "ERRF");
	jl_io_printc(pjlc->sg.usrd, msg);
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "ERRF");
	//Write to the errf file
	_jl_fl_save(pjlc->sg.usrd, msg, pjlc->fl.errf_filename, strlen(msg));
	jl_io_printc(pjlc->sg.usrd, "saved to errf!\n");
	
	jl_io_close_block(pjlc->sg.usrd); //Close Block "ERRF"
}

/** @endcond **/

/**
 * Save A File To The File System.  Save Data of "bytes" bytes in "file" to
 * file "name"
 * @param pusr: Library Context
 * @param file: Data To Save To File
 * @param name: The Name Of The File to save to
 * @param bytes: Size of "File"
 */
void jl_fl_save(jl_t* pusr, void *file, const char *name, uint32_t bytes) {
	truncate(name, 0);
	_jl_fl_save(pusr, file, name, bytes);
}

/**
 * Load a File from the file system.  Returns bytes loaded from "file_name"
 * @param pusr: Library Context
 * @param file_name: file to load
 * @returns A readable "strt" containing the bytes from the file.
 */
strt jl_fl_load(jl_t* pusr, char *file_name) {
	_jl_fl_reset_cursor(file_name);
	u32t i;
	unsigned char *file = malloc(MAXFILELEN);
	uint8_t offs = (file_name[0] == '!');
	int fd = open(file_name + offs, O_RDWR);
	
	//Open Block FLLD
	jl_io_offset(pusr, JL_IO_SIMPLE, "FLLD");
	
	if(fd <= 0) {
		jl_io_printc(pusr, "failed to open: \"");
		jl_io_printc(pusr, file_name);
		jl_io_printc(pusr, "\"\n");
		jl_sg_kill(pusr, "file_file_load: Failed to open file");
	}
	int Read = read(fd, file, MAXFILELEN + 1);
	pusr->info = Read;

	jl_io_printc(pusr, "read ");
	jl_io_printi(pusr, pusr->info);
	jl_io_printc(pusr, "bytes\n");
	close(fd);
	strt frtn = jl_me_strt_make(pusr->info, STRT_KEEP);
	for( i = 0; i < pusr->info; i++) {
		frtn->data[i] = file[i];
	}
	frtn->data = file;
	
	jl_io_close_block(pusr); //Close Block "FLLD"
	return frtn;
}

/**
 * Save file "filename" with contents "data" of size "dataSize" to package
 * "packageFileName"
 * @param pusr: Library Context
 * @param packageFileName: Name of package to Save to
 * @param fileName: the file to Save to within the package.
 * @param data: the data to save to the file
 * @param dataSize: the # of bytes to save from the data to the file.
 * @returns 0: On success
 * @returns 1: If File is unable to be made.
 */
char jl_fl_pk_save(jl_t* pusr, char *packageFileName, char *fileName,
	void *data, uint64_t dataSize)
{
	jl_io_offset(pusr, JL_IO_SIMPLE, "PKSV"); // {
	jl_io_printc(pusr, "opening \"");
	jl_io_printc(pusr, packageFileName);
	jl_io_printc(pusr, "\"....\n");
	struct zip *archive = zip_open(packageFileName, ZIP_CREATE 
		/*| ZIP_CHECKCONS*/, NULL);
	if(archive == NULL) {
		jl_io_close_block(pusr); // !}
		return 1;
	}else{
		jl_io_printc(pusr, "opened file system:\"");
		jl_io_printc(pusr, packageFileName);
		jl_io_printc(pusr, "\".\n");
	}

	struct zip_source *s;
	if ((s=zip_source_buffer(archive, (void *)data, dataSize, 0)) == NULL) {
		zip_source_free(s);
		_jl_fl_errf(pusr->pjlc, "[JLVM/FILE/PKDJ/SAVE] src null error[replace]: ");
		_jl_fl_errf(pusr->pjlc, (char *)zip_strerror(archive));
		jl_sg_kill(pusr, "\n");
	}
//	printf("%d,%d,%d\n",archive,sb.index,s);
	if(zip_file_add(archive, fileName, s, ZIP_FL_OVERWRITE)) {
		jl_io_printc(pusr, "add/err: \"");
		jl_io_printc(pusr, zip_strerror(archive));
		jl_io_printc(pusr, "\"\n");
	}else{
		jl_io_printc(pusr, "added \"");
		jl_io_printc(pusr, fileName);
		jl_io_printc(pusr, "\" to file sys.\n");
	}
	zip_close(archive);
	jl_io_printc(pusr, "DONE!\n");
	jl_io_close_block(pusr); // }
	return 0;
}

static void _jl_fl_pk_load_quit(jl_t* pusr) {
	jl_io_close_block(pusr); //Close Block "PKLD"
}

/**
 * Load file "filename" in package "packageFileName" & Return contents
 * -ERRF:
 *	-ERRF_NONE:	can't find filename in packageFileName
 *	-ERRF_FIND:	can't find packageFileName
 * @param pusr: Library Context
 * @param packageFileName: Package to load file from
 * @param filename: file within package to load
 * @returns: contents of file ( "filename" ) in package ( "packageFileName" )
*/
uint8_t *jl_fl_pk_load(jl_t* pusr, const char *packageFileName,
	const char *filename)
{
	//Open Block PKLD
	jl_io_offset(pusr, JL_IO_SIMPLE, "PKLD");
	int zerror = 0;
	jl_io_printc(pusr, "loading package:\"");
	jl_io_printc(pusr, packageFileName);
	jl_io_printc(pusr, "\"...\n");
	jl_io_printc(pusr, "error check 1.\n");
	struct zip *zipfile = zip_open(packageFileName, ZIP_CHECKCONS, &zerror);
	jl_io_printc(pusr, "error check 2.\n");
	if(zerror == ZIP_ER_OPEN) {
		jl_io_printc(pusr, " NO EXIST!");
		pusr->errf = JL_ERR_FIND;
		_jl_fl_pk_load_quit(pusr);
		return NULL;
	}
	jl_io_printc(pusr, "error check 3.\n");
	if(zipfile == NULL) {
		jl_sg_kill(pusr, ":couldn't load pckg!\n");
	}
	jl_io_printc(pusr, "error check 4.\n");
	jl_io_printc(pusr, (char *)zip_strerror(zipfile));
	jl_io_printc(pusr, "loaded package.\n");
	unsigned char *fileToLoad = malloc(PKFMAX);
	jl_io_printc(pusr, "opening file in package....\n");
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	jl_io_printc(pusr, "call pass.");
	if(file == NULL) {
		jl_io_printc(pusr, "couldn't open up file: \"");
		jl_io_printc(pusr, filename);
		jl_io_printc(pusr, "\" in ");
		jl_io_printc(pusr, packageFileName);
		jl_io_printc(pusr, "because: ");
		jl_io_printc(pusr, (void *)zip_strerror(zipfile));
		jl_io_printc(pusr, "\n");
		pusr->errf = JL_ERR_NONE;
		_jl_fl_pk_load_quit(pusr);
		return NULL;
	}
	jl_io_printc(pusr, "opened file in package\n");
	jl_io_printc(pusr, "reading opened file....\n");
	if((pusr->info = zip_fread(file, fileToLoad, PKFMAX)) == -1) {
		jl_sg_kill(pusr, "file reading failed");
	}
	jl_io_printc(pusr, "read ");
	jl_io_printi(pusr, pusr->info);
	jl_io_printc(pusr, "bytes\n");
	jl_io_printc(pusr, "read opened file.\n");
	zip_close(zipfile);
	jl_io_printc(pusr, "done.\n");
	pusr->errf = JL_ERR_NERR;
	_jl_fl_pk_load_quit(pusr);
	return fileToLoad;
}

/**
 * Create a folder (directory)
 * @param pusr: library context
 * @param pfilebase: name of directory to create
*/
void jl_fl_mkdir(jl_t* pusr, strt pfilebase) {
	jl_io_offset(pusr, JL_IO_SIMPLE, "MDIR"); // {
	if(mkdir((void *)pfilebase->data, 0)) {
		int errsv = errno;
		if(errsv == EEXIST) {
			jl_io_printc(pusr, "Directory Exist! Continue...\n");
		}else{
			_jl_fl_errf(pusr->pjlc, ":Directory: Uh oh...:\n:");
			_jl_fl_errf(pusr->pjlc, strerror(errsv));
			jl_sg_kill(pusr, "\n");
		}
	}else{
		jl_io_printc(pusr, "Created Directory!\n");
	}
	jl_io_close_block(pusr); // } : MDIR
}

/**
 * Create the media package file & load a file from it.
 * @param pusr: library context
 * @param pfilebase: name of file to load from package
 * @param pzipfile: the name of the zipfile to create.
 * @param contents: the contents to put in the file
 * @param size: the size (in bytes) of the contents.
 * @return x: the data contents of the file.
*/
uint8_t * jl_fl_mkfile(jl_t* pusr, char *pzipfile, char *pfilebase,
	char *contents, uint32_t size)
{

//	if(!pfilebase) { return; }
	uint8_t *freturn;

	//Create Block "MKFL"
	jl_io_offset(pusr, JL_IO_SIMPLE, "MKFL"); // {

	jl_io_printc(pusr, "Creating File...\n");
	jl_fl_save(pusr, contents, pzipfile, size);
	jl_io_printc(pusr, "Attempt Complete!\n");
	SDL_Delay(1000); //give file system time to update
	jl_io_printc(pusr, "Try loading....\n");
	if(
		((freturn = jl_fl_pk_load(pusr, pzipfile, pfilebase))== NULL) &&
		(pusr->errf == JL_ERR_FIND) )//Package still doesn't exist!!
	{
		jl_sg_kill(pusr, ":Failed To Create file\n");
	}
	jl_io_printc(pusr, "Good loading!\n");
	//Close Block "MKFL"
	jl_io_close_block(pusr); // }
	jl_io_printc(pusr, "File Made!\n");
	return freturn;
}

/**
 * Load media package, create it if it doesn't exist.
 * @param pusr: Library Context
 * @param Fname: File in Media Package to load.
 * @param pzipfile: Where to make the package.
 * @param pdata: Media Package Data to save if it doesn't exist.
 * @param psize: Size of "pdata" 
*/
uint8_t *jl_fl_media(jl_t* pusr, char *Fname, char *pzipfile,
	void *pdata, uint64_t psize)
{
	uint8_t *freturn = jl_fl_pk_load(pusr, pzipfile, Fname);
	jl_io_printc(pusr, "JL_FL_MEDIA Returning\n");
	//If Package doesn't exist!! - create
	if( (freturn == NULL) && (pusr->errf == JL_ERR_FIND) )
		return jl_fl_mkfile(pusr, pzipfile, Fname, pdata, psize);
	else
		return freturn;
}

/**
 * Get the designated location for a resource pack. Resloc = Resource Location
 * @param pusr: Library Context.
 * @param pprg_name: Program Name
 * @param pfilename: Name Of Resource Pack
 * @returns: The designated location for a resouce pack
*/
char * jl_fl_get_resloc(jl_t* pusr, char* pprg_name, char* pfilename) {
	jvct_t * pjlc = pusr->pjlc;
	int i;
	char *filebase;
	char *location;
	strt filebases = jl_me_strt_make(0, STRT_KEEP);
	strt errfs = jl_me_strt_make(0, STRT_KEEP);
	
	//Open Block "FLBS"
	jl_io_offset(pusr, JL_IO_SIMPLE, "FLBS");
	
	jl_io_printc(pusr, "Getting FileBase....\n");
	
	#if JL_PLAT == JL_PLAT_PHONE
	jl_me_strt_merg(pusr, filebases, Strt(JLVM_FILEBASE));
	jl_me_strt_merg(pusr, filebases, Strt(pprg_name));
	jl_me_strt_merg(pusr, filebases, Strt("/"));
	filebase = (void *)filebases->data;
	//Setting errf
	jl_me_strt_merg(pusr, errfs, Strt(JLVM_FILEBASE));
	jl_me_strt_merg(pusr, errfs, Strt("errf.txt"));

	#elif JL_PLAT == JL_PLAT_COMPUTER
	filebase = SDL_GetPrefPath("JLVM", pprg_name);
	if(filebase == NULL) {
		jl_me_strt_merg(pusr, filebases, Strt("JLVM/"));
		jl_me_strt_merg(pusr, filebases, Strt(pprg_name));
		filebase = (void *)filebases->data;
	}else{
		filebases = Strt(filebase);
	}
	jl_io_printc(pusr, "Setting Errf....\n");
	jl_me_strt_merg(pusr, errfs, filebases);
	jl_me_strt_trunc(pusr, errfs, filebases->size - 5);
	jl_me_strt_merg(pusr, errfs, Strt("errf.txt"));
	jl_io_printc(pusr, "Set Errf....\n");
	#else //OTHER
	#endif
	
	pjlc->fl.errf_filename = (void *)errfs->data;
	jl_io_printc(pusr, "errf_filename:");
	jl_io_printc(pusr, pjlc->fl.errf_filename);
	jl_io_printc(pusr, "\n");

	jl_io_printc(pusr, filebase);
	
	//filebase maker
	#if JL_PLAT == JL_PLAT_PHONE
	jl_fl_mkdir(pusr, Strt(JLVM_FILEBASE));
	#else
	jl_io_printc(pusr, "FB2....\n");
	int fsize = strlen(filebase)+strlen(JLVM_FILEBASE) + 1;
	char *filebase2 = malloc(fsize);
	jl_io_printc(pusr, "Clear....\n");
	for(i = 0; i < fsize; i++) {
		filebase2[i] = '\0';
	}
	jl_io_printc(pusr, "Add....\n");
	strncat(filebase2,filebase,strlen(filebase));
	strncat(filebase2,JLVM_FILEBASE,strlen(JLVM_FILEBASE));
	jl_io_printc(pusr, "Make....\n");
	jl_fl_mkdir(pusr, Strt(filebase2));
	#endif
	jl_fl_mkdir(pusr, Strt(filebase));

	jl_io_printc(pusr, "Merge....\n");
	strt pvar_pkfl = jl_me_strt_make(0, STRT_KEEP);
	jl_me_strt_merg(pusr, pvar_pkfl, Strt(filebase));
	jl_me_strt_merg(pusr, pvar_pkfl, Strt(pfilename));

	location = jl_me_string_fstrt(pusr, pvar_pkfl);

	jl_io_printc(pusr, "filebase: ");
	jl_io_printc(pusr, filebase);
	jl_io_printc(pusr, location);
	
	jl_io_close_block(pusr); //Close Block "FLBS"
	jl_io_printc(pusr, "finished resloc w/ \"");
	jl_io_printc(pusr, location);
	jl_io_printc(pusr, "\"\n");
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
static uint8_t _jl_fl_user_select_open_dir(jl_t* pusr, char *dirname) {
	DIR *dir;
	struct dirent *ent;
	jvct_t * pjlc = pusr->pjlc;
	uint8_t offset = 0;

	_jl_fl_user_select_check_extradir(dirname);
	if(dirname[1] == '\0') {
		jl_me_alloc(pusr, (void**)&dirname, 0, 0);
		dirname = SDL_GetPrefPath("JLVM", "\0");
		_jl_fl_user_select_check_extradir(dirname);
	}
	offset = dirname[0] == '!';
	pjlc->fl.dirname = dirname;
	pjlc->fl.cursor = 0;
	pjlc->fl.cpage = 0;
	cl_list_clear(pjlc->fl.filelist);
	printf("dirname=%s\n", pjlc->fl.dirname);
	if ((dir = opendir (dirname + offset)) != NULL) {
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
			pjlc->fl.returnit = 1;
			pjlc->fl.dirname[strlen(dirname)-1] = '\0';
			pjlc->fl.inloop = 0;
			pusr->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else if(errsv == ENOENT) { // Directory Doesn't Exist
			return 0;
		}else if(errsv == EACCES) { // Doesn't have permission
			return 0;
		}else if((errsv == EMFILE) || (errsv == ENFILE) ||
			(errsv == ENOMEM)) //Not enough memory!
		{
			jl_sg_kill(pusr, "FileViewer Can't Open Directory:"
				"Not Enough Memory!");
		}else{ //Unknown error
			jl_sg_kill(pusr, "FileViewer Can't Open Directory:"
				"Unknown Error!");
		}
	}
	return 1;
}

/**
 * Open directory for file viewer.
 * If '!' is put at the beginning of "program_name", then it's treated as a
 *	relative path instead of a program name.
 * @param pusr: The library context
 * @param program_name: program name or '!'+relative path
 * @param newfiledata: any new files created with the fileviewer will
 *	automatically be saved with this data.
 * @param newfilesize: size of "newfiledata"
 * @returns 0: if can't open the directory. ( Doesn't exist, Bad permissions )
 * @returns 1: on success.
**/
uint8_t jl_fl_user_select_init(jl_t* pusr, const char *program_name,
	void *newfiledata, uint64_t newfilesize)
{
	jvct_t * pjlc = pusr->pjlc;
	pusr->loop = JL_SG_WM_DN;
	pjlc->fl.returnit = 0;
	pjlc->fl.inloop = 1;
	pjlc->fl.newfiledata = newfiledata;
	pjlc->fl.newfilesize = newfilesize;
	pjlc->fl.prompt = 0;
	pjlc->fl.promptstring = NULL;
	if(program_name[0] == '!') {
		char *path = jl_me_copy(pusr,program_name,strlen(program_name));
		return _jl_fl_user_select_open_dir(pusr, path);
	}else{
		return _jl_fl_user_select_open_dir(pusr,
			SDL_GetPrefPath("JLVM",program_name));
	}
}

static void _jl_fl_user_select_up(jl_t* pusr) {
	if(pusr->ctrl.h == 1) {
		jvct_t * pjlc = pusr->pjlc;
		if((pjlc->fl.cursor > 0) || pjlc->fl.cpage) pjlc->fl.cursor--;
	}
}

static void _jl_fl_user_select_dn(jl_t* pusr) {
	if(pusr->ctrl.h == 1) {
		jvct_t * pjlc = pusr->pjlc;
		if(pjlc->fl.cursor + (pjlc->fl.cpage * (pjlc->fl.drawupto+1)) <
			cl_list_count(pjlc->fl.filelist) - 1)
		{
			pjlc->fl.cursor++;
		}
	}
}

static void _jl_fl_user_select_rt(jl_t* pusr) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_dn(pusr);
	}
}

static void _jl_fl_user_select_lt(jl_t* pusr) {
	int i;
	for(i = 0; i < 5; i++) {
		_jl_fl_user_select_up(pusr);
	}
}

static char* _jl_fl_full_fname(jl_t* pusr,char *selecteddir,char *selecteditem){
	jvct_t * pjlc = pusr->pjlc;

	char *newdir = malloc(
		strlen(pjlc->fl.dirname) +
		strlen(selecteditem) + 2);
	memcpy(newdir, pjlc->fl.dirname,
		strlen(pjlc->fl.dirname));
	memcpy(newdir + strlen(pjlc->fl.dirname),
		selecteditem,
		strlen(selecteditem));
	newdir[strlen(pjlc->fl.dirname) +
		strlen(selecteditem)] = '/';
	newdir[strlen(pjlc->fl.dirname) +
		strlen(selecteditem) + 1] = '\0';
	return newdir;
}

static void _jl_fl_open_file(jl_t*pusr, char *selecteditem) {
	jvct_t * pjlc = pusr->pjlc;
	char *newdir = _jl_fl_full_fname(pusr, pjlc->fl.dirname, selecteditem);

	free(pjlc->fl.dirname);
	pjlc->fl.dirname = NULL;
	_jl_fl_user_select_open_dir(pusr,newdir);
}

static void _jl_fl_user_select_do(jl_t* pusr) {
	if(pusr->ctrl.h == 1) {
		jvct_t * pjlc = pusr->pjlc;
		struct cl_list_iterator *iterator;
		int i;
		char *stringtoprint;

		iterator = cl_list_iterator_create(pjlc->fl.filelist);
		for(i = 0; i < cl_list_count(pjlc->fl.filelist); i++) {
			stringtoprint = cl_list_iterator_next(iterator);
			if(i ==
				pjlc->fl.cursor + //ON PAGE
				(pjlc->fl.cpage * (pjlc->fl.drawupto+1))) //PAGE
			{
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
			pjlc->fl.inloop = 0;
			pusr->loop = JL_SG_WM_EXIT; //Go into exit loop
		}else{
			_jl_fl_open_file(pusr, pjlc->fl.selecteditem);
		}
	}
}

/**
 * Run the file viewer.
 * @param pusr: Library Context.
**/
void jl_fl_user_select_loop(jl_t* pusr) {
	jvct_t * pjlc = pusr->pjlc;
	struct cl_list_iterator *iterator;
	int i;
	char *stringtoprint;
	pjlc->fl.drawupto = ((int)(20.f * jl_dl_p(pusr))) - 1;

	iterator = cl_list_iterator_create(pjlc->fl.filelist);

	jl_gr_draw_image(pusr, 0, 1, 0., 0., 1., jl_dl_p(pusr), 1, 127);
	jl_gr_draw_text(pusr, "Select File", .02, .02, .04,255);

	jl_ct_run_event(pusr,JL_CT_MAINUP, _jl_fl_user_select_up, jl_dont);
	jl_ct_run_event(pusr,JL_CT_MAINDN, _jl_fl_user_select_dn, jl_dont);
	jl_ct_run_event(pusr,JL_CT_MAINRT, _jl_fl_user_select_rt, jl_dont);
	jl_ct_run_event(pusr,JL_CT_MAINLT, _jl_fl_user_select_lt, jl_dont);
	//Draw files
	for(i = 0; i < cl_list_count(pjlc->fl.filelist); i++) {
		stringtoprint = cl_list_iterator_next(iterator);
		if(strcmp(stringtoprint, "..") == 0) {
			stringtoprint = "//containing folder//";
		}else if(strcmp(stringtoprint, ".") == 0) {
			stringtoprint = "//this folder//";
		}
		if(i - (pjlc->fl.cpage * (pjlc->fl.drawupto+1)) >= 0)
			jl_gr_draw_text(pusr, stringtoprint,
				.06, .08 + (.04 *
				(i - (pjlc->fl.cpage * (pjlc->fl.drawupto+1)))),
				.04,255);
		if(i - (pjlc->fl.cpage * (pjlc->fl.drawupto+1)) >
			pjlc->fl.drawupto - 1)
		{
			break;
		 	cl_list_iterator_destroy(iterator);
	 	}
	}
	if(pjlc->fl.cursor > pjlc->fl.drawupto) {
		pjlc->fl.cursor = 0;
		pjlc->fl.cpage++;
	}
	if(pjlc->fl.cursor < 0) {
		pjlc->fl.cursor = pjlc->fl.drawupto;
		pjlc->fl.cpage--;
	}
	if(pjlc->fl.prompt) {
		if(jl_gr_draw_textbox(pusr, .02, jl_dl_p(pusr) - .06, .94, .02,
			&pjlc->fl.promptstring))
		{
			char *name = _jl_fl_full_fname(pusr,
				pjlc->fl.dirname,
				(char*)pjlc->fl.promptstring->data);
			name[strlen(name) - 1] = '\0';
			jl_fl_save(pusr, pjlc->fl.newfiledata,
				name, pjlc->fl.newfilesize);
			_jl_fl_user_select_open_dir(pusr, pjlc->fl.dirname);
			pjlc->fl.prompt = 0;
		}
	}else{
		jl_gr_draw_text(pusr, ">", .02, .08 + (.04 * pjlc->fl.cursor),
			.04,255);
		jl_gr_draw_text(pusr, pjlc->fl.dirname, .02, jl_dl_p(pusr) - .02, .02, 255);
		jl_ct_run_event(pusr, JL_CT_SELECT, _jl_fl_user_select_do,
			jl_dont);
	}
	pjlc->fl.btns[0]->loop(pusr);
	pjlc->fl.btns[1]->loop(pusr);
}

/**
 * Get the results from the file viewer.
 * @param pusr: Library Context.
 * @returns: If done, name of selected file.  If not done, NULL is returned.
**/
char *jl_fl_user_select_get(jl_t* pusr) {
	jvct_t * pjlc = pusr->pjlc;
	
	if(pjlc->fl.returnit)
		return pjlc->fl.dirname;
	else
		return NULL;
}

static void _jl_fl_btn_makefile_action(jl_t* pusr) {
	jvct_t *pjlc = pusr->pjlc;
	pjlc->fl.prompt = 1;
}

static void _jl_fl_btn_makefile(jl_t* pusr) {
	jvct_t *pjlc = pusr->pjlc;
	if(pjlc->fl.newfiledata == NULL) return;
	jl_gr_draw_glow_button(pusr, pjlc->fl.btns[0], "+ New File",
		_jl_fl_btn_makefile_action);
}

static void _jl_fl_btn_makefolder(jl_t* pusr) {
	jvct_t *pjlc = pusr->pjlc;
	jl_gr_draw_glow_button(pusr, pjlc->fl.btns[1], "+ New Folder",
		_jl_fl_btn_makefile_action);
}

void _jl_fl_kill(jvct_t * pjlc) {
	if(pjlc->has.fileviewer) {
		jl_io_printc(pjlc->sg.usrd, "killing fl...\n");
		cl_list_destroy(pjlc->fl.filelist);
		jl_io_printc(pjlc->sg.usrd, "killed fl!\n");
	}
}

void _jl_fl_initb(jvct_t * pjlc) {
	//Create the variables
	pjlc->fl.filelist = cl_list_create();
	pjlc->fl.inloop = 0;
	pjlc->fl.btns[0] = jl_gr_sprite_make(pjlc->sg.usrd, 0, 1, 1, 255,
		.8, 0., .1, .1, _jl_fl_btn_makefile, 0);
	pjlc->fl.btns[1] = jl_gr_sprite_make(pjlc->sg.usrd, 0, 1, 2, 255,
		.9, 0., .1, .1, _jl_fl_btn_makefolder, 0);
	pjlc->has.fileviewer = 1;
}

void _jl_fl_inita(jvct_t * pjlc) {
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "FILE"); // {
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "INIT"); // {

	jl_io_printc(pjlc->sg.usrd, "program name:");
	jl_dl_progname(pjlc->sg.usrd, Strt("JLVM"));

	char * pkfl =
		jl_fl_get_resloc(pjlc->sg.usrd, "JLLB", "media.zip");
	remove(pkfl);

	truncate(pjlc->fl.errf_filename, 0);
	_jl_fl_errf(pjlc, ":Starting...\n");
	jl_io_close_block(pjlc->sg.usrd); // } Close Block "INIT"
	jl_io_close_block(pjlc->sg.usrd); // } Close Block "FILE"
	jl_io_printc(pjlc->sg.usrd, "finished file init\n");
}
