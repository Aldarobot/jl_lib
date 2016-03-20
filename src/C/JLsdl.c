#include "jl_pr.h"

float jl_sdl_seconds_past__(jl_t* jl) {
	jl->time.this_tick = SDL_GetTicks();
	// milliseconds / 1000 to get seconds
	return ((float)(jl->time.this_tick - jl->time.prev_tick)) / 1000.f;
}

// internal functions:

void jl_sdl_init__(jl_t* jl) {
	jl->time.psec = 0.f;
	jl->time.prev_tick = 0;
	jl->time.fps = JL_FPS;
}
