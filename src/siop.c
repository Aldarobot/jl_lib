/*
 * (c) Jeron A. Lau
 * jlvm_siop is a library for printing to various terminals.
*/

#include "header/jlvmpu.h"

void jal5_siop_cplo(sgrp_user_t* pusr, char * print) {
	int i;
	for(i = 0; i < ((jvct_t *)pusr->pjct)->Siop.offs; i++) {
		printf(" ");
	}
#if PLATFORM==0
	printf("[%s] %s",
		((jvct_t *)pusr->pjct)->Siop.head[
			((jvct_t *)pusr->pjct)->Siop.offs],
		print);
#else
	SDL_Log("[%s] %s",
		((jvct_t *)pusr->pjct)->Siop.head[
			((jvct_t *)pusr->pjct)->Siop.offs],
		print);
#endif
}

void siop_offs_sett(sgrp_user_t* pusr, char * this) {
	if(this == NULL) {
		return;
	}
	int i;
	for(i = 0; i < ((jvct_t *)pusr->pjct)->Siop.offs; i++) {
		if(strcmp(this, ((jvct_t *)pusr->pjct)->Siop.head[i]) == 0) {
			((jvct_t *)pusr->pjct)->Siop.offs = i;
			jal5_siop_cplo(pusr, "");
			return;
		}
	}
	((jvct_t *)pusr->pjct)->Siop.offs++;
	for(i = 0; i < 4; i++) {
		((jvct_t *)pusr->pjct)->Siop.head[
			((jvct_t *)pusr->pjct)->Siop.offs][i] = this[i];
	}
	((jvct_t *)pusr->pjct)->Siop.head[
			((jvct_t *)pusr->pjct)->Siop.offs][4] = '\0';
	jal5_siop_cplo(pusr, "");
	return;
}

void siop_prnt_lwst(sgrp_user_t* pusr, strt print) {
	if(print == NULL) {
		print = Strt(STRT_NULL);
	}
	jal5_siop_cplo(pusr, (void *)print->data);
	if(print->type == STRT_TEMP) {
		amem_strt_free(print);
	}
}

void _jal5_siop_init(jvct_t * pjct) {
	#if PLATFORM == 1
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	int i, j;
	for(i = 0; i < 16; i++) {
		for(j = 0; j < 5; j++) {
			pjct->Siop.head[i][j] = '\0';
		}
	}
	siop_offs_sett(pjct->Sgrp.usrd, "JLVM");
}
