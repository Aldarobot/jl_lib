/*
 * (c) Jeron A. Lau
 * jl/io is a library for printing to various terminals and creating a stack
 * trace
*/

#include "header/jl_pr.h"

#if JL_PLAT == JL_PLAT_COMPUTER
	#define JL_IO_PRINTF(...) printf(__VA_ARGS__)
#else
	#define JL_IO_PRINTF(...) SDL_Log(__VA_ARGS__)
#endif

// 0 = release
// 1 = enable program debugging
#define JL_IO_DEBUG 1

//PROTOTYPES:
static void _jl_io_current(jl_t *jlc);

static inline void _jl_io_new_block(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->io.ofs2;

	_jlc->io.ofs2 = 0;
	JL_IO_PRINTF("[");
	for(i = _jlc->io.offs - ofs2; i < _jlc->io.offs; i++) {
		JL_IO_PRINTF("/");
		JL_IO_PRINTF("%4s", _jlc->io.head[i+1]);
	}
	JL_IO_PRINTF("]");
	_jl_io_current(jlc);
}

static inline void _jl_io_old_block(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->io.ofs2;

	_jlc->io.ofs2 = 0;
	JL_IO_PRINTF("[\\");
	for(i = _jlc->io.offs; i > _jlc->io.offs + ofs2; i--) {
		JL_IO_PRINTF("%4s", _jlc->io.head[i+1]);
		JL_IO_PRINTF("\\");
	}
	JL_IO_PRINTF("\b]");
	_jl_io_current(jlc);
}

static inline void jl_io_print_descriptor_(jvct_t *_jlc) {
	if(_jlc->io.ofs2 > 0)
		_jl_io_new_block(_jlc->jlc);
	else if(_jlc->io.ofs2 < 0)
		_jl_io_old_block(_jlc->jlc);
	else
		JL_IO_PRINTF("[%4s] ", _jlc->io.head[_jlc->io.offs]);
}

static void jl_io_test_overreach(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	
	if(_jlc->io.offs > 15) {
		JL_IO_PRINTF("\nOverreached block count %d!!!\n", _jlc->io.offs);
		_jlc->io.offs = 15;
		_jl_io_current(jlc);
		JL_IO_PRINTF("\nQuitting...\n");
		exit(0);
	}
}

static void jl_io_reset_print_descriptor_(jvct_t *_jlc) {
	int i;

	// Check to see if too many blocks are open.
	jl_io_test_overreach(_jlc->jlc);
	// Print enough spaces for the open blocks.
	for(i = 0; i < _jlc->io.offs; i++) JL_IO_PRINTF(" ");
	// Print the print descriptor.
	jl_io_print_descriptor_(_jlc);
}

static void _jl_io_current(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;

	JL_IO_PRINTF(" <");
	for(i = 0; i < _jlc->io.offs; i++) {
		JL_IO_PRINTF(jl_me_format(jlc, "%4s",
			_jlc->io.head[i+1]));
		JL_IO_PRINTF("/");
	}
	JL_IO_PRINTF("\b>\n");
}

void _jl_io_printc(jl_t* jlc, const char * input) {
	jvct_t *_jlc = jlc->_jlc;
	int i = 0;

	while(i != -1) {
		str_t text = input + (80 * i);
		// If string is empty; quit
		if((!text) || (!text[0])) break;
		// Clear and reset the print buffer
		jl_io_reset_print_descriptor_(_jlc);
		// Print upto 80 characters to the terminal
		int chr_cnt;
		char convert[10];
		if(strlen(text) > chr_cnt - 1) {
			chr_cnt = 73 - _jlc->io.offs;
//			i++;
		}else{
			chr_cnt = strlen(text);
			i = -1; // break
		}

		sprintf(convert, "%%%ds\n", chr_cnt);

		JL_IO_PRINTF(convert, text);
	}
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

	// Check to see if too many blocks are open.
	jl_io_test_overreach(jlc);
	// Skip over hidden values
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
		JL_IO_PRINTF("[EXIT] ");
	}
}

/**
 * Change the tag of the current block.
 * @param jlc: the library context.
 * @param tag: the tag attached to the block.
**/
void jl_io_tag(jl_t* jlc, i16_t tag) {
	jvct_t *_jlc = jlc->_jlc;

	_jlc->io.tag[_jlc->io.offs] = tag;
}

/**
 * Open an offset block.
 * @param jlc: the library context.
 * @param this: the name of the block.
 * @param tag: the tag attached to the block.
*/
void jl_io_offset(jl_t* jlc, i16_t tag, char * this) {
	jvct_t *_jlc = jlc->_jlc;
	int i = 0;

	if(this == NULL) return;
	if(strncmp(this, _jlc->io.head[_jlc->io.offs], 4) != 0) {
		//extend
		_jlc->io.offs++;
		_jlc->io.ofs2++;
		for(i = 0; i < 4; i++) {
			_jlc->io.head[_jlc->io.offs][i] = this[i];
		}
		_jlc->io.head[_jlc->io.offs][4] = '\0';
	}
	jl_io_tag(jlc, tag);
	return;
}

void jl_io_function(jl_t* jlc, const char* fn_name) {
	#if JL_IO_DEBUG == 1
	jvct_t* _jlc = jlc->_jlc;
	
	_jlc->io.level++;
	jl_me_copyto(fn_name, _jlc->io.stack[_jlc->io.level], 30);
	#endif
}

void jl_io_return(jl_t* jlc, const char* fn_name) {
	#if JL_IO_DEBUG == 1
	jvct_t* _jlc = jlc->_jlc;

	if(strcmp(fn_name, _jlc->io.stack[_jlc->io.level])) {
		_jl_fl_errf(jlc->_jlc, "Function \"");
		_jl_fl_errf(jlc->_jlc, _jlc->io.stack[_jlc->io.level]);
		_jl_fl_errf(jlc->_jlc, "\" didn't return.");
		jl_sg_kill(jlc);
	}
	jl_me_clr(_jlc->io.stack[_jlc->io.level], 30);
	_jlc->io.level--;
	#endif
}

void jl_io_stacktrace(jl_t* jlc) {
	#if JL_IO_DEBUG == 1
	jvct_t* _jlc = jlc->_jlc;
	int i;

	_jl_fl_errf(jlc->_jlc, "Stacktrace:\n");
	for(i = 0; i <= _jlc->io.level; i++) {
		_jl_fl_errf(jlc->_jlc, _jlc->io.stack[i]);
		_jl_fl_errf(jlc->_jlc, "\n");
	}
	#else
	jl_sg_kill(jlc, "No stacktrace in non-debug mode.");
	#endif
}

void _jl_io_init(jvct_t * _jlc) {
	int i, j;

	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	for(i = 1; i < 16; i++) {
		for(j = 0; j < 5; j++) {
			_jlc->io.head[i][j] = '\0';
		}
		_jlc->io.tag[i] = 0;
	}
	#if JL_IO_DEBUG == 1
	for(i = 0; i < 50; i++) {
		for(j = 0; j < 30; j++) {
			_jlc->io.stack[i][j] = 0;
		}
	}
	_jlc->io.level = 0;
	#endif
	_jlc->io.maxtag = 0;
	_jlc->io.printfn = malloc(sizeof(void *));
	_jlc->io.offs = 0;
	_jlc->io.ofs2 = 0;
	jl_io_tag_set(_jlc->jlc, JL_IO_MINIMAL, 1, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_PROGRESS, 1, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_SIMPLE, 1, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_INTENSE, 1, NULL);
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "JLLB\0");
	// Clear & Print to the print buffer.
	jl_io_reset_print_descriptor_(_jlc);
}

void _jl_io_kill(jl_t * jlc) {
	jl_io_close_block(jlc); //Close Block "KILL"
	jl_io_printc(jlc, "Killed Program\n");
	jl_io_close_block(jlc); //Close Block "JLVM"
}
