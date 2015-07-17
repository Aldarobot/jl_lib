/*
 * me: memory manager
 * 
 * A simple memory library.  Includes creating variables, setting and
 * getting variables, and doing simple and complicated math functions on
 * the variables.  Has a specialized string type.
*/

#include "header/jl_pr.h"
#include <malloc.h>

//MEMORY
#define JLVM_MEMORY_SIZE 1000*256*256*16

uint8_t *memory;

//Return Amount Of Total Memory Being Used
uint32_t jl_me_tbiu(void) {
	struct mallinfo mi;
	mi = mallinfo();
	return mi.uordblks;
}

static void *_jl_me_hydd_allc(jvct_t* _jlc, void *a, uint32_t size) {
	if((a = realloc(a, size)) == NULL) {
		jl_sg_kill(_jlc->jlc, "realloc() memory error!");
	}
	return a;
}

//Update Memory To Size "kb" kilobytes
static inline void _jlvm_jl_me_resz(jvct_t* _jlc, uint32_t kb) {
	printf("[jl_me] updating memory size....\n");
	printf("[jl_me] total size = %d\n", jl_me_tbiu());
	printf("[jl_me] update to =  %d\n", kb);
	printf("[jl_me] SDL Size =   %d\n", 0);
	printf("[jl_me] JLVM Size =  %d\n", 0);
	printf("[jl_me] NonLib Size =%d\n", 0);
	printf("[jl_me] Unknown =    %d\n", 0);
	memory = _jl_me_hydd_allc(_jlc, memory, (1000*kb));
	malloc_trim(0); //Remove Free Memory
}

/**
 * Clear memory pointed to by "mem" of size "size"
 * @param pmem: memory to clear
 * @param size: size of "mem"
*/
void jl_me_clr(void *pmem, uint64_t size) {
	uint8_t *fmem = pmem;
	uint64_t i;
	for(i = 0; i < size; i++) {
		fmem[i] = 0;
	}
}

static void _jl_me_alloc_malloc(jl_t* jlc, void **a, uint32_t size) {
	if(size == 0)
		jl_sg_kill(jlc, "Double Free or free on NULL pointer");
	*a = malloc(size);
	if(*a == NULL)
		jl_sg_kill(jlc, "jl_me_alloc: Out Of Memory!");
	jl_me_clr(*a, size);
}

/**
 * Copy "size" bytes of "src" to a new pointer of "size" bytes and return it.
 * @param jlc: The library context.
 * @param src: source buffer
 * @param size: # of bytes of "src" to copy to "dst"
 * @returns: a new pointer to 
*/
void * jl_me_copy(jl_t* jlc, const void *src, size_t size) {
	void *dest = NULL; _jl_me_alloc_malloc(jlc, &dest, size);
	void *rtn = memcpy(dest, src, size);
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
		void *old = *a;
		void *new = jl_me_copy(jlc, old, size);
		*a = new;
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
strt jl_me_strt_make(u32t size, u08t type) {
	strt a = malloc(sizeof(strt_t));
	a->data = malloc(size+1);
	a->size = size;
	a->type = type;
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
strt jl_me_strt_c8ts(const char *string) {
	uint32_t size = strlen(string);
	strt a = jl_me_strt_make(size, STRT_TEMP);
	int i;
	for( i = 0; i < size; i++) {
		a->data[i] = string[i];
	}
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
	a->type = STRT_KEEP;
	a->data[size] = '\0';
	return a;
}

void _jl_me_truncate_curs(strt pstr) {
	if(pstr->curs > pstr->size) {
		pstr->curs = pstr->size;
	}
}

/**
 * Returns the byte at the cursor of a "strt".
 * @param pstr: the string to read.
 * @returns: byte at the cursor of "pstr"
*/
u08t jl_me_strt_byte(strt pstr) {
	_jl_me_truncate_curs(pstr);
	return pstr->data[pstr->curs];
}

/**
 * Add a byte ( "pvalue" ) at the cursor in a string ( "pstr" ), then increment
 * the cursor value [ truncated to the string size ]
 * @param pstr: The string to add a byte to.
 * @param pvalue: the byte to add to "pstr".
*/
void jl_me_strt_add_byte(strt pstr, u08t pvalue) {
	_jl_me_truncate_curs(pstr);
	pstr->data[pstr->curs] = pvalue;
	pstr->curs++;
	_jl_me_truncate_curs(pstr);
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
	pstr->data =
		_jl_me_hydd_allc(jlc->_jlc, pstr->data, pstr->size+1);
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
			_jl_me_hydd_allc(jlc->_jlc, pstr->data, pstr->size+1);
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
		jl_sg_kill(jlc, "NULL A STRING");
	}else if(b == NULL) {
		jl_sg_kill(jlc, "NULL B STRING");
	}
	if(sizeb > size) size = sizeb;
	a->data = _jl_me_hydd_allc(jlc->_jlc, a->data, size + 1);
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
	a->size = size;
	a->data = _jl_me_hydd_allc(jlc->_jlc, a->data, a->size + 1);
}

//Print a number out as a string and return it (Type=STRT_TEMP)
strt jl_me_strt_fnum(s32t a) {
	strt new = jl_me_strt_make(30, STRT_TEMP);
	sprintf((void*)new->data, "%d", a);
	return new;
}

char* jl_me_string_fnum(jl_t* jlc, int32_t a) {
	char *string = malloc(30);
	jl_me_clr(string, 30);
	sprintf(string, "%d", a);
//	string = _jl_me_hydd_allc(jlc->_jlc, string, strlen(string + 1));
	return string;
}

/**
 * Get a string ( char * ) from a 'strt'.  Then, free the 'strt'.
 * @param jlc: The library context.
 * @param a: the 'strt' to convert to a string ( char * )
 * @returns: a new string (char *) with the same contents as "a"
*/
char* jl_me_string_fstrt(jl_t* jlc, strt a) {
	char *rtn = jl_me_copy(jlc, (char*)(a->data), a->size + 1);
	jl_me_strt_free(a);
	return rtn;
}

/**
 * Generate a random integer from 0 to "a"
 * @param a: 1 more than the maximum # to return
 * @returns: a random integer from 0 to "a"
*/
u32t jl_me_random_int(u32t a) {
	return rand()%a;
}

/**
 * Tests if the next thing in array script is equivalent to particle.
 * @param script: The array script.
 * @param particle: The phrase to look for.
 * @return 1: If particle is at the cursor.
 * @return 0: If particle is not at the cursor.
*/
u08t jl_me_test_next(strt script, strt particle) {
	char * point = (void*)script->data + script->curs;//the cursor
	char * place = strstr(point, (void*)particle->data);//search for partical
	printf("%s, %s\n", point, particle->data);
	if(place == point) {//if partical at begining return true otherwise false
		return 1;
	}else{
		return 0;
	}
}

/*
 * Returns string "script" truncated to "psize" or to the byte "end" in
 * "script".  It is dependant on which happens first. (Type=STRT_KEEP)
 * @param script: The array script.
 * @param end: byte to end at if found.
 * @param psize: maximum size of truncated "script" to return.
 * @returns: a "strt" that is a truncated array script.
*/
strt jl_me_read_upto(strt script, u08t end, u32t psize) {
	strt compiled = jl_me_strt_make(psize, STRT_KEEP);
	while((jl_me_strt_byte(script) != end) && (jl_me_strt_byte(script) != 0)) {
		strncat((void*)compiled->data,
			(void*)script->data + script->curs, 1);
		script->curs++;
	}
//	strcat(compiled, "\0");
	return compiled;
}

//struct cl_list *g_vmap_list;

jvct_t* _jl_me_init(void) {
	//Create a context for the currently loaded program
	jvct_t* _jlc = malloc(sizeof(jvct_t));
	//Set the current program ID to 0[RESERVED DEFAULT]
	_jlc->cprg = 0;
	//Prepare user data structure
	_jlc->jlc = malloc(sizeof(jl_t));
	_jlc->jlc->_jlc = _jlc;
	//Make sure that non-initialized things aren't used
	_jlc->has.graphics = 0;
	_jlc->has.fileviewer = 0;

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
