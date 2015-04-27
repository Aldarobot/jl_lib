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
	jl_io_offset(pjlc->sg.usrd, "AUDI");
	#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
	jl_io_print_lowc(pjlc->sg.usrd, "loading music ");
	jl_io_print_lows(pjlc->sg.usrd, jl_me_strt_fnum(IDinStack));
	jl_io_print_lowc(pjlc->sg.usrd, "/");
	jl_io_print_lows(pjlc->sg.usrd, jl_me_strt_fnum(pjlc->au.smax));
	#endif
	pjlc->au.jmus[IDinStack]._MUS =
		Mix_LoadMUS_RW(SDL_RWFromMem(data, dataSize), 1);
	pjlc->au.jmus[IDinStack]._VOL = volumeChange;
	if(pjlc->au.jmus[IDinStack]._MUS == NULL) {
		_jl_fl_errf(pjlc, ":Couldn't load music because:");
		_jl_fl_errf(pjlc, (char *)SDL_GetError());
		jl_sg_die(pjlc, "\n");
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		jl_io_print_lowc(pjlc->sg.usrd, "loaded music ");
		jl_io_print_lows(pjlc->sg.usrd, jl_me_strt_fnum(IDinStack));
		jl_io_print_lowc(pjlc->sg.usrd, "/");
		jl_io_print_lows(pjlc->sg.usrd, jl_me_strt_fnum(pjlc->au.smax));
		#endif
	}
}

uint8_t audi_musi_spli(void) {
	return Mix_PlayingMusic();
}

/*allocates "PMusMax" slots for music*/
static inline void jls_ini(jvct_t *pjlc, int PMusMax) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf(" [JLL/jls/ini] allocating music space:%d\n",PMusMax);
	#endif
	if(PMusMax) {
		pjlc->au.jmus = malloc(PMusMax * sizeof(__mixr_jmus));
		pjlc->au.smax = PMusMax;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf(" [JLL/jls/ini] allocated music space!\n");
	#endif
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
	jl_io_offset(pjlc->sg.usrd, "LOAD");
	int i;
//	uint32_t cursor = 0;
	jls_ini(pjlc, 1000000);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "meanwhile....\n");
	#endif
	u32t fil = 0;
	u32t fid = 0;
	while(1) {
		u32t *bytes = (void *)data;
		uint8_t *fdat = malloc(bytes[0]);
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lows(pjlc->sg.usrd, jl_me_strt_fnum(bytes[0]));
		#endif
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pjlc->sg.usrd,"getting data....\n");
		#endif
		for(i = 0; i < bytes[0]; i++) {
			fdat[i] = data[fil+i+sizeof(uint32_t)];
		}

		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		jl_io_print_lowc(pjlc->sg.usrd, "loading music....\n");
		#endif
		jl_au_load(pjlc,fid,fdat,bytes[0],255);

		fil += bytes[0]+4; //move init next location
		fid++; //increase to the next music id
		//if(data[fil] == 0)
			break;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "loaded music!\n");
	#endif
}

/** @cond **/
void _jl_au_init(jvct_t *pjlc) {
	jl_io_offset(pjlc->sg.usrd, "AUDI");
	jl_io_offset(pjlc->sg.usrd, "INIT");
	// Open the audio device
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "initailizing audio...\n");
	#endif
	if ( Mix_OpenAudio(11025, AUDIO_S16, 1, 2048) < 0 ) {
		_jl_fl_errf(pjlc, ":Couldn't set 11025 Hz 16-bit audio because:");
		_jl_fl_errf(pjlc, (char *)SDL_GetError());
		jl_sg_die(pjlc, "\n");
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(pjlc->sg.usrd, "audio has been set.\n");
		#endif
	}
	//Load Sound Effects & Music
	uint8_t *aud = NULL;
	aud = jl_fl_media(pjlc->sg.usrd, "jlex/2/_aud", (void *)
		jl_fl_get_resloc(pjlc->sg.usrd, Strt("JLVM"), Strt("jlvm.zip"))
			->data,
		jal5_head_jlvm(), jal5_head_size());
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "Loading audiostuffs...\n");
	#endif
	if(aud != NULL) {
		jlvm_ini_sounds(pjlc,aud);
	}
	pjlc->au.idis = UINT32_MAX; //audio by default is disabled
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "Loaded audiostuffs!\n");
	#endif
	jl_io_offset(pjlc->sg.usrd, "JLVM");
}

void _jal5_audi_kill(jvct_t *pjlc) {
	jl_io_offset(pjlc->sg.usrd, "AUDI");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "Quiting....\n");
	#endif
	Mix_CloseAudio();
	u32t i;
	for(i = 0; i < pjlc->au.smax; i++) {
		Mix_FreeMusic(pjlc->au.jmus[i]._MUS);
	}
	free(pjlc->au.jmus);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(pjlc->sg.usrd, "Quit Successfully!\n");
	#endif
	jl_io_offset(pjlc->sg.usrd, "JLVM");
}
/** @endcond **/
