#ifndef JLGR
#define JLGR

#include "jl.h"

// Types:
typedef struct{
	jl_t* jlc;
}jl_gr_t;

// Prototypes:
jl_gr_t* jl_gr_init(jl_t* jlc, str_t window_name, u8_t fullscreen);
void jl_gr_loop_set(jl_gr_t* jl_gr, jl_fnct onescreen, jl_fnct upscreen,
	jl_fnct downscreen);
void jl_gr_loop(jl_gr_t* jl_gr, void* data, u32_t dataSize);
void jl_gr_kill(jl_gr_t* jl_gr);

#endif
