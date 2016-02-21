/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLAU.c
 *	Audio
 * 		This library can play/record music/sound effects.
*/
#include "jl_pr.h"
#include "JLau.h"

/**
 * load music from data pointed to by "data" of length "dataSize" into slot
 * "IDinStack", set volume of music to "p_vol"
 */
void jl_au_load(jl_au_t* jl_au, u32_t IDinStack, void *data, u32_t dataSize,
	u8_t volumeChange)
{
	//Open Block "AUDI/LOAD"
	jl_io_function(jl_au->jlc, "AU_Load");

	jl_io_print(jl_au->jlc,"loading music %d/%d....",IDinStack,jl_au->smax);
	jl_io_print(jl_au->jlc, "ausize: %d", dataSize);
	jl_io_print(jl_au->jlc, "audata: \"%4s\"", data);
	jl_au->jmus[IDinStack]._MUS =
		Mix_LoadMUS_RW(SDL_RWFromMem(data, dataSize), 1);
	jl_au->jmus[IDinStack]._VOL = volumeChange;
	if(jl_au->jmus[IDinStack]._MUS == NULL) {
		jl_io_print(jl_au->jlc, ":Couldn't load music because: %s",
			(char *)SDL_GetError());
		jl_sg_kill(jl_au->jlc);
	}else{
		jl_io_print(jl_au->jlc, "loaded music %d/%d",
			IDinStack, jl_au->smax);
	}
	
	jl_io_return(jl_au->jlc, "AU_Load");
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
static inline void jls_ini(jl_au_t* jl_au, int PMusMax) {
	jl_io_function(jl_au->jlc, "AU_Init");
	jl_io_print(jl_au->jlc, "allocating music space: %d", PMusMax);

	if(PMusMax) {
		jl_au->jmus = malloc(PMusMax * sizeof(jl_au_jmus_t__));
		jl_au->smax = PMusMax;
	}
	jl_io_print(jl_au->jlc, "allocated music space!");
	jl_io_return(jl_au->jlc, "AU_Init");
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
void _jl_au_play(jl_au_t* jl_au) {
	Mix_VolumeMusic(jl_au->jmus[jl_au->idis]._VOL);
	Mix_FadeInMusic(jl_au->jmus[jl_au->idis]._MUS, 1, jl_au->sofi * 1000);
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
 * "p_secondsOfFadeOut" seconds, then fade in music with ID "IDinStack"
 * for "p_secondsOfFadeIn" seconds
 * @param jl_au: The jl_au library context.
 * @param 
*/
void jl_au_mus_play(jl_au_t* jl_au, u32_t IDinStack, u8_t secondsOfFadeOut,
	u8_t secondsOfFadeIn)
{
	jl_au->idis = IDinStack;
	jl_au->sofi = secondsOfFadeIn;
	jl_au->sofo = secondsOfFadeOut;
	//If music playing already, then halt
	if(Mix_PlayingMusic()) jl_au_mus_halt(jl_au->sofo);
	//If music not playing, then start.
	else _jl_au_play(jl_au);
}

/** @cond **/
// TODO: Figure out whether needed or not.
void jl_au_loop(jvct_t* _jlc) {
	//If Music Isn't playing AND Music isn't disabled: play music
/*	if ( !Mix_PlayingMusic() && (_jlc->au.idis!=UINT32_MAX)) {
		_jl_au_play(_jlc);
	}*/
}
/** @endcond **/

static inline void _jl_au_init_sounds(jl_au_t* jl_au, uint8_t *data) {
	uint32_t fil = 0;
	uint32_t fid = 0;

	jl_io_function(jl_au->jlc, "AU_Load");

//	uint32_t cursor = 0;
	jls_ini(jl_au, 1000000);
	jl_io_print(jl_au->jlc, "meanwhile....");

	while(1) {
		uint32_t *bytes = (void *)(data + fil);
		uint32_t size = *bytes;

		if(size == 0) break; // If Size Is 0 signal to stop.
		jl_io_print(jl_au->jlc,"getting data of size %d....\n", size);
		fil += sizeof(uint32_t); //move init next location
		jl_io_print(jl_au->jlc,"jl_au_load() we are at [data+%d]",fil);
		jl_au_load(jl_au,fid,data + fil,size,255);
		fil += size; //move init next location
		fid++; //increase to the next music id
	}
	jl_au->jlc->info = fid;
	jl_io_print(jl_au->jlc, "loaded music!");
	
	jl_io_return(jl_au->jlc, "AU_Load");
}

/**
 * Load all audiotracks from a zipfile and give them ID's.
 * info: info is set to number of images loaded.
 * @param jlc: library context
 * @param pzipfile: full file name of a zip file.
 * @param pigid: which audio group to load the soundtracks into.
*/
void jl_au_add_audio(jl_t* jlc, str_t pzipfile, uint16_t pigid) {
	strt aud = jl_fl_media(jlc, "jlex/2/_aud", pzipfile,
		jl_gem(), jl_gem_size());
	jl_io_function(jlc, "AU_Load");
	jl_io_print(jlc, "Audio Size: %d", jlc->info);
	jl_io_print(jlc, "Loading audiostuffs....");
	if((aud != NULL) || (jlc->info > 4)) {
		_jl_au_init_sounds(jlc->_jlc,aud->data);
	}
	jl_io_print(jlc, "Loaded audiostuffs!");
	jl_io_return(jlc, "AU_Load");
}

/** @cond **/
static inline void _jl_au_print_openblock(jl_t* jlc) {
	jl_io_function(jlc, "AU");
}

static inline void _jl_au_print_closeblock(jl_t* jlc) {
	jl_io_return(jlc, "AU");
}

jl_au_t* jl_au_init(jl_t* jlc) {
	jl_au_t* jl_au = NULL;

	// Allocate context.
	jl_me_alloc(jlc, (void**)&jl_au, sizeof(jl_au_t), 0);
	jl_au->jlc = jlc;
	//audio by default is disabled
	jl_au->smax = 0;
	jl_au->idis = UINT32_MAX; 
	//Open Block AUDI
	_jl_au_print_openblock(jlc);
	// Open the audio device
	jl_io_print(jlc, "initializing audio....");
	if ( Mix_OpenAudio(11025, AUDIO_S16, 1, 2048) < 0 ) {
		jl_io_print(jlc,
			":Couldn't set 11025 Hz 16-bit audio because: %s",
			(char *)SDL_GetError());
		_jl_au_print_closeblock(jlc);
		jl_sg_kill(jlc);
	}else{
		jl_io_print(jlc, "audio has been set.");
	}
	//Load Sound Effects & Music
	jl_au_add_audio(jlc,jl_fl_get_resloc(jlc, JL_MAIN_DIR, JL_MAIN_MEF), 0);
//	jl_gr_draw_msge(jlc, 0, 0, 0, "LOADED AUDIOSTUFFS!");
	//Close Block AUDI
	_jl_au_print_closeblock(jlc);
	// Return the context.
	return jl_au;
}

void jl_au_kill(jl_au_t* jl_au) {
	//Open Block AUDI
	_jl_au_print_openblock(jl_au->jlc);
	jl_io_print(jl_au->jlc, "Quiting....");
	//Free Everything
	Mix_CloseAudio();
	m_u32_t i;
	for(i = 0; i < jl_au->smax; i++) {
		Mix_FreeMusic(jl_au->jmus[i]._MUS);
	}
	free(jl_au->jmus);
	
	jl_io_print(jl_au->jlc, "Quit Successfully!");
	//Close Block AUDI
	_jl_au_print_closeblock(jl_au->jlc);
}
/** @endcond **/
