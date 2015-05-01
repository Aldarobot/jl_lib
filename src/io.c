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
		printf("\b]\n");
		pjlc->io.ofs2 = 0;
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
	_jl_io_indt(pusr);

	printf("[%s] %s\n",
		pjlc->io.head[
			pjlc->io.offs],
		print);
#else
	SDL_Log("[%s] %s\n",
		pjlc->io.head[
			pjlc->io.offs],
		print);
#endif
}

void _jl_io_print_no(jl_t* pusr, const char * print) { }

void jl_io_tag_set(jl_t* pusr,
	uint16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn)
{
	jvct_t *pjlc = pusr->pjlc;

	printf("sdl\n");
	if(tag > pjlc->io.maxtag) {
		pjlc->io.maxtag = tag;
		pjlc->io.printfn = realloc(
			pjlc->io.printfn, sizeof(void *) * (pjlc->io.maxtag+1));
	}
	printf("sdl2\n");
	if(tagfn) { //User-Defined function
		printf("sdl3\n");
		if(shouldprint) pjlc->io.printfn[tag] = tagfn;
		else pjlc->io.printfn[tag] = _jl_io_print_no;	
	}else{ //NULL
		printf("sdl3\n");
		if(shouldprint) pjlc->io.printfn[tag] = _jl_io_print_lowc;
		else pjlc->io.printfn[tag] = _jl_io_print_no;
	}
	printf("sdl3\n");
}

void jl_io_tag(jl_t* pusr, int16_t tag) {
	jvct_t *pjlc = pusr->pjlc;
	pjlc->io.tag = tag;
}

/*
 * Print the terminal.
*/
void jl_io_print_lowc(jl_t* pusr, const char * print) {
	jvct_t *pjlc = pusr->pjlc;

	//Skip over hidden values
	pjlc->io.printfn[pjlc->io.tag + JL_IO_TAG_MAX](pusr, print);
}

void jl_io_offset(jl_t* pusr, char * this) {
	jvct_t *pjlc = pusr->pjlc;
	int i = 0;

	if(this == NULL) {
		return;
	}
	for(i = 0; i < pjlc->io.offs; i++) {
		if(strcmp(this, pjlc->io.head[i]) == 0) {
			pjlc->io.ofs2 -=
				pjlc->io.offs - i;
			pjlc->io.offs = i;
			return;
		}
	}
	//extend
	pjlc->io.offs++;
	for(i = 0; i < 4; i++) {
		pjlc->io.head[pjlc->io.offs][i] = this[i];
	}
	pjlc->io.head[pjlc->io.offs][4] = '\0';
	pjlc->io.ofs2 ++;
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
	}
	pjlc->io.maxtag = 0;
	pjlc->io.tag = 0;
	pjlc->io.printfn = malloc(sizeof(void *));
	pjlc->io.offs = 0;
	pjlc->io.ofs2 = 0;
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_MINIMAL, 1, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_PROGRESS, 1, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_SIMPLE, 1, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_INTENSE, 1, NULL);
	jl_io_tag(pjlc->sg.usrd, 0 - JL_IO_TAG_MAX); //Reset
	jl_io_offset(pjlc->sg.usrd, "JLVM");
}
