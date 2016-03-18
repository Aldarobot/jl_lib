/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRthread.c
 *	This file handles a separate thread for drawing graphics.
**/
#include "jl_pr.h"
#include "JLGRinternal.h"

int jlgr_thread_main(void* data) {
	jl_gr_t* jl_gr = data;
	
}

void jlgr_thread_init(jl_gr_t* jl_gr) {
	SDL_Thread *thread;

	thread = SDL_CreateThread(jlgr_thread_main, "JL_Lib/Graphics", jl_gr);
}
