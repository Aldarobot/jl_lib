#include "header/jl_pr.h"

#define PKFMAX 10000000
#define FAKE "jlvm.zip"

int32_t JL_bytesRead;
strt gvar_pkfl;
char *errf;

void jl_fl_save(jl_t* pusr, void *file, char *name, uint32_t bytes) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("SAVING....\n");
	#endif
	jvct_t * pjlc = pusr->pjlc;
	#if JLVM_DEBUG >= JLVM_DEBUG_INTENSE
	jl_io_print_lowc(pusr, "doing....");
	jl_io_print_lowc(pusr, jl_me_strt_fnum(bytes));
	#endif
	int errsv;
	ssize_t n_bytes;
	int fd = open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	truncate(name, 0);
	if(fd <= 0) {
		errsv = errno;

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
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	int at = 
	#endif
	lseek(fd, 0, SEEK_END);
	n_bytes = write(fd, file, bytes);
	if(n_bytes <= 0) {
		errsv = errno;
		close(fd);
		jlvm_dies(pjlc,
			jl_me_strt_merg(Strt("Write failed: "),Strt(strerror(errsv)),STRT_TEMP));
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(0,"JLVM",
		jl_me_strt_merg(
			jl_me_strt_merg("Wrote ", jl_me_strt_fnum((int)n_bytes)),
			jl_me_strt_merg(" bytes @ ", jl_me_strt_fnum(at))
		)
	);
	#endif
	close(fd);
	return;
}

void _jl_fl_errf(jvct_t * pjlc, char *msg) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM] saving to errf: %s: %s\n", errf, msg);
	#endif
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
	JL_bytesRead = Read;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/FILE/LOAD] read %d bytes\n", JL_bytesRead);
	#endif
	close(fd);
	strt frtn = jl_me_strt_make(JL_bytesRead, STRT_KEEP);
	for( i = 0; i < JL_bytesRead; i++) {
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
	if(zip_add(archive, fileName, s)) {
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("[JLVM/FILE/PKDJ/SAVE] add/err: %s\n", zip_strerror(archive));
		printf("[JLVM/FILE/PKDJ/SAVE] maybe exist: trying replacing..\n");
		printf("[JLVM/FILE/PKDJ/SAVE] getting index....\n");
		#endif
		struct zip_stat sb;
		if(zip_stat(archive, fileName, 0, &sb)!=0) {

			jlvm_dies(pusr->pjlc, jl_me_strt_merg(
				Strt("[JLVM/FILE/PKDJ/SAVE] !!index getting fail\n"),
				jl_me_strt_merg(
					Strt("[JLVM/FILE/PKDJ/SAVE] index f: "),
					Strt(zip_strerror(archive)), STRT_TEMP
				), STRT_TEMP
			));
		}
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("[JLVM/FILE/PKDJ/SAVE] got index.\n");
		#endif

		if(zip_replace(archive, sb.index, s))
		{
			zip_source_free(s);
			#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
			printf("[JLVM/FILE/PKDJ/SAVE] repl/error replacing file: %s\n", zip_strerror(archive));
			#endif
		}
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
	jl_io_print_lowc(pusr, jl_me_strt_merg(
		jl_me_strt_merg("loading package:\"",packageFileName), "\"..."));
	#endif
	struct zip *zipfile = zip_open(packageFileName, ZIP_CHECKCONS, &zerror);
	if(zerror == ZIP_ER_OPEN) {
		jl_io_print_lowc(pusr, " NO EXIST!");
		pusr->errf = JL_ERRF_FIND;
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
		pusr->errf = JL_ERRF_NONE;
		return NULL;
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("[JLVM/FILE/LOAD] opened file in package\n");
		#endif
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/LOAD] reading opened file....\n");
	#endif
	if((JL_bytesRead = zip_fread(file, fileToLoad, PKFMAX)) == -1) {
		jlvm_dies(pusr->pjlc, Strt("file reading failed"));
	}
	char * readbytes = malloc(30);
	sprintf(readbytes, "read %d bytes", JL_bytesRead);
	jl_io_print_lowc(pusr, readbytes);
	free(readbytes);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/LOAD] read opened file.\n");
	#endif
	zip_close(zipfile);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/LOAD] done.\n");
	#endif
	pusr->errf = JL_ERRF_NERR;
	jl_io_offset(pusr, "JLVM");
	return fileToLoad;
}

uint8_t *jl_fl_pk_mnld(jl_t* pusr, char *Fname) {
	uint8_t *freturn;
	if(
		((freturn = jl_fl_pk_load(pusr,(void *)gvar_pkfl->data,Fname)) == NULL) &&
		pusr->errf == JL_ERRF_FIND ) //Package doesn't exist!! - create
	{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pusr, "Creating Package...\n");
		#endif
		jl_fl_save(pusr, jal5_head_jlvm(), (void *)gvar_pkfl->data,
			jal5_head_size());
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pusr, "Attempt Complete!\n");
		#endif
		SDL_Delay(1000); //give file system time to update
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pusr, "Try loading again....\n");
		#endif
		if(
			((freturn = jl_fl_pk_load(pusr,(void *)gvar_pkfl->data,Fname))
				== NULL) &&
			pusr->errf == JL_ERRF_FIND )//Package still doesn't exist!!
		{
			jlvm_dies(pusr->pjlc, Strt("Failed To Create jlvm.zip"));
		}
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pusr, "Good loading!\n");
		#endif
	}
	return freturn;
}

/*
 * Create a folder (directory)
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

	strt pvar_pkfl = jl_me_strt_merg(Strt(filebase), Strt(FAKE), STRT_KEEP);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pusr, "filebase: ");
	jl_io_print_lowc(pusr, filebase);
	jl_io_print_lowc(pusr, pvar_pkfl);

	jl_io_offset(pusr, "INIT");
	jl_io_offset(pusr, "PKFL");

	jl_io_print_lowc(pusr, pvar_pkfl);
	#endif
	return pvar_pkfl;
}

void jl_fl_user_select_init(jl_t* pusr, char *program_name) {
	DIR *dir;
	struct dirent *ent;
	jvct_t * pjlc = pusr->pjlc;
	
	pjlc->fl.filelist = cl_list_create();
	if ((dir = opendir (SDL_GetPrefPath("JLVM",program_name))) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			char *element = malloc(strlen(ent->d_name));
			memcpy(element, ent->d_name, strlen(ent->d_name));
			cl_list_add(pjlc->fl.filelist, element);
		}
		closedir (dir);
	} else {
		//Couldn't open Directory
	}
	cl_list_destroy(pjlc->fl.filelist);
}

void jl_fl_user_select_loop(jl_t* pusr) {
}

void _jal5_file_init(jvct_t * pjlc) {

	jl_io_offset(pjlc->sg.usrd, "FILE");
	jl_io_offset(pjlc->sg.usrd, "INIT");

	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "program name:");
	#endif
	lsdl_prog_name(Strt("JLVM"));

	gvar_pkfl = jl_fl_get_resloc(pjlc->sg.usrd, Strt("JLVM"), Strt(FAKE));
	remove((void*)gvar_pkfl->data);

	_jl_fl_errf(pjlc, "Segmentation Fault / Floatation Exception etc.");
	jl_io_offset(pjlc->sg.usrd, "JLVM");
}
