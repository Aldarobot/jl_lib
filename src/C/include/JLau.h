#ifndef JLAU
#define JLAU

#include "jl.h"
#include "SDL_mixer.h"

// Types:
typedef struct {
	Mix_Music *_MUS;
	char _VOL;
}jl_au_jmus_t__;

typedef struct{
	jl_t* jl;
	uint32_t idis; //Which music to play next
	uint8_t sofi; //Seconds Of Fade In
	uint8_t sofo; //Seconds Of Fade Out
	int smax; //Music Stack Maximum Music pieces
	jl_au_jmus_t__ *jmus; //Pointer Of "smax" Music Pieces
	double pofr; //Point Of Return (Where Music Should Start)
}jl_au_t;

// Prototypes:
void jl_au_load(jl_au_t* jl_au, u32_t IDinStack, void *data, u32_t dataSize,
	u8_t volumeChange);
void jl_au_mus_play(jl_au_t* jl_au, u32_t IDinStack, u8_t secondsOfFadeOut,
	u8_t secondsOfFadeIn);
void jl_au_mus_halt(u8_t p_secondsOfFadeOut);
uint8_t jl_au_mus_playing(void);
void jl_au_panning(uint8_t left, uint8_t toCenter);
void jl_au_panning_default(void);
void jl_au_add_audio(jl_t* jl, str_t pzipfile, uint16_t pigid);
jl_au_t* jl_au_init(jl_t* jl);
void jl_au_kill(jl_au_t* jl_au);

#endif
