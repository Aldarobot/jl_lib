/*
 * me: memory manager
 * 
 * A simple memory library.  Includes creating variables, setting and
 * getting variables, and doing simple and complicated math functions on
 * the variables.  Has a specialized string type.
*/

#include "jl_pr.h"
#include <malloc.h>

/************************/
/*** Static Functions ***/
/************************/

//Update Memory To Size "kb" kilobytes
static inline void _jlvm_jl_me_resz(jvct_t* _jl, uint32_t kb) {
	printf("[jl_me] updating memory size....\n");
	printf("[jl_me] total size = %ld\n", jl_me_tbiu());
	printf("[jl_me] update to =  %d\n", kb);
	printf("[jl_me] SDL Size =   %d\n", 0);
	printf("[jl_me] JLVM Size =  %d\n", 0);
	printf("[jl_me] NonLib Size =%d\n", 0);
	printf("[jl_me] Unknown =    %d\n", 0);
}

static void _jl_me_init_alloc(void **a, uint32_t size) {
	if(size == 0) {
		printf("mina: Double Free or free on NULL pointer!\n");
		exit(-1);
	}
	*a = malloc(size);
	if(*a == NULL) {
		printf("mina: jl_me_alloc: Out Of Memory!");
		exit(-1);
	}
	jl_mem_clr(*a, size);
}

/************************/
/*** Global Functions ***/
/************************/

/**
 * Return Amount Of Total Memory Being Used
 * @returns The total amount of memory being used in bytes.
**/
u64_t jl_me_tbiu(void) {
	struct mallinfo mi;

	malloc_trim(0); //Remove Free Memory
	mi = mallinfo();
	return mi.uordblks;
}

void jl_me_leak_init(jl_t* jl) {
	jvct_t * jl_ = jl->_jl;

	jl_->me.usedmem = jl_me_tbiu();
}

/**
 * Exit if there's been a memory leak since the last call to jl_me_leak_init().
**/
void jl_me_leak_fail(jl_t* jl, str_t fn_name) {
	jvct_t * jl_ = jl->_jl;

	if(jl_me_tbiu() != jl_->me.usedmem) {
		jl_print(jl, "%s: Memory Leak Fail", fn_name);
		jl_sg_kill(jl);
	}
}

/**
 * Allocate, Resize, or Free Dynamic Memory.  All memory allocated by this
 * function is uninitialized.
 * To allocate dynamic memory:	void* memory = fn(jl, NULL, size);
 * To resize dynamic memory:	memory = fn(jl, memory, new_size);
 * To free dynamic memory:	memory = fn(jl, memory, 0);
 * @param jl: The library context.
 * @param a: Pointer to the memory to resize/free, or NULL if allocating memory.
 * @param size: # of bytes to resize to/allocate, or 0 to free.
**/
void *jl_mem(jl_t* jl, void *a, uint32_t size) {
	if(size == 0) { // Free
		if(a == NULL) {
			jl_print(jl, "Double Free or free on NULL pointer");
			exit(-1);
		}else{
			free(a);
		}
		return NULL;
	}else{ // Allocate or Resize
		if((a = realloc(a, size)) == NULL) {
			jl_print(jl, "realloc() failed! Out of memory?");
			exit(-1);
		}
	}
	return a;
}

/**
 * Allocate & Initialize Dynamic Memory.  All memory allocated by this function
 * is initialized as 0.
 * @param jl: The library context.
 * @param size: # of bytes to allocate.
**/
void *jl_memi(jl_t* jl, uint32_t size) {
	// Make sure size is non-zero.
	if(!size) {
		jl_print(jl, "jl_memi(): size must be more than 0");
		exit(-1);
	}
	// Allocate Memory.
	void* a = jl_mem(jl, NULL, size);

	// Clear the memory.
	jl_mem_clr(a, size);
	// Return the memory
	return a;
}

/**
 * Copy "size" bytes of "src" to a new pointer of "size" bytes and return it.
 * @param jl: The library context.
 * @param src: source buffer
 * @param size: # of bytes of "src" to copy to "dst"
 * @returns: a new pointer to 
*/
void * jl_mem_copy(jl_t* jl, const void *src, size_t size) {
	void *dest = jl_memi(jl, size);
	jl_mem_copyto(src, dest, size);
	return dest;
}

/**
 * Format a string.
**/
m_str_t jl_me_format(jl_t* jl, str_t format, ... ) {
	if(format) {
		va_list arglist;

		va_start( arglist, format );
		vsprintf( jl->temp, format, arglist );
		va_end( arglist );
		return jl->temp;
	}else{
		return NULL;
	}
}

/**
 * Generate a random integer from 0 to "a"
 * @param a: 1 more than the maximum # to return
 * @returns: a random integer from 0 to "a"
*/
u32_t jl_me_random_int(u32_t a) {
	return rand()%a;
}

/**
 * Save a pointer to a buffer and get the previous value of it.
 * @param jl: The library context.
 * @param which: A number from 1-15, specifies which temporary pointer to use.
 * @param tmp_ptr: The new pointer to save to the buffer.
 * @returns: The old/previous value of the pointer.
**/
void *jl_me_tmp_ptr(jl_t* jl, uint8_t which, void *tmp_ptr) {
	jvct_t* _jl = jl->_jl;
	void *rtn = _jl->me.tmp_ptr[which];

	_jl->me.tmp_ptr[which] = tmp_ptr;
	return rtn;
}

/************************/
/***  ETOM Functions  ***/
/************************/

jvct_t* _jl_me_init(void) {
	//Create a context for the currently loaded program
	jvct_t* _jl = NULL;
	_jl_me_init_alloc((void**)&_jl, sizeof(jvct_t));
	m_u8_t i;

	//Set the current program ID to 0[RESERVED DEFAULT]
	_jl->cprg = 0;
	//Prepare user data structure
	_jl->jl = NULL;
	_jl_me_init_alloc((void**)&(_jl->jl), sizeof(jl_t));
	_jl->jl->_jl = _jl;
	_jl->jl->errf = JL_ERR_NERR; // No error
	//Make sure that non-initialized things aren't used
	_jl->has.graphics = 0;
	_jl->has.fileviewer = 0;
	_jl->has.filesys = 0;
	_jl->has.input = 0;
	_jl->me.status = JL_STATUS_GOOD;
	// Clear temporary pointer buffer.
	for(i = 0; i < 16; i++) _jl->me.tmp_ptr[i] = NULL;

/*	printf("u %p, %p, c %p,%p\n",
		_jl->jl, ((jvct_t *)(_jl->jl->_jl))->sg.usrd,
		_jl, _jl->jl->_jl);*/
//	g_vmap_list = cl_list_create();
	return _jl;
}

void _jl_me_kill(jvct_t* _jl) {
	free(_jl);
//	cl_list_destroy(g_vmap_list);
}
