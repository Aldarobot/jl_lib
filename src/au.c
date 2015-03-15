/*
 * JLVM BaseLibrary: Audio
*/
#include "header/jl_pr.h"


/* load music from data pointed to by "data" of length "dataSize" into slot
 * "p_IDinStack", set volume of music to "p_vol"*/
void jls_lms(jvct_t *jcpt, int IDinStack, void *data, int dataSize,
	char volumeChange)
{
	jl_io_offset(jcpt->sg.usrd, "AUDI");
	#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
	jl_io_print_lows(jcpt->sg.usrd,
		jl_me_strt_merg(
			jl_me_strt_merg(Strt("loading music "),
				jl_me_strt_fnum(IDinStack),
				STRT_TEMP),
			jl_me_strt_merg(Strt("/"),
				jl_me_strt_fnum(jcpt->au.smax),
				STRT_TEMP),
			STRT_TEMP
		)
	);
	#endif
	jcpt->au.jmus[IDinStack]._MUS =
		Mix_LoadMUS_RW(SDL_RWFromMem(data, dataSize), 1);
	jcpt->au.jmus[IDinStack]._VOL = volumeChange;
	if(jcpt->au.jmus[IDinStack]._MUS == NULL) {
		jlvm_dies(jcpt, jl_me_strt_merg(
			Strt("Couldn't load music because:"),
			Strt((char *)SDL_GetError()),
			STRT_TEMP)
		);
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		jl_io_print_lows(jcpt->sg.usrd,
			jl_me_strt_merg(
				jl_me_strt_merg(
					Strt("loaded music "),
					jl_me_strt_fnum(IDinStack),
					STRT_TEMP),
				jl_me_strt_merg(
					Strt("/"),
					jl_me_strt_fnum(jcpt->au.smax),
					STRT_TEMP),
				STRT_TEMP
			)
		);
		#endif
	}
}

uint8_t audi_musi_spli(void) {
	return Mix_PlayingMusic();
}

/*allocates "PMusMax" slots for music*/
static inline void jls_ini(jvct_t *jcpt, int PMusMax) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf(" [JLL/jls/ini] allocating music space:%d\n",PMusMax);
	#endif
	if(PMusMax) {
		jcpt->au.jmus = malloc(PMusMax * sizeof(__mixr_jmus));
		jcpt->au.smax = PMusMax;
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

void _jal5_audi_play(jvct_t *jcpt) {
	Mix_VolumeMusic(jcpt->au.jmus[jcpt->au.idis]._VOL);
	Mix_FadeInMusic(jcpt->au.jmus[jcpt->au.idis]._MUS, 1, jcpt->au.sofi * 1000);
}

void audi_musi_halt(u08t p_secondsOfFadeOut) {
	Mix_FadeOutMusic(p_secondsOfFadeOut * 1000);
}

void audi_musi_play(jl_t* pusr, u32t p_IDinStack,
	u08t p_secondsOfFadeOut, u08t p_secondsOfFadeIn)
{
	jvct_t *jcpt = pusr->pjlc;
	jcpt->au.idis = p_IDinStack;
	jcpt->au.sofi = p_secondsOfFadeIn;
	jcpt->au.sofo = p_secondsOfFadeOut;
	if ( Mix_PlayingMusic() ) { //If Music Playing Already
		audi_musi_halt(jcpt->au.sofo);
	}else{ //If Music Not Playing
		_jal5_audi_play(jcpt);
	}
}

void _jal5_audi_loop(jvct_t* jcpt) {
	//If Music Isn't playing AND Music isn't disabled: play music
	if ( !Mix_PlayingMusic() && (jcpt->au.idis!=UINT32_MAX)) {
		_jal5_audi_play(jcpt);
	}
}

static inline void jlvm_ini_sounds(jvct_t *jcpt, u08t *data) {
	jl_io_offset(jcpt->sg.usrd, "LOAD");
	int i;
//	uint32_t cursor = 0;
	jls_ini(jcpt, 1000000);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "meanwhile....\n");
	#endif
	u32t fil = 0;
	u32t fid = 0;
	while(1) {
		u32t *bytes = (void *)data;
		uint8_t *fdat = malloc(bytes[0]);
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(jcpt->sg.usrd, jl_me_strt_fnum(bytes[0]));
		#endif
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(jcpt->sg.usrd,"getting data....\n");
		#endif
		for(i = 0; i < bytes[0]; i++) {
			fdat[i] = data[fil+i+sizeof(uint32_t)];
		}

		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		jl_io_print_lowc(jcpt->sg.usrd, "loading music....\n");
		#endif
		jls_lms(jcpt,fid,fdat,bytes[0],255);

		fil += bytes[0]+4; //move init next location
		fid++; //increase to the next music id
		//if(data[fil] == 0)
			break;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "loaded music!\n");
	#endif
}

void _jal5_audi_init(jvct_t *jcpt) {
	jl_io_offset(jcpt->sg.usrd, "AUDI");
	jl_io_offset(jcpt->sg.usrd, "INIT");
	// Open the audio device
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "initailizing audio...\n");
	#endif
	if ( Mix_OpenAudio(11025, AUDIO_S16, 1, 2048) < 0 ) {
		jlvm_dies(jcpt, jl_me_strt_merg(
			Strt("Couldn't set 11025 Hz 16-bit audio because:"),
			Strt((char *)SDL_GetError()),
			STRT_TEMP
		));
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jl_io_print_lowc(jcpt->sg.usrd, "audio has been set.\n");
		#endif
	}
	//Load Sound Effects & Music
	uint8_t *aud = NULL;
	aud = jl_fl_pk_mnld(jcpt->sg.usrd, "jlex/2/_aud");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "Loading audiostuffs...\n");
	#endif
	if(aud != NULL) {
		jlvm_ini_sounds(jcpt,aud);
	}
	jcpt->au.idis = UINT32_MAX; //audio by default is disabled
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "Loaded audiostuffs!\n");
	#endif
	jl_io_offset(jcpt->sg.usrd, "JLVM");
}

void _jal5_audi_kill(jvct_t *jcpt) {
	jl_io_offset(jcpt->sg.usrd, "AUDI");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "Quiting....\n");
	#endif
	Mix_CloseAudio();
	u32t i;
	for(i = 0; i < jcpt->au.smax; i++) {
		Mix_FreeMusic(jcpt->au.jmus[i]._MUS);
	}
	free(jcpt->au.jmus);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "Quit Successfully!\n");
	#endif
	jl_io_offset(jcpt->sg.usrd, "JLVM");
}
