/*
 * (c) Jeron A. Lau
 * jlvm_siop is a library for printing to various terminals.
*/

#include "header/jlvmpu.h"

void jal5_siop_cplo(int8_t offs, char * this, char * print) {
#if PLATFORM==0
	printf("[%s] %s\n", this, print);
#else
	SDL_Log("[%s] %s\n", this, print);
#endif
}

void siop_prnt_lwst(int8_t offs, strt this, strt print) {
	jal5_siop_cplo(offs, (void *)this->data, (void *)print->data);
}

void _jal5_siop_init(jvct_t * pjct) {
	#if PLATFORM == 1
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
}
