/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * au.c
 *	JLVM BaseLibrary: Audio
 * 		This library can play and record / music or sound effects.
*/
#include "header/jl_pr.h"


/**
 * load music from data pointed to by "data" of length "dataSize" into slot
 * "p_IDinStack", set volume of music to "p_vol"
 */
void jl_au_load(jvct_t *_jlc, int IDinStack, void *data, int dataSize,
	char volumeChange)
{
	//Open Block "AUDI/LOAD"
	jl_io_offset(_jlc->jlc, JL_IO_PROGRESS, "AUDI");
	jl_io_offset(_jlc->jlc, JL_IO_PROGRESS, "LOAD");

	jl_io_printc(_jlc->jlc, "loading music....\n");
	jl_io_printc(_jlc->jlc, "loading music ");
	jl_io_printi(_jlc->jlc, IDinStack);
	jl_io_printc(_jlc->jlc, "/");
	jl_io_printi(_jlc->jlc, _jlc->au.smax);
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "LOAD"); //INTENSE
	jl_io_printc(_jlc->jlc, "ausize: ");
	jl_io_printi(_jlc->jlc, dataSize);
	jl_io_printc(_jlc->jlc, "\n");
	jl_io_printc(_jlc->jlc, "audata: \"");
	jl_io_printt(_jlc->jlc, 4, data);
	jl_io_printc(_jlc->jlc, "\"\n");
	_jlc->au.jmus[IDinStack]._MUS =
		Mix_LoadMUS_RW(SDL_RWFromMem(data, dataSize), 1);
	_jlc->au.jmus[IDinStack]._VOL = volumeChange;
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "LOAD");
	if(_jlc->au.jmus[IDinStack]._MUS == NULL) {
		_jl_fl_errf(_jlc, ":Couldn't load music because:");
		_jl_fl_errf(_jlc, (char *)SDL_GetError());
		jl_sg_kill(_jlc->jlc, "\n");
	}else{
		jl_io_printc(_jlc->jlc, "loaded music ");
		jl_io_printi(_jlc->jlc, IDinStack);
		jl_io_printc(_jlc->jlc, "/");
		jl_io_printi(_jlc->jlc, _jlc->au.smax);
		jl_io_printc(_jlc->jlc, "\n");
	}
	
	jl_io_close_block(_jlc->jlc); //Close Block "LOAD"
	jl_io_close_block(_jlc->jlc); //Close Block "AUDI"
}

/**
 * Test if music is playing. 
 * @returns 1: if music is playing
 * @returns 0: otherwise
*/
uint8_t jl_au_mus_playing(void) {
	return Mix_PlayingMusic();
}

/*allocates "PMusMax" slots for music*/
static inline void jls_ini(jvct_t *_jlc, int PMusMax) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "AUDI");
	jl_io_printc(_jlc->jlc, "allocating music space:");
	jl_io_printi(_jlc->jlc, PMusMax);
	jl_io_printc(_jlc->jlc, "\n");

	if(PMusMax) {
		_jlc->au.jmus = malloc(PMusMax * sizeof(__mixr_jmus));
		_jlc->au.smax = PMusMax;
	}
	jl_io_printc(_jlc->jlc, "allocated music space!\n");
	jl_io_close_block(_jlc->jlc); //Close Block "AUDI"
}

/**
 * set where the music is coming from.  
 * @param left: How much to the left compared to the right.
 *	255 is left 0 is right
 * @param toCenter: Overall volume
*/
void jl_au_panning(uint8_t left, uint8_t toCenter) {
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

/**
 * sets where music is comming from to center ( resets panning )
*/
void jl_au_panning_default(void) {
	Mix_SetPanning(MIX_CHANNEL_POST,255,255);
}

/** @cond **/
void _jl_au_play(jvct_t *_jlc) {
	Mix_VolumeMusic(_jlc->au.jmus[_jlc->au.idis]._VOL);
	Mix_FadeInMusic(_jlc->au.jmus[_jlc->au.idis]._MUS, 1, _jlc->au.sofi * 1000);
}
/** @endcond **/

/**
 * fade out any previously playing music (if there is any) for
 * "p_secondsOfFadeOut" seconds
 * @param p_secondsOfFadeOut: How many seconds to fade music.
*/
void jl_au_mus_halt(u8_t p_secondsOfFadeOut) {
	Mix_FadeOutMusic(p_secondsOfFadeOut * 1000);
}

/** 
 * fade out any previously playing music (If there is any) for
 * "p_secondsOfFadeOut" seconds, then fade in music with ID "p_IDinStack"
 * for "p_secondsOfFadeIn" seconds
*/
void jl_au_mus_play(jl_t* jlc, uint32_t p_IDinStack,
	uint8_t p_secondsOfFadeOut, uint8_t p_secondsOfFadeIn)
{
	jvct_t *_jlc = jlc->_jlc;
	_jlc->au.idis = p_IDinStack;
	_jlc->au.sofi = p_secondsOfFadeIn;
	_jlc->au.sofo = p_secondsOfFadeOut;
	if ( Mix_PlayingMusic() ) { //If Music Playing Already
		jl_au_mus_halt(_jlc->au.sofo);
	}else{ //If Music Not Playing
		_jl_au_play(_jlc);
	}
}

/** @cond **/
void _jl_au_loop(jvct_t* _jlc) {
	//If Music Isn't playing AND Music isn't disabled: play music
	if ( !Mix_PlayingMusic() && (_jlc->au.idis!=UINT32_MAX)) {
		_jl_au_play(_jlc);
	}
}
/** @endcond **/

static inline void _jl_au_init_sounds(jvct_t *_jlc, uint8_t *data) {
	uint32_t fil = 0;
	uint32_t fid = 0;

	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "AUDI");
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "LOAD");

//	uint32_t cursor = 0;
	jls_ini(_jlc, 1000000);
	jl_io_printc(_jlc->jlc, "meanwhile....\n");

	while(1) {
		uint32_t *bytes = (void *)(data + fil);
		uint32_t size = *bytes;

		if(size == 0) break; // If Size Is 0 signal to stop.
		jl_io_printi(_jlc->jlc, size);
		jl_io_printc(_jlc->jlc,"getting data....\n");
		fil += sizeof(uint32_t); //move init next location
		jl_io_printc(_jlc->jlc,"jl_au_load() we are at [data + ");
		jl_io_printi(_jlc->jlc,fil);
		jl_io_printc(_jlc->jlc,"\n");
		jl_au_load(_jlc,fid,data + fil,size,255);
		fil += size; //move init next location
		fid++; //increase to the next music id
	}
	_jlc->jlc->info = fid;
	jl_io_printc(_jlc->jlc, "loaded music!\n");
	
	jl_io_close_block(_jlc->jlc); //Close Block "LOAD"
	jl_io_close_block(_jlc->jlc); //Close Block "AUDI"
}

/**
 * Load all audiotracks from a zipfile and give them ID's.
 * info: info is set to number of images loaded.
 * @param jlc: library context
 * @param pzipfile: full file name of a zip file.
 * @param pigid: which audio group to load the soundtracks into.
*/
void jl_au_add_audio(jl_t* jlc, char *pzipfile, uint16_t pigid) {
	uint8_t *aud = jl_fl_media(jlc, "jlex/2/_aud", pzipfile,
		jal5_head_jlvm(), jal5_head_size());
	jl_io_offset(jlc, JL_IO_MINIMAL, "LOAD"); // { : Open Block "LOAD"
	jl_io_printc(jlc, "AUDIO_AUDIO:");
	jl_io_printi(jlc, jlc->info);
	jl_io_printc(jlc, "\nLoading audiostuffs...\n");
	jl_io_close_block(jlc); // } : Close Block "LOAD"
	if((aud != NULL) || (jlc->info > 4)) {
		_jl_au_init_sounds(jlc->_jlc,aud);
	}
	jl_io_offset(jlc, JL_IO_MINIMAL, "LOAD"); // { : Open Block "LOAD"
	jl_io_printc(jlc, "Loaded audiostuffs!\n");
	jl_io_close_block(jlc); // } : Close Block "LOAD"
}

/** @cond **/
static void _jl_au_print_openblock(jl_t* jlc) {
	jl_io_offset(jlc, JL_IO_SIMPLE, "AUDI");
}

void _jl_au_init(jvct_t *_jlc) {
	//Open Block AUDI
	_jl_au_print_openblock(_jlc->jlc);
	// Open the audio device
	jl_io_printc(_jlc->jlc, "initailizing audio...\n");
	if ( Mix_OpenAudio(11025, AUDIO_S16, 1, 2048) < 0 ) {
		_jl_fl_errf(_jlc, ":Couldn't set 11025 Hz 16-bit audio because:");
		_jl_fl_errf(_jlc, (char *)SDL_GetError());
		jl_sg_kill(_jlc->jlc, "\n");
	}else{
		jl_io_printc(_jlc->jlc, "audio has been set.\n");
	}
	//Load Sound Effects & Music
	jl_au_add_audio(_jlc->jlc, 
		jl_fl_get_resloc(_jlc->jlc, "JLLB", "media.zip"), 0);
	//audio by default is disabled
	_jlc->au.smax = 0;
	_jlc->au.idis = UINT32_MAX; 
	//
	jl_io_printc(_jlc->jlc, "Loaded audiostuffs!\n");
	//Close Block AUDI
	jl_io_close_block(_jlc->jlc);
}

void _jl_au_kill(jvct_t *_jlc) {
	//Open Block AUDI
	_jl_au_print_openblock(_jlc->jlc);
	jl_io_printc(_jlc->jlc, "Quiting....\n");
	//Free Everything
	Mix_CloseAudio();
	m_u32_t i;
	for(i = 0; i < _jlc->au.smax; i++) {
		Mix_FreeMusic(_jlc->au.jmus[i]._MUS);
	}
	free(_jlc->au.jmus);

	jl_io_printc(_jlc->jlc, "Quit Successfully!\n");
	//Close Block AUDI
	jl_io_close_block(_jlc->jlc);
}
/** @endcond **/
