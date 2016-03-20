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

static void jlgr_init_event(jl_t* jl, void* data) {
	jl_gr_t* jl_gr = jl->jl_gr;
	jlgr_thread_packet_t* packet = data;

	switch(packet->id) {
		case JLGR_COMM_DRAWFIN:
			jl_gr->main.rtn = 1;
			break;
		default: break;
	}
}

//
// Global Functions
//

/**
 * Create a window.
 * @param jl: The library context.
 * @param window_name: The name of the window.
 * @param fullscreen: 0 for windowed mode, 1 for fullscreen.
 * @param fn_: Graphic initialization function run on graphical thread.
 * @returns The jl_gr library context.
**/
jl_gr_t* jlgr_init(jl_t* jl, str_t window_name, u8_t fullscreen, jl_fnct fn_) {
	jl_gr_t* jl_gr = jl_memi(jl, sizeof(jl_gr_t));
	jlgr_thread_packet_t packet = { JLGR_COMM_INIT, 0, 0, fn_ };

	jl_print_function(jl, "JL/GR/INIT");
	jl_gr->jl = jl;
	jl->jl_gr = jl_gr;
	jl_gr->dl.fullscreen = fullscreen;
	jl_gr->gr.taskbar = NULL;
	jl_gr->jl->mouse = NULL;
	// Initialize Subsystem
	JL_PRINT_DEBUG(jl, "Initializing Input....");
	jl_ct_init__(jl_gr); // Prepare to read input.
	JL_PRINT_DEBUG(jl, "Initialized CT! / Initializing file viewer....");
	jl_gr_fl_init(jl_gr);
	JL_PRINT_DEBUG(jl, "Initializing file viewer!");
	jl_print_return(jl, "JL/GR/INIT");
	// Create mutex for multi-threading
	jl_gr->mutex = jl_thread_mutex_new(jl);
	jl_gr->mutexs.usr_ctx = jl_thread_mutex_new(jl);
	// Create communicators for multi-threading
	jl_gr->comm2draw = jl_thread_comm_make(jl,sizeof(jlgr_thread_packet_t));
	jl_gr->comm2main = jl_thread_comm_make(jl,sizeof(jlgr_thread_packet_t));
	// Start Drawing thread.
	jlgr_thread_init(jl_gr);
	// Send graphical Init function
	jl_thread_comm_send(jl,jl_gr->comm2draw,&packet);
	// Wait for drawing thread to initialize.
	jl_print(jl, "Main thread wait....");
	jl_gr->main.rtn = 0;
	while(!jl_gr->main.rtn)
		jl_thread_comm_recv(jl,jl_gr->comm2main,jlgr_init_event);
	jl_print(jl, "Main thread done did wait....");
	return jl_gr;
}

/**
 * Set the functions to be called when the window redraws.
 * @param jl_gr: The jl_gr library context.
 * @param onescreen: The function to redraw the screen when there's only 1 
 *  screen.
 * @param upscreen: The function to redraw the upper or primary display.
 * @param downscreen: The function to redraw the lower or secondary display.
 * @param resize: The function called when window is resized.
**/
void jlgr_loop_set(jl_gr_t* jl_gr, jl_fnct onescreen, jl_fnct upscreen,
	jl_fnct downscreen, jl_fnct resize)
{
	jl_fnct redraw[4] = { onescreen, upscreen, downscreen, resize };
	jlgr_thread_packet_t packet;
	int i;

	// 
	for(i = 0; i < 4; i++) {
		packet = (jlgr_thread_packet_t) {
			JLGR_COMM_SEND, i, 0, redraw[i]
		};
		jl_thread_comm_send(jl_gr->jl, jl_gr->comm2draw, &packet);
	}
}

/**
 * Make sure that the screen is redrawn.
 * @param jl_gr: The jl_gr library context.
 * @param data: The data to be copied into the drawing thread.
 * @param dataSize: How many bytes are pointed to by "data".
**/
void jlgr_loop(jl_gr_t* jl_gr, void* data, u32_t dataSize) {
	// Update events.
	jl_ct_loop__(jl_gr);
	// Copy 
	jl_thread_mutex_cpy(jl_gr->jl,jl_gr->mutexs.usr_ctx,&data,
		jl_gr->share.usr_ctx, dataSize);
}

/**
 * Resize the window.
 * @param jl_gr: The library context.
**/
void jl_gr_resz(jl_gr_t* jl_gr, u16_t w, u16_t h) {
	jlgr_thread_send(jl_gr, JLGR_COMM_RESIZE, w, h, NULL);
}

/**
 * Destroy the window and free the jl_gr library context.
 * @param jl_gr: The jl_gr library context.
**/
void jlgr_kill(jl_gr_t* jl_gr) {
	jlgr_thread_packet_t packet = { JLGR_COMM_KILL, 0, 0, NULL };

	jl_print(jl_gr->jl, "Sending Kill to threads....");
	jl_thread_comm_send(jl_gr->jl, jl_gr->comm2draw, &packet);
	jl_print(jl_gr->jl, "Waiting on threads....");
	jlgr_thread_kill(jl_gr); // Shut down thread.
	jl_gr_pr_old(jl_gr, jl_gr->sg.bg.up);
	jl_gr_pr_old(jl_gr, jl_gr->sg.bg.dn);
}

// End of file.
