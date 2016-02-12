/*
 * me: memory manager
 * 
 * A simple memory library.  Includes creating variables, setting and
 * getting variables, and doing simple and complicated math functions on
 * the variables.  Has a specialized string type.
*/

#include "header/jl_pr.h"
#include <malloc.h>

/************************/
/*** Static Functions ***/
/************************/

static void *jl_me_realloc__(jvct_t* _jlc, void *a, uint32_t size) {
	if((a = realloc(a, size + 1)) == NULL) {
		_jl_fl_errf(_jlc, "realloc() memory error!\n");
		jl_sg_kill(_jlc->jlc);
	}
	return a;
}

//Update Memory To Size "kb" kilobytes
static inline void _jlvm_jl_me_resz(jvct_t* _jlc, uint32_t kb) {
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
	jl_me_clr(*a, size);
}

static void _jl_me_alloc_malloc(jl_t* jlc, void **a, uint32_t size) {
	if(size == 0) {
		_jl_fl_errf(jlc->_jlc, "Double Free or free on NULL pointer\n");
		jl_sg_kill(jlc);
	}
	*a = malloc(size);
	if(*a == NULL) {
		_jl_fl_errf(jlc->_jlc, "jl_me_alloc: Out Of Memory!\n");
		jl_sg_kill(jlc);
	}
	jl_me_clr(*a, size);
}

static void _jl_me_truncate_curs(strt pstr) {
	if(pstr->curs > pstr->size) {
		pstr->curs = pstr->size;
	}
}

static void jl_me_strt_increment(strt pstr, u8_t incrementation) {
	pstr->curs += incrementation;
	_jl_me_truncate_curs(pstr);
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

void jl_me_leak_init(jl_t* jlc) {
	jvct_t * jlc_ = jlc->_jlc;

	jlc_->me.usedmem = jl_me_tbiu();
}

/**
 * Exit if there's been a memory leak since the last call to jl_me_leak_init().
**/
void jl_me_leak_fail(jl_t* jlc, str_t fn_name) {
	jvct_t * jlc_ = jlc->_jlc;

	if(jl_me_tbiu() != jlc_->me.usedmem) {
		jl_io_printc(jlc, fn_name);
		jl_io_printc(jlc, ": Memory Leak Fail");
		jl_sg_kill(jlc);
	}
}

/**
 * Clear memory pointed to by "mem" of size "size"
 * @param pmem: memory to clear
 * @param size: size of "mem"
**/
void jl_me_clr(void *pmem, uint64_t size) {
	uint8_t *fmem = pmem;
	uint64_t i;
	for(i = 0; i < size; i++) {
		fmem[i] = 0;
	}
}

/**
 * Copy "size" bytes of "src" to a new pointer of "size" bytes and return it.
 * @param jlc: The library context.
 * @param src: source buffer
 * @param size: # of bytes of "src" to copy to "dst"
 * @returns: a new pointer to 
*/
void * jl_me_copy(jl_t* jlc, const void *src, size_t size) {
	void *dest = NULL; _jl_me_alloc_malloc(jlc, &dest, size + 1);
	void *rtn = memcpy(dest, src, size);
	((char*)dest)[size] = 0;
	return rtn;
}

/**
 * If "a" points to a null pointer, then allocate space of "size" bytes unless
 * 	"size" is 0.  Anything allocated will be initialized to 0.
 * Else If "size" is zero, then free the pointer pointed to by "a"
 * Else, replace pointer pointed to by "a" with new pointer with "size" bytes,
 *	with same contents of pointer pointed to by "a" upto "oldsize" bytes.
 *	Any bytes not set from the old pointer will be initialized to "0"
 * @param jlc: The library context.
 * @param a: Pointer to the memory to change.
 * @param size: How many bytes to allocate.
 * @param oldsize: How many bytes where allocated before [ 0 is default ].
*/
void jl_me_alloc(jl_t* jlc, void **a, uint32_t size, uint32_t oldsize) {
	if(*a == NULL) {
		_jl_me_alloc_malloc(jlc, a, size);
	}else if(size == 0) {
		free(*a);
		*a = NULL;
	}else{
		*a = jl_me_realloc__(jlc->_jlc, *a, size);
	//	memset(newptr + oldsize, 0, size - oldsize);
	//	*a = newptr;
	}
}

/**
 * Clears an already existing string and resets it's cursor value.
 * @param pa: string to clear.
*/
void jl_me_strt_clear(strt pa) {
	pa->curs = 0;
	jl_me_clr(pa->data, pa->size + 1);
}

/**
 * Allocates a "strt" of size "size" and returns it.
 * @param size: How many bytes/characters to allocate.
 * @param type: whether automatic freeing should be allowed or not.
 * @returns: A new initialized "strt".
*/
strt jl_me_strt_make(u32_t size) {
	strt a = malloc(sizeof(strt_t));
	a->data = malloc(size+1);
	a->size = size;
	a->curs = 0;
	jl_me_strt_clear(a);
	return a;
}

/**
 * frees a "strt".
 * @param pstr: the "strt" to free
*/
void jl_me_strt_free(strt pstr) {
	free(pstr->data);
	free(pstr);
}

/**
 * convert "string" into a (temporary) strt and return it.
 * @param string: String to convert
 * @returns: new "strt" with same contents as "string".
*/
strt jl_me_strt_mkfrom_str(str_t string) {
	uint32_t size = strlen(string);
	strt a = jl_me_strt_make(size);
	jl_me_copyto(string, a->data, size);
	a->data[size] = '\0';
	return a;
}

/**
 *
*/
strt jl_me_strt_mkfrom_data(jl_t* jlc, uint32_t size, void *data) {
	strt a = malloc(sizeof(strt_t));
	a->data = jl_me_copy(jlc, data, size);
	a->size = size;
	a->curs = 0;
	a->data[size] = '\0';
	return a;
}

/**
 * Returns the byte at the cursor of a "strt".
 * @param pstr: the string to read.
 * @returns: byte at the cursor of "pstr"
*/
u8_t jl_me_strt_byte(strt pstr) {
	_jl_me_truncate_curs(pstr);
	return pstr->data[pstr->curs];
}

/**
 * Get the byte at the cursor of "strt", and increment the cursor value
**/
u8_t jl_me_strt_get_byte(strt pstr) {
	uint8_t* area = ((void*)pstr->data) + pstr->curs;
	jl_me_strt_increment(pstr, 1);
	return *area;
}

/**
 * Get data at the cursor of "pstr", and increment the cursor value.
 * @param pstr: the string to read.
 * @param varsize: the size of variable pointed to by "var" in bytes (1,2,4,8).
 * @param var: the variable to save the data to.
**/
void jl_me_strt_loadto(strt pstr, u32_t varsize, void* var) {
	void* area = ((void*)pstr->data) + pstr->curs;

	jl_me_copyto(area, var, varsize);
	jl_me_strt_increment(pstr, varsize);
}

/**
 * Add variable data at the cursor of "pstr", and increment the cursor value.
 * @param pstr: the string to read.
 * @param: pval: the integer to add to "pstr"
*/
void jl_me_strt_saveto(strt pstr, u32_t varsize, const void* var) {
	void* area = ((void*)pstr->data) + pstr->curs;

	jl_me_copyto(var, area, varsize);
	jl_me_strt_increment(pstr, varsize);
}

/**
 * Add a byte ( "pvalue" ) at the cursor in a string ( "pstr" ), then increment
 * the cursor value [ truncated to the string size ]
 * @param pstr: The string to add a byte to.
 * @param pvalue: the byte to add to "pstr".
*/
void jl_me_strt_add_byte(strt pstr, u8_t pvalue) {
	_jl_me_truncate_curs(pstr);
	pstr->data[pstr->curs] = pvalue;
	jl_me_strt_increment(pstr, 1);
}

/**
 * Delete byte at cursor in string.
*/
void jl_me_strt_delete_byte(jl_t *jlc, strt pstr) {
	int i;
	
	if(pstr->size == 0) return;
	for(i = pstr->curs; i < pstr->size - 1; i++)
		pstr->data[i] = pstr->data[i+1];
	pstr->size--;
	pstr->data[pstr->size] = '\0';
	pstr->data = jl_me_realloc__(jlc->_jlc, pstr->data, pstr->size);
	_jl_me_truncate_curs(pstr);
}

/**
 * Inserts a byte at cursor in string pstr.  If not enough size is available,
 * the new memory will be allocated. Value 0 is treated as null byte - dont use.
*/
void jl_me_strt_insert_byte(jl_t *jlc, strt pstr, uint8_t pvalue) {
	if(strlen((char*)pstr->data) == pstr->size) {
		pstr->size++;
		pstr->data =
			jl_me_realloc__(jlc->_jlc, pstr->data, pstr->size);
	}
	if(jl_me_strt_byte(pstr) == '\0') {
		jl_me_strt_add_byte(pstr, pvalue);
		jl_me_strt_add_byte(pstr, '\0');
	}else{
		int i;
		uint32_t pstr_len = pstr->size;
		pstr->data[pstr_len] = '\0';
		for(i = pstr_len - 1; i > pstr->curs; i--)
			pstr->data[i] = pstr->data[i-1];
		jl_me_strt_add_byte(pstr, pvalue);
	}
}

/**
 * At the cursor in string 'a' replace 'bytes' bytes of 'b' at it's cursor.
 * jl_me_strt_strt(jlc, { data="HELLO", curs=2 }, { "WORLD", curs=2 }, 2);
 *  would make 'a'
 *	"HELLO"-"LL" = "HE\0\0O"
 *	"WORLD"[2] and [3] = "RL"
 *	"HE"+"RL"+"O" = "HERLO"
 * @param jlc: library context
 * @param a: string being modified
 * @param b: string being copied into 'a'
 * @param bytes: the number of bytes to copy over
 */
void jl_me_strt_strt(jl_t *jlc, strt a, strt b, uint64_t bytes) {
	int32_t i;
	uint32_t size = a->size;
	uint32_t sizeb = a->curs + bytes;

	if(a == NULL) {
		_jl_fl_errf(jlc->_jlc, "NULL A STRING");
		jl_sg_kill(jlc);
	}else if(b == NULL) {
		_jl_fl_errf(jlc->_jlc, "NULL B STRING");
		jl_sg_kill(jlc);
	}
	if(sizeb > size) size = sizeb;
	a->data = jl_me_realloc__(jlc->_jlc, a->data, size);
	for(i = 0; i < bytes; i++) {
		a->data[i + a->curs] = b->data[i + b->curs];
	}
	a->size = size;
	a->data[a->size] = '\0';
}

/**
 * Add string "b" at the end of string "a"
 * @param 'jlc': library context
 * @param 'a': string being modified
 * @param 'b': string being appended onto "a"
 */
void jl_me_strt_merg(jl_t *jlc, strt a, strt b) {
	a->curs = a->size;
	jl_me_strt_strt(jlc, a, b, b->size);
}

/**
 * Truncate the string to a specific length.
 * @param 'jlc': library context
 * @param 'a': string being modified
 * @param 'size': size to truncate to.
 */
void jl_me_strt_trunc(jl_t *jlc, strt a, uint32_t size) {
	a->curs = 0;
	a->size = size;
	a->data = jl_me_realloc__(jlc->_jlc, a->data, a->size);
}

// Print a number out as a string and return it (Type=STRT_TEMP)
strt jl_me_strt_fnum(i32_t a) {
	strt new = jl_me_strt_make(30);
	sprintf((void*)new->data, "%d", a);
	return new;
}

char* jl_me_string_fnum(jl_t* jlc, int32_t a) {
	char *string = NULL;
	jl_me_alloc(jlc, (void**)&string, 30, 0);
	sprintf(string, "%d", a);
	return string;
}

const char* jl_me_string_fnum_tmp(jl_t* jlc, int32_t a) {
	jvct_t * _jlc = jlc->_jlc;
	jl_me_clr((void*)_jlc->me.temp_buff, 30);
	sprintf((void*)_jlc->me.temp_buff, "%d", a);
	return (void*)_jlc->me.temp_buff;
}

/**
 * Get a string ( char * ) from a 'strt'.  Then, free the 'strt'.
 * @param jlc: The library context.
 * @param a: the 'strt' to convert to a string ( char * )
 * @returns: a new string (char *) with the same contents as "a"
*/
char* jl_me_string_fstrt(jl_t* jlc, strt a) {
	char *rtn = jl_me_copy(jlc, (char*)(a->data), a->size);
	jl_me_strt_free(a);
	return rtn;
}

/**
 * Format a string.
**/
m_str_t jl_me_format(jl_t* jlc, str_t format, ... ) {
	va_list arglist;

	va_start( arglist, format );
	vsprintf( jlc->temp, format, arglist );
	va_end( arglist );
	return jlc->temp;
}

/**
 * Print to a string.
 * @param jlc: The library context.
 * @param string: The string to print to.
 * @param format: The format string
 * @param var: The variable to put into the format string.
 * @param n: The number of allocated bytes in "string"
 * @returns: 1 if it hit the limit, 0 if there's still room.
**/
uint8_t jl_me_string_print(jl_t* jlc, char *string, const char* format,
	const char *var, u64_t n)
{
	jvct_t * _jlc = jlc->_jlc;

	if((!format) || (!format[0])) return 0;
	jl_me_clr((void*)_jlc->me.temp_buff, 30);
	sprintf((void*)_jlc->me.temp_buff, format, var);
	strncat(string, (void*)_jlc->me.temp_buff, n - strlen(string));
	return (strlen(string) > n - 1);
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
 * Tests if the next thing in array script is equivalent to particle.
 * @param script: The array script.
 * @param particle: The phrase to look for.
 * @return 1: If particle is at the cursor.
 * @return 0: If particle is not at the cursor.
*/
u8_t jl_me_test_next(strt script, str_t particle) {
	char * point = (void*)script->data + script->curs; //the cursor
	char * place = strstr(point, particle); //search for partical
	if(place == point) {//if partical at begining return true otherwise false
		return 1;
	}else{
		return 0;
	}
}

/**
 * Returns string "script" truncated to "psize" or to the byte "end" in
 * "script".  It is dependant on which happens first.
 * @param jlc: The library context.
 * @param script: The array script.
 * @param end: byte to end at if found.
 * @param psize: maximum size of truncated "script" to return.
 * @returns: a "strt" that is a truncated array script.
*/
strt jl_me_read_upto(jl_t* jlc, strt script, u8_t end, u32_t psize) {
	strt compiled = jl_me_strt_make(psize);
	compiled->curs = 0;
	while((jl_me_strt_byte(script) != end) && (jl_me_strt_byte(script) != 0)) {
		strncat((void*)compiled->data,
			(void*)script->data + script->curs, 1);
		script->curs++;
		compiled->curs++;
	}
	jl_me_strt_trunc(jlc, compiled, compiled->curs);
	return compiled;
}

/**
 * Save a pointer to a buffer and get the previous value of it.
 * @param jlc: The library context.
 * @param which: A number from 1-15, specifies which temporary pointer to use.
 * @param tmp_ptr: The new pointer to save to the buffer.
 * @returns: The old/previous value of the pointer.
**/
void *jl_me_tmp_ptr(jl_t* jlc, uint8_t which, void *tmp_ptr) {
	jvct_t* _jlc = jlc->_jlc;
	void *rtn = _jlc->me.tmp_ptr[which];

	_jlc->me.tmp_ptr[which] = tmp_ptr;
	return rtn;
}

/************************/
/***  ETOM Functions  ***/
/************************/

jvct_t* _jl_me_init(void) {
	//Create a context for the currently loaded program
	jvct_t* _jlc = NULL;
	_jl_me_init_alloc((void**)&_jlc, sizeof(jvct_t));
	m_u8_t i;

	//Set the current program ID to 0[RESERVED DEFAULT]
	_jlc->cprg = 0;
	//Prepare user data structure
	_jlc->jlc = NULL;
	_jl_me_init_alloc((void**)&(_jlc->jlc), sizeof(jl_t));
	_jlc->jlc->_jlc = _jlc;
	//Make sure that non-initialized things aren't used
	_jlc->has.graphics = 0;
	_jlc->has.fileviewer = 0;
	_jlc->has.filesys = 0;
	_jlc->has.input = 0;
	_jlc->me.status = JL_STATUS_GOOD;
	// Clear temporary pointer buffer.
	for(i = 0; i < 16; i++) _jlc->me.tmp_ptr[i] = NULL;

/*	printf("u %p, %p, c %p,%p\n",
		_jlc->jlc, ((jvct_t *)(_jlc->jlc->_jlc))->sg.usrd,
		_jlc, _jlc->jlc->_jlc);*/
//	g_vmap_list = cl_list_create();
	return _jlc;
}

void _jl_me_kill(jvct_t* _jlc) {
	free(_jlc);
//	cl_list_destroy(g_vmap_list);
}
