#include "header/jlvmpu.h"
#include <malloc.h>

//MEMORY
#define JLVM_MEMORY_SIZE 1000*256*256*16

uint8_t *memory;

//Return Amount Of Total Memory Being Used
uint32_t jlvm_amem_tbiu(void) {
	struct mallinfo mi;
	mi = mallinfo();
	return mi.uordblks;
}

void *_jlvm_amem_hydd_allc(jvct_t* pjct, void *a, uint32_t size) {
	if((a = realloc(a, size)) == NULL) {
		jlvm_dies(pjct, Strt("malloc() memory error!"));
	}
	return a;
}

//Update Memory To Size "kb" kilobytes
static inline void _jlvm_amem_resz(jvct_t* pjct, uint32_t kb) {
	printf("[AMEM] updating memory size....\n");
	printf("[AMEM] total size = %d\n", jlvm_amem_tbiu());
	printf("[AMEM] update to =  %d\n", kb);
	printf("[AMEM] SDL Size =   %d\n", 0);
	printf("[AMEM] JLVM Size =  %d\n", 0);
	printf("[AMEM] NonLib Size =%d\n", 0);
	printf("[AMEM] Unknown =    %d\n", 0);
	memory = _jlvm_amem_hydd_allc(pjct, memory, (1000*kb));
	malloc_trim(0); //Remove Free Memory
}

strt amem_strt_make(u32t size, u08t type) {
	strt a = malloc(sizeof(strt));
	a->data = malloc(size+1);
	a->size = size;
	a->curs = 0;
	a->type = type;
	int i;
	for( i = 0; i < size; i++) {
		a->data[i] = '\0';
	}
	return a;
}

strt amem_strt_c8ts(char *string) {
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

void _jal5_amem_vary_make(u32t vari, u08t size) {
	
}

//return a string that contains a at beginning followed by b
strt amem_strt_merg(strt a, strt b, u08t type) {
/*	if(a == NULL) {
		jlvm_dies("NULL A STRING");
	}else if(b == NULL) {
		jlvm_dies("NULL B STRING");
	}*/
	strt str = amem_strt_make(a->size + b->size, type);
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

//Get a string from a number.
strt amem_strt_fnum(s32t a) {
	strt new = amem_strt_make(30, STRT_TEMP);
	int i;
	for(i = 0; i < 30; i++) {
		new->data[i] = 0;
	}
	sprintf((void*)new->data, "%d", a);
	return new;
}

u32t amem_rand_nint(u32t a) {
	return rand()%a;
}

u08t amem_test_next(char * script, char * particle) {
	char * point = script;//make the beginning the command
	char * place = strstr(point, particle);//search for partical
	if(place == point) {//if partical at begining return true otherwise false
		return 1;
	}else{
		return 0;
	}
}

void _jal5_amem_init(void) {
//		__jal5_vmap_cprg()->Amem;
}

//struct cl_list *g_vmap_list;

//VMAP
jvct_t* _jlvm_vmap_init(void) {
	//Create a context for the currently loaded program
	jvct_t* jprg = malloc(sizeof(jvct_t));
	//Set the current program ID to 0[RESERVED DEFAULT]
	jprg->cprg = 0;
	//Prepare user data structure
	jprg->Sgrp.usrd = malloc(sizeof(sgrp_user_t));
	jprg->Sgrp.usrd->pjct = jprg;
	#if JLVM_DEBUG >= JLVM_DEBUG_INTENSE
	printf("u %p, %p, c %p,%p\n",
		jprg->Sgrp.usrd, ((jvct_t *)(jprg->Sgrp.usrd->pjct))->Sgrp.usrd,
		jprg, jprg->Sgrp.usrd->pjct);
	#endif
//	g_vmap_list = cl_list_create();
	return jprg;
}

void jlvm_vmap_kill(jvct_t* jprg) {
	free(jprg);
//	cl_list_destroy(g_vmap_list);
}
