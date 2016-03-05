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
#include "JLGRinternal.h"

/**
 * Create a window.
 * @param jlc: The library context.
 * @param window_name: The name of the window.
 * @param fullscreen: 0 for windowed mode, 1 for fullscreen.
 * @returns The jl_gr library context.
**/
jl_gr_t* jl_gr_init(jl_t* jlc, str_t window_name, u8_t fullscreen) {
	jl_gr_t* jl_gr = NULL;

	// Allocate memory
	jl_me_alloc(jlc, (void**)&jl_gr, sizeof(jl_gr_t), 0);
	jl_gr->jlc = jlc;
	jlc->jl_gr = jl_gr;
	// Initialize Subsystem
	jl_io_print(jlc, "Initializing SDL....");
	jl_dl_init__(jl_gr); // create the window.
	jl_io_print(jlc, "Initializing SG....");
	jl_sg_inita__(jl_gr); // Load default graphics from package.
	jl_io_print(jlc, "Initializing GL....");
	jl_gl_init__(jl_gr); // Drawing Set-up
	jl_io_print(jlc, "Initialized GL! / Initializing GR....");
	jl_gr_init__(jl_gr); // Set-up sprites & menubar
	jl_io_print(jlc, "Initialized GR!");
	jl_sg_initb__(jl_gr);
	jl_io_print(jlc, "Initialized SG! / Initializing CT....");
	jl_ct_init__(jl_gr); // Prepare to read input.
	jl_io_print(jlc, "Initialized CT! / Initializing file viewer....");
	jl_gr_fl_init(jl_gr);
	jl_io_print(jlc, "Initializing file viewer!");
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
	jl_gr->sg.redraw.upper = upscreen;
	jl_gr->sg.redraw.lower = downscreen;
	jl_gr->sg.redraw.single = onescreen;
}

/**
 * Make sure that the screen is redrawn.
 * @param jl_gr: The jl_gr library context.
 * @param data: The data to be copied into the drawing thread.
 * @param dataSize: How many bytes are pointed to by "data".
**/
void jl_gr_loop(jl_gr_t* jl_gr, void* data, u32_t dataSize) {
	//Update events.
	jl_ct_loop__(jl_gr);
	// Deselect any pre-renderer.
	jl_gr->gl.cp = NULL;
	//Redraw screen.
	_jl_sg_loop(jl_gr);
	//Update Screen.
	jl_dl_loop__(jl_gr);
}

/**
 * Resize the window.
 * @param jl_gr: The library context.
**/
void jl_gr_resz(jl_gr_t* jl_gr, u16_t x, u16_t y) {
	jvct_t* _jlc = jl_gr->jlc->_jlc;
	if(_jlc->has.graphics) {
		jl_taskbar_t *ctx = jl_gr->gr.taskbar->data.ctx;

		//Menu Bar
		ctx->redraw = 2;
		jl_gr_sp_rsz(jl_gr, jl_gr->gr.taskbar);
		//Mouse
		jl_gr_sp_rsz(jl_gr, jl_gr->jlc->mouse);
	}
	// Reset aspect ratio stuff.
	jl_dl_resz__(jl_gr, x, y);
	// Update the actual window.
	jl_gl_resz__(jl_gr);
	// Update the size of the background.
	jl_sg_resz__(jl_gr->jlc);
}

/**
 * Destroy the window and free the jl_gr library context.
 * @param jl_gr: The jl_gr library context.
**/
void jl_gr_kill(jl_gr_t* jl_gr) {
	jl_gr_pr_old(jl_gr, jl_gr->sg.bg.up);
	jl_gr_pr_old(jl_gr, jl_gr->sg.bg.dn);
	jl_dl_kill__(jl_gr); // Kill window
}

// End of file.
