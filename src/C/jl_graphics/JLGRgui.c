/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRgui.c
 *	This file handles sprites.
**/
#include "jl_pr.h"
#include "JLGRinternal.h"

static inline void jlgr_taskbar_init__(jl_gr_t* jl_gr) {
	jl_rect_t rc = { 0.f, 0.f, 1.f, .11f };
	jl_rect_t rc_icon = { 0., 0., .1, .1};
	jl_rect_t rc_shadow = {-.01, .01, .1, .1 };
	uint8_t shadow_color[] = { 0, 0, 0, 64 };
	jl_taskbar_t *ctx;
	jl_vo_t *icon = jl_gl_vo_make(jl_gr, 2);

	// Make the taskbar.
	jl_gr->gr.taskbar = jl_gr_sp_new(
		jl_gr, rc, jl_gr_taskbar_draw_,
		jl_gr_taskbar_loop_, sizeof(jl_taskbar_t));
	// Get the context.
	ctx = jl_gr->gr.taskbar->data.ctx;
	// Initialize the context.
	ctx->redraw = 2;
	// Set the icon & Shadow vertex objects
	ctx->icon = icon;
	// Make the shadow vertex object.
	jl_gr_vos_rec(jl_gr, &icon[0], rc_shadow, shadow_color, 0);
	// Make the icon vertex object.
	jl_gr_vos_image(jl_gr, &icon[1], rc_icon, 0, 1,
		JLGR_ID_UNKNOWN, 255);
	// Clear the taskbar & make pre-renderer.
	for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
		m_u8_t i;

		for(i = 0; i < 3; i++) ctx->func[i][ctx->cursor] = NULL;
		jl_gr_sp_rdr(jl_gr, jl_gr->gr.taskbar);
	}
	ctx->cursor = -1;
	// Set the loop.
	jl_gr->gr.menuoverlay = jl_gr_taskbar_loop_;
}

static inline void jlgr_mouse_init__(jl_gr_t* jl_gr) {
	jl_rect_t rc = { 0.f, 0.f, .075f, .075f };
	jl_sprite_t* mouse;
	#if JL_PLAT == JL_PLAT_COMPUTER //if computer
		jl_vo_t *mouse_vo = jl_gl_vo_make(jl_gr, 1);
	#endif

	jl_gr->jl->mouse = jl_gr_sp_new(
		jl_gr, rc, jl_gr_mouse_draw_,
		jl_gr_mouse_loop_,
	#if JL_PLAT == JL_PLAT_COMPUTER //if computer
			sizeof(jl_vo_t*));
		// Set the context to the vertex object.
		((jl_sprite_t*)jl_gr->jl->mouse)->data.ctx = mouse_vo;
	#elif JL_PLAT == JL_PLAT_PHONE // if phone
			0);
		// Set the context to NULL.
		((jl_sprite_t*)jl_gr->jl->mouse)->data.ctx = NULL;
	#endif
	mouse = jl_gr->jl->mouse;
	jl_gr_sp_rsz(jl_gr, mouse);
	// Set the mouse's collision width and height to 0
	mouse->data.cb.w = 0.f;
	mouse->data.cb.h = 0.f;
}

/**
 * Create the taskbar and the mouse.
 * @param jl_gr: The graphical context.
**/
void jlgr_gui_init_(jl_gr_t* jl_gr) {
	// Create the Taskbar.
	jlgr_taskbar_init__(jl_gr);
	// Create the Mouse
	jlgr_mouse_init__(jl_gr);
}
