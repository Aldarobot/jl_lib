/*
 * (c) Jeron A. Lau
 * jl/io is a library for printing to various terminals.
*/

#include "header/jl_pr.h"

static void _jl_io_indt(jl_t* pusr) {
	int i;
	for(i = 0; i < ((jvct_t *)pusr->pjlc)->io.offs; i++) {
		printf(" ");
	}
}

void jl_io_print_lowc(jl_t* pusr, char * print) {
	int i;
	if(((jvct_t *)pusr->pjlc)->io.ofs2 > 0) {
		_jl_io_indt(pusr);
		printf("[/");
		for(i = ((jvct_t *)pusr->pjlc)->io.offs -
				((jvct_t *)pusr->pjlc)->io.ofs2;
			i < ((jvct_t *)pusr->pjlc)->io.offs;
			i++)
		{
			printf( ((jvct_t *)pusr->pjlc)->io.head[i+1]);
			printf("/");
		}
		printf("\b]\n");
		((jvct_t *)pusr->pjlc)->io.ofs2 = 0;
	}else if(((jvct_t *)pusr->pjlc)->io.ofs2 < 0) {
		_jl_io_indt(pusr);
		printf("[\\");
		for(i = ((jvct_t *)pusr->pjlc)->io.offs;
			i > ((jvct_t *)pusr->pjlc)->io.offs +
				((jvct_t *)pusr->pjlc)->io.ofs2;
			i--)
		{
			printf( ((jvct_t *)pusr->pjlc)->io.head[i+1]);
			printf("\\");
		}
		printf("\b]\n");
		((jvct_t *)pusr->pjlc)->io.ofs2 = 0;
	}
	_jl_io_indt(pusr);
#if PLATFORM==0
	printf("[%s] %s",
		((jvct_t *)pusr->pjlc)->io.head[
			((jvct_t *)pusr->pjlc)->io.offs],
		print);
#else
	SDL_Log("[%s] %s",
		((jvct_t *)pusr->pjlc)->io.head[
			((jvct_t *)pusr->pjlc)->io.offs],
		print);
#endif
}

void jl_io_offset(jl_t* pusr, char * this) {
	if(this == NULL) {
		return;
	}
	int i;
	for(i = 0; i < ((jvct_t *)pusr->pjlc)->io.offs; i++) {
		if(strcmp(this, ((jvct_t *)pusr->pjlc)->io.head[i]) == 0) {
			if(((jvct_t *)pusr->pjlc)->io.ofs2 > 0) {
				jl_io_print_lowc(pusr, "\n");
			}
			((jvct_t *)pusr->pjlc)->io.ofs2 -=
				((jvct_t *)pusr->pjlc)->io.offs - i;
			((jvct_t *)pusr->pjlc)->io.offs = i;
			return;
		}
	}
	//extend
	if(((jvct_t *)pusr->pjlc)->io.ofs2 < 0) {
		jl_io_print_lowc(pusr, "\n");
	}
	((jvct_t *)pusr->pjlc)->io.offs++;
	for(i = 0; i < 4; i++) {
		((jvct_t *)pusr->pjlc)->io.head[
			((jvct_t *)pusr->pjlc)->io.offs][i] = this[i];
	}
	((jvct_t *)pusr->pjlc)->io.head[
			((jvct_t *)pusr->pjlc)->io.offs][4] = '\0';
	((jvct_t *)pusr->pjlc)->io.ofs2 ++;
	return;
}

void jl_io_print_lows(jl_t* pusr, strt print) {
	if(print == NULL) {
		print = Strt(STRT_NULL);
	}
	jl_io_print_lowc(pusr, (void *)print->data);
	if(print->type == STRT_TEMP) {
		jl_me_strt_free(print);
	}
}

void _jl_io_init(jvct_t * pjlc) {
	#if PLATFORM == 1
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	int i, j;
	for(i = 0; i < 16; i++) {
		for(j = 0; j < 5; j++) {
			pjlc->io.head[i][j] = '\0';
		}
	}
	jl_io_offset(pjlc->sg.usrd, "JLVM");
}
