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
		jlvm_dies(pjlc, Strt("realloc() memory error!"));
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
	strt a = malloc(sizeof(strt));
	a->data = malloc(size+1);
	a->size = size;
	a->type = type;
	jl_me_strt_clear(a);
	return a;
}

void jl_me_strt_free(strt pstr) {
	free(pstr->data);
	free(pstr);
}

strt jl_me_strt_c8ts(char *string) {
	u32t size = strlen(string);
	strt a = malloc(sizeof(strt)+1);
	a->data = malloc(size);
	a->size = size;
	a->curs = 0;
	a->type = STRT_TEMP;
	int i;
	for( i = 0; i < size; i++) {
		a->data[i] = string[i];
	}
	a->data[size] = '\0';
	return a;
}

strt jl_me_strt_mkfrom_data(uint32_t size, void *data) {
	strt a = malloc(sizeof(strt)+1);
	a->data = malloc(size);
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

void _jal5_jl_me_vary_make(u32t vari, u08t size) {
	
}

//return a string that contains a at beginning followed by b
strt jl_me_strt_merg(strt a, strt b, u08t type) {
/*	if(a == NULL) {
		jlvm_dies("NULL A STRING");
	}else if(b == NULL) {
		jlvm_dies("NULL B STRING");
	}*/
	strt str = jl_me_strt_make(a->size + b->size, type);
	int i;
	for(i = 0; i < a->size; i++) {
		str->data[i] = a->data[i];
	}
	for(i = 0; i < b->size; i++) {
		str->data[i+a->size] = b->data[i];
	}
	str->data[a->size + b->size] = '\0';
	return str;
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
	#if JLVM_DEBUG >= JLVM_DEBUG_INTENSE
	printf("u %p, %p, c %p,%p\n",
		jprg->sg.usrd, ((jvct_t *)(jprg->sg.usrd->pjlc))->sg.usrd,
		jprg, jprg->sg.usrd->pjlc);
	#endif
//	g_vmap_list = cl_list_create();
	return jprg;
}

void _jl_me_kill(jvct_t* jprg) {
	free(jprg);
//	cl_list_destroy(g_vmap_list);
}
