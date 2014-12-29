#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <zip.h>

#define MAXFILELEN 1000000

#define COMP_IMPORT "#import <"
#define LLIB_IMPORT "#import \""
#define COMP_COMMENT "//"
#define COMP_BIG_COMMENT "/*"
#define COMP_END_COMMENT "*/"
#define COMP_VOID "void"
#define COMP_STRING "strt"
#define COMP_END ")"
#define COMP_NEXT ","
#define COMP_BRACKET "{"
#define COMP_END_BRACKET "}"
#define FUNC_RETURN "return;"
#define STN_LIB_JL_GRAPHICS "jlgr.JLE"

int32_t JL_bytesRead;
uint8_t *output;
uint32_t cursor = 0;
uint8_t *VariableString;

uint8_t *list_load;
uint8_t *list_loadf;

uint64_t kill = 0;
uint64_t loop = 0;
uint64_t rdup = 0;
uint64_t rddn = 0;
uint64_t otre = 0;
uint64_t imgs = 0;

char JL_savePackageFile(char *packageFileName, char *fileName,
	void *data, uint64_t dataSize)
{
	int *zerror=malloc(1000);
	printf("opening \"%s\"....\n", packageFileName);
	struct zip *archive = zip_open(packageFileName, ZIP_CREATE, zerror);
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
		printf("maybe exsist: trying replacing..\n");

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
	printf("[GENJLE]Wrote %d bytes @ %d.\n", n_bytes, at);
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
	printf("[GENJLE]read %d bytes\n", JL_bytesRead);
	close(fd);
	return file;
}

void normalMode(char *ImageName) {
	int i, j;
	unsigned char *input;
	unsigned char *file_name = malloc(100);
	for(i =0; i < 100; i++) {
		file_name[i] = '\0';
	}
	strcat(file_name,"IMG/");
	strcat(file_name,ImageName);
	printf("loading input file %s....\n", file_name);
	input = JL_loadFile(file_name);
	printf("loaded input file.\n");
	i = 0;
	char boolean = 0;
	while(1) {
		if(ImageName[i] == '.') {
			ImageName[i] = '\0';
			boolean = 1;
		}
		if(ImageName[i] == '\0') {
			break;
		}
		if(boolean) {
			ImageName[i] = '\0';
		}
		i++;
	}
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
	for(i = 0; i < strlen(ImageName); i++) {
		output[cursor] = ImageName[i];
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

	strcat(VariableString, "\tchar ");
	strcat(VariableString, ImageName);
	strcat(VariableString, "[];\n");
	
}

void addByteToOutput(char byte) {
	output[cursor] = byte;
	cursor++;
}

void addStringToOutput(char *string) {
	int i;
	for(i = 0; i < strlen(string); i++) {
		addByteToOutput(string[i]);
	}
}

void initialBytes(void) {
	addStringToOutput("JLE_1.0:d:");
	cursor+=8*6;
}

void addLocation(uint8_t *output, uint64_t cursor, uint64_t data) {
	int i;
	uint8_t *datav = (void *)&data;
	for(i = 0; i < 8; i++) {
		output[i+cursor] = datav[i];
	}
}

char compiler_test_if_next_is(char * script, char * particle) {
	char * point = script;//make the beginning the command
	char * place = strstr(point, particle);//search for partical
	if(place == point) {//if partical at begining return true otherwise false
		return 1;
	}else{
		return 0;
	}
}

char *compiler_read_sent_string(char * script, char end) {
	int i;
	int cursor = 0;
	char *compiled = malloc(100);
	for(i = 0; i < 100; i++) {
		compiled[i] = '\0';
	}
	while((script[cursor] != end) && (script[cursor] != 0)) {
		strncat(compiled, script + cursor, 1);
		cursor++;
	}
//	strcat(compiled, "\0");
	return compiled;
}

uint8_t parameters(uint8_t *list) {
	uint8_t parameterCount = 0;
	while(1) {
		if(compiler_test_if_next_is(list, COMP_END)) {
			list++;
			if(parameterCount < 1) {
				printf("[GENJLE]need void in parameters:\"%s\"\n",
					compiler_read_sent_string(list,';'));
				printf("[GENJLE] FAILURE!!!!\n");
				return 1;
			}
			list_load = list;
			break;
		}
		if(compiler_test_if_next_is(list, COMP_VOID)) {
			continue;
		}
		if(compiler_test_if_next_is(list, COMP_STRING)) {
			parameterCount++;
			list+=strlen(COMP_STRING)+1;
			char *n1 = compiler_read_sent_string(list,')');
			char *n2 = compiler_read_sent_string(list,',');
			char *n3;
			if(strlen(n1) > strlen(n2)) {
				n3 = n2;
			}else{
				n3 = n1;
			}
			printf("[GENJLE] Parameter %d: \"%s\"\n", parameterCount, n3);
			list+=strlen(n3);
			continue;
		}
		printf("[GENJLE/PAR]unidentified phrase:\"%s\"\n",
			compiler_read_sent_string(list,';'));
		printf("[GENJLE/PAR] FAILURE!!!!\n");
		return 1;
	}
	return 0;
}

uint8_t test_for_comments(char *list) {
	if(compiler_test_if_next_is(list, COMP_COMMENT)) {
		list+=strlen(compiler_read_sent_string(list,'\n'))+1;
		list_load = list;
		return 1;
	}
	return 0;
}

uint8_t test_for_whitespace(uint8_t *list) {
	if(list[0] == ' ' || list[0] == '\t' || list[0] == '\n') {
		list++;
		list_load = list;
		return 1;
	}
	return 0;
}

uint8_t function(uint8_t *list) {
	printf("[GENJLE/FN] creating function...\n");
	while(1) {
		if(test_for_comments(list)) {
			list = list_load;
			continue;
		}
		if(test_for_whitespace(list)) {
			list = list_load;
			continue;
		}
		if(compiler_test_if_next_is(list, FUNC_RETURN)) {
			list+=strlen(FUNC_RETURN);
			continue;
		}
		if(compiler_test_if_next_is(list, COMP_END_BRACKET)) {
			list++;
			list_loadf = list;
			break;
		}
		printf("[GENJLE/FN]unidentified phrase:\n[GENJLE/FN]is: \"%s\"\n",
			compiler_read_sent_string(list,';'));
		printf("[GENJLE/FN] FAILURE!!!!\n");
		return 1;
	}
	return 0;
}

void main(int argc, char *argv[]) {
	int i;
	printf("[GENJLE] Compiling....\n");

	output = malloc(10000000);
	int j;
	for(i = 0; i < 10000000; i++) {
		output[i] = '\0';
	}
	initialBytes();
	printf("[GENJLE] Loading Main Class....\n");
	uint32_t file_name_cursor = 0;
	uint8_t file_name[100];
	uint8_t *list = JL_loadFile("SRC/main/init.JPL");
	printf(list);
	printf("[GENJLE] Loaded Main Class!\n");
	while(1) {
		if(test_for_comments(list)) {
			list = list_load;
			continue;
		}
		if(test_for_whitespace(list)) {
			list = list_load;
			continue;
		}
		if(compiler_test_if_next_is(list, COMP_IMPORT)) {
			printf("[GENJLE]found import:");
			list+=strlen(COMP_IMPORT);
			char *str = compiler_read_sent_string(list,'>');
			if(strcmp(str,STN_LIB_JL_GRAPHICS)==0) {
				printf("[STANDARD LIBRARY: JL_GRAPHICS]\n");
				list+=strlen(STN_LIB_JL_GRAPHICS)+2;
//				printf("%s\n", list);
			}
			continue;
		}
//		printf("OMG(%s,%s)\n", LLIB_IMPORT, list);
		if(compiler_test_if_next_is(list, LLIB_IMPORT)) {
			printf("[GENJLE]found import:");
			list+=strlen(LLIB_IMPORT);
			char *str = compiler_read_sent_string(list,'\"');
			printf("[CLASS@:[%s]]\n", str);
			list+=strlen(str)+2;
//			printf("%s\n", list);
			continue;
		}
		if(compiler_test_if_next_is(list, COMP_VOID)) {
			list+=strlen(COMP_VOID)+1;
			printf("[GENJLE]found void function: [%s]\n",
				compiler_read_sent_string(list,'('));
			list+=5;
			if(parameters(list)) {
				return;
			}
			list = list_load;
			while(1) {
				if(list[0] == ' ' || list[0] == '\t' || list[0] == '\n') {
					list++;
				}else{
					break;
				}
			}
			if(compiler_test_if_next_is(list, COMP_BRACKET)) {
				list++;
			}else{
				printf("[GENJLE]expected \"{\" instead found:\n\"%s\"\n",
					compiler_read_sent_string(list,';'));
				printf("[GENJLE] FAILURE!!!!\n");
				return;
			}
			if(function(list)) {
				return;
			}
			list = list_loadf;
			continue;
		}
		if(list[0] == '\0') {
			break;
		}
		printf("[GENJLE]unidentified phrase:\"%s\"\n",
			compiler_read_sent_string(list,';'));
		printf("[GENJLE] FAILURE!!!!\n");
		return;
/*		if(list[0] == '\n') {
			printf("COMPILING %s...\n", file_name);
//			normalMode(file_name);
			printf("COMPILED %s\n", file_name);
			for(i = 0; i < 100; i++) {
				file_name[i] = '\0';
			}
			file_name_cursor = 0;
			list++;
		}
		file_name[file_name_cursor] = list[0];
		file_name_cursor++;
		list++;*/
	}
	printf("[GENJLE]compile success!!!\n");
	printf("[GENJLE]including graphics....\n");
	imgs = cursor;
	uint8_t *graphics = (void *)JL_loadFile("GEN/imgs.kymj");
	for(i = 0; i < JL_bytesRead; i++) {
		output[cursor] = graphics[i];
		cursor++;
	}
	printf("[GENJLE]saving output....\n");
//	JL_save(output, "OUT/JLVM/EXE.JLE", cursor);
	if(JL_savePackageFile("GEN/jlvm.zip","jlex/2/init",output, cursor)) {
		printf("[GENJLE]saving failed!\n");
		exit(-1);
	}
	printf("[GENJLE]success\n");
	exit(0);
}
