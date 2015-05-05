/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * au.c
 *	JLVM BaseLibrary: Audio
*/
#include "header/jl_pr.h"


/**
 * load music from data pointed to by "data" of length "dataSize" into slot
 * "p_IDinStack", set volume of music to "p_vol"
 */
void jl_au_load(jvct_t *pjlc, int IDinStack, void *data, int dataSize,
	char volumeChange)
{
	//Open Block "AUDI/LOAD"
	jl_io_offset(pjlc->sg.usrd, "AUDI", JL_IO_TAG_PROGRESS-JL_IO_TAG_MAX);
	jl_io_offset(pjlc->sg.usrd, "LOAD", JL_IO_TAG_PROGRESS-JL_IO_TAG_MAX);

	jl_io_printc(pjlc->sg.usrd, "loading music....\n");
	jl_io_printc(pjlc->sg.usrd, "loading music ");
	jl_io_printi(pjlc->sg.usrd, IDinStack);
	jl_io_printc(pjlc->sg.usrd, "/");
	jl_io_printi(pjlc->sg.usrd, pjlc->au.smax);
	pjlc->au.jmus[IDinStack]._MUS =
		Mix_LoadMUS_RW(SDL_RWFromMem(data, dataSize), 1);
	pjlc->au.jmus[IDinStack]._VOL = volumeChange;
	if(pjlc->au.jmus[IDinStack]._MUS == NULL) {
		_jl_fl_errf(pjlc, ":Couldn't load music because:");
		_jl_fl_errf(pjlc, (char *)SDL_GetError());
		jl_sg_die(pjlc, "\n");
	}else{
		jl_io_printc(pjlc->sg.usrd, "loaded music ");
		jl_io_printi(pjlc->sg.usrd, IDinStack);
		jl_io_printc(pjlc->sg.usrd, "/");
		jl_io_printi(pjlc->sg.usrd, pjlc->au.smax);
	}
	
	jl_io_close_block(pjlc->sg.usrd); //Close Block "LOAD"
	jl_io_close_block(pjlc->sg.usrd); //Close Block "AUDI"
}

uint8_t audi_musi_spli(void) {
	return Mix_PlayingMusic();
}

/*allocates "PMusMax" slots for music*/
static inline void jls_ini(jvct_t *pjlc, int PMusMax) {
	jl_io_offset(pjlc->sg.usrd, "AUDI", JL_IO_TAG_SIMPLE-JL_IO_TAG_MAX);
	jl_io_printc(pjlc->sg.usrd, "allocating music space:");
	jl_io_printi(pjlc->sg.usrd, PMusMax);
	jl_io_printc(pjlc->sg.usrd, "\n");

	if(PMusMax) {
		pjlc->au.jmus = malloc(PMusMax * sizeof(__mixr_jmus));
		pjlc->au.smax = PMusMax;
	}
	jl_io_printc(pjlc->sg.usrd, "allocated music space!\n");
	jl_io_close_block(pjlc->sg.usrd); //Close Block "AUDI"
}

void audi_sdir_setm(uint8_t left, uint8_t toCenter) {
	uint8_t right = 255 - left;
	if(toCenter) {
		if(right > left) {
			left += (uint8_t)(((float)right/255.f) * ((float)toCenter));
		}else{
			right+= (uint8_t)(((float)left /255.f) * ((float)toCenter));
		}
	}
	Mix_SetPanning(MIX_CHANNEL_POST, left, right);
}

void audi_sdir_setd(void) {
	Mix_SetPanning(MIX_CHANNEL_POST,255,255);
}

/** @cond **/
void _jal5_audi_play(jvct_t *pjlc) {
	Mix_VolumeMusic(pjlc->au.jmus[pjlc->au.idis]._VOL);
	Mix_FadeInMusic(pjlc->au.jmus[pjlc->au.idis]._MUS, 1, pjlc->au.sofi * 1000);
}
/** @endcond **/

void audi_musi_halt(u08t p_secondsOfFadeOut) {
	Mix_FadeOutMusic(p_secondsOfFadeOut * 1000);
}

void audi_musi_play(jl_t* pusr, u32t p_IDinStack,
	u08t p_secondsOfFadeOut, u08t p_secondsOfFadeIn)
{
	jvct_t *pjlc = pusr->pjlc;
	pjlc->au.idis = p_IDinStack;
	pjlc->au.sofi = p_secondsOfFadeIn;
	pjlc->au.sofo = p_secondsOfFadeOut;
	if ( Mix_PlayingMusic() ) { //If Music Playing Already
		audi_musi_halt(pjlc->au.sofo);
	}else{ //If Music Not Playing
		_jal5_audi_play(pjlc);
	}
}

/** @cond **/
void _jl_au_loop(jvct_t* pjlc) {
	//If Music Isn't playing AND Music isn't disabled: play music
	if ( !Mix_PlayingMusic() && (pjlc->au.idis!=UINT32_MAX)) {
		_jal5_audi_play(pjlc);
	}
}
/** @endcond **/

static inline void jlvm_ini_sounds(jvct_t *pjlc, u08t *data) {
	int i;
	uint32_t fil = 0;
	uint32_t fid = 0;

	jl_io_offset(pjlc->sg.usrd, "AUDI", JL_IO_TAG_SIMPLE-JL_IO_TAG_MAX);
	jl_io_offset(pjlc->sg.usrd, "LOAD", JL_IO_TAG_SIMPLE-JL_IO_TAG_MAX);

//	uint32_t cursor = 0;
	jls_ini(pjlc, 1000000);
	jl_io_printc(pjlc->sg.usrd, "meanwhile....\n");

	while(1) {
		u32t *bytes = (void *)data;
		uint8_t *fdat = malloc(bytes[0]);

		jl_io_printi(pjlc->sg.usrd, bytes[0]);
		jl_io_printc(pjlc->sg.usrd,"getting data....\n");
		for(i = 0; i < bytes[0]; i++) {
			fdat[i] = data[fil+i+sizeof(uint32_t)];
		}
		jl_au_load(pjlc,fid,fdat,bytes[0],255);
		fil += bytes[0]+4; //move init next location
		fid++; //increase to the next music id
		//if(data[fil] == 0)
			break;
	}
	jl_io_printc(pjlc->sg.usrd, "loaded music!\n");
	
	jl_io_close_block(pjlc->sg.usrd); //Close Block "LOAD"
	jl_io_close_block(pjlc->sg.usrd); //Close Block "AUDI"
}

/** @cond **/
static void _jl_au_print_openblock(jl_t* pusr) {
	jl_io_offset(pusr, "AUDI", JL_IO_TAG_SIMPLE-JL_IO_TAG_MAX);
}

void _jl_au_init(jvct_t *pjlc) {
	//Open Block AUDI
	_jl_au_print_openblock(pjlc->sg.usrd);
	// Open the audio device
	jl_io_printc(pjlc->sg.usrd, "initailizing audio...\n");
	if ( Mix_OpenAudio(11025, AUDIO_S16, 1, 2048) < 0 ) {
		_jl_fl_errf(pjlc, ":Couldn't set 11025 Hz 16-bit audio because:");
		_jl_fl_errf(pjlc, (char *)SDL_GetError());
		jl_sg_die(pjlc, "\n");
	}else{
		jl_io_printc(pjlc->sg.usrd, "audio has been set.\n");
	}
	//Load Sound Effects & Music
	uint8_t *aud = NULL;
	aud = jl_fl_media(pjlc->sg.usrd, "jlex/2/_aud", (void *)
		jl_fl_get_resloc(pjlc->sg.usrd, Strt("JLLB"), Strt("media.zip"))
			->data,
		jal5_head_jlvm(), jal5_head_size());
	jl_io_printc(pjlc->sg.usrd, "Loading audiostuffs...\n");
	if(aud != NULL) {
		jlvm_ini_sounds(pjlc,aud);
	}
	pjlc->au.idis = UINT32_MAX; //audio by default is disabled
	jl_io_printc(pjlc->sg.usrd, "Loaded audiostuffs!\n");
	//Close Block AUDI
	jl_io_close_block(pjlc->sg.usrd);
}

void _jal5_audi_kill(jvct_t *pjlc) {
	//Open Block AUDI
	_jl_au_print_openblock(pjlc->sg.usrd);
	jl_io_printc(pjlc->sg.usrd, "Quiting....\n");
	//Free Everything
	Mix_CloseAudio();
	u32t i;
	for(i = 0; i < pjlc->au.smax; i++) {
		Mix_FreeMusic(pjlc->au.jmus[i]._MUS);
	}
	free(pjlc->au.jmus);

	jl_io_printc(pjlc->sg.usrd, "Quit Successfully!\n");
	//Close Block AUDI
	jl_io_close_block(pjlc->sg.usrd);
}
/** @endcond **/
