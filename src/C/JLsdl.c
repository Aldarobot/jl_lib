#include "jl_pr.h"

float jl_sdl_seconds_past__(jl_t* jlc) {
	jlc->time.this_tick = SDL_GetTicks();
	// milliseconds / 1000 to get seconds
	return ((float)(jlc->time.this_tick - jlc->time.prev_tick)) / 1000.f;
}
