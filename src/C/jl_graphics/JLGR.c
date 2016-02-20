/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGR.c
 *	A High Level Graphics Library that supports sprites, texture loading,
 *	2D rendering & 3D rendering.
 */
#include "jl_pr.h"
#include "JLgr.h"

/**
 * Create a window.
 * @param jlc: The library context.
 * @param window_name: The name of the window.
 * @param fullscreen: 0 for windowed mode, 1 for fullscreen.
 * @returns The jl_gr library context.
**/
jl_gr_t* jl_gr_init(jl_t* jlc, str_t window_name, u8_t fullscreen) {
	jl_gr_t* jl_gr = NULL;
	jl_me_alloc(jlc, (void**)&jl_gr, sizeof(jl_gr_t), 0);
	jl_gr->jlc = jlc;
	return jl_gr;
}

/**
 * Set the functions to be called when the window redraws.
 * @param jl_gr: The jl_gr library context.
 * @param onescreen: The function to redraw the screen when there's only 1 
 *  screen.
 * @param upscreen: The function to redraw the upper or primary display.
 * @param downscreen: The function to redraw the lower or secondary display.
**/
void jl_gr_loop_set(jl_gr_t* jl_gr, jl_fnct onescreen, jl_fnct upscreen,
	jl_fnct downscreen)
{
	
}

/**
 * Make sure that the screen is redrawn.
 * @param jl_gr: The jl_gr library context.
 * @param data: The data to be copied into the drawing thread.
 * @param dataSize: How many bytes are pointed to by "data".
**/
void jl_gr_loop(jl_gr_t* jl_gr, void* data, u32_t dataSize) {
	
}

/**
 * Destroy the window and free the jl_gr library context.
 * @param jl_gr: The jl_gr library context.
**/
void jl_gr_kill(jl_gr_t* jl_gr) {
}

// End of file.
