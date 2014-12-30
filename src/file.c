#include "header/jlvmpu.h"

#define PKFMAX 10000000

int32_t JL_bytesRead;
char *gvar_pkfl;
char *errf;

void file_file_save(sgrp_user_t* pusr, char *file, char *name, uint32_t bytes) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("SAVING....\n");
	#endif
	jvct_t * pjct = pusr->pjct;
	#if JLVM_DEBUG >= JLVM_DEBUG_INTENSE
	jal5_siop_cplo(0,"JLVM","doing....");
	jal5_siop_cplo(0,"JLVM",amem_strt_fnum(bytes));
	#endif
	int errsv;
	ssize_t n_bytes;
	int fd = open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	truncate(name, 0);
	if(fd <= 0) {
		errsv = errno;

		siop_prnt_lwst(0, Strt("JLVM"),
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
	int zerror;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/FILE/LOAD",amem_strt_merg(
		amem_strt_merg("loading package:\"",packageFileName), "\"..."));
	#endif
	struct zip *zipfile = zip_open(packageFileName, ZIP_CHECKCONS, &zerror);
	if(zerror == ZIP_ER_OPEN) {
		jal5_siop_cplo(0,"JLVM"," NO EXIST!");
		pusr->errf = ERRF_FIND;
		return NULL;
	}
	if(zipfile == NULL) {
		jlvm_dies(pusr->pjct, Strt("couldn't load pckg!"));
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/FILE/LOAD",(char *)zip_strerror(zipfile));
	jal5_siop_cplo(0,"JLVM/FILE/LOAD","loaded package.\n");
	#endif
	unsigned char *fileToLoad = malloc(PKFMAX);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/FILE/LOAD","opening file in package....\n");
	#endif
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/FILE/LOAD", "call pass.");
	#endif
	if(file == NULL) {
		siop_prnt_lwst(0,Strt("JLVM/FILE/LOAD"),
			amem_strt_merg(
			amem_strt_merg(Strt("couldn't open up file: \""),
				Strt(filename), STRT_TEMP),
			amem_strt_merg(Strt("\" in "),
				Strt(packageFileName), STRT_TEMP),STRT_TEMP));
		siop_prnt_lwst(0,Strt("JLVM/FILE/LOAD"),
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
	return fileToLoad;
}

uint8_t *file_pkdj_mnld(sgrp_user_t* pusr, char *Fname) {
	uint8_t *freturn;
	if(
		((freturn = file_pkdj_load(pusr,gvar_pkfl,Fname)) == NULL) &&
		pusr->errf == ERRF_FIND ) //Package doesn't exist!! - create
	{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM","Creating Package...");
		#endif
		file_file_save(pusr, jal5_head_jlvm(), gvar_pkfl,
			jal5_head_size());
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM","Attempt Complete!");
		#endif
		SDL_Delay(1000); //give file system time to update
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM","Try loading again....");
		#endif
		if(
			((freturn = file_pkdj_load(pusr,gvar_pkfl,Fname))
				== NULL) &&
			pusr->errf == ERRF_FIND )//Package still doesn't exist!!
		{
			jlvm_dies(pusr->pjct, Strt("Failed To Create jlvm.zip"));
		}
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM","Good loading!");
		#endif
	}
	return freturn;
}

void jal5_make_fdir(jvct_t * pjct, const char *pfilebase) {
	if(mkdir(pfilebase, 0)) {
		int errsv = errno;
		if(errsv == EEXIST) {
			#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
			jal5_siop_cplo(0,"JLVM","Directory Exist! Continue...");
			#endif
		}else{
			jlvm_dies(pjct,
				amem_strt_merg(
					Strt("Directory: Uh oh..."),
					Strt(strerror(errsv)), STRT_TEMP
				)
			);
		}
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM","Created Directory!");
		#endif
	}
}

void _jal5_file_init(jvct_t * pjct) {

	int i;
	const char *fprg_name = "JLVM";

	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM","program name:");
	#endif
	jlvmpi_prg_name(fprg_name);

	#if PLATFORM == 1 //PHONE
	char *filebase = (void*)amem_strt_merg(
		Strt(JLVM_FILEBASE),
		amem_strt_merg(Strt(fprg_name), Strt("/"), STRT_TEMP),
		STRT_KEEP
	)->data;
	errf = amem_strt_merg(Strt(JLVM_FILEBASE), Strt("errf.txt"), STRT_KEEP)
		->data;
	#elif PLATFORM == 0 //COMPUTER
	char *filebase = SDL_GetPrefPath("JLVM",fprg_name);
	if(filebase == NULL) {
		filebase = (void*) amem_strt_merg(
			Strt("JLVM/"),Strt(fprg_name),STRT_KEEP)->data;
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
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM/FLBS",filebase);
		#endif
	const char *fake = "jlvm.zip";
	int fvar_maxx = strlen(filebase)+strlen(fake)+2;
	gvar_pkfl = malloc(fvar_maxx);
	for(i = 0; i < fvar_maxx; i++) {
		gvar_pkfl[i] = '\0';
	}

	//filebase maker
	#if PLATFORM == 1
	jal5_make_fdir(pjct, JLVM_FILEBASE);
	#else
	int fsize = strlen(filebase)+strlen(JLVM_FILEBASE);
	char *filebase2 = malloc(fsize);
	for(i = 0; i < fsize; i++) {
		filebase2[i] = 0;
	}
	strcat(filebase2,filebase);
	strcat(filebase2,JLVM_FILEBASE);
	jal5_make_fdir(pjct, filebase2);
	#endif
	jal5_make_fdir(pjct, filebase);

	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/FLBS",gvar_pkfl);
	#endif
	strcat(gvar_pkfl, filebase);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/FLBS","fake");
	#endif
	strcat(gvar_pkfl, fake);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM/PKFL",gvar_pkfl);
	#endif
	remove(gvar_pkfl);
	jal5_file_errf(pjct, "Segmentation Fault / Floatation Exception etc.");
}
