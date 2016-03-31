/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRguiEvents.c
 *	This file handles events for taskbar and mouse.
**/
#include "JLGRinternal.h"

static u8_t jlgr_taskbar_idle__(jlgr_t* jlgr, jl_taskbar_t *ctx) {
	// If A NULL function then, stop looping taskbar.
	if( !(ctx->func[0][ctx->cursor]) ) return 1;
	// Run the not loop.
	ctx->func[JL_GRTP_NOT][ctx->cursor](jlgr);
	return 0;
}

// Run when the taskbar is left alone.
static void _jlgr_taskbar_loop_pass(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	jl_taskbar_t *ctx = jlgr->gr.taskbar->ctx;

	for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
		if(jlgr_taskbar_idle__(jlgr, ctx))
			break;
	}
}

// Run when the taskbar is clicked/pressed
static void _jlgr_taskbar_loop_run(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	jl_taskbar_t *ctx;

	//If mouse isn't over the taskbar - dont run pressed.
	if(jlgr->main.ct.msy >= .1) {
		_jlgr_taskbar_loop_pass(jl);
		return;
	}
	// Set context
	ctx = jlgr->gr.taskbar->ctx;
	// Figure out what's selected.
	u8_t selected = (m_u8_t)((1. - jlgr->main.ct.msx) / .1);
	
	for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
		// If A NULL function then, stop looping taskbar.
		if( !(ctx->func[0][ctx->cursor]) ) break;
		// Run the "not" or "yes" loop.
		ctx->func[(ctx->cursor == selected) ?
			JL_GRTP_YES: JL_GRTP_NOT][ctx->cursor](jlgr);
		// If need to redraw run "rdr"
		if(ctx->redraw) jlgr_sprite_redraw(jlgr, jlgr->gr.taskbar);
	}
}

//
// Used elsewhere in library
//

// Runs every frame when taskbar is visible.
void jlgr_taskbar_loop_(jl_t* jl, jl_sprite_t* sprite) {
	jlgr_t* jlgr = jl->jlgr;

	// Draw the pre-rendered taskbar.
	jlgr_sprite_draw(jlgr, jlgr->gr.taskbar);
	// Run the proper loops.
	jl_ct_run_event(jlgr, JL_CT_PRESS, _jlgr_taskbar_loop_run,
		_jlgr_taskbar_loop_pass);
}

// Run every frame for mouse
void jlgr_mouse_loop_(jl_t* jl, jl_sprite_t* sprite) {
	jlgr_t* jlgr = jl->jlgr;
	jl_sprite_t* mouse = jlgr->mouse;

//Update Mouse
	mouse->tr.x = jl_ct_gmousex(jlgr);
	mouse->tr.y = jl_ct_gmousey(jlgr);
	mouse->tr.z = 0.;
	mouse->cb.x = mouse->tr.x;
	mouse->cb.y = mouse->tr.y;
}

//
//
// Upper Level Functions
//
//

/**
 * Empty sprite loop. ( Don't do anything )
 * @param jl: The library context
 * @param spr: The sprite
**/
void jlgr_sp_dont(jl_t* jl, jl_sprite_t* sprite) { }

/**
 * Toggle whether or not to show the menu bar.
 *
 * @param jl: the libary context
**/
void jlgr_togglemenubar(jlgr_t* jlgr) {
	if(jlgr->gr.menuoverlay == jlgr_sp_dont)
		jlgr->gr.menuoverlay = jlgr_taskbar_loop_;
	else
		jlgr->gr.menuoverlay = jlgr_sp_dont;
}
