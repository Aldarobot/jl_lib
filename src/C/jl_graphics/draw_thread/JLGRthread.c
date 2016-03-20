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

static inline void jlgr_thread_resize(jl_gr_t* jl_gr, u16_t w, u16_t h) {
	// Reset aspect ratio stuff.
	jl_dl_resz__(jl_gr, w, h);
	// Update the actual window.
	jl_gl_resz__(jl_gr);
	// Update the size of the background.
	jl_sg_resz__(jl_gr->jl);
	// Taskbar resize.
	if(jl_gr->gr.taskbar) {
		jl_taskbar_t *ctx = jl_gr->gr.taskbar->data.ctx;

		//Menu Bar
		ctx->redraw = 2;
		jl_gr_sp_rsz(jl_gr, jl_gr->gr.taskbar);
	}
	// Mouse resize
	if(jl_gr->jl->mouse) jl_gr_sp_rsz(jl_gr, jl_gr->jl->mouse);
	// Program's Resize
	jl_fnct resize_ = jl_gr->draw.redraw.resize;
	resize_(jl_gr->jl);
}

static void jlgr_thread_event(jl_t* jl, void* data) {
	jl_gr_t* jl_gr = jl->jl_gr;
	jlgr_thread_packet_t* packet = data;
	jl_gr->draw.rtn = 0;

	switch(packet->id) {
		case JLGR_COMM_RESIZE:
			jlgr_thread_resize(jl_gr, packet->x, packet->y);
			break;
		case JLGR_COMM_KILL:
			jl_print(jl, "Thread exiting....");
			jl_gr->draw.rtn = 1;
			break;
		case JLGR_COMM_INIT:
			JL_PRINT_DEBUG(jl,"Running prg's graphical init func....");
			packet->fn(jl);
			jl_gr->draw.rtn = 2;
			break;
		case JLGR_COMM_SEND:
			if(packet->x==0) jl_gr->draw.redraw.single = packet->fn;
			if(packet->x==1) jl_gr->draw.redraw.upper = packet->fn;
			if(packet->x==2) jl_gr->draw.redraw.lower = packet->fn;
			if(packet->x==3) {
				jl_gr->draw.redraw.resize = packet->fn;
				packet->fn(jl);
			}
		default:
			break;
	}
}

static u8_t jlgr_thread_draw_event__(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;
	jl_gr->draw.rtn = 0;

	jl_thread_comm_recv(jl, jl_gr->comm2draw, jlgr_thread_event);
	return jl_gr->draw.rtn;
}

static void jlgr_thread_draw_init__(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;
	jlgr_thread_packet_t packet = { JLGR_COMM_DRAWFIN, 0, 0 };

	// Initialize subsystems
	JL_PRINT_DEBUG(jl, "Creating the window....");
	jl_dl_init__(jl_gr);
	JL_PRINT_DEBUG(jl, "Loading default graphics from package....");
	jl_sg_inita__(jl_gr);
	JL_PRINT_DEBUG(jl, "Setting up OpenGL....");
	jl_gl_init__(jl_gr);
	JL_PRINT_DEBUG(jl, "Load graphics....");
	jl_gr_init__(jl_gr);
	JL_PRINT_DEBUG(jl, "Creating Mouse / Taskbar sprites....");
	jlgr_gui_init_(jl_gr);
	JL_PRINT_DEBUG(jl, "Looking for init packet....");
	// Wait until recieve init packet....
	while(jlgr_thread_draw_event__(jl) != 2);
	JL_PRINT_DEBUG(jl, "Sending finish packet....");
	// Tell main thread to stop waiting.
	jl_thread_comm_send(jl, jl_gr->comm2main, &packet);
}

void jlgr_thread_send(jl_gr_t* jl_gr, u8_t id, u16_t x, u16_t y, jl_fnct fn) {
	jlgr_thread_packet_t packet = { id, x, y, fn };

	// Send resize packet.
	jl_thread_comm_send(jl_gr->jl, jl_gr->comm2draw, &packet);
}

int jlgr_thread_draw(void* data) {
	jl_t* jl = data;
	jl_gr_t* jl_gr = jl->jl_gr;

	// Initialize subsystems
	jl_thread_mutex_use(jl, jl_gr->mutex, jlgr_thread_draw_init__);
	// Redraw loop
	while(1) {
		// Check for events.
		if(jlgr_thread_draw_event__(jl)) break;
		// Deselect any pre-renderer.
		jl_gr->gl.cp = NULL;
		//Redraw screen.
		_jl_sg_loop(jl_gr);
		//Update Screen.
		jl_dl_loop__(jl_gr);
	}
	return 0;
}

void jlgr_thread_init(jl_gr_t* jl_gr) {
	jl_gr->running = 1;
	jl_gr->thread = jl_thread_new(jl_gr->jl, "JL_Lib/Graphics",
		jlgr_thread_draw);
}

void jlgr_thread_kill(jl_gr_t* jl_gr) {
	jl_gr->running = 0;
	jl_dl_kill__(jl_gr); // Kill window
	jl_thread_old(jl_gr->jl, jl_gr->thread);
}
