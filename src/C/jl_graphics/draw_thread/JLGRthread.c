/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRthread.c
 *	This file handles a separate thread for drawing graphics.
**/
#include "JLGRinternal.h"

static void jlgr_thread_resize(jlgr_t* jlgr, u16_t w, u16_t h) {
	jl_print(jlgr->jl, "Resizing to %dx%d....", w, h);
	// Reset aspect ratio stuff.
	jl_dl_resz__(jlgr, w, h);
	// Update the actual window.
	jl_gl_resz__(jlgr);
	// Update the size of the background.
	jl_sg_resz__(jlgr->jl);
	// Taskbar resize.
	if(jlgr->gr.taskbar) {
		jl_taskbar_t *ctx = jlgr->gr.taskbar->ctx;

		//Menu Bar
		ctx->redraw = 2;
		jlgr_sprite_resize(jlgr, jlgr->gr.taskbar);
	}
	// Mouse resize
	if(jlgr->mouse) jlgr_sprite_resize(jlgr, jlgr->mouse);
	// Program's Resize
	jl_fnct resize_ = jlgr->draw.redraw.resize;
	resize_(jlgr->jl);
}

static void jlgr_thread_event(jl_t* jl, void* data) {
	jlgr_t* jlgr = jl->jlgr;
	jlgr_thread_packet_t* packet = data;
	jlgr->draw.rtn = 0;

	switch(packet->id) {
		case JLGR_COMM_RESIZE:
			jlgr_thread_resize(jlgr, packet->x, packet->y);
			break;
		case JLGR_COMM_KILL:
			jl_print(jl, "Thread exiting....");
			jlgr->draw.rtn = 1;
			break;
		case JLGR_COMM_INIT:
			JL_PRINT_DEBUG(jl,"Running prg's graphical init func....");
			packet->fn(jl);
			jlgr->draw.rtn = 2;
			break;
		case JLGR_COMM_SEND:
			if(packet->x==0) jlgr->draw.redraw.single = packet->fn;
			if(packet->x==1) jlgr->draw.redraw.upper = packet->fn;
			if(packet->x==2) jlgr->draw.redraw.lower = packet->fn;
			if(packet->x==3) {
				jlgr->draw.redraw.resize = packet->fn;
				packet->fn(jl);
			}
		default:
			break;
	}
}

static u8_t jlgr_thread_draw_event__(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	jlgr->draw.rtn = 0;

	jl_thread_comm_recv(jl, jlgr->comm2draw, jlgr_thread_event);
	return jlgr->draw.rtn;
}

void jl_wm_updatewh_(jlgr_t* jlgr); // TODO: Move

static void jlgr_thread_draw_init__(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	jlgr_thread_packet_t packet = { JLGR_COMM_DRAWFIN, 0, 0 };

	// Initialize subsystems
	JL_PRINT_DEBUG(jl, "Creating the window....");
	jl_dl_init__(jlgr);
	JL_PRINT_DEBUG(jl, "Loading default graphics from package....");
	jl_sg_inita__(jlgr);
	JL_PRINT_DEBUG(jl, "Setting up OpenGL....");
	jl_gl_init__(jlgr);
	JL_PRINT_DEBUG(jl, "Load graphics....");
	jlgr_init__(jlgr);
	JL_PRINT_DEBUG(jl, "Creating Mouse / Taskbar sprites....");
	jlgr_gui_init_(jlgr);
	JL_PRINT_DEBUG(jl, "Force Resize");
	jl_wm_updatewh_(jlgr);
	jlgr_thread_resize(jlgr, jlgr->dl.current.w,jlgr->dl.current.h);
	JL_PRINT_DEBUG(jl, "Looking for init packet....");
	// Wait until recieve init packet....
	while(jlgr_thread_draw_event__(jl) != 2);
	JL_PRINT_DEBUG(jl, "Sending finish packet....");
	// Tell main thread to stop waiting.
	jl_thread_comm_send(jl, jlgr->comm2main, &packet);
}

void jlgr_thread_send(jlgr_t* jlgr, u8_t id, u16_t x, u16_t y, jl_fnct fn) {
	jlgr_thread_packet_t packet = { id, x, y, fn };

	// Send resize packet.
	jl_thread_comm_send(jlgr->jl, jlgr->comm2draw, &packet);
}

int jlgr_thread_draw(void* data) {
	jl_t* jl = data;
	jlgr_t* jlgr = jl->jlgr;

	// Initialize subsystems
	jl_thread_mutex_use(jl, jlgr->mutex, jlgr_thread_draw_init__);
	// Redraw loop
	while(1) {
		// Check for events.
		if(jlgr_thread_draw_event__(jl)) break;
		// Deselect any pre-renderer.
		jlgr->gl.cp = NULL;
		//Redraw screen.
		_jl_sg_loop(jlgr);
		//Update Screen.
		jl_dl_loop__(jlgr);
	}
	return 0;
}

void jlgr_thread_init(jlgr_t* jlgr) {
	jlgr->running = 1;
	jlgr->thread = jl_thread_new(jlgr->jl, "JL_Lib/Graphics",
		jlgr_thread_draw);
}

void jlgr_thread_kill(jlgr_t* jlgr) {
	jlgr->running = 0;
	jl_dl_kill__(jlgr); // Kill window
	jl_thread_old(jlgr->jl, jlgr->thread);
}
