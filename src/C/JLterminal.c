/*
 * (c) Jeron A. Lau
 * jl/io is a library for printing to various terminals and creating a stack
 * trace
*/

#include "jl_pr.h"

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

static void jl_io_indent__(jvct_t *_jlc, i8_t o) {
	int i;
	// Print enough spaces for the open blocks.
	for(i = 0; i < _jlc->io.level + o; i++) JL_IO_PRINTF(" ");
}

static inline void _jl_io_new_block(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->io.ofs2;

	_jlc->io.ofs2 = 0;
	JL_IO_PRINTF("[");
	for(i = _jlc->io.level - ofs2; i < _jlc->io.level; i++) {
		JL_IO_PRINTF("/");
		JL_IO_PRINTF("%s", _jlc->io.stack[i+1]);
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
	for(i = _jlc->io.level; i > _jlc->io.level + ofs2; i--) {
		JL_IO_PRINTF("%s", _jlc->io.stack[i+1]);
		JL_IO_PRINTF("\\");
	}
	JL_IO_PRINTF("\b]");
	_jl_io_current(jlc);
}

static inline void jl_io_print_descriptor_(jvct_t *_jlc) {
	if(_jlc->io.ofs2 > 0) {
		jl_io_indent__(_jlc, -1);
		_jl_io_new_block(_jlc->jlc);
	}else if(_jlc->io.ofs2 < 0) {
		jl_io_indent__(_jlc, 0);
		_jl_io_old_block(_jlc->jlc);
	}
	jl_io_indent__(_jlc, 0);
	JL_IO_PRINTF("[%s] ", _jlc->io.stack[_jlc->io.level]);
}

static void jl_io_test_overreach(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;
	
	if(_jlc->io.level > 49) {
		JL_IO_PRINTF("Overreached block count %d!!!\n", _jlc->io.level);
		JL_IO_PRINTF("Quitting....\n");
		exit(0);
	}
}

static void jl_io_reset_print_descriptor_(jvct_t *_jlc) {
	// Check to see if too many blocks are open.
	jl_io_test_overreach(_jlc->jlc);
	// Print the print descriptor.
	jl_io_print_descriptor_(_jlc);
}

static void _jl_io_current(jl_t *jlc) {
	jvct_t *_jlc = jlc->_jlc;
	int i;

	JL_IO_PRINTF(" <");
	for(i = 0; i < _jlc->io.level; i++) {
		JL_IO_PRINTF(jl_me_format(jlc, "%s",
			_jlc->io.stack[i+1]));
		JL_IO_PRINTF("/");
	}
	JL_IO_PRINTF("\b>\n");
}

void _jl_io_printc(jl_t* jlc, str_t input) {
	jvct_t *_jlc = jlc->_jlc;
	int i = 0;

	jl_me_copyto(input, _jlc->me.buffer, strlen(input) + 1);
	while(i != -1) {
		str_t text = _jlc->me.buffer + (80 * i);
		// If string is empty; quit
		if((!text) || (!text[0])) break;
		// Clear and reset the print buffer
		jl_io_reset_print_descriptor_(_jlc);
		// Print upto 80 characters to the terminal
		int chr_cnt;
		char convert[10];
		if(strlen(text) > chr_cnt - 1) {
			chr_cnt = 73 - _jlc->io.level;
//			i++;
		}else{
			chr_cnt = strlen(text);
			i = -1; // break
		}

		sprintf(convert, "%%%ds\n", chr_cnt);

		JL_IO_PRINTF(convert, text);
		jl_fl_print(jlc, _jlc->fl.paths.errf,
			jl_me_format(jlc, convert, text));
	}
}

void jl_io_print_no(jl_t* jlc, const char * print) { }

/**
 * Create a new printing tag.
 * @param jlc: the library context.
 * @param tag: the value (0-infinity) of the tag.
 * @param shouldprint: whether this tag is enabled.
 * @param tagfn: the function to run when printing.  For default use NULL.
*/
void jl_io_tag_set(jl_t* jlc, jl_io_print_fnt tagfn) {
	jvct_t *_jlc = jlc->_jlc;

	if(tagfn) //User-Defined function
		_jlc->io.printfn = tagfn;
	else //NULL
		_jlc->io.printfn = _jl_io_printc;
}

/**
 * Print text to the terminal.
 * @param jlc: the library context.
 * @param print: the string to print.
*/
void jl_io_printc(jl_t* jlc, const char * print) {
	// Check to see if too many blocks are open.
	jl_io_test_overreach(jlc);
	// Skip over hidden values
	_jl_io_printc(jlc, print);
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

void jl_io_function(jl_t* jlc, str_t fn_name) {
	jvct_t* _jlc = jlc->_jlc;
	i32_t size = strlen(fn_name);
	
	_jlc->io.level++;
	_jlc->io.ofs2++;
	jl_me_copyto(fn_name, _jlc->io.stack[_jlc->io.level], size);
	_jlc->io.stack[_jlc->io.level][size] = '\0';
}

void jl_io_return(jl_t* jlc, str_t fn_name) {
	jvct_t* _jlc = jlc->_jlc;

	if(strcmp(fn_name, _jlc->io.stack[_jlc->io.level])) {
		jl_io_print(jlc, "Error returning \"%s\":\n", fn_name);
		jl_io_print(jlc, "\tFunction \"%s\" didn't return.",
			_jlc->io.stack[_jlc->io.level]);
		jl_sg_kill(jlc);
	}
	jl_me_clr(_jlc->io.stack[_jlc->io.level], 30);
	_jlc->io.level--;
	_jlc->io.ofs2 -= 1;
}

void jl_io_stacktrace(jl_t* jlc) {
	jvct_t* _jlc = jlc->_jlc;
	int i;

	jl_io_print(jlc, "Stacktrace:");
	for(i = 0; i <= _jlc->io.level; i++) {
		jl_io_print(jlc, _jlc->io.stack[i]);
	}
}

void _jl_io_init(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	int i;

	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	for(i = 0; i < 50; i++) {
		jl_me_clr(_jlc->io.stack[i], 30);
	}
	_jlc->io.level = 0;
	_jlc->io.printfn = malloc(sizeof(void *));
	_jlc->io.ofs2 = 0;
	jl_io_tag_set(jlc, NULL);
	jl_io_function(jlc, "JL_Lib");
}

void _jl_io_kill(jl_t * jlc) {
	jl_io_return(jlc, "SG_Kill");
	jl_io_print(jlc, "Killed Program!");
	jl_io_return(jlc, "JL_Lib");
}
