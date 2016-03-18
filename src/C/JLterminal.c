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

static void _jl_io_current(jl_t *jlc, u8_t thread_id) {
	jvct_t *_jlc = jlc->_jlc;
	int i;

	JL_IO_PRINTF(" <");
	for(i = 0; i < _jlc->thread[thread_id].print.level; i++) {
		JL_IO_PRINTF(jl_me_format(jlc, "%s",
			_jlc->thread[thread_id].print.stack[i+1]));
		JL_IO_PRINTF("/");
	}
	JL_IO_PRINTF("\b>\n");
}

static void jl_io_indent__(jvct_t *_jlc, i8_t o, u8_t thread_id) {
	int i;
	// Print enough spaces for the open blocks.
	for(i = 0; i < _jlc->thread[thread_id].print.level + o; i++)
		JL_IO_PRINTF(" ");
}

static inline void _jl_io_new_block(jl_t *jlc, u8_t thread_id) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->thread[thread_id].print.ofs2;
	u8_t level = _jlc->thread[thread_id].print.level;

	_jlc->thread[thread_id].print.ofs2 = 0;
	JL_IO_PRINTF("[");
	for(i = level - ofs2; i < level; i++) {
		JL_IO_PRINTF("/");
		JL_IO_PRINTF("%s", _jlc->thread[thread_id].print.stack[i+1]);
	}
	JL_IO_PRINTF("]");
	_jl_io_current(jlc, thread_id);
}

static inline void _jl_io_old_block(jl_t *jlc, u8_t thread_id) {
	jvct_t *_jlc = jlc->_jlc;
	int i;
	i8_t ofs2 = _jlc->thread[thread_id].print.ofs2;
	u8_t level = _jlc->thread[thread_id].print.level;

	_jlc->thread[thread_id].print.ofs2 = 0;
	JL_IO_PRINTF("[\\");
	for(i = level; i > level + ofs2; i--) {
		JL_IO_PRINTF("%s", _jlc->thread[thread_id].print.stack[i+1]);
		JL_IO_PRINTF("\\");
	}
	JL_IO_PRINTF("\b]");
	_jl_io_current(jlc, thread_id);
}

static inline void jl_io_print_descriptor_(jvct_t *_jlc, u8_t thread_id) {
	if(_jlc->thread[thread_id].print.ofs2 > 0) {
		jl_io_indent__(_jlc, -1, thread_id);
		_jl_io_new_block(_jlc->jlc, thread_id);
	}else if(_jlc->thread[thread_id].print.ofs2 < 0) {
		jl_io_indent__(_jlc, 0, thread_id);
		_jl_io_old_block(_jlc->jlc, thread_id);
	}
	jl_io_indent__(_jlc, 0, thread_id);
	JL_IO_PRINTF("[%s] ", _jlc->thread[thread_id].print.stack
		[_jlc->thread[thread_id].print.level]);
}

static void jl_io_test_overreach(jl_t* jlc, u8_t thread_id) {
	jvct_t *_jlc = jlc->_jlc;
	u8_t level = _jlc->thread[thread_id].print.level;
	
	if(level > 49) {
		JL_IO_PRINTF("Overreached block count %d!!!\n", level);
		JL_IO_PRINTF("Quitting....\n");
		exit(0);
	}
}

static void jl_io_reset_print_descriptor_(jvct_t *_jlc, u8_t thread_id) {
	// Check to see if too many blocks are open.
	jl_io_test_overreach(_jlc->jlc, thread_id);
	// Print the print descriptor.
	jl_io_print_descriptor_(_jlc, thread_id);
}

void _jl_io_printc(jl_t* jlc, str_t input) {
	jvct_t *_jlc = jlc->_jlc;
	uint8_t thread_id = jl_thread_current(jlc);
	int i = 0;

	jl_me_copyto(input, _jlc->me.buffer, strlen(input) + 1);
	while(i != -1) {
		str_t text = _jlc->me.buffer + (80 * i);
		// If string is empty; quit
		if((!text) || (!text[0])) break;
		// Clear and reset the print buffer
		jl_io_reset_print_descriptor_(_jlc, thread_id);
		// Print upto 80 characters to the terminal
		int chr_cnt = 73 - _jlc->thread[thread_id].print.level;
		char convert[10];
		if(strlen(text) > chr_cnt - 1) {
			i++;
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
	uint8_t thread_id = jl_thread_current(jlc);

	// Check to see if too many blocks are open.
	jl_io_test_overreach(jlc, thread_id);
	// Skip over hidden values
	_jl_io_printc(jlc, print);
}

static void jl_io_function__(jvct_t* _jlc, str_t fn_name, u8_t thread_id) {
	i32_t size = strlen(fn_name);

	_jlc->thread[thread_id].print.level++;
	_jlc->thread[thread_id].print.ofs2++;
	jl_me_copyto(fn_name, _jlc->thread[thread_id].print.stack
		[_jlc->thread[thread_id].print.level], size);
	_jlc->thread[thread_id].print.stack
		[_jlc->thread[thread_id].print.level][size] = '\0';
}

/**
 * Open a printing block.
 * @param jl: The library context.
 * @param fn_name: The name of the block.
**/
void jl_io_function(jl_t* jl, str_t fn_name) {
	uint8_t thread_id = jl_thread_current(jl);

	jl_io_function__(jl->_jlc, fn_name, thread_id);
}

/**
 * Close a printing block.
 * @param jl: The library context.
 * @param fn_name: The name of the block.
**/
void jl_io_return(jl_t* jlc, str_t fn_name) {
	jvct_t* _jlc = jlc->_jlc;
	uint8_t thread_id = jl_thread_current(jlc);

	if(strcmp(fn_name, _jlc->thread[thread_id].print.stack
		[_jlc->thread[thread_id].print.level]))
	{
		jl_io_print(jlc, "Error returning \"%s\" on thread #%d:\n",
			fn_name, thread_id);
		jl_io_print(jlc, "\tFunction \"%s\" didn't return.",
			_jlc->thread[thread_id].print.stack[
				_jlc->thread[thread_id].print.level]);
		jl_sg_kill(jlc);
	}
	jl_me_clr(_jlc->thread[thread_id].print.stack
		[_jlc->thread[thread_id].print.level], 30);
	_jlc->thread[thread_id].print.level--;
	_jlc->thread[thread_id].print.ofs2 -= 1;
}

void jl_io_stacktrace(jl_t* jlc) {
	jvct_t* _jlc = jlc->_jlc;
	uint8_t thread_id = jl_thread_current(jlc);
	int i;

	jl_io_print(jlc, "Stacktrace for thread #%d (Most Recent Call Last):",
		thread_id);
	for(i = 0; i <= _jlc->thread[thread_id].print.level; i++) {
		jl_io_print(jlc, _jlc->thread[thread_id].print.stack[i]);
	}
}

void jl_io_init_thread__(jl_t* jlc, u8_t thread_id) {
	jvct_t* _jlc = jlc->_jlc;
	uint8_t i;

	for(i = 0; i < 50; i++) {
		jl_me_clr(_jlc->thread[thread_id].print.stack[i], 30);
	}
	_jlc->thread[thread_id].print.level = 0;
	_jlc->thread[thread_id].print.ofs2 = 0;
	jl_io_function__(_jlc, "JL_Lib", thread_id);
}

void _jl_io_init(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;

	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	_jlc->io.printfn = jl_io_print_no;
	jl_io_tag_set(jlc, NULL);
	jl_io_init_thread__(jlc, 0);
}

void _jl_io_kill(jl_t * jlc) {
	jl_io_return(jlc, "JL_Lib");
}
