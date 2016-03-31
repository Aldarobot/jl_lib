/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRsprite.c
 *	Handles the sprites.
 */
#include "JLGRinternal.h"

static void jlgr_sprite_draw_to_pr__(jl_t *jl) {
	jl_print(jl, "get");
	jl_sprite_t *sprite = jl_mem_temp(jl, NULL);

	jl_print(jl, "%d\\%p", sizeof(jl_sprite_t), sprite->draw);
	((jlgr_sprite_fnt)sprite->draw)(jl, sprite);
}


static inline void jlgr_sprite_redraw_tex__(jlgr_t* jlgr, jl_sprite_t *spr) {
	jl_print(jlgr->jl, "%p", spr->draw);
	jl_mem_temp(jlgr->jl, spr);
	jl_gl_pr(jlgr, spr->pr, jlgr_sprite_draw_to_pr__);
}

//
// Exported Functions
//

/**
 * THREAD: Draw thread only.
 * Run a sprite's draw routine to draw on it's pre-rendered texture.
 *
 * @param jl: The library context
 * @param spr: Which sprite to draw.
**/
void jlgr_sprite_redraw(jlgr_t* jlgr, jl_sprite_t *spr) {
	jl_print(jlgr->jl, "Redraw on thread #%d", jl_thread_current(jlgr->jl));
	// If pre-renderer hasn't been intialized, initialize & redraw.
	if(!spr->pr) jlgr_sprite_resize(jlgr, spr);
	// Else, Redraw texture.
	else jlgr_sprite_redraw_tex__(jlgr, spr);
}

/**
 * THREAD: Draw thread only.
 * Render a sprite's pre-rendered texture onto the screen.
 *
 * @param jl: The library context.
 * @param spr: The sprite.
**/
void jlgr_sprite_draw(jlgr_t* jlgr, jl_sprite_t *spr) {
	jl_print(jlgr->jl, "Draw on thread #%d", jl_thread_current(jlgr->jl));
	jl_pr_t *pr = spr->pr;

	if(!pr) {
		jl_print(jlgr->jl, "jlgr_sprite_drw: not init'd!");
		jl_sg_kill(jlgr->jl);
	}
	jl_gl_transform_pr_(jlgr, pr, spr->tr.x, spr->tr.y, spr->tr.z,
		1., 1., 1.);

	jl_gl_draw_pr_(jlgr->jl, pr);
}

/**
 * THREAD: Draw thread only.
 * Resize a sprite to the current window - and redraw.
**/
void jlgr_sprite_resize(jlgr_t* jlgr, jl_sprite_t *spr) {
	jl_print(jlgr->jl, "Resize on thread #%d", jl_thread_current(jlgr->jl));
	u16_t res = (jlgr->gl.cp ? jlgr->gl.cp->w : jlgr->dl.full_w)
		* spr->rw;

	// Initialize or Resize
	if(jl_gl_pr_isi_(jlgr, spr->pr)) {
		jl_gl_pr_rsz(jlgr,spr->pr,spr->rw,spr->rh,res);
	}else{
		spr->pr = jl_gl_pr_new(jlgr,
			spr->rw, spr->rh, res);
	}
	// Redraw
	jlgr_sprite_redraw_tex__(jlgr, spr);
}

/**
 * THREAD: Main thread only.
 * Run a sprite's loop.
 * @param jl: The library context.
 * @param spr: Which sprite to loop.
**/
void jlgr_sprite_loop(jlgr_t* jlgr, jl_sprite_t *spr) {
	jl_print_function(jlgr->jl, "Sprite/Loop");
	((jlgr_sprite_fnt)spr->loop)(jlgr->jl, spr);
	jl_print_return(jlgr->jl, "Sprite/Loop");
}

/**
 * THREAD: Main thread only.
 * Create a new sprite.
 *
 * @param jl: The library context.
 * @param rc: The rectangle bounding box & pr size.
 * @param a: the transparency each pixel is multiplied by; 255 is
 *	solid and 0 is totally invisble.
 * @param draw: the draw function.
 * @param loop: the loop function.
 * @param ctxs: how many bytes to allocate for the sprite's context.
 * @returns: the new sprite
**/
jl_sprite_t * jlgr_sprite_new(jlgr_t* jlgr, jl_rect_t rc,
	jlgr_sprite_fnt draw, jlgr_sprite_fnt loop, u32_t ctxs)
{
	jl_sprite_t *rtn = NULL;

	rtn = malloc(sizeof(jl_sprite_t));
	// Set translations
	rtn->tr.x = rc.x; rtn->tr.y = rc.y; rtn->tr.z = 0.f;
	// Set collision box.
	rtn->cb.x = rc.x; rtn->cb.y = rc.y;
	rtn->cb.w = rc.w; rtn->cb.h = rc.h;
	// Set real dimensions
	rtn->rw = rc.w;
	rtn->rh = rc.h;
	// Set draw function.
	rtn->draw = draw;
	// Set loop
	rtn->loop = loop;
	// No pre-renderer made yet.
	rtn->pr = NULL;
	// Allocate context
	if(ctxs) rtn->ctx = malloc(ctxs);
	return rtn; 
}

/**
 * THREAD: Main thread only.
 * test if 2 sprites collide.
 *
 * @param 'jl': library context
 * @param 'sprite1': sprite 1
 * @param 'sprite2': sprite 2
 * @return 0: if the sprites don't collide in their bounding boxes.
 * @return 1: if the sprites do collide in their bounding boxes.
**/
u8_t jlgr_sprite_collide(jlgr_t* jlgr,
	jl_sprite_t *sprite1, jl_sprite_t *sprite2)
{
	if (
		(sprite1->cb.y >= (sprite2->cb.y+sprite2->cb.h)) ||
		(sprite1->cb.x >= (sprite2->cb.x+sprite2->cb.w)) ||
		(sprite2->cb.y >= (sprite1->cb.y+sprite1->cb.h)) ||
		(sprite2->cb.x >= (sprite1->cb.x+sprite1->cb.w)) )
	{
		return 0;
	}else{
		return 1;
	}
}
