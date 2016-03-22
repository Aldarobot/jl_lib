/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLprint.c
 *	is a library for printing to various terminals and creating a stack
 *	trace.
**/

#include "jl_pr.h"

#if JL_PLAT == JL_PLAT_COMPUTER
	#define JL_PRINT(...) printf(__VA_ARGS__)
#else
	#define JL_PRINT(...) SDL_Log(__VA_ARGS__)
#endif

static void _jl_print_current(jl_t *jl, u8_t thread_id) {
	jvct_t *_jl = jl->_jl;
	int i;

	JL_PRINT(" <");
	for(i = 0; i < _jl->thread[thread_id].print.level; i++) {
		JL_PRINT(jl_mem_format(jl, "%s",
			_jl->thread[thread_id].print.stack[i+1]));
		JL_PRINT("/");
	}
	JL_PRINT("\b>\n");
}

static void jl_print_indent__(jvct_t *_jl, i8_t o, u8_t thread_id) {
	int i;
	// Print enough spaces for the open blocks.
	for(i = 0; i < _jl->thread[thread_id].print.level + o; i++)
		JL_PRINT(" ");
}

static inline void _jl_print_new_block(jl_t *jl, u8_t thread_id) {
	jvct_t *_jl = jl->_jl;
	int i;
	i8_t ofs2 = _jl->thread[thread_id].print.ofs2;
	u8_t level = _jl->thread[thread_id].print.level;

	_jl->thread[thread_id].print.ofs2 = 0;
	JL_PRINT("[");
	for(i = level - ofs2; i < level; i++) {
		JL_PRINT("/");
		JL_PRINT("%s", _jl->thread[thread_id].print.stack[i+1]);
	}
	JL_PRINT("]");
	_jl_print_current(jl, thread_id);
}

static inline void _jl_print_old_block(jl_t *jl, u8_t thread_id) {
	jvct_t *_jl = jl->_jl;
	int i;
	i8_t ofs2 = _jl->thread[thread_id].print.ofs2;
	u8_t level = _jl->thread[thread_id].print.level;

	_jl->thread[thread_id].print.ofs2 = 0;
	JL_PRINT("[\\");
	for(i = level; i > level + ofs2; i--) {
		JL_PRINT("%s", _jl->thread[thread_id].print.stack[i+1]);
		JL_PRINT("\\");
	}
	JL_PRINT("\b]");
	_jl_print_current(jl, thread_id);
}

static inline void jl_print_descriptor_(jvct_t *_jl, u8_t thread_id) {
	if(_jl->thread[thread_id].print.ofs2 > 0) {
		jl_print_indent__(_jl, -1, thread_id);
		_jl_print_new_block(_jl->jl, thread_id);
	}else if(_jl->thread[thread_id].print.ofs2 < 0) {
		jl_print_indent__(_jl, 0, thread_id);
		_jl_print_old_block(_jl->jl, thread_id);
	}
	jl_print_indent__(_jl, 0, thread_id);
	JL_PRINT("[%s] ", _jl->thread[thread_id].print.stack
		[_jl->thread[thread_id].print.level]);
}

static void jl_print_test_overreach(jl_t* jl, u8_t thread_id) {
	jvct_t *_jl = jl->_jl;
	u8_t level = _jl->thread[thread_id].print.level;
	
	if(level > 49) {
		JL_PRINT("Overreached block count %d!!!\n", level);
		JL_PRINT("Quitting....\n");
		exit(0);
	}
}

static void jl_print_reset_print_descriptor_(jvct_t *_jl, u8_t thread_id) {
	// Check to see if too many blocks are open.
	jl_print_test_overreach(_jl->jl, thread_id);
	// Print the print descriptor.
	jl_print_descriptor_(_jl, thread_id);
}

void _jl_printc(jl_t* jl, str_t input) {
	jvct_t *_jl = jl->_jl;
	uint8_t thread_id = jl_thread_current(jl);
	int i = 0;

	jl_mem_copyto(input, _jl->me.buffer, strlen(input) + 1);
	while(i != -1) {
		str_t text = _jl->me.buffer + (80 * i);
		// If string is empty; quit
		if((!text) || (!text[0])) break;
		// Clear and reset the print buffer
		jl_print_reset_print_descriptor_(_jl, thread_id);
		// Print upto 80 characters to the terminal
		int chr_cnt = 73 - _jl->thread[thread_id].print.level;
		char convert[10];
		if(strlen(text) > chr_cnt - 1) {
			i++;
		}else{
			chr_cnt = strlen(text);
			i = -1; // break
		}

		sprintf(convert, "%%%ds\n", chr_cnt);

		JL_PRINT(convert, text);
		jl_file_print(jl, _jl->fl.paths.errf,
			jl_mem_format(jl, convert, text));
	}
}

void jl_print_no(jl_t* jl, const char * print) { }

/**
 * Create a new printing tag.
 * @param jl: the library context.
 * @param tag: the value (0-infinity) of the tag.
 * @param shouldprint: whether this tag is enabled.
 * @param tagfn: the function to run when printing.  For default use NULL.
*/
void jl_print_tag_set(jl_t* jl, jl_print_fnt tagfn) {
	jvct_t *_jl = jl->_jl;

	if(tagfn) //User-Defined function
		_jl->io.printfn = tagfn;
	else //NULL
		_jl->io.printfn = _jl_printc;
}

/**
 * Print text to the terminal.
 * @param jl: the library context.
 * @param print: the string to print.
*/
void jl_printc(jl_t* jl, const char * print) {
	uint8_t thread_id = jl_thread_current(jl);

	// Check to see if too many blocks are open.
	jl_print_test_overreach(jl, thread_id);
	// Skip over hidden values
	_jl_printc(jl, print);
}

static void jl_print_function__(jvct_t* _jl, str_t fn_name, u8_t thread_id) {
	i32_t size = strlen(fn_name);

	_jl->thread[thread_id].print.level++;
	_jl->thread[thread_id].print.ofs2++;
	jl_mem_copyto(fn_name, _jl->thread[thread_id].print.stack
		[_jl->thread[thread_id].print.level], size);
	_jl->thread[thread_id].print.stack
		[_jl->thread[thread_id].print.level][size] = '\0';
}

/**
 * Open a printing block.
 * @param jl: The library context.
 * @param fn_name: The name of the block.
**/
void jl_print_function(jl_t* jl, str_t fn_name) {
	uint8_t thread_id = jl_thread_current(jl);

	jl_print_function__(jl->_jl, fn_name, thread_id);
}

/**
 * Close a printing block.
 * @param jl: The library context.
 * @param fn_name: The name of the block.
**/
void jl_print_return(jl_t* jl, str_t fn_name) {
	jvct_t* _jl = jl->_jl;
	uint8_t thread_id = jl_thread_current(jl);

	if(strcmp(fn_name, _jl->thread[thread_id].print.stack
		[_jl->thread[thread_id].print.level]))
	{
		jl_print(jl, "Error returning \"%s\" on thread #%d:\n",
			fn_name, thread_id);
		jl_print(jl, "\tFunction \"%s\" didn't return.",
			_jl->thread[thread_id].print.stack[
				_jl->thread[thread_id].print.level]);
		jl_sg_kill(jl);
	}
	jl_mem_clr(_jl->thread[thread_id].print.stack
		[_jl->thread[thread_id].print.level], 30);
	_jl->thread[thread_id].print.level--;
	_jl->thread[thread_id].print.ofs2 -= 1;
}

void jl_print_stacktrace(jl_t* jl) {
	jvct_t* _jl = jl->_jl;
	uint8_t thread_id = jl_thread_current(jl);
	int i;

	jl_print(jl, "Stacktrace for thread #%d (Most Recent Call Last):",
		thread_id);
	for(i = 0; i <= _jl->thread[thread_id].print.level; i++) {
		jl_print(jl, _jl->thread[thread_id].print.stack[i]);
	}
}

void jl_print_init_thread__(jl_t* jl, u8_t thread_id) {
	jvct_t* _jl = jl->_jl;
	uint8_t i;

	for(i = 0; i < 50; i++) {
		jl_mem_clr(_jl->thread[thread_id].print.stack[i], 30);
	}
	_jl->thread[thread_id].print.level = 0;
	_jl->thread[thread_id].print.ofs2 = 0;
	jl_print_function__(_jl, "JL_Lib", thread_id);
}

void jl_print_init__(jl_t* jl) {
	jvct_t * _jl = jl->_jl;

	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	_jl->io.printfn = jl_print_no;
	jl_print_tag_set(jl, NULL);
	jl_print_init_thread__(jl, 0);
}

void jl_print_kill__(jl_t * jl) {
	jl_print_return(jl, "JL_Lib");
}
