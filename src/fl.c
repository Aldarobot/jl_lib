#include "header/jlvm_pr.h"

#define PKFMAX 10000000
#define FAKE "jlvm.zip"

int32_t JL_bytesRead;
strt gvar_pkfl;
char *errf;

void file_file_save(sgrp_user_t* pusr, char *file, char *name, uint32_t bytes) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("SAVING....\n");
	#endif
	jvct_t * pjct = pusr->pjct;
	#if JLVM_DEBUG >= JLVM_DEBUG_INTENSE
	jal5_siop_cplo(pusr, "doing....");
	jal5_siop_cplo(pusr, amem_strt_fnum(bytes));
	#endif
	int errsv;
	ssize_t n_bytes;
	int fd = open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	truncate(name, 0);
	if(fd <= 0) {
		errsv = errno;

		siop_prnt_lwst(pusr,
			amem_strt_merg(
				amem_strt_merg(
					Strt("file_file_save: Failed to open file: "),
					Strt(name),STRT_TEMP),
				amem_strt_merg(
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
		jlvm_dies(pjct,
			amem_strt_merg(Strt("Write failed: "),Strt(strerror(errsv)),STRT_TEMP));
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM",
		amem_strt_merg(
			amem_strt_merg("Wrote ", amem_strt_fnum((int)n_bytes)),
			amem_strt_merg(" bytes @ ", amem_strt_fnum(at))
		)
	);
	#endif
	close(fd);
	return;
}

void jal5_file_errf(jvct_t * pjct, char *msg) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM] saving to errf: %s: %s\n", errf, msg);
	#endif
	file_file_save(pjct->Sgrp.usrd, msg,errf,strlen(msg));
}

void JL_ResetCursor(char *file_name) {
	int fd = open(file_name, O_RDWR);
	lseek(fd, 0, SEEK_SET);
	close(fd);
}

strt file_file_load(sgrp_user_t* pusr, char *file_name) {
	JL_ResetCursor(file_name);
	u32t i;
	unsigned char *file = malloc(MAXFILELEN);
	int fd = open(file_name, O_RDWR);
	if(fd <= 0) {
		printf("failed to open %s\n", file_name);
		jlvm_dies(pusr->pjct, Strt("file_file_load: Failed to open file"));
	}
	int Read = read(fd, file, MAXFILELEN + 1);
	JL_bytesRead = Read;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/FILE/LOAD] read %d bytes\n", JL_bytesRead);
	#endif
	close(fd);
	strt frtn = amem_strt_make(JL_bytesRead, STRT_KEEP);
	for( i = 0; i < JL_bytesRead; i++) {
		frtn->data[i] = file[i];
	}
	frtn->data = file;
	return frtn;
}

char file_pkdj_save(sgrp_user_t* pusr, char *packageFileName, char *fileName,
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
		jlvm_dies(pusr->pjct, amem_strt_merg(
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

			jlvm_dies(pusr->pjct, amem_strt_merg(
				Strt("[JLVM/FILE/PKDJ/SAVE] !!index getting fail\n"),
				amem_strt_merg(
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

uint8_t *file_pkdj_load(sgrp_user_t* pusr, char *packageFileName, char *filename)
{
	siop_offs_sett(pusr, "FILE");
	siop_offs_sett(pusr, "LOAD");
	int zerror;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, amem_strt_merg(
		amem_strt_merg("loading package:\"",packageFileName), "\"..."));
	#endif
	struct zip *zipfile = zip_open(packageFileName, ZIP_CHECKCONS, &zerror);
	if(zerror == ZIP_ER_OPEN) {
		jal5_siop_cplo(pusr, " NO EXIST!");
		pusr->errf = ERRF_FIND;
		return NULL;
	}
	if(zipfile == NULL) {
		jlvm_dies(pusr->pjct, Strt("couldn't load pckg!"));
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, (char *)zip_strerror(zipfile));
	jal5_siop_cplo(pusr, "loaded package.\n");
	#endif
	unsigned char *fileToLoad = malloc(PKFMAX);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, "opening file in package....\n");
	#endif
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, "call pass.");
	#endif
	if(file == NULL) {
		siop_prnt_lwst(pusr,
			amem_strt_merg(
			amem_strt_merg(Strt("couldn't open up file: \""),
				Strt(filename), STRT_TEMP),
			amem_strt_merg(Strt("\" in "),
				Strt(packageFileName), STRT_TEMP),STRT_TEMP));
		siop_prnt_lwst(pusr,
			amem_strt_merg(Strt("because: "),
				Strt(zip_strerror(zipfile)), STRT_TEMP));
		pusr->errf = ERRF_NONE;
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
		jlvm_dies(pusr->pjct, Strt("file reading failed"));
	}
	printf("[JLVM/FILE/LOAD]read %d bytes\n", JL_bytesRead);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/LOAD] read opened file.\n");
	#endif
	zip_close(zipfile);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/FILE/LOAD] done.\n");
	#endif
	pusr->errf = ERRF_NERR;
	siop_offs_sett(pusr, "JLVM");
	return fileToLoad;
}

uint8_t *file_pkdj_mnld(sgrp_user_t* pusr, char *Fname) {
	uint8_t *freturn;
	if(
		((freturn = file_pkdj_load(pusr,(void *)gvar_pkfl->data,Fname)) == NULL) &&
		pusr->errf == ERRF_FIND ) //Package doesn't exist!! - create
	{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(pusr, "Creating Package...\n");
		#endif
		file_file_save(pusr, jal5_head_jlvm(), (void *)gvar_pkfl->data,
			jal5_head_size());
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(pusr, "Attempt Complete!\n");
		#endif
		SDL_Delay(1000); //give file system time to update
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(pusr, "Try loading again....\n");
		#endif
		if(
			((freturn = file_pkdj_load(pusr,(void *)gvar_pkfl->data,Fname))
				== NULL) &&
			pusr->errf == ERRF_FIND )//Package still doesn't exist!!
		{
			jlvm_dies(pusr->pjct, Strt("Failed To Create jlvm.zip"));
		}
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(pusr, "Good loading!\n");
		#endif
	}
	return freturn;
}

/*
 * Create a folder (directory)
*/
void file_make_fdir(sgrp_user_t* pusr, strt pfilebase) {
	if(mkdir((void *)pfilebase->data, 0)) {
		int errsv = errno;
		if(errsv == EEXIST) {
			#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
			jal5_siop_cplo(pusr, "Directory Exist! Continue...\n");
			#endif
		}else{
			jlvm_dies(pusr->pjct,
				amem_strt_merg(
					Strt("Directory: Uh oh..."),
					Strt(strerror(errsv)), STRT_TEMP
				)
			);
		}
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(pusr, "Created Directory!\n");
		#endif
	}
}

/*
 * Return the location of the resource pack for program with name
 * "pprg_name"
*/
strt file_reso_loca(sgrp_user_t* pusr, strt pprg_name, strt pfilename) {
	int i;
	
	#if PLATFORM == 1 //PHONE
	char *filebase = (void*)amem_strt_merg(
		Strt(JLVM_FILEBASE),
		amem_strt_merg(pprg_name, Strt("/"), STRT_TEMP),
		STRT_KEEP
	)->data;
	errf = amem_strt_merg(Strt(JLVM_FILEBASE), Strt("errf.txt"), STRT_KEEP)
		->data;
	#elif PLATFORM == 0 //COMPUTER
	char *filebase = SDL_GetPrefPath("JLVM",(char *)pprg_name->data);
	if(filebase == NULL) {
		filebase = (void*) amem_strt_merg(
			Strt("JLVM/"),pprg_name,STRT_KEEP)->data;
	}
	printf("filebase: %s\n", filebase);
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
	
	siop_offs_sett(pusr, "FLBS");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, filebase);
	#endif
	
	//filebase maker
	#if PLATFORM == 1
	file_make_fdir(pusr, Strt(JLVM_FILEBASE));
	#else
	int fsize = strlen(filebase)+strlen(JLVM_FILEBASE);
	char *filebase2 = malloc(fsize);
	for(i = 0; i < fsize; i++) {
		filebase2[i] = 0;
	}
	strcat(filebase2,filebase);
	strcat(filebase2,JLVM_FILEBASE);
	file_make_fdir(pusr, Strt(filebase2));
	#endif
	file_make_fdir(pusr, Strt(filebase));

	strt pvar_pkfl = amem_strt_merg(Strt(filebase), Strt(FAKE), STRT_KEEP);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, pvar_pkfl);
	#endif
	
	siop_offs_sett(pusr, "INIT");
	siop_offs_sett(pusr, "PKFL");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pusr, pvar_pkfl);
	#endif
	return pvar_pkfl;
}

void _jal5_file_init(jvct_t * pjct) {

	siop_offs_sett(pjct->Sgrp.usrd, "FILE");
	siop_offs_sett(pjct->Sgrp.usrd, "INIT");

	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(pjct->Sgrp.usrd, "program name:");
	#endif
	lsdl_prog_name(Strt("JLVM"));

	gvar_pkfl = file_reso_loca(pjct->Sgrp.usrd, Strt("JLVM"), Strt(FAKE));
	remove((void*)gvar_pkfl->data);

	jal5_file_errf(pjct, "Segmentation Fault / Floatation Exception etc.");
	siop_offs_sett(pjct->Sgrp.usrd, "JLVM");
}