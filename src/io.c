/*
 * (c) Jeron A. Lau
 * jl/io is a library for printing to various terminals.
*/

#include "header/jl_pr.h"

static void _jl_io_indt(jl_t* jlc) {
	int i;
	for(i = 0; i < ((jvct_t *)(jlc->_jlc))->io.offs; i++) {
		printf(" ");
	}
}

static void _jl_io_newline(jvct_t *_jlc) {
	printf("\n");
	_jl_io_indt(_jlc->jlc);
	printf("[%4s] ", _jlc->io.head[_jlc->io.offs]);
	_jlc->io.newline = 0;
}

static void _jl_io_current(jvct_t *_jlc) {
	int i;

	printf(" <");
	for(i = 0; i < _jlc->io.offs; i++) {
		printf("%4s", _jlc->io.head[i+1]);
		printf("/");
	}
	printf("\b>");
}

static inline void _jl_io_new_block(jvct_t *_jlc) {
	int i;
	if(_jlc->io.newline != 2) printf("\n");
	_jlc->io.offs-=_jlc->io.ofs2;
	_jl_io_indt(_jlc->jlc);
	_jlc->io.offs+=_jlc->io.ofs2;
	printf("[/");
	for(i = _jlc->io.offs - _jlc->io.ofs2;
		i < _jlc->io.offs;
		i++)
	{
		printf("%4s", _jlc->io.head[i+1]);
		printf("/");
	}
	printf("\b]");
	_jl_io_current(_jlc);
	_jlc->io.ofs2 = 0;
	_jlc->io.newline = 1;
}

static inline void _jl_io_old_block(jvct_t *_jlc) {
	int i;
	printf("\n");
	_jl_io_indt(_jlc->jlc);
	printf("[\\");
	for(i = _jlc->io.offs;
		i > _jlc->io.offs +
			_jlc->io.ofs2;
		i--)
	{
		printf("%4s", _jlc->io.head[i+1]);
		printf("\\");
	}
	printf("\b]");
	_jl_io_current(_jlc);
	_jlc->io.ofs2 = 0;
	_jlc->io.newline = 1;
}

static inline void _jl_io_printc_endline(jvct_t *_jlc, const char *print) {
	_jlc->io.newline = 1;
	char *string = NULL;
	jl_me_alloc(_jlc->jlc, (void**)&string, strlen(print) + 1, 0);
	memcpy(string, print, strlen(print));
	string[strlen(print) - 1] = '\0';
	printf("%s", string);
	free(string);
}

void _jl_io_printc(jl_t* jlc, const char * print) {
	jvct_t *_jlc = jlc->_jlc;
#if JL_PLAT == JL_PLAT_COMPUTER
	if(_jlc->io.ofs2 > 0) {
		_jl_io_new_block(_jlc);
	}else if(_jlc->io.ofs2 < 0) {
		_jl_io_old_block(_jlc);
	}
	if(print[0] == '\n') {
		_jl_io_newline(_jlc);
		print++;
	}else if(_jlc->io.newline) {
		_jl_io_newline(_jlc);
	}
	if(print[strlen(print) - 1] == '\n') {
		_jl_io_printc_endline(_jlc, print);
	}else{
		printf("%s", print);
	}
#else
	SDL_Log("[%s] %s\n",
		_jlc->io.head[_jlc->io.offs],
		print);
#endif
}

void _jl_io_print_no(jl_t* jlc, const char * print) { }

/**
 * Create a new printing tag.
 * @param jlc: the library context.
 * @param tag: the value (0-infinity) of the tag.
 * @param shouldprint: whether this tag is enabled.
 * @param tagfn: the function to run when printing.  For default use NULL.
*/
void jl_io_tag_set(jl_t* jlc,
	int16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn)
{
	jvct_t *_jlc = jlc->_jlc;
	uint16_t realtag = tag + _JL_IO_MAX;

	if(realtag > _jlc->io.maxtag) {
		_jlc->io.maxtag = realtag;
		_jlc->io.printfn = realloc(_jlc->io.printfn,
			(sizeof(void *) * (_jlc->io.maxtag+1)) + 1);
	}
	if(tagfn) { //User-Defined function
		if(shouldprint) _jlc->io.printfn[realtag] = tagfn;
		else _jlc->io.printfn[realtag] = _jl_io_print_no;	
	}else{ //NULL
		if(shouldprint) _jlc->io.printfn[realtag] = _jl_io_printc;
		else _jlc->io.printfn[realtag] = _jl_io_print_no;
	}
}

/**
 * Print text to the terminal.
 * @param jlc: the library context.
 * @param print: the string to print.
*/
void jl_io_printc(jl_t* jlc, const char * print) {
	jvct_t *_jlc = jlc->_jlc;

	if(_jlc->io.offs > 15) {
		printf("\nOverreached block count %d!!!\n", _jlc->io.offs);
		_jlc->io.offs = 15;
		_jl_io_current(_jlc);
		printf("\nQuitting...\n");
		exit(0);
	}
	//Skip over hidden values
	_jlc->io.printfn[_jlc->io.tag[_jlc->io.offs] + _JL_IO_MAX]
		(jlc, print);
}

/**
 * Print truncated text to the terminal.
 * @param jlc: the library context.
 * @param a: the truncating string.
 * @param print: the string to print.
*/
void jl_io_printt(jl_t *jlc, uint8_t a, const char *print) {
	char *string = NULL; jl_me_alloc(jlc, (void**)&string, strlen(print) + 1, 0);
	char *string2= NULL; jl_me_alloc(jlc, (void**)&string2, 6, 0);
	sprintf(string2, "%%%ds", a);
	sprintf(string, string2, print);
	jl_io_printc(jlc, string);
	free(string);
	free(string2);
}

/**
 * Print an integer to the terminal.
 * @param jlc: the library context.
 * @param print: the number to print.
*/
void jl_io_printi(jl_t *jlc, int print) {
	char *string = NULL; jl_me_alloc(jlc, (void**) &string, 10, 0);
	sprintf(string, "%d", print);
	jl_io_printc(jlc, string);
	free(string);
}

/**
 * Print a double to the terminal.
 * @param jlc: the library context.
 * @param print: the number to print.
*/
void jl_io_printd(jl_t *jlc, double print) {
	char *string = NULL; jl_me_alloc(jlc, (void**)&string, 20, 0);
	sprintf(string, "%f", print);
	jl_io_printc(jlc, string);
	free(string);
}

/**
 * Close an offset block.
 * @param jlc: the library context.
*/
void jl_io_close_block(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;

	_jlc->io.ofs2 -= 1;
	if(_jlc->io.offs != 1) {
		_jlc->io.offs -= 1;
	}else{
		if(_jlc->io.newline) printf("\n");
		printf("[EXIT] ");
	}
}

/**
 * Open an offset block.
 * @param jlc: the library context.
 * @param this: the name of the block.
 * @param tag: the tag attached to the block.
*/
void jl_io_offset(jl_t* jlc, int16_t tag, char * this) {
	jvct_t *_jlc = jlc->_jlc;
	int i = 0;

	if(this == NULL) {
		return;
	}
	if(strncmp(this, _jlc->io.head[_jlc->io.offs], 4) != 0) {
		//extend
		_jlc->io.offs++;
		_jlc->io.ofs2++;
		for(i = 0; i < 4; i++) {
			_jlc->io.head[_jlc->io.offs][i] = this[i];
		}
		_jlc->io.head[_jlc->io.offs][4] = '\0';
	}
	_jlc->io.tag[_jlc->io.offs] = tag;
	return;
}

void _jl_io_init(jvct_t * _jlc) {
	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	int i, j;
	for(i = 1; i < 16; i++) {
		for(j = 0; j < 5; j++) {
			_jlc->io.head[i][j] = '\0';
		}
		_jlc->io.tag[i] = 0;
	}
	_jlc->io.maxtag = 0;
	_jlc->io.printfn = malloc(sizeof(void *));
	_jlc->io.offs = 0;
	_jlc->io.ofs2 = 0;
	_jlc->io.newline = 2;
	jl_io_tag_set(_jlc->jlc, JL_IO_MINIMAL, 1, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_PROGRESS, 0, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_SIMPLE, 0, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_INTENSE, 0, NULL);
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "JLLB");
}

void _jl_io_kill(jl_t * jlc) {
	jl_io_close_block(jlc); //Close Block "KILL"
	jl_io_printc(jlc, "Killed Program\n");
	jl_io_close_block(jlc); //Close Block "JLVM"
}
