/*
 * JLVM BaseLibrary: Audio
*/
#include "header/jlvmpu.h"


/* load music from data pointed to by "data" of length "dataSize" into slot
 * "p_IDinStack", set volume of music to "p_vol"*/
void jls_lms(jvct_t *jcpt, int IDinStack, void *data, int dataSize,
	char volumeChange)
{
	#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
	siop_prnt_lwst(0, Strt("JLVM/AUDI"),
		amem_strt_merg(
			amem_strt_merg(Strt("loading music "),
				amem_strt_fnum(IDinStack),
				STRT_TEMP),
			amem_strt_merg(Strt("/"),
				amem_strt_fnum(jcpt->Audi.smax),
				STRT_TEMP),
			STRT_TEMP
		)
	);
	#endif
	jcpt->Audi.jmus[IDinStack]._MUS =
		Mix_LoadMUS_RW(SDL_RWFromMem(data, dataSize), 1);
	jcpt->Audi.jmus[IDinStack]._VOL = volumeChange;
	if(jcpt->Audi.jmus[IDinStack]._MUS == NULL) {
		jlvm_dies(jcpt, amem_strt_merg(
			Strt("Couldn't load music because:"),
			Strt((char *)SDL_GetError()),
			STRT_TEMP)
		);
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		siop_prnt_lwst(0, Strt("JLVM/AUDI"), 
			amem_strt_merg(
				amem_strt_merg(
					Strt("loaded music "),
					amem_strt_fnum(IDinStack),
					STRT_TEMP),
				amem_strt_merg(
					Strt("/"),
					amem_strt_fnum(jcpt->Audi.smax),
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
		jcpt->Audi.jmus = malloc(PMusMax * sizeof(__jal5_mixr_jmus));
		jcpt->Audi.smax = PMusMax;
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
	Mix_VolumeMusic(jcpt->Audi.jmus[jcpt->Audi.idis]._VOL);
	Mix_FadeInMusic(jcpt->Audi.jmus[jcpt->Audi.idis]._MUS, 1, jcpt->Audi.sofi * 1000);
}

void audi_musi_halt(u08t p_secondsOfFadeOut) {
	Mix_FadeOutMusic(p_secondsOfFadeOut * 1000);
}

void audi_musi_play(sgrp_user_t* pusr, u32t p_IDinStack,
	u08t p_secondsOfFadeOut, u08t p_secondsOfFadeIn)
{
	jvct_t *jcpt = pusr->pjct;
	jcpt->Audi.idis = p_IDinStack;
	jcpt->Audi.sofi = p_secondsOfFadeIn;
	jcpt->Audi.sofo = p_secondsOfFadeOut;
	if ( Mix_PlayingMusic() ) { //If Music Playing Already
		audi_musi_halt(jcpt->Audi.sofo);
	}else{ //If Music Not Playing
		_jal5_audi_play(jcpt);
	}
}

void _jal5_audi_loop(jvct_t* jcpt) {
	//If Music Isn't playing AND Music isn't disabled: play music
	if ( !Mix_PlayingMusic() && (jcpt->Audi.idis!=UINT32_MAX)) {
		_jal5_audi_play(jcpt);
	}
}

static inline void jlvm_ini_sounds(jvct_t *jcpt, u08t *data) {
	int i;
//	uint32_t cursor = 0;
	jls_ini(jcpt, 1000000);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0, "JLVM/AUDI/INIT/LOAD","meanwhile....");
	#endif
	u32t fil = 0;
	u32t fid = 0;
	while(1) {
		u32t *bytes = (void *)data;
		uint8_t *fdat = malloc(bytes[0]);
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0, "JLVM/AUDI/INIT/LOAD",amem_strt_fnum(bytes[0]));
		#endif
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0, "JLVM/AUDI/INIT/LOAD","getting data....");
		#endif
		for(i = 0; i < bytes[0]; i++) {
			fdat[i] = data[fil+i+sizeof(uint32_t)];
		}

		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		jal5_siop_cplo(0, "JLVM/AUDI/INIT/LOAD","loading music....");
		#endif
		jls_lms(jcpt,fid,fdat,bytes[0],255);

		fil += bytes[0]+4; //move init next location
		fid++; //increase to the next music id
		//if(data[fil] == 0)
			break;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0, "JLVM/AUDI/INIT/LOAD","loaded music!");
	#endif
}

void _jal5_audi_init(jvct_t *jcpt) {
	// Open the audio device
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM","initailizing audio...");
	#endif
	if ( Mix_OpenAudio(11025, AUDIO_S16, 1, 2048) < 0 ) {
		jlvm_dies(jcpt, amem_strt_merg(
			Strt("Couldn't set 11025 Hz 16-bit audio because:"),
			Strt((char *)SDL_GetError()),
			STRT_TEMP
		));
	}else{
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		jal5_siop_cplo(0,"JLVM","audio has been set.\n");
		#endif
	}
	//Load Sound Effects & Music
	uint8_t *aud = NULL;
	aud = file_pkdj_load(jcpt->Sgrp.usrd, "jlex/2/_aud");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM","Loading Audiostuffs...");
	#endif
	if(aud != NULL) {
		jlvm_ini_sounds(jcpt,aud);
	}
	jcpt->Audi.idis = UINT32_MAX; //Audio by default is disabled
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"JLVM","Loaded Audiostuffs!");
	#endif
}

void _jal5_audi_kill(jvct_t *jcpt) {
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"[JLVM/AUDI]","Quiting....");
	#endif
	Mix_CloseAudio();
	u32t i;
	for(i = 0; i < jcpt->Audi.smax; i++) {
		Mix_FreeMusic(jcpt->Audi.jmus[i]._MUS);
	}
	free(jcpt->Audi.jmus);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jal5_siop_cplo(0,"[JLVM/AUDI]","Quit Successfully!");
	#endif
}
