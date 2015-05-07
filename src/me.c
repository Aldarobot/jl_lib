/*
 * me: memory manager
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

void *_jl_me_hydd_allc(jvct_t* pjlc, void *a, uint32_t size) {
	if((a = realloc(a, size)) == NULL) {
		jl_sg_die(pjlc, "realloc() memory error!");
	}
	return a;
}

//Update Memory To Size "kb" kilobytes
static inline void _jlvm_jl_me_resz(jvct_t* pjlc, uint32_t kb) {
	printf("[jl_me] updating memory size....\n");
	printf("[jl_me] total size = %d\n", jl_me_tbiu());
	printf("[jl_me] update to =  %d\n", kb);
	printf("[jl_me] SDL Size =   %d\n", 0);
	printf("[jl_me] JLVM Size =  %d\n", 0);
	printf("[jl_me] NonLib Size =%d\n", 0);
	printf("[jl_me] Unknown =    %d\n", 0);
	memory = _jl_me_hydd_allc(pjlc, memory, (1000*kb));
	malloc_trim(0); //Remove Free Memory
}

void * jl_me_alloc(u32t size) {
	u08t * a = malloc(size);
	u32t i;
	for(i = 0; i < size; i++) {
		a[i] = 0;
	}
	return a;
}

void jl_me_strt_clear(strt pa) {
	pa->curs = 0;
	int i;
	for( i = 0; i < pa->size + 1; i++) {
		pa->data[i] = '\0';
	}
}

strt jl_me_strt_make(u32t size, u08t type) {
	strt a = malloc(sizeof(strt_t));
	a->data = malloc(size+1);
	a->size = size;
	a->type = type;
	a->curs = 0;
	jl_me_strt_clear(a);
	return a;
}

void jl_me_strt_free(strt pstr) {
	free(pstr->data);
	free(pstr);
}

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

strt jl_me_strt_mkfrom_data(uint32_t size, void *data) {
	strt a = malloc(sizeof(strt_t));
	a->data = malloc(size + 1);
	a->size = size;
	a->curs = 0;
	a->type = STRT_KEEP;
	int i;
	for( i = 0; i < size; i++) {
		a->data[i] = ((uint8_t *)data)[i];
	}
	a->data[size] = '\0';
	return a;
}

void _jl_me_truncate_curs(strt pstr) {
	if(pstr->curs > pstr->size) {
		pstr->curs = pstr->size;
	}
}

u08t jl_me_strt_byte(strt pstr) {
	_jl_me_truncate_curs(pstr);
	return pstr->data[pstr->curs];
}

void jl_me_strt_add_byte(strt pstr, u08t pvalue) {
	_jl_me_truncate_curs(pstr);
	pstr->data[pstr->curs] = pvalue;
	pstr->curs++;
	_jl_me_truncate_curs(pstr);
}

/**
 * Delete byte at cursor in string.
*/
void jl_me_strt_delete_byte(jl_t *pusr, strt pstr) {
	int i;
	
	if(pstr->size == 0) return;
	for(i = pstr->curs; i < pstr->size - 1; i++)
		pstr->data[i] = pstr->data[i+1];
	pstr->size--;
	pstr->data[pstr->size] = '\0';
	pstr->data =
		_jl_me_hydd_allc(pusr->pjlc, pstr->data, pstr->size+1);
	_jl_me_truncate_curs(pstr);
}

/**
 * Inserts a byte at cursor in string pstr.  If not enough size is available,
 * the new memory will be allocated. Value 0 is treated as null byte - dont use.
*/
void jl_me_strt_insert_byte(jl_t *pusr, strt pstr, uint8_t pvalue) {
	if(strlen((char*)pstr->data) == pstr->size) {
		pstr->size++;
		pstr->data =
			_jl_me_hydd_allc(pusr->pjlc, pstr->data, pstr->size+1);
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

void _jal5_jl_me_vary_make(u32t vari, u08t size) {
	
}

/**
 * Replace "bytes" bytes of string "b" at the point "p" of string "a" 
 * @param 'pusr': library context
 * @param 'a': string being modified
 * @param 'b': string being appended onto "a"
 * @param 'p': the place in a to start copying b
 * @param 'bytes': the number of bytes to copy over
 */
void jl_me_strt_strt(jl_t *pusr, strt a, strt b, uint64_t p, uint64_t bytes) {
	if(a == NULL) {
		jl_sg_die(pusr->pjlc, "NULL A STRING");
	}else if(b == NULL) {
		jl_sg_die(pusr->pjlc, "NULL B STRING");
	}
	a->data = _jl_me_hydd_allc(pusr->pjlc, a->data, p + bytes + 1);
	int i;
	for(i = 0; i < bytes; i++) {
		a->data[i+p] = b->data[i];
	}
	a->size = p + bytes;
	a->data[a->size] = '\0';
}

/**
 * Add string "b" at the end of string "a"
 * @param 'pusr': library context
 * @param 'a': string being modified
 * @param 'b': string being appended onto "a"
 */
void jl_me_strt_merg(jl_t *pusr, strt a, strt b) {
	jl_me_strt_strt(pusr, a, b, a->size, b->size);
}

/**
 * Truncate the string to a specific length.
 * @param 'pusr': library context
 * @param 'a': string being modified
 * @param 'size': size to truncate to.
 */
void jl_me_strt_trunc(jl_t *pusr, strt a, uint32_t size) {
	a->size = size;
	a->data = _jl_me_hydd_allc(pusr->pjlc, a->data, a->size + 1);
}

char* jl_me_string_fnum(jl_t* pusr, int32_t a) {
	char *string = malloc(30);
	int i;
	for(i = 0; i < 30; i++) {
		string[i] = 0;
	}
	sprintf(string, "%d", a);
//	string = _jl_me_hydd_allc(pusr->pjlc, string, strlen(string + 1));
	return string;
}

//Print a number out as a string and return it (Type=STRT_TEMP)
strt jl_me_strt_fnum(s32t a) {
	strt new = jl_me_strt_make(30, STRT_TEMP);
	int i;
	for(i = 0; i < 30; i++) {
		new->data[i] = 0;
	}
	sprintf((void*)new->data, "%d", a);
	return new;
}

u32t jl_me_random_int(u32t a) {
	return rand()%a;
}

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

strt jl_me_read_upto(strt script, u08t end, u32t psize) {
	int i;
	strt compiled = jl_me_strt_make(psize, STRT_KEEP);
	for(i = 0; i < psize; i++) {
		compiled->data[i] = '\0';
	}
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
	jvct_t* jprg = malloc(sizeof(jvct_t));
	//Set the current program ID to 0[RESERVED DEFAULT]
	jprg->cprg = 0;
	//Prepare user data structure
	jprg->sg.usrd = malloc(sizeof(jl_t));
	jprg->sg.usrd->pjlc = jprg;

/*	printf("u %p, %p, c %p,%p\n",
		jprg->sg.usrd, ((jvct_t *)(jprg->sg.usrd->pjlc))->sg.usrd,
		jprg, jprg->sg.usrd->pjlc);*/
//	g_vmap_list = cl_list_create();
	return jprg;
}

void _jl_me_kill(jvct_t* jprg) {
	free(jprg);
//	cl_list_destroy(g_vmap_list);
}
