#include "jl_pr.h"

//
// Internal Functions
//

static void _jl_me_truncate_curs(strt pstr) {
	if(pstr->curs > pstr->size) {
		pstr->curs = pstr->size;
	}
}

static void jl_me_strt_increment(strt pstr, u8_t incrementation) {
	pstr->curs += incrementation;
	_jl_me_truncate_curs(pstr);
}

//
// Exported Functions
//

/**
 * Clears an already existing string and resets it's cursor value.
 * @param pa: string to clear.
*/
void jl_me_strt_clear(jl_t* jl, strt pa) {
	pa->curs = 0;
	jl_me_strt_resize(jl, pa, 0);
	jl_mem_clr(pa->data, pa->size + 1);
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
	jl_mem_clr(a->data, a->size + 1);
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
 *
*/
strt jl_me_strt_mkfrom_data(jl_t* jl, u32_t size, const void *data) {
	strt a = jl_me_strt_make(size);
	jl_mem_copyto(data, a->data, size);
	a->data[size] = '\0'; // Null terminalte
	return a;
}

/**
 * Converts "string" into a strt and returns it.
 * @param string: String to convert
 * @returns: new "strt" with same contents as "string".
*/
strt jl_me_strt_mkfrom_str(str_t string) {
	return jl_me_strt_mkfrom_data(NULL, strlen(string), string);
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

	jl_mem_copyto(area, var, varsize);
	jl_me_strt_increment(pstr, varsize);
}

/**
 * Add variable data at the cursor of "pstr", and increment the cursor value.
 * @param pstr: the string to read.
 * @param: pval: the integer to add to "pstr"
*/
void jl_me_strt_saveto(strt pstr, u32_t varsize, const void* var) {
	void* area = ((void*)pstr->data) + pstr->curs;

	jl_mem_copyto(var, area, varsize);
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
void jl_me_strt_delete_byte(jl_t *jl, strt pstr) {
	int i;
	
	if(pstr->size == 0) return;
	for(i = pstr->curs; i < pstr->size - 1; i++)
		pstr->data[i] = pstr->data[i+1];
	pstr->size--;
	pstr->data[pstr->size] = '\0';
	pstr->data = jl_mem(jl, pstr->data, pstr->size);
	_jl_me_truncate_curs(pstr);
}

void jl_me_strt_resize(jl_t *jl, strt pstr, u32_t newsize) {
	pstr->size = newsize;
	pstr->data = jl_mem(jl, pstr->data, pstr->size);
}

/**
 * Inserts a byte at cursor in string pstr.  If not enough size is available,
 * the new memory will be allocated. Value 0 is treated as null byte - dont use.
*/
void jl_me_strt_insert_byte(jl_t *jl, strt pstr, uint8_t pvalue) {
	if(strlen((char*)pstr->data) == pstr->size) {
		jl_me_strt_resize(jl, pstr, pstr->size + 1);
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

void jl_me_strt_insert_data(jl_t *jl, strt pstr, void* data, u32_t size) {
//	int i;
//	uint8_t* data2 = data;

	// Add size
	jl_me_strt_resize(jl, pstr, pstr->size + size);
	// Copy data.
	jl_mem_copyto(data, pstr->data + pstr->curs, size);
	// Increase cursor
	pstr->curs+=size;
//	for(i = 0; i < size; i++) {
//		jl_me_strt_insert_byte(jl, pstr, data2[i]);
//	}
}

/**
 * At the cursor in string 'a' replace 'bytes' bytes of 'b' at it's cursor.
 * jl_me_strt_strt(jl, { data="HELLO", curs=2 }, { "WORLD", curs=2 }, 2);
 *  would make 'a'
 *	"HELLO"-"LL" = "HE\0\0O"
 *	"WORLD"[2] and [3] = "RL"
 *	"HE"+"RL"+"O" = "HERLO"
 * @param jl: library context
 * @param a: string being modified
 * @param b: string being copied into 'a'
 * @param bytes: the number of bytes to copy over
 */
void jl_me_strt_strt(jl_t *jl, strt a, const strt b, uint64_t bytes) {
	int32_t i;
	uint32_t size = a->size;
	uint32_t sizeb = a->curs + bytes;

	if(a == NULL) {
		jl_print(jl, "jl_me_strt_strt: NULL A STRING");
		exit(-1);
	}else if(b == NULL) {
		jl_print(jl, "jl_me_strt_strt: NULL B STRING");
		exit(-1);
	}
	if(sizeb > size) size = sizeb;
	a->data = jl_mem(jl, a->data, size + 1);
	for(i = 0; i < bytes; i++) {
		a->data[i + a->curs] = b->data[i + b->curs];
	}
	a->size = size;
	a->data[a->size] = '\0';
}

/**
 * Add string "b" at the end of string "a"
 * @param 'jl': library context
 * @param 'a': string being modified
 * @param 'b': string being appended onto "a"
 */
void jl_me_strt_merg(jl_t *jl, strt a, const strt b) {
	a->curs = a->size;
	jl_me_strt_strt(jl, a, b, b->size);
}

/**
 * Truncate the string to a specific length.
 * @param 'jl': library context
 * @param 'a': string being modified
 * @param 'size': size to truncate to.
 */
void jl_me_strt_trunc(jl_t *jl, strt a, uint32_t size) {
	a->curs = 0;
	a->size = size;
	a->data = jl_mem(jl, a->data, a->size + 1);
}

/**
 * Get a string ( char * ) from a 'strt'.  Then, free the 'strt'.
 * @param jl: The library context.
 * @param a: the 'strt' to convert to a string ( char * )
 * @returns: a new string (char *) with the same contents as "a"
*/
char* jl_me_string_fstrt(jl_t* jl, strt a) {
	char *rtn = (void*)a->data;
	free(a);
	return rtn;
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
 * @param jl: The library context.
 * @param script: The array script.
 * @param end: byte to end at if found.
 * @param psize: maximum size of truncated "script" to return.
 * @returns: a "strt" that is a truncated array script.
*/
strt jl_me_read_upto(jl_t* jl, strt script, u8_t end, u32_t psize) {
	strt compiled = jl_me_strt_make(psize);
	compiled->curs = 0;
	while((jl_me_strt_byte(script) != end) && (jl_me_strt_byte(script) != 0)) {
		strncat((void*)compiled->data,
			(void*)script->data + script->curs, 1);
		script->curs++;
		compiled->curs++;
	}
	jl_me_strt_trunc(jl, compiled, compiled->curs);
	return compiled;
}
