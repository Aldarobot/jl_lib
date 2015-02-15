/*
 * Jeron lau Virtual machine Resource Packager
 *  -CB: JERON LAU 
 *************************
 * [JÊRÔN LAŪ Ḟûrtcūul Mucēn rēsōwrs pākāj'ûr]
 * [ KB: JÊRÔN LAŪ ]
 *************************
 * Jeron lau Máquina virtual Recursos Envasador
 *  -RP: Jeron Lau
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include "src/lib/libzip/jni/zip.h"

#define MAXFILELEN 100000000
#define IMAGE_DESKTOP "umed/img.text"
#define MUSIC_DESKTOP "umed/aud.text"
#define JLVM_PACKAGE "umed/genr/jlvm.zip"
#define JLVM_IMAG "jlex/2/_img"
#define JLVM_AUDI "jlex/2/_aud"
#define HEADER "JLVM0:JYMJ"

int32_t JL_bytesRead;
uint8_t *output;
uint32_t cursor = 0;
uint8_t *VariableString;
uint8_t *VariableStringB;
uint32_t *imageLocations;

char JL_savePackageFile(char *packageFileName, char *fileName,
	void *data, uint64_t dataSize)
{
	printf("opening \"%s\"....\n", packageFileName);
	struct zip *archive = zip_open(packageFileName, 0 /*| ZIP_CHECKCONS*/, NULL);
	if(archive == NULL) {
		return 1;
	}else{
		printf("opened file system:\"%s\".\n",packageFileName);
	}

	struct zip_source *s;
	if ((s=zip_source_buffer(archive, (void *)data, dataSize, 0)) == NULL) {
		zip_source_free(s);
		printf("src null error[replace]: %s\n", zip_strerror(archive));
	}
//	printf("%d,%d,%d\n",archive,sb.index,s);
	if(zip_add(archive, fileName, s)) {
		printf("add/err: %s\n", zip_strerror(archive));
		printf("maybe exist: trying replacing..\n");

		printf("getting index....\n");
		struct zip_stat sb;
		if(zip_stat(archive, fileName, 0, &sb)!=0) {
			printf("!!index getting fail\n");
			printf("index f: %s\n", zip_strerror(archive));
		}
		printf("got index.\n");

		if(zip_replace(archive, sb.index, s))
		{
			zip_source_free(s);
			printf("repl/error replacing file: %s\n", zip_strerror(archive));
		}
	}else{
		printf("added %s to file sys.\n", fileName);
	}
	zip_close(archive);
	printf("DONE!\n");
	return 0;
}

uint8_t *JL_loadPackageFile(int fileSize, char *packageFileName, char *filename) {
	int *zerror = NULL;
	printf("loading package....\n");
	struct zip *zipfile = zip_open(packageFileName, ZIP_CREATE, zerror);
	if(zipfile == NULL) {
		exit(-1);
	}else{
		printf(" loaded package.\n");
	}
	unsigned char *fileToLoad = malloc(fileSize);

	printf(" opening file in package....\n");
	struct zip_file *file = zip_fopen(zipfile, filename, ZIP_FL_UNCHANGED);
	if(file == NULL) {
		printf(" couldn't open up file because %s\n", zip_strerror(zipfile));
		exit(-1);
	}else{
		printf(" opened file in package\n");
	}
	printf(" reading opened file....\n");
	zip_fread(file, fileToLoad, fileSize);
	printf(" read opened file.\n");
	zip_close(zipfile);
	printf("done.\n");
	return fileToLoad;
}

void JL_save(char *file, char *name, uint32_t bytes) {
	ssize_t n_bytes;
	int fd = open(name, O_RDWR);
	truncate(name, 0);
	if(fd <= 0) {
		fprintf(stderr, "Failed to open file: %s.\n", name);
		exit(-1);
	}
	int at = lseek(fd, 0, SEEK_END);
	n_bytes = write(fd, file, bytes);
	if(n_bytes <= 0) {
		int errsv = errno;
		fprintf(stderr, "Write failed: %s.\n", strerror(errsv));
		close(fd);
		exit(-1);
	}
	printf("Wrote %d bytes @ %d.\n", n_bytes, at);
	close(fd);
	return;
}

void JL_ResetCursor(char *file_name) {
	ssize_t n_bytes;
	int fd = open(file_name, O_RDWR);
	lseek(fd, 0, SEEK_SET);
	close(fd);
}

uint8_t *JL_loadFile(char *file_name) {
	JL_ResetCursor(file_name);
	unsigned char *file = malloc(MAXFILELEN);
	ssize_t n_bytes;
	int fd = open(file_name, O_RDWR);
	if(fd <= 0) {
		fprintf(stderr, "Failed to open file\n");
		exit(-1);
	}
	int Read = read(fd, file, MAXFILELEN + 1);
	JL_bytesRead = Read;
	printf("read %d bytes\n", JL_bytesRead);
	close(fd);
	return file;
}

void normalMode(char *ImageName) {
	int i, j;
	unsigned char *finput;
	unsigned char *file_name = malloc(100);
	
	//Find File And Load
	for(i =0; i < 100; i++) {
		file_name[i] = '\0';
	}
	strcat(file_name,"umed/IMG/");
	strcat(file_name,ImageName);
	strcat(file_name,".jymj");
	printf("loading input file \"%s\"....\n", file_name);
	finput = JL_loadFile(file_name); //load image from file system
	printf("loaded input file.\n");
	
	//Check If File Is Of Correct Format
	i = 0; //Input cursor
	
	printf("header:%9s\n", finput);
	if(strncmp(finput, HEADER, sizeof(HEADER)-1) != 0) {
		printf("error: bad file type: %9s!=%s", finput, HEADER);
		exit(-1);
	}
	i += strlen(HEADER); //Pass File Type
	
	//Find out how many bytes to load
	int FSIZE;
	if(finput[i] == 1) { //Normal Quality[Lowy]
		FSIZE = 1048832;
		printf("normal quality\n");
	}else if(finput[i] == 2) { //High Quality[Norm]
		FSIZE = 2097664;
		printf("high quality\n");
	}else if(finput[i] == 3) { //Picture[High]
		FSIZE = 3145728;
		printf("pic quality\n");
	}else{
		printf("bad file type(must be 1-3) is: %d\n", finput[i]);
		exit(-1);
	}
	i+=1; //Pass quality format

	//Copy entire input file into output file
	printf("[JVRP] image@%d\n", cursor);
	for(i = 0; i < FSIZE; i++) {
		output[cursor] = finput[i];
		cursor++;
	}
}

void musicMode(char *MusicName) {
	int i, j;
	unsigned char *input;
	unsigned char *file_name = malloc(100);
	for(i =0; i < 100; i++) {
		file_name[i] = '\0';
	}
	strcat(file_name,"umed/AUD/");
	strcat(file_name,MusicName);
	strcat(file_name,".ogg");
	printf("loading input file \"%s\"....\n", file_name);
	input = JL_loadFile(file_name);
	printf("loaded input file.\n");
	i = 0;
	char boolean = 0;
	while(1) {
		if(MusicName[i] == '.') {
			MusicName[i] = '\0';
			boolean = 1;
		}
		if(MusicName[i] == '\0') {
			break;
		}
		if(boolean) {
			MusicName[i] = '\0';
		}
		if(MusicName[i] == '/' || MusicName[i] == '\\' || MusicName[i] == ',') {
			MusicName[i] = '_';
		}
		i++;
	}
	uint32_t FSIZE = JL_bytesRead;
	uint8_t *tu = (void *)&FSIZE;
	output[cursor] = tu[0];
	cursor++;
	output[cursor] = tu[1];
	cursor++;
	output[cursor] = tu[2];
	cursor++;
	output[cursor] = tu[3];
	cursor++;
	for(i = 0; i < FSIZE; i++) {
		output[cursor] = input[i];
		cursor++;
	}
	printf("added music of size %d\n", ((uint32_t *)output)[0]);
}

int main(int argc, char *argv[]) {
	int i;
	printf("JLVM resource packager V1.0\n");

	output = malloc(10000000);
	VariableString = malloc(10000000);
	printf("allocated space.\n");
	int j;
	for(i = 0; i < 10000000; i++) {
		output[i] = '\0';
		VariableString[i] = '\0';
	}

	printf("cleared space\n");

	uint8_t *list = JL_loadFile(IMAGE_DESKTOP);
	uint8_t *list_orig = list;
	uint8_t file_name[100];
	uint32_t file_name_cursor = 0;

	for(i = 0; i < 100; i++) {
		file_name[i] = '\0';
	}
	printf("%s\n",list);
	if((list[0] == '\n') || (list[0] == '\0')) {
		printf("NOTHING!\n");
		return;
	}
	list--;
	while(1) {
		list++;
		if(list[0] == '\0') {
			break;
		}
		if(list[0] == '\n') {
			printf("[JVRP/IMGS] COMPILING %s...\n", file_name);
			normalMode(file_name);
			printf("[JVRP/IMGS] COMPILED %s\n", file_name);
			for(i = 0; i < 100; i++) {
				file_name[i] = '\0';
			}
			file_name_cursor = 0;
			list++;
		}
		file_name[file_name_cursor] = list[0];
		file_name_cursor++;
	}
	printf("saving output of size %d....\n", cursor);
//	JL_save(output, "GEN/imgs.kymj", cursor);
	JL_savePackageFile(JLVM_PACKAGE, JLVM_IMAG, output, cursor);
	printf("\nvariables:\n");
	printf("%s\n", VariableString);
//
	printf("[JVRP/AUDI] switching to audio...\n");
	cursor = 0;
	VariableStringB = malloc(10000000);
	printf("allocated space.\n");
	for(i = 0; i < 10000000; i++) {
		output[i] = '\0';
		VariableStringB[i] = '\0';
	}
	free(list_orig);
	printf("cleared space\n");

	list = JL_loadFile(MUSIC_DESKTOP);
	file_name_cursor = 0;

	for(i = 0; i < 100; i++) {
		file_name[i] = '\0';
	}
	printf("%s\n",list);
	if((list[0] == '\n') || (list[0] == '\0')) {
		printf("NOTHING!\n");
		return;
	}
	list--;
	while(1) {
		list++;
		if(list[0] == '\0') {
			break;
		}
		if(list[0] == '\n') {
			printf("[JVRP/AUDI] COMPILING %s...\n", file_name);
			musicMode(file_name);
			printf("[JVRP/AUDI] COMPILED %s\n", file_name);
			for(i = 0; i < 100; i++) {
				file_name[i] = '\0';
			}
			file_name_cursor = 0;
			list++;
		}
		file_name[file_name_cursor] = list[0];
		file_name_cursor++;
	}
	printf("saving output of size %d....\n", cursor);
	JL_savePackageFile(JLVM_PACKAGE, JLVM_AUDI, output, cursor);
	printf("\nvariables:\n");
	printf("%s\n", VariableStringB);
	printf("success\n");
	//
	
	exit(0);
}
