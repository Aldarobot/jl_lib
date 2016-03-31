/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRguiEvents.c
 *	This file handles drawing taskbar and mouse.
**/
#include "JLGRinternal.h"

//
// Static Functions
//

static inline void jlgr_taskbar_shadow__(jlgr_t* jlgr, jl_taskbar_t* ctx) {
	m_u8_t i;

	// Clear Texture.
	jl_gl_clear(jlgr, 0, 0, 0, 0);
	// Draw Shadows.
	for(i = 0; i < 10; i++) {
		jl_vec3_t tr = { .9 - (.1 * i), 0., 0. };

		if(!ctx->func[0][i]) break;
		jlgr_draw_vo(jlgr, &(ctx->icon[0]), &tr);
	}
	// Set redraw = true.
	ctx->redraw = 1;
}

//
// Used elsewhere in library.
//

// Run whenever a redraw is needed for an icon.
void jlgr_taskbar_draw_(jl_t* jl, jl_sprite_t* sprite) {
	jlgr_t* jlgr = jl->jlgr;
	jl_taskbar_t* ctx = jlgr->gr.taskbar->ctx;

	// If needed, draw shadow.
	if(ctx->redraw == 2) {
		jlgr_taskbar_shadow__(jlgr, ctx);
	}
	// Run the selected icon's redraw function.
	if(ctx->func[JL_GRTP_RDR][ctx->cursor]) {
		ctx->func[JL_GRTP_RDR][ctx->cursor](jlgr);
	}
}

// Run when mouse needs to be redrawn.
void jlgr_mouse_draw_(jl_t* jl, jl_sprite_t* sprite) {
	// Draw mouse, if using a computer.
#if JL_PLAT == JL_PLAT_COMPUTER //if computer
	jlgr_t* jlgr = jl->jlgr;
	jl_sprite_t* mouse = jlgr->mouse;
	jl_vo_t* mouse_vo = mouse->ctx;
	jl_rect_t rc = { 0.f, 0.f, 1.f, 1.f };

	jlgr_vos_image(jlgr, &(mouse_vo[0]), rc, 0,
		JL_IMGI_FONT, 255, 255);
	jlgr_draw_vo(jlgr, mouse_vo, NULL);
#endif
}

void jlgr_gui_draw_(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;
	jvct_t* _jl = jl->_jl;

	// Draw taskbar, if needed
	if(!_jl->fl.inloop) {
		jl_taskbar_t *ctx = jlgr->gr.taskbar->ctx;

		if(ctx->redraw) {
			for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
				if(!(ctx->func[0][ctx->cursor])) break;
				jlgr_sprite_redraw(jlgr, jlgr->gr.taskbar);
			}
			ctx->redraw = 0;
		}
	}
	// Draw mouse
	if(jlgr->mouse) jlgr_sprite_draw(jlgr, jlgr->mouse);
}
