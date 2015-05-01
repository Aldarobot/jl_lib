/*
 * (c) Jeron A. Lau
 * jl/io is a library for printing to various terminals.
*/

#include "header/jl_pr.h"

static void _jl_io_indt(jl_t* pusr) {
	int i;
	for(i = 0; i < ((jvct_t *)(pusr->pjlc))->io.offs; i++) {
		printf(" ");
	}
}

static void _jl_io_newline(jvct_t *pjlc) {
	printf("\n");
	_jl_io_indt(pjlc->sg.usrd);
	printf("[%s] ", pjlc->io.head[pjlc->io.offs]);
	pjlc->io.newline = 0;
}

void _jl_io_print_lowc(jl_t* pusr, const char * print) {
	jvct_t *pjlc = pusr->pjlc;
#if JL_PLAT == JL_PLAT_COMPUTER
	int i;
	if(pjlc->io.ofs2 > 0) {
		_jl_io_indt(pusr);
		printf("[/");
		for(i = pjlc->io.offs -
				pjlc->io.ofs2;
			i < pjlc->io.offs;
			i++)
		{
			printf("%s", pjlc->io.head[i+1]);
			printf("/");
		}
		printf("\b]");
		pjlc->io.ofs2 = 0;
		pjlc->io.newline = 1;
	}else if(pjlc->io.ofs2 < 0) {
		_jl_io_indt(pusr);
		printf("[\\");
		for(i = pjlc->io.offs;
			i > pjlc->io.offs +
				pjlc->io.ofs2;
			i--)
		{
			printf( pjlc->io.head[i+1]);
			printf("\\");
		}
		printf("\b]\n");
		pjlc->io.ofs2 = 0;
	}

	if(print[0] == '\n') {
		_jl_io_newline(pjlc);
		print++;
	}else if(pjlc->io.newline) {
		_jl_io_newline(pjlc);
	}
	if(print[strlen(print) - 1] == '\n') {
		pjlc->io.newline = 1;
		char *string = malloc(strlen(print));
		memcpy(string, print, strlen(print));
		string[strlen(print) - 1] = '\0';
		printf("%s", string);
		free(string);
	}else{
		printf(print);
	}
#else
	SDL_Log("[%s] %s\n",
		pjlc->io.head[pjlc->io.offs],
		print);
#endif
}

void _jl_io_print_no(jl_t* pusr, const char * print) { }

void jl_io_tag_set(jl_t* pusr,
	int16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn)
{
	jvct_t *pjlc = pusr->pjlc;
	uint16_t realtag = tag + JL_IO_TAG_MAX;

	if(realtag > pjlc->io.maxtag) {
		pjlc->io.maxtag = realtag;
		pjlc->io.printfn = realloc(
			pjlc->io.printfn, sizeof(void *) * (pjlc->io.maxtag+1));
	}
	if(tagfn) { //User-Defined function
		if(shouldprint) pjlc->io.printfn[realtag] = tagfn;
		else pjlc->io.printfn[realtag] = _jl_io_print_no;	
	}else{ //NULL
		if(shouldprint) pjlc->io.printfn[realtag] = _jl_io_print_lowc;
		else pjlc->io.printfn[realtag] = _jl_io_print_no;
	}
}

/**
 * Print the terminal.
 * @param pusr: the library context.
 * @param print: the text to print.
*/
void jl_io_print_lowc(jl_t* pusr, const char * print) {
	jvct_t *pjlc = pusr->pjlc;

	//Skip over hidden values
	pjlc->io.printfn[pjlc->io.tag[pjlc->io.offs] + JL_IO_TAG_MAX]
		(pusr, print);
}

void jl_io_close_block(jl_t* pusr) {
	jvct_t *pjlc = pusr->pjlc;
	pjlc->io.ofs2 -= 1;
	pjlc->io.offs -= 1;
}

void jl_io_offset(jl_t* pusr, char * this, int16_t tag) {
	jvct_t *pjlc = pusr->pjlc;
	int i = 0;

	if(this == NULL) {
		return;
	}
	if(strncmp(this, pjlc->io.head[pjlc->io.offs], 4) != 0) {
		//extend
		pjlc->io.offs++;
		pjlc->io.ofs2++;
		for(i = 0; i < 4; i++) {
			pjlc->io.head[pjlc->io.offs][i] = this[i];
		}
		pjlc->io.head[pjlc->io.offs][4] = '\0';
	}
	pjlc->io.tag[pjlc->io.offs] = tag;
	return;
}

/*
 *
*/
void jl_io_print_lows(jl_t* pusr, strt print) {
	if(print == NULL) {
		jl_io_print_lowc(pusr, "");
	}else{
		jl_io_print_lowc(pusr, (void *)print->data);
	}
	if(print->type == STRT_TEMP) {
		jl_me_strt_free(print);
	}
}

void _jl_io_init(jvct_t * pjlc) {
	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	int i, j;
	for(i = 0; i < 16; i++) {
		for(j = 0; j < 5; j++) {
			pjlc->io.head[i][j] = '\0';
		}
		pjlc->io.tag[i] = 0;
	}
	pjlc->io.maxtag = 0;
	pjlc->io.printfn = malloc(sizeof(void *));
	pjlc->io.offs = 0;
	pjlc->io.ofs2 = 0;
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_MINIMAL -JL_IO_TAG_MAX, 1, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_PROGRESS-JL_IO_TAG_MAX, 0, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_SIMPLE  -JL_IO_TAG_MAX, 0, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_INTENSE -JL_IO_TAG_MAX, 0, NULL);
	jl_io_offset(pjlc->sg.usrd, "JLVM", JL_IO_TAG_MINIMAL-JL_IO_TAG_MAX);
}
