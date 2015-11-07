/*
 * (c) Jeron A. Lau
 * jl/io is a library for printing to various terminals and creating a stack
 * trace
*/

#include "header/jl_pr.h"

#if JL_PLAT == JL_PLAT_COMPUTER
	#define JL_IO_PRINTF1(X) printf(X)
	#define JL_IO_PRINTF2(X, Y) printf(X, Y)
#else
	#define JL_IO_PRINTF1(X) SDL_Log(X)
	#define JL_IO_PRINTF2(X, Y) SDL_Log(X, Y)
#endif

// 0 = release
// 1 = enable program debugging
#define JL_IO_DEBUG 1

//PROTOTYPES:
static void _jl_io_indt(jl_t* jlc);
static void jl_io_add_to_buffer2_(jl_t* jlc, const char* x, const char* y);
static void jl_io_add_to_buffer_(jl_t* jlc, const char* what_to_add);
static void _jl_io_current(jl_t *jlc);

static inline void _jl_io_new_block(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->io.ofs2;

	_jlc->io.ofs2 = 0;
	jl_io_add_to_buffer_(jlc, "[");
	for(i = _jlc->io.offs - ofs2; i < _jlc->io.offs; i++) {
		jl_io_add_to_buffer_(jlc, "/");
		jl_io_add_to_buffer2_(jlc, "%4s", _jlc->io.head[i+1]);
	}
	jl_io_add_to_buffer_(jlc, "]");
	_jl_io_current(jlc);
}

static inline void _jl_io_old_block(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->io.ofs2;

	_jlc->io.ofs2 = 0;
	jl_io_add_to_buffer_(jlc, "[\\");
	for(i = _jlc->io.offs; i > _jlc->io.offs + ofs2; i--) {
		jl_io_add_to_buffer2_(jlc, "%4s", _jlc->io.head[i+1]);
		jl_io_add_to_buffer_(jlc, "\\");
	}
	jl_io_add_to_buffer_(jlc, "\b]");
	_jl_io_current(jlc);
}

static inline void jl_io_print_descriptor_(jvct_t *_jlc) {
	if(_jlc->io.ofs2 > 0) {
		_jl_io_new_block(_jlc->jlc);
	}else if(_jlc->io.ofs2 < 0) {
		_jl_io_old_block(_jlc->jlc);
	}else{
		jl_io_add_to_buffer2_(_jlc->jlc, "[%4s] ",
			_jlc->io.head[_jlc->io.offs]);
	}
}

static void jl_io_reset_print_descriptor_(jvct_t *_jlc) {
	// Clear the buffer
	jl_me_clr(_jlc->io.buffer, 81);
	// Pre-write to the buffer
	_jl_io_indt(_jlc->jlc);
	// Print the print descriptor.
	jl_io_print_descriptor_(_jlc);
}

static void jl_io_if_newline(jvct_t *_jlc) {
	// Print the buffer
	JL_IO_PRINTF2("%s", _jlc->io.buffer);
	// Clear and reset the print buffer
	jl_io_reset_print_descriptor_(_jlc);
}

// If last thing printed ends in a newline - print the whole line.
static void jl_io_test_if_newline(jvct_t *_jlc, const char* what_to_add) {
	if(what_to_add[strlen(what_to_add) - 1] == '\n' ||
		_jlc->io.buffer[strlen(_jlc->io.buffer) - 1] == '\n')
	{
		jl_io_if_newline(_jlc);
	}
}

static void jl_io_add_to_buffer_(jl_t* jlc, const char* what_to_add) {
	jvct_t *_jlc = jlc->_jlc;

	if(jl_me_string_print(jlc, _jlc->io.buffer, "%s", what_to_add, 80)) {
		jl_io_if_newline(_jlc);
		jl_io_add_to_buffer_(jlc, "\n");
	}
	jl_io_test_if_newline(_jlc, what_to_add);
}

static void jl_io_add_to_buffer2_(jl_t* jlc, const char* x, const char* y) {
	jvct_t *_jlc = jlc->_jlc;

	if(jl_me_string_print(jlc, _jlc->io.buffer, x, y, 80)) {
		jl_io_if_newline(_jlc);
		jl_io_add_to_buffer_(jlc, "\n");
	}
	jl_io_test_if_newline(_jlc, x);
}

static void jl_io_test_overreach(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	
	if(_jlc->io.offs > 15) {
		JL_IO_PRINTF2("\nOverreached block count %d!!!\n", _jlc->io.offs);
		_jlc->io.offs = 15;
		_jl_io_current(jlc);
		JL_IO_PRINTF1("\nQuitting...\n");
		exit(0);
	}
}

static void _jl_io_indt(jl_t* jlc) {
	jvct_t* _jlc = jlc->_jlc;
	int i;

	// Check to see if too many blocks are open.
	jl_io_test_overreach(jlc);
	// Print enough spaces for the open blocks.
	for(i = 0; i < _jlc->io.offs; i++)
		jl_me_string_print(jlc, _jlc->io.buffer, " ", NULL, 80);
}

static void _jl_io_current(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;

	jl_io_add_to_buffer_(jlc, " <");
	for(i = 0; i < _jlc->io.offs; i++) {
		jl_io_add_to_buffer2_(jlc, "%4s", _jlc->io.head[i+1]);
		jl_io_add_to_buffer_(jlc, "/");
	}
	jl_io_add_to_buffer_(jlc, "\b>\n");
}

void _jl_io_printc(jl_t* jlc, const char * print) {
	jl_io_add_to_buffer2_(jlc, "%s", print);
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
		JL_IO_PRINTF1("[EXIT] ");
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
	if(strncmp(this,
		_jlc->io.head[
		_jlc->io.offs],
		4) != 0)
	{
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
	jl_io_tag_set(_jlc->jlc, JL_IO_MINIMAL, 0, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_PROGRESS, 0, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_SIMPLE, 0, NULL);
	jl_io_tag_set(_jlc->jlc, JL_IO_INTENSE, 0, NULL);
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "JLLB\0");
	// Clear & Print to the print buffer.
	jl_io_reset_print_descriptor_(_jlc);
}

void _jl_io_kill(jl_t * jlc) {
	jl_io_close_block(jlc); //Close Block "KILL"
	jl_io_printc(jlc, "Killed Program\n");
	jl_io_close_block(jlc); //Close Block "JLVM"
}
