/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRguiEvents.c
 *	This file handles events for taskbar and mouse.
**/
#include "jl_pr.h"
#include "JLGRinternal.h"

static u8_t jl_gr_taskbar_idle__(jl_gr_t* jl_gr, jl_taskbar_t *ctx) {
	// If A NULL function then, stop looping taskbar.
	if( !(ctx->func[0][ctx->cursor]) ) return 1;
	// Run the not loop.
	ctx->func[JL_GRTP_NOT][ctx->cursor](jl_gr);
	return 0;
}

// Run when the taskbar is left alone.
static void _jl_gr_taskbar_loop_pass(jl_t* jlc) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jl_taskbar_t *ctx = jl_gr->gr.taskbar->data.ctx;

	for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
		if(jl_gr_taskbar_idle__(jl_gr, ctx))
			break;
	}
}

// Run when the taskbar is clicked/pressed
static void _jl_gr_taskbar_loop_run(jl_t* jlc) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jl_taskbar_t *ctx;

	//If mouse isn't over the taskbar - dont run pressed.
	if(jl_gr->main.ct.msy >= .1) {
		_jl_gr_taskbar_loop_pass(jlc);
		return;
	}
	// Set context
	ctx = jl_gr->gr.taskbar->data.ctx;
	// Figure out what's selected.
	u8_t selected = (m_u8_t)((1. - jl_gr->main.ct.msx) / .1);
	
	for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
		// If A NULL function then, stop looping taskbar.
		if( !(ctx->func[0][ctx->cursor]) ) break;
		// Run the "not" or "yes" loop.
		ctx->func[(ctx->cursor == selected) ?
			JL_GRTP_YES: JL_GRTP_NOT][ctx->cursor](jl_gr);
		// If need to redraw run "rdr"
		if(ctx->redraw) jl_gr_sp_rdr(jl_gr, jl_gr->gr.taskbar);
	}
}

//
// Used elsewhere in library
//

// Runs every frame when taskbar is visible.
void jl_gr_taskbar_loop_(jl_t* jlc, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jlc->jl_gr;

	// Draw the pre-rendered taskbar.
	jl_gr_sp_drw(jl_gr, jl_gr->gr.taskbar);
	// Run the proper loops.
	jl_ct_run_event(jl_gr, JL_CT_PRESS, _jl_gr_taskbar_loop_run,
		_jl_gr_taskbar_loop_pass);
}

// Run every frame for mouse
void jl_gr_mouse_loop_(jl_t* jlc, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jl_sprite_t* mouse = jl_gr->jlc->mouse;

//Update Mouse
	mouse->data.tr.x = jl_ct_gmousex(jl_gr);
	mouse->data.tr.y = jl_ct_gmousey(jl_gr);
	mouse->data.tr.z = 0.;
	mouse->data.cb.x = mouse->data.tr.x;
	mouse->data.cb.y = mouse->data.tr.y;
}

//
//
// Upper Level Functions
//
//

/**
 * Empty sprite loop. ( Don't do anything )
 * @param jlc: The library context
 * @param spr: The sprite
**/
void jl_gr_sp_dont(jl_t* jlc, jl_sprd_t* spr) { }

/**
 * Toggle whether or not to show the menu bar.
 *
 * @param jlc: the libary context
**/
void jl_gr_togglemenubar(jl_gr_t* jl_gr) {
	if(jl_gr->gr.menuoverlay == jl_gr_sp_dont)
		jl_gr->gr.menuoverlay = jl_gr_taskbar_loop_;
	else
		jl_gr->gr.menuoverlay = jl_gr_sp_dont;
}
