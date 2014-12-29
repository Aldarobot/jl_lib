/*
 * (c) Jeron A. Lau
 *
 * CUHC: Compocompu Utilities; Header Convert
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define MAXFILELEN 1000000000
#define IMAGE_DESKTOP "IMG.TXT"

uint32_t JL_bytesRead;
uint8_t *output;
uint64_t cursor = 0;
uint8_t *VariableString;
uint8_t file_name[100];

void JL_save(char *file, char *name, int bytes) {
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
		fprintf(stderr, "Write failed.\n");
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

void jal5_cuhc_conv(const char *ImageName) {
	int i, j;
	unsigned char *input;
	unsigned char *file_name = malloc(100);
	char *fp_file_name = malloc(strlen(ImageName));
	for(i = 0; i < strlen(ImageName); i++) {
		fp_file_name[i] = ImageName[i];
	}
	for(i =0; i < 100; i++) {
		file_name[i] = '\0';
	}
	strcat(file_name,ImageName);
	printf("loading input file %s....\n", file_name);
	input = JL_loadFile(file_name);
	printf("loaded input file.\n");
	i = 0;
	char boolean = 0;
	while(1) {
		if(fp_file_name[i] == '.') {
			fp_file_name[i] = '\0';
			boolean = 1;
		}
		if(fp_file_name[i] == '\0') {
			break;
		}
		if(boolean) {
			fp_file_name[i] = '\0';
		}
		if(fp_file_name[i] == '/' || fp_file_name[i] == '\\' || fp_file_name[i] == ',') {
			fp_file_name[i] = '_';
		}
		i++;
	}
	printf("creating text...\n");
	output[cursor] = 'c';
	cursor++;
	output[cursor] = 'h';
	cursor++;
	output[cursor] = 'a';
	cursor++;
	output[cursor] = 'r';
	cursor++;
	output[cursor] = ' ';
	cursor++;
	for(i = 0; i < strlen(fp_file_name); i++) {
		output[cursor] = fp_file_name[i];
		cursor++;
	}
	output[cursor] = ' ';
	cursor++;
	output[cursor] = '[';
	cursor++;
	output[cursor] = ']';
	cursor++;
	output[cursor] = '=';
	cursor++;
	output[cursor] = '{';
	cursor++;
	output[cursor] = '\n';
	cursor++;
	output[cursor] = '\t';
	cursor++;

	for(i = 0; i < JL_bytesRead; i++) {
		unsigned char *printed = malloc(4);
		for(j = 0; j < 4; j++) {
			printed[j] = '\0';
		}
		sprintf(printed, "%d", input[i]);
		if(i != 0) {
			output[cursor] = ',';
			cursor++;
		}
		for(j = 0; j < strlen(printed); j++) {
			output[cursor] = printed[j];
			cursor++;
		}
	}
	output[cursor] = '\n';
	cursor++;
	output[cursor] = '}';
	cursor++;
	output[cursor] = ';';
	cursor++;

	printf("adding varstr\n");
	strcat(VariableString, "\tchar ");
	strcat(VariableString, fp_file_name);
	strcat(VariableString, "[];\n");
	
}

void jal5_cuhc_save(char *outp) {
	printf("[CUHC] saving output....\n");
	JL_save(output, outp, cursor);
	printf("[CUHC]\n[CUHC] variables:\n");
	printf("[CUHC] %s\n", VariableString);
	printf("[CUHC] success\n");
	return;
}

void jal5_cuhc_init(void) {
	int i;
	printf("[CUHC] header convert V6.0\n");

	output = malloc(MAXFILELEN);
	printf("[CUHC] allocated space.\n");
	int j;
	for(i = 0; i < 10000000; i++) {
		output[i] = '\0';
	}

	printf("[CUHC] cleared space\n");

	VariableString = malloc(100);
	for(i = 0; i < 100; i++) {
		file_name[i] = '\0';
		VariableString[i] = '\0';
	}
}

void jal5_cuhc_main(int argc, char *argv[]) {
	jal5_cuhc_init();

	uint8_t *list = JL_loadFile(IMAGE_DESKTOP);
	uint32_t file_name_cursor = 0;
	int i = 0;

	printf("[CUHC] %s\n",list);
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
			printf("[CUHC] COMPILING %s...\n", file_name);
			jal5_cuhc_conv(file_name);
			printf("[CUHC] COMPILED %s\n", file_name);
			for(i = 0; i < 100; i++) {
				file_name[i] = '\0';
			}
			file_name_cursor = 0;
			list++;
		}
		file_name[file_name_cursor] = list[0];
		file_name_cursor++;
	}
	jal5_cuhc_save("GEN/img.h");
	exit(0);
}
