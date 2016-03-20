/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRguiEvents.c
 *	This file handles drawing taskbar and mouse.
**/
#include "jl_pr.h"
#include "JLGRinternal.h"

//
// Static Functions
//

static inline void jl_gr_taskbar_shadow__(jl_gr_t* jl_gr, jl_taskbar_t* ctx) {
	m_u8_t i;

	// Clear Texture.
	jl_gl_clear(jl_gr, 0, 0, 0, 0);
	// Draw Shadows.
	for(i = 0; i < 10; i++) {
		jl_vec3_t tr = { .9 - (.1 * i), 0., 0. };

		if(!ctx->func[0][i]) break;
		jl_gr_draw_vo(jl_gr, &(ctx->icon[0]), &tr);
	}
	// Set redraw = true.
	ctx->redraw = 1;
}

//
// Used elsewhere in library.
//

// Run whenever a redraw is needed for an icon.
void jl_gr_taskbar_draw_(jl_t* jl, jl_sprd_t* sprd) {
	jl_gr_t* jl_gr = jl->jl_gr;
	jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;

	// If needed, draw shadow.
	if(ctx->redraw == 2) {
		jl_gr_taskbar_shadow__(jl_gr, ctx);
	}
	// Run the selected icon's redraw function.
	if(ctx->func[JL_GRTP_RDR][ctx->cursor]) {
		ctx->func[JL_GRTP_RDR][ctx->cursor](jl_gr);
	}
}

// Run when mouse needs to be redrawn.
void jl_gr_mouse_draw_(jl_t* jl, jl_sprd_t* sprd) {
	// Draw mouse, if using a computer.
#if JL_PLAT == JL_PLAT_COMPUTER //if computer
	jl_gr_t* jl_gr = jl->jl_gr;
	jl_sprite_t* mouse = jl_gr->jl->mouse;
	jl_vo_t* mouse_vo = mouse->data.ctx;
	jl_rect_t rc = { 0.f, 0.f, 1.f, 1.f };

	jl_gr_vos_image(jl_gr, &(mouse_vo[0]), rc, 0,
		JL_IMGI_FONT, 255, 255);
	jl_gr_draw_vo(jl_gr, mouse_vo, NULL);
#endif
}

void jlgr_gui_draw_(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;
	jvct_t* _jl = jl->_jl;

	// Draw taskbar, if needed
	if(!_jl->fl.inloop) {
		jl_taskbar_t *ctx = jl_gr->gr.taskbar->data.ctx;

		if(ctx->redraw) {
			for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
				if(!(ctx->func[0][ctx->cursor])) break;
				jl_gr_sp_rdr(jl_gr, jl_gr->gr.taskbar);
			}
			ctx->redraw = 0;
		}
	}
	// Draw mouse
	if(jl->mouse) jl_gr_sp_drw(jl_gr, jl->mouse);
}
