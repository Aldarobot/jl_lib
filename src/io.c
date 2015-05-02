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
	printf("[%4s] ", pjlc->io.head[pjlc->io.offs]);
	pjlc->io.newline = 0;
}

static void _jl_io_current(jvct_t *pjlc) {
	int i;

	printf(" <");
	for(i = 0; i < pjlc->io.offs; i++) {
		printf("%4s", pjlc->io.head[i+1]);
		printf("/");
	}
	printf("\b>");
}

static inline void _jl_io_new_block(jvct_t *pjlc) {
	int i;
	if(pjlc->io.newline != 2) printf("\n");
	pjlc->io.offs-=pjlc->io.ofs2;
	_jl_io_indt(pjlc->sg.usrd);
	pjlc->io.offs+=pjlc->io.ofs2;
	printf("[/");
	for(i = pjlc->io.offs - pjlc->io.ofs2;
		i < pjlc->io.offs;
		i++)
	{
		printf("%4s", pjlc->io.head[i+1]);
		printf("/");
	}
	printf("\b]");
	_jl_io_current(pjlc);
	pjlc->io.ofs2 = 0;
	pjlc->io.newline = 1;
}

static inline void _jl_io_old_block(jvct_t *pjlc) {
	int i;
	printf("\n");
	_jl_io_indt(pjlc->sg.usrd);
	printf("[\\");
	for(i = pjlc->io.offs;
		i > pjlc->io.offs +
			pjlc->io.ofs2;
		i--)
	{
		printf("%4s", pjlc->io.head[i+1]);
		printf("\\");
	}
	printf("\b]");
	_jl_io_current(pjlc);
	pjlc->io.ofs2 = 0;
	pjlc->io.newline = 1;
}

static inline void _jl_io_printc_endline(jvct_t *pjlc, const char *print) {
	pjlc->io.newline = 1;
	char *string = jl_me_alloc(strlen(print) + 1);
	memcpy(string, print, strlen(print));
	string[strlen(print) - 1] = '\0';
	printf("%s", string);
	free(string);
}

void _jl_io_printc(jl_t* pusr, const char * print) {
	jvct_t *pjlc = pusr->pjlc;
#if JL_PLAT == JL_PLAT_COMPUTER
	if(pjlc->io.ofs2 > 0) {
		_jl_io_new_block(pjlc);
	}else if(pjlc->io.ofs2 < 0) {
		_jl_io_old_block(pjlc);
	}
	if(print[0] == '\n') {
		_jl_io_newline(pjlc);
		print++;
	}else if(pjlc->io.newline) {
		_jl_io_newline(pjlc);
	}
	if(print[strlen(print) - 1] == '\n') {
		_jl_io_printc_endline(pjlc, print);
	}else{
		printf("%s", print);
	}
#else
	SDL_Log("[%s] %s\n",
		pjlc->io.head[pjlc->io.offs],
		print);
#endif
}

void _jl_io_print_no(jl_t* pusr, const char * print) { }

/**
 * Create a new printing tag.
 * @param pusr: the library context.
 * @param tag: the value (0-infinity) of the tag.
 * @param shouldprint: whether this tag is enabled.
 * @param tagfn: the function to run when printing.  For default use NULL.
*/
void jl_io_tag_set(jl_t* pusr,
	int16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn)
{
	jvct_t *pjlc = pusr->pjlc;
	uint16_t realtag = tag + JL_IO_TAG_MAX;

	if(realtag > pjlc->io.maxtag) {
		pjlc->io.maxtag = realtag;
		pjlc->io.printfn = realloc(pjlc->io.printfn,
			(sizeof(void *) * (pjlc->io.maxtag+1)) + 1);
	}
	if(tagfn) { //User-Defined function
		if(shouldprint) pjlc->io.printfn[realtag] = tagfn;
		else pjlc->io.printfn[realtag] = _jl_io_print_no;	
	}else{ //NULL
		if(shouldprint) pjlc->io.printfn[realtag] = _jl_io_printc;
		else pjlc->io.printfn[realtag] = _jl_io_print_no;
	}
}

/**
 * Print text to the terminal.
 * @param pusr: the library context.
 * @param print: the string to print.
*/
void jl_io_printc(jl_t* pusr, const char * print) {
	jvct_t *pjlc = pusr->pjlc;

	if(pjlc->io.offs > 15) {
		printf("\nOverreached block count %d!!!\n", pjlc->io.offs);
		pjlc->io.offs = 15;
		_jl_io_current(pjlc);
		printf("\nQuitting...\n");
		exit(0);
	}
	//Skip over hidden values
	pjlc->io.printfn[pjlc->io.tag[pjlc->io.offs] + JL_IO_TAG_MAX]
		(pusr, print);
}

/**
 * Print truncated text to the terminal.
 * @param pusr: the library context.
 * @param a: the truncating string.
 * @param print: the string to print.
*/
void jl_io_printt(jl_t *pusr, uint8_t a, const char *print) {
	char *string = jl_me_alloc(strlen(print) + 1);
	char *string2 = jl_me_alloc(6);
	sprintf(string2, "%%%ds", a);
	sprintf(string, string2, print);
	jl_io_printc(pusr, string);
	free(string);
	free(string2);
}

/**
 * Print an integer to the terminal.
 * @param pusr: the library context.
 * @param print: the number to print.
*/
void jl_io_printi(jl_t *pusr, int print) {
	char *string = jl_me_alloc(10);
	sprintf(string, "%d", print);
	jl_io_printc(pusr, string);
	free(string);
}

/**
 * Print a double to the terminal.
 * @param pusr: the library context.
 * @param print: the number to print.
*/
void jl_io_printd(jl_t *pusr, double print) {
	char *string = jl_me_alloc(20);
	sprintf(string, "%f", print);
	jl_io_printc(pusr, string);
	free(string);
}

/**
 * Close an offset block.
 * @param pusr: the library context.
*/
void jl_io_close_block(jl_t* pusr) {
	jvct_t *pjlc = pusr->pjlc;

	pjlc->io.ofs2 -= 1;
	if(pjlc->io.offs != 1)
		pjlc->io.offs -= 1;
	else
		printf("\n[DONE!]\n");
}

/**
 * Open an offset block.
 * @param pusr: the library context.
 * @param this: the name of the block.
 * @param tag: the tag attached to the block.
*/
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

void _jl_io_init(jvct_t * pjlc) {
	#if JL_PLAT == JL_PLAT_PHONE
	// Enable standard application logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	#endif
	int i, j;
	for(i = 1; i < 16; i++) {
		for(j = 0; j < 5; j++) {
			pjlc->io.head[i][j] = '\0';
		}
		pjlc->io.tag[i] = 0;
	}
	pjlc->io.maxtag = 0;
	pjlc->io.printfn = malloc(sizeof(void *));
	pjlc->io.offs = 0;
	pjlc->io.ofs2 = 0;
	pjlc->io.newline = 2;
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_MINIMAL -JL_IO_TAG_MAX, 1, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_PROGRESS-JL_IO_TAG_MAX, 0, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_SIMPLE  -JL_IO_TAG_MAX, 0, NULL);
	jl_io_tag_set(pjlc->sg.usrd, JL_IO_TAG_INTENSE -JL_IO_TAG_MAX, 0, NULL);
	jl_io_offset(pjlc->sg.usrd, "JLVM", JL_IO_TAG_MINIMAL-JL_IO_TAG_MAX);
}

void _jl_io_kill(jl_t * pusr) {
	jl_io_printc(pusr, "Killed Program\n");
	jl_io_close_block(pusr); //Close Block "KILL"
	jl_io_close_block(pusr); //Close Block "JLVM"
}
