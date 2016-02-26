/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * gr.c
 *	A High Level Graphics Library that supports sprites, texture loading,
 *	2D rendering & 3D rendering.
 */
#include "jl_pr.h"
#include "JLGRinternal.h"

typedef enum{
	JLGR_ID_NULL,
	JLGR_ID_UNKNOWN,
	JLGR_ID_FLIP_IMAGE,
	JLGR_ID_SLOW_IMAGE,
	JLGR_ID_GOOD_IMAGE,
	JLGR_ID_TASK_MAX //how many taskbuttons
}jlgr_id_t;

typedef enum{
	JL_GRTP_NOT,
	JL_GRTP_YES,
	JL_GRTP_RDR,
}jl_gr_task_press_t;

/*screen being displayed ( on two screens which one on bottom, if keyboard is
	being displayed on bottom screen then this will be displayed on top )*/
float timeTilMessageVanish = 0.f;

char *GMessage[2] = {
	"SWITCH SCREEN: UPPER",
	"SWITCH SCREEN: LOWER"
};

//Upper OpenGL prototypes
void jl_gl_vo_free(jl_gr_t* jl_gr, jl_vo_t *pv);
void jl_gl_draw_pr(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gl_pr_old(jl_gr_t* jl_gr, jl_vo_t* pv);
uint8_t jl_gl_pr_isi(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gl_pr_old_(jl_gr_t* jl_gr, jl_pr_t** pr);

//Graphics Prototypes
static void _jl_gr_textbox_rt(jl_gr_t* jl_gr);
static void _jl_gr_textbox_lt(jl_gr_t* jl_gr);

/***      @cond       ***/
/************************/
/*** Static Functions ***/
/************************/

	static inline void _jl_gr_init_vos(jl_gr_t* jl_gr) {
		jl_gr->gr.vos.whole_screen = jl_gl_vo_make(jl_gr, 1);
	}
	
	static inline void _jl_gr_flip_scrn(jl_gr_t* jl_gr) {
// TODO: Re-Implement
//		if(jl_gr->loop == JL_SG_WM_UP) {
//			jl_gr->loop = JL_SG_WM_DN;
			timeTilMessageVanish = 8.5f;
//		}else{
//			jl_gr->loop = JL_SG_WM_UP;
//			timeTilMessageVanish = 8.5f;
//		}
	}
	
	static void jl_gr_sprite_draw_to_pr__(jl_t *jlc) {
		jl_sprite_t *spr = jl_me_tmp_ptr(jlc, 0, NULL);

		spr->draw(jlc->jl_gr, &(spr->data));
	}

	static void _jl_gr_popup_loop(jl_t *jlc) {
//		jl_gr_draw_rect(jlc, .1, .1, .8, .2, 127, 127, 255, 255);
//		jl_gr_draw_rect(jlc, .1, .3, .8, .8, 64, 127, 127, 255);
	}

	static void _jl_gr_textbox_lt(jl_gr_t* jl_gr) {
		jl_ct_typing_disable();
		if(jl_gr->gr.textbox_string->curs)
			jl_gr->gr.textbox_string->curs--;
	}

	static void _jl_gr_textbox_rt(jl_gr_t* jl_gr) {
		jl_ct_typing_disable();
		if(jl_gr->gr.textbox_string->curs < jl_gr->gr.textbox_string->size)
			jl_gr->gr.textbox_string->curs++;
	}

	static void jl_gr_taskbar_icon__(jl_gr_t* jl_gr, uint16_t g, uint16_t i,
		uint8_t c)
	{
		jl_rect_t rc_icon = { 0., 0., .1, .1};
		jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;
		jl_vec3_t tr = { .9 - (.1 * ctx->cursor), 0., 0. };

		jl_gr_vos_image(jl_gr, &(ctx->icon[1]), rc_icon, g, i, c, 255);
		jl_gr_draw_vo(jl_gr, &(ctx->icon[1]), &tr);
	}

	static void jl_gr_taskbar_text__(jl_gr_t* jl_gr, m_u8_t* color,
		str_t text)
	{
		jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;
		jl_vec3_t tr = { .9 - (.1 * ctx->cursor), 0., 0. };

		jl_gr_draw_text(jl_gr, text, tr,
			(jl_font_t) { 0, JL_IMGI_ICON, 0, color, 
				.1 / strlen(text)});
	}
	
	static void jl_gr_menu_flip_draw__(jl_gr_t* jl_gr) {
		jl_gr_taskbar_icon__(jl_gr, 0, JL_IMGI_ICON, JLGR_ID_FLIP_IMAGE);
	}

	static void jl_gr_menu_flip_press__(jl_gr_t* jl_gr) {
		if(jl_gr->jlc->ctrl.h != 1) return;
		_jl_gr_flip_scrn(jl_gr);
	}
	
	static void jl_gr_menu_name_draw2__(jl_gr_t* jl_gr) {
		jl_gr_taskbar_icon__(jl_gr, 0, JL_IMGI_ICON, JLGR_ID_UNKNOWN);
	}

	static void jl_gr_menu_name_draw__(jl_gr_t* jl_gr) {
		jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;
		f32_t text_size = jl_gl_ar(jl_gr) * .5;

		jl_gr_menu_name_draw2__(jl_gr);
		jl_gr_draw_text(jl_gr, jl_gr->dl.windowTitle[0],
			(jl_vec3_t) { 1. - (jl_gl_ar(jl_gr) * (ctx->cursor+1.)),
				0., 0. },
			(jl_font_t) { 0, JL_IMGI_ICON, 0, jl_gr->jlc->fontcolor, 
				text_size});
		jl_gr_draw_text(jl_gr, jl_gr->dl.windowTitle[1],
			(jl_vec3_t) { 1. - (jl_gl_ar(jl_gr) * (ctx->cursor+1.)),
				text_size, 0. },
			(jl_font_t) { 0, JL_IMGI_ICON, 0, jl_gr->jlc->fontcolor, 
				text_size});
	}

	static void _jl_gr_menu_slow_draw(jl_gr_t* jl_gr) {
		jl_t* jlc = jl_gr->jlc;
		m_u8_t color[] = { 255, 255, 255, 255 };

		// Draw the icon based on whether on time or not.
		jl_gr_taskbar_icon__(jl_gr, 0, JL_IMGI_ICON, jl_gr->sg.on_time ?
			JLGR_ID_GOOD_IMAGE : JLGR_ID_SLOW_IMAGE);
		// If not on time report the seconds that passed.
		if(!jl_gr->sg.on_time)
			jl_gr_taskbar_text__(jl_gr, color,
				jl_me_format(jlc, "%d fps", jlc->time.fps));
	}

	static void _jl_gr_menu_slow_loop(jl_gr_t* jl_gr) {
		jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;

		if(jl_gr->sg.changed || !jl_gr->sg.on_time) ctx->redraw = 1;
	}

	//TODO: Move
	static inline u8_t jl_gr_taskbar_idle__(jl_gr_t* jl_gr, jl_taskbar_t *ctx) {
		// If A NULL function then, stop looping taskbar.
		if( !(ctx->func[0][ctx->cursor]) ) return 1;
		// Run the not loop.
		ctx->func[JL_GRTP_NOT][ctx->cursor](jl_gr);
		return 0;
	}

	// Run when the taskbar is left alone.
	static void _jl_gr_taskbar_loop_pass(jl_gr_t* jl_gr) {
		jl_taskbar_t *ctx = jl_gr->gr.taskbar->data.ctx;

		if(ctx->redraw) {
			for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
				if(jl_gr_taskbar_idle__(jl_gr, ctx)) break;
				jl_gr_sp_rdr(jl_gr, jl_gr->gr.taskbar);
			}
			ctx->redraw = 0;
		}else{
			for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++)
				if(jl_gr_taskbar_idle__(jl_gr, ctx)) break;
		}
	}

	// TODO: MOVE
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

	// Run when the taskbar is clicked/pressed
	static void _jl_gr_taskbar_loop_run(jl_gr_t* jl_gr) {
		jl_taskbar_t *ctx;

		//If mouse isn't over the taskbar - dont run pressed.
		if(jl_gr->ct.msy >= .1) {
			_jl_gr_taskbar_loop_pass(jl_gr);
			return;
		}
		// Set context
		ctx = jl_gr->gr.taskbar->data.ctx;
		// Figure out what's selected.
		u8_t selected = (m_u8_t)((1. - jl_gr->ct.msx) / .1);
		
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

	// Run whenever a redraw is needed for an icon.
	static void _jl_gr_taskbar_draw(void* jl_gr2, jl_sprd_t* sprd) {
		jl_gr_t* jl_gr = jl_gr2;
		jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;

		if(ctx->redraw == 2) {
			jl_gr_taskbar_shadow__(jl_gr, ctx);
			
		}
		// Run the selected icon's redraw function
		if(ctx->func[JL_GRTP_RDR][ctx->cursor]) {
			ctx->func[JL_GRTP_RDR][ctx->cursor](jl_gr);
		}
	}

	// Runs every frame.
	static void _jl_gr_taskbar_loop(void* jl_gr2, jl_sprd_t* sprd) {
		jl_gr_t* jl_gr = jl_gr2;

		// Draw the pre-rendered taskbar.
		jl_gr_sp_drw(jl_gr, jl_gr->gr.taskbar);
		// Run the proper loops.
		jl_ct_run_event(jl_gr, JL_CT_PRESS, _jl_gr_taskbar_loop_run,
			_jl_gr_taskbar_loop_pass);
	}

	static void _jl_gr_taskbar_dont(void* jl_gr, jl_sprd_t* sprd) { }

	static void jl_gr_menu_init__(jl_gr_t* jl_gr) {
		jl_rect_t rc = { 0.f, 0.f, 1.f, .11f };
		jl_rect_t rc_icon = { 0., 0., .1, .1};
		jl_rect_t rc_shadow = {-.01, .01, .1, .1 };
		uint8_t shadow_color[] = { 0, 0, 0, 64 };
		jl_taskbar_t *ctx;
		jl_vo_t *icon = jl_gl_vo_make(jl_gr, 2);

		// Make the taskbar.
		jl_gr->gr.taskbar = jl_gr_sp_new(
			jl_gr, rc, _jl_gr_taskbar_draw,
			_jl_gr_taskbar_loop, sizeof(jl_taskbar_t));
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
	}

	static void _jl_gr_mouse_loop(void* jl_gr2, jl_sprd_t* sprd) {
		jl_gr_t* jl_gr = jl_gr2;
		jl_sprite_t* mouse = jl_gr->jlc->mouse;
		jl_vo_t* mouse_vo = mouse->data.ctx;

	//Update Mouse
		mouse->data.tr.x = jl_ct_gmousex(jl_gr);
		mouse->data.tr.y = jl_ct_gmousey(jl_gr);
		mouse->data.cb.x = mouse->data.tr.x;
		mouse->data.cb.y = mouse->data.tr.y;
	// Draw mouse
		#if JL_PLAT == JL_PLAT_COMPUTER //if computer
			jl_gr_draw_vo(jl_gr, mouse_vo, &(mouse->data.tr));
		#endif
	}

	static inline void _jl_gr_mouse_init(jl_gr_t* jl_gr) {
		jl_rect_t rc = { 0.f, 0.f, .075f, .075f };
		jl_vo_t *mouse = jl_gl_vo_make(jl_gr, 1);

		#if JL_PLAT == JL_PLAT_COMPUTER //if computer
			jl_gr_vos_image(jl_gr, &(mouse[0]), rc, 0,
				JL_IMGI_FONT, 255, 255);
		#endif
		jl_gr->jlc->mouse = jl_gr_sp_new(
			jl_gr, rc, jl_gr_sp_dont,
			_jl_gr_mouse_loop,
		#if JL_PLAT == JL_PLAT_COMPUTER //if computer
				sizeof(jl_vo_t*));
			// Set the context to the vertex object.
			((jl_sprite_t*)jl_gr->jlc->mouse)->data.ctx = mouse;
		#elif JL_PLAT == JL_PLAT_PHONE // if phone
				0);
			// Set the context to NULL.
			((jl_sprite_t*)jl_gr->jlc->mouse)->data.ctx = NULL;
		#endif
	}

/**      @endcond      **/
/************************/
/*** Global Functions ***/
/************************/

	void jl_gr_sp_dont(void* jl_gr, jl_sprd_t* spr) { }
	void jl_gr_dont(jl_gr_t* jl_gr) { }

	/**
	 * Prepare to draw an image that takes up the entire pre-renderer or
	 * screen.
	 * @param jlc: The library context.
  	 * @param g: the image group that the image pointed to by 'i' is in.
	 * @param i:  the ID of the image.
	 * @param c: is 0 unless you want to use the image as
	 * 	a charecter map, then it will zoom into charecter 'chr'.
	 * @param a: the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
	**/
	void jl_gr_fill_image_set(jl_gr_t* jl_gr,u16_t g,u16_t i,u8_t c,u8_t a){
		jl_rect_t rc = { 0., 0., 1., jl_gl_ar(jl_gr) };

		jl_gr_vos_image(jl_gr, jl_gr->gr.vos.whole_screen, rc,
			g, i, c, a);
	}

	/**
	 * Draw the image prepared with jl_gr_fill_image_set().
	 * @param jlc: The library context.
	**/
	void jl_gr_fill_image_draw(jl_gr_t* jl_gr) {
		jl_gr_draw_vo(jl_gr, jl_gr->gr.vos.whole_screen, NULL);
	}

	/**
	 * Delete a pr image for a vertex object.
	 * @param jlc: The library context.
	 * @param pv: The pr's vertex object.
	**/
	void jl_gr_pr_old(jl_gr_t* jl_gr, jl_vo_t* pv) {
		jl_gl_pr_old(jl_gr, pv);
	}

	/**
	 * Create or replace a pr image for a vertex object based on
	 * the bounding box.
	 * @param jlc: The library context.
	 * @param pv: The vertex object to create/replace a pr for.
	 * @param xres: The resolution across the x axis for the pre-renderer.
	**/
	void jl_gr_pr_new(jl_gr_t* jl_gr, jl_vo_t* pv, u16_t xres) {
		uint8_t isi = jl_gl_pr_isi(jl_gr, pv);
		if(isi == 2) {
			jl_io_print(jl_gr->jlc, "jl_gr_pr_new: VO is NULL");
			jl_sg_kill(jl_gr->jlc);
		}
		// If prender is already initialized, resize, otherwise init.
		if(isi) jl_gl_pr_rsz(jl_gr, pv->pr, pv->cb.w, pv->cb.h, xres);
		else pv->pr = jl_gl_pr_new(jl_gr, pv->cb.w, pv->cb.h, xres);
		// Check if pre-renderer is initialized.
		if(!pv->pr) {
			jl_io_print(jl_gr->jlc, "Prerender Failed Allocation.");
			jl_sg_kill(jl_gr->jlc);
		}
		if(!jl_gl_pr_isi(jl_gr, pv)) {
			jl_io_print(jl_gr->jlc, "jl_gr_pr_new: didn't make!");
			jl_sg_kill(jl_gr->jlc);
		}
	}

	/**
	 * Render an image onto a vertex object's pr.
	 * @param jlc: The library context.
	 * @param vo: The vertex object that contains the pre-renderer.
	 * @param par__redraw: The redraw functions for the pre-renderer.
	**/
	void jl_gr_pr(jl_gr_t* jl_gr, jl_vo_t* vo, jl_fnct par__redraw) {
		jl_gl_pr(jl_gr, vo->pr, par__redraw);
	}

	/**
	 * Draw a vertex object's pre-rendered texture.
  	 * @param jlc: The library context.
  	 * @param pv: The vertex object to get the pre-rendered texture from.
  	 * @param vec: The vector of offset/translation.
	**/
	void jl_gr_pr_draw(jl_gr_t* jl_gr, jl_vo_t* pv, jl_vec3_t* vec) {
		if(pv == NULL) pv = jl_gr->gl.temp_vo;
		jl_gl_pr_draw(jl_gr, pv->pr, vec, NULL);
	}

	/**
	 * Convert a color.
	 * @param jlc: The library context.
	 * @param rgba: The color to convert ( Not freed - Reusable ).
	 * @param vc: How many vertices to acount for.
	 * @param gradient: 1 if "rgba" is a gradient array, 0 if solid color.
	 * @returns: The converted color
	**/
	jl_ccolor_t* jl_gr_convert_color(jl_gr_t* jl_gr, uint8_t *rgba, uint32_t vc,
		uint8_t gradient)
	{
		if(gradient) return jl_gl_clrcg(jl_gr, rgba, vc);
		else return jl_gl_clrcs(jl_gr, rgba, vc);
	}

	/**
	 * Change the coloring scheme for a vertex object.
 	 * @param jlc: The library context.
 	 * @param pv: The Vertex Object
 	 * @param cc: The Converted Color Object to use on the Vertex Object.
 	 *	The library takes care of freeing this variable.
	**/
	void jl_gr_vo_color(jl_gr_t* jl_gr, jl_vo_t* pv, jl_ccolor_t* cc) {
		jl_gl_clrc(jl_gr, pv, cc);
	}

	/**
	 * Draw a vertex object with offset by translation.
  	 * @param jlc: The library context.
  	 * @param pv: The vertex object to draw.
  	 * @param vec: The vector of offset/translation.
	**/
	void jl_gr_draw_vo(jl_gr_t* jl_gr, jl_vo_t* pv, jl_vec3_t* vec) {
		if(vec == NULL) {
			jl_gl_transform_vo_(jl_gr, pv,
				0.f, 0.f, 0.f, 1., 1., 1.);
		}else{
			jl_gl_transform_vo_(jl_gr, pv,
				vec->x, vec->y, vec->z, 1., 1., 1.);
		}
		jl_gl_draw(jl_gr, pv);
	}

	/**
	 * Set a Vertex object to vector graphics.
	 * 
	**/
	void jl_gr_vos_vec(jl_gr_t* jl_gr, jl_vo_t *pv, uint16_t tricount,
		float* triangles, uint8_t* colors, uint8_t multicolor)
	{
		int i;

		// Overwrite the vertex object
		jl_gl_vect(jl_gr, pv, tricount * 3, triangles);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jl_gr, pv, colors);
		else jl_gl_clrs(jl_gr, pv, colors);
		// Set collision box.
		pv->cb.x = 1.f, pv->cb.y = jl_gl_ar(jl_gr), pv->cb.z = 0.f;
		pv->cb.w = 0.f, pv->cb.h = 0.f, pv->cb.d = 0.f;
		// Find the lowest and highest values to make a box.
		for(i = 0; i < tricount*9; i+=3) {
			float xx = triangles[i];
			float yy = triangles[i+1];
			if(xx < pv->cb.x) pv->cb.x = xx;
			if(yy < pv->cb.y) pv->cb.y = yy;
			if(xx > pv->cb.w) pv->cb.w = xx;
			if(yy > pv->cb.h) pv->cb.h = yy;
		}
		pv->cb.w -= pv->cb.x, pv->cb.h -= pv->cb.y;
	}

	/**
	 * Set a vertex object to a rectangle.
 	 * @param jlc: The library context.
 	 * @param pv: The vertex object
	 * @param rc: The rectangle coordinates.
	 * @param colors: The color(s) to use - [ R, G, B, A ]
	 * @param multicolor: If 0: Then 1 color is used.
	 *	If 1: Then 1 color per each vertex is used.
	**/
	void jl_gr_vos_rec(jl_gr_t* jl_gr, jl_vo_t *pv, jl_rect_t rc, u8_t* colors,
		uint8_t multicolor)
	{
		float rectangle_coords[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w + rc.x,	rc.y,		0.f,
			rc.w + rc.x,	rc.h+rc.y,	0.f };

		// Overwrite the vertex object
		jl_gl_poly(jl_gr, pv, 4, rectangle_coords);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jl_gr, pv, colors);
		else jl_gl_clrs(jl_gr, pv, colors);
		// Set collision box.
		pv->cb.x = rc.x, pv->cb.y = rc.y, pv->cb.z = 0.f;
		pv->cb.w = rc.w, pv->cb.h = rc.h, pv->cb.d = 0.f;
	}

	/**
	 * Set a vertex object to an Image.
	 *
	 * @param jlc: The library context
  	 * @param pv: The vertex object
 	 * @param rc: the rectangle to draw the image in.
   	 * @param g: the image group that the image pointed to by 'i' is in.
	 * @param i:  the ID of the image.
	 * @param c: is 0 unless you want to use the image as
	 * 	a charecter map, then it will zoom into charecter 'chr'.
	 * @param a: the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
	**/
	void jl_gr_vos_image(jl_gr_t* jl_gr, jl_vo_t *pv, jl_rect_t rc,
		u16_t g, u16_t i, u8_t c, u8_t a)
	{
		//From bottom left & clockwise
		float Oone[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w+rc.x,	rc.y,		0.f,
			rc.w+rc.x,	rc.h+rc.y,	0.f };
		// Overwrite the vertex object
		jl_gl_poly(jl_gr, pv, 4, Oone);
		// Texture the vertex object
		jl_gl_txtr(jl_gr, pv, c, a, g, i);
		// Set collision box.
		pv->cb.x = rc.x, pv->cb.y = rc.y, pv->cb.z = 0.f;
		pv->cb.w = rc.w, pv->cb.h = rc.h, pv->cb.d = 0.f;
	}

	void jl_gr_vos_texture(jl_gr_t* jl_gr, jl_vo_t *pv, jl_rect_t rc,
		jl_tex_t* tex, u8_t c, u8_t a)
	{
		//From bottom left & clockwise
		float Oone[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w+rc.x,	rc.y,		0.f,
			rc.w+rc.x,	rc.h+rc.y,	0.f };
		// Overwrite the vertex object
		jl_gl_poly(jl_gr, pv, 4, Oone);
		// Texture the vertex object
		jl_gl_txtr_(jl_gr, pv, c, a, tex->gl_texture);
		// Set collision box.
		pv->cb.x = rc.x, pv->cb.y = rc.y, pv->cb.z = 0.f;
		pv->cb.w = rc.w, pv->cb.h = rc.h, pv->cb.d = 0.f;
	}

	/**
	 * Free a vertex object.
	 * @param jlc: The library context
	 * @param pv: The vertex object to free
	**/
	void jl_gr_vo_old(jl_gr_t* jl_gr, jl_vo_t* pv) {
		jl_gl_vo_free(jl_gr, pv);
	}

	// TODO: Move.
	static inline void jl_gr_sp_redraw_tex__(jl_gr_t* jl_gr,
		jl_sprite_t *spr)
	{
		jl_me_tmp_ptr(jl_gr->jlc, 0, spr);
		jl_gl_pr(jl_gr, spr->pr, jl_gr_sprite_draw_to_pr__);
	}

	/**
	 * Run a sprite's draw routine to draw on it's pre-rendered texture.
	 *
	 * @param jlc: The library context
	 * @param spr: Which sprite to draw.
	**/
	void jl_gr_sp_rdr(jl_gr_t* jl_gr, jl_sprite_t *spr) {
		// If pre-renderer hasn't been intialized, initialize & redraw.
		if(!spr->pr) jl_gr_sp_rsz(jl_gr, spr);
		// Else, Redraw texture.
		else jl_gr_sp_redraw_tex__(jl_gr, spr);
	}

	/**
	 * Resize a sprite to the current window - and redraw.
	**/
	void jl_gr_sp_rsz(jl_gr_t* jl_gr, jl_sprite_t *spr) {
		u16_t res = (jl_gr->gl.cp ? jl_gr->gl.cp->w : jl_gr->dl.full_w)
			* spr->data.rw;

		// Initialize or Resize
		if(jl_gl_pr_isi_(jl_gr, spr->pr)) {
			jl_gl_pr_rsz(jl_gr,spr->pr,spr->data.rw,spr->data.rh,res);
		}else{
			spr->pr = jl_gl_pr_new(jl_gr,
				spr->data.rw, spr->data.rh, res);
		}
		// Redraw
		jl_gr_sp_redraw_tex__(jl_gr, spr);
	}

	/**
	 * Resize and redraw a sprite.
	**/
	void jl_gr_sp_rds(jl_gr_t* jl_gr, jl_sprite_t *spr) {
		// Redraw
		jl_gr_sp_rdr(jl_gr, spr);
	}

	/**
	 * Run a sprite's loop.
	 * @param jlc: The library context.
	 * @param spr: Which sprite to loop.
	**/
	void jl_gr_sp_rnl(jl_gr_t* jl_gr, jl_sprite_t *spr) {
		spr->loop(jl_gr->jlc, &(spr->data));
	}

	/**
	 * Render a sprite's pre-rendered texture onto the screen.
	 *
	 * @param jlc: The library context.
	 * @param spr: The sprite.
	**/
	void jl_gr_sp_drw(jl_gr_t* jl_gr, jl_sprite_t *spr) {
		jl_pr_t *pr = spr->pr;

		if(!pr) {
			jl_io_print(jl_gr->jlc, "jl_gr_sp_drw: not init'd!");
			jl_sg_kill(jl_gr->jlc);
		}
		jl_gl_transform_pr_(jl_gr, pr, spr->data.tr.x,
			spr->data.tr.y, spr->data.tr.z, 1., 1., 1.);

		jl_gl_draw_pr_(jl_gr->jlc, pr);
	}

	/**
	 * Create a new sprite.
	 *
	 * @param jlc: The library context.
	 * @param rc: The rectangle bounding box & pr size.
 	 * @param a: the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
	 * @param draw: the draw function.
 	 * @param loop: the loop function.
 	 * @param ctxs: how many bytes to allocate for the sprite's context.
 	 * @returns: the new sprite
	**/
	jl_sprite_t * jl_gr_sp_new(jl_gr_t* jl_gr, jl_rect_t rc,
		jl_gr_sp_fnt draw, jl_gr_sp_fnt loop, u32_t ctxs)
	{
		jl_sprite_t *rtn = NULL;

		rtn = malloc(sizeof(jl_sprite_t));
		// Set spr->data->spr to spr
		rtn->data.spr = rtn;
		// Set translations
		rtn->data.tr.x = rc.x; rtn->data.tr.y = rc.y; rtn->data.tr.z = 0.f;
		// Set collision box.
		rtn->data.cb.x = rc.x; rtn->data.cb.y = rc.y;
		rtn->data.cb.w = rc.w; rtn->data.cb.h = rc.h;
		// Set real dimensions
		rtn->data.rw = rc.w;
		rtn->data.rh = rc.h;
		// Set draw function.
		rtn->draw = draw;
		// Set loop
		rtn->loop = loop;
		// No pre-renderer made yet.
		rtn->pr = NULL;
		// Allocate context
		if(ctxs) rtn->data.ctx = malloc(ctxs);
		return rtn; 
	}

	/**
	 * test if 2 sprites collide.
	 *
	 * @param 'jlc': library context
	 * @param 'sprite1': sprite 1
	 * @param 'sprite2': sprite 2
	 * @return 0: if the sprites don't collide in their bounding boxes.
	 * @return 1: if the sprites do collide in their bounding boxes.
	**/
	u8_t jl_gr_sprite_collide(jl_gr_t* jl_gr,
		jl_sprite_t *sprite1, jl_sprite_t *sprite2)
	{
		if (
			(sprite1->data.cb.y >= (sprite2->data.cb.y+sprite2->data.cb.h)) ||
			(sprite1->data.cb.x >= (sprite2->data.cb.x+sprite2->data.cb.w)) ||
			(sprite2->data.cb.y >= (sprite1->data.cb.y+sprite1->data.cb.h)) ||
			(sprite2->data.cb.x >= (sprite1->data.cb.x+sprite1->data.cb.w)) )
		{
			return 0;
		}else{
			return 1;
		}
	}

	/**
	 * Draw text on the current pre-renderer.
	 * @param 'jlc': library context
	 * @param 'str': the text to draw
	 * @param 'loc': the position to draw it at
	 * @param 'f': the font to use.
	**/
	void jl_gr_draw_text(jl_gr_t* jl_gr, str_t str, jl_vec3_t loc,
		jl_font_t f)
	{
		const void *Str = str;
		const uint8_t *STr = Str;
		uint32_t i;
		jl_rect_t rc = { loc.x, loc.y, f.size, f.size };
		jl_vec3_t tr = { 0., 0., 0. };
		jl_vo_t* vo = jl_gr->gl.temp_vo;

		if(str == NULL) return;
		for(i = 0; i < strlen(str); i++) {
			//Font 0:0
			jl_gr_vos_image(jl_gr,vo,rc,0,JL_IMGI_FONT,STr[i],255);
			jl_gl_transform_chr_(jl_gr, vo, tr.x, tr.y, tr.z,
				1., 1., 1.);
			jl_gl_draw_chr(jl_gr, vo,((double)f.colors[0])/255.,
				((double)f.colors[1])/255.,
				((double)f.colors[2])/255.,
				((double)f.colors[3])/255.);
			tr.x += f.size;
		}
	}

	/**
	 * draw an integer on the screen
 	 * @param 'jlc': library context
	 * @param 'num': the number to draw
	 * @param 'loc': the position to draw it at
	 * @param 'f': the font to use.
	 */
	void jl_gr_draw_int(jl_gr_t* jl_gr, i64_t num, jl_vec3_t loc, jl_font_t f) {
		char display[10];
		sprintf(display, "%ld", num);
		jl_gr_draw_text(jl_gr, display, loc, f);
	}

	/**
	 * draw a floating point on the screen
 	 * @param 'jlc': library context
	 * @param 'num': the number to draw
	 * @param 'dec': the number of places after the decimal to include.
	 * @param 'loc': the position to draw it at
	 * @param 'f': the font to use.
	 */
	void jl_gr_draw_float(jl_gr_t* jl_gr, f64_t num, u8_t dec, jl_vec3_t loc,
		jl_font_t f)
	{
		char display[10];
		char convert[10];

		sprintf(convert, "%%%df", dec);
		sprintf(display, convert, num);
		jl_gr_draw_text(jl_gr, display, loc, f);
	}

	/**
	 * Draw text within the boundary of a sprite
	 * @param 'jlc': library context
	 * @param 'spr': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_area(jl_gr_t* jl_gr, jl_sprite_t * spr, str_t txt){
		float fontsize = .9 / strlen(txt);
		jl_gr_draw_text(jl_gr, txt,
			(jl_vec3_t) { .05,.5 * (jl_gl_ar(jl_gr) - fontsize),0. },
			(jl_font_t) { 0, JL_IMGI_ICON, 0, jl_gr->jlc->fontcolor, 
				fontsize});
	}

	/**
	 * Draw a sprite, then draw text within the boundary of a sprite
 	 * @param 'jlc': library context
	 * @param 'spr': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_sprite(jl_gr_t* jl_gr,jl_sprite_t * spr, str_t txt) {
		jl_gr_fill_image_set(jl_gr, 0, JL_IMGI_ICON, 1, 255);
		jl_gr_fill_image_draw(jl_gr);
		jl_gr_draw_text_area(jl_gr, spr, txt);
	}

	/**
	 * Draw centered text across screen
  	 * @param 'jlc': library context.
	 * @param 'str': the text to draw
	 * @param 'yy': y coordinate to draw it at
	 * @param 'color': 255 = opaque, 0 = invisible
	 */
	void jl_gr_draw_ctxt(jl_gr_t* jl_gr, char *str, float yy, uint8_t* color) {
		jl_gr_draw_text(jl_gr, str,
			(jl_vec3_t) { 0., yy, 0. },
			(jl_font_t) { 0, JL_IMGI_ICON, 0, color, 
				1. / ((float)(strlen(str)))} );
	}

	/**
	 * Draw a background on the screen
	**/
	void jl_gr_draw_bg(jl_gr_t* jl_gr, u16_t g, u16_t i, u8_t c) {
		jl_gr_fill_image_set(jl_gr, g, i, c, 127);
		jl_gr_fill_image_draw(jl_gr);
	}

	void jl_gr_draw_loadingbar(jl_gr_t* jl_gr, f64_t loaded) {
		jl_rect_t bar = { .05, jl_gl_ar(jl_gr)*.4,
			.95,jl_gl_ar(jl_gr)*.45};
		u8_t colors[] = { 0, 255, 0, 255};

		jl_gr_vos_rec(jl_gr, NULL, bar, colors, 0);
	}

	//TODO: MOVE
	void jl_gr_draw_msge__(jl_t* jlc) {
		jl_gr_t* jl_gr = jlc->jl_gr;

		jl_gl_pr_scr(jl_gr);
		jl_gr_draw_bg(jl_gr, jl_gr->gr.msge.g, jl_gr->gr.msge.i,
			jl_gr->gr.msge.c);
		if(jl_gr->gr.msge.message)
			jl_gr_draw_ctxt(jl_gr, jl_gr->gr.msge.message, 9./32.,
				jlc->fontcolor);
	}

	/**
	 * Print message on the screen.
   	 * @param 'jlc': library context.
	 * @param 'message': the message 
	 */
	void jl_gr_draw_msge_(jl_gr_t* jl_gr,u16_t g,u16_t i,u8_t c,
		m_str_t message)
	{
		jvct_t* _jlc = jl_gr->jlc->_jlc;
		if(_jlc->has.quickloop) {
			if(jl_sdl_seconds_past__(jl_gr->jlc) <
			 (1.f/(float)JL_FPS))
				return;
			else
				jl_gr->jlc->time.prev_tick =
					jl_gr->jlc->time.this_tick;
		}
		u8_t prev_loop = jl_gr->jlc->loop;
		u8_t prev_flin = _jlc->fl.inloop;

		jl_gr->gr.msge.message = message;
		jl_gr->gr.msge.g = g;
		jl_gr->gr.msge.i = i;
		jl_gr->gr.msge.c = c;
		// Set mode to EXIT.
		_jlc->fl.inloop = 1;
		jl_gr->jlc->loop = JL_SG_WM_EXIT;
		jl_sg_mode_reset(jl_gr->jlc);
		jl_sg_mode_override(jl_gr->jlc, JL_SG_WM_EXIT, jl_gr_draw_msge__);
		// Run Minimal loop.
			// Update events ( minimal )
			jl_ct_quickloop_(jl_gr);
			// Deselect any pre-renderer.
			jl_gr->gl.cp = NULL;
			// Redraw screen.
			_jl_sg_loop(jl_gr);
			// Update Screen.
			jl_dl_loop__(jl_gr);
		// Return to original loop mode.
		jl_gr->jlc->loop = prev_loop;
		_jlc->fl.inloop = prev_flin;
		jl_sg_mode_reset(jl_gr->jlc);
	}

	/**
	 * Print a message on the screen and then terminate the program
 	 * @param 'jlc': library context
 	 * @param 'message': the message 
	 */
	void jl_gr_term_msge(jl_gr_t* jl_gr, char *message) {
		jl_gr_draw_msge(jl_gr, 0, JL_IMGI_ICON, 1, message);
		jl_io_print(jl_gr->jlc, message);
		jl_sg_kill(jl_gr->jlc);
	}

	/**
	 * Create a popup window.
	 */
	void jl_gr_popup(jl_gr_t* jl_gr, char *name, char *message,
		jl_popup_button_t *btns, uint8_t opt_count)
	{
		jl_gr->gr.popup.window_name = name;
		jl_gr->gr.popup.message = message;
		jl_gr->gr.popup.btns = btns;
		jl_sg_mode_override(jl_gr->jlc, JL_SG_WM_EXIT, jl_sg_exit);
		jl_sg_mode_override(jl_gr->jlc, JL_SG_WM_LOOP, _jl_gr_popup_loop);
		jl_sg_mode_override(jl_gr->jlc, JL_SG_WM_RESZ, jl_dont);
	}

	/**
	 * Re-draw/-size a slide button, and activate if it is pressed.
 	 * @param 'txt': the text to draw on the button.
	**/
	void jl_gr_slidebtn_rsz(jl_gr_t* jl_gr, jl_sprite_t * spr, str_t txt) {
		jl_gr_draw_text_sprite(jl_gr, spr, txt);
	}

	/**
	 * Run the Slide Button loop. ( activated when pressed, moved when
	 *  hovered over. ) - And Draw Slide Button.
	 * @param 'jlc': the libary context
 	 * @param 'spr': the Slide Button Sprite.
 	 * @param 'defaultx': the default x position of the button.
 	 * @param 'slidex': how much the x should change when hovered above.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_slidebtn_rnl(jl_gr_t* jl_gr, jl_sprite_t * spr,  float defaultx,
		float slidex, jl_gr_fnct prun)
	{
		spr->data.tr.x = defaultx;
		if(jl_gr_sprite_collide(jl_gr, jl_gr->jlc->mouse, spr)) {
			jl_ct_run_event(jl_gr, JL_CT_PRESS, prun, jl_gr_dont);
			spr->data.tr.x = defaultx + slidex;
		}
		jl_gr_sp_drw(jl_gr, spr);
	}

	/**
	 * Draw a glow button, and activate if it is pressed.
	 * @param 'jlc': the libary context
 	 * @param 'spr': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_glow_button(jl_gr_t* jl_gr, jl_sprite_t * spr,
		char *txt, jl_gr_fnct prun)
	{
		jl_gr_sp_rdr(jl_gr, spr);
		jl_gr_sp_drw(jl_gr, spr);
		if(jl_gr_sprite_collide(jl_gr, jl_gr->jlc->mouse, spr)) {
			jl_rect_t rc = { spr->data.cb.x, spr->data.cb.y,
				spr->data.cb.w, spr->data.cb.h };
			uint8_t glow_color[] = { 255, 255, 255, 64 };

			// Draw glow
			jl_gr_vos_rec(jl_gr,
				jl_gr->gl.temp_vo, rc, glow_color, 0);
			jl_gr_draw_vo(jl_gr, jl_gr->gl.temp_vo, NULL);
			// Description
			jl_gr_draw_text(jl_gr, txt,
				(jl_vec3_t)
					{0., jl_gl_ar(jl_gr) - .0625, 0.},
				(jl_font_t) { 0, JL_IMGI_ICON, 0,
					jl_gr->jlc->fontcolor, .05 });
			// Run if press
			jl_ct_run_event(jl_gr,JL_CT_PRESS,prun,jl_gr_dont);
		}
	}

	/**
	 * Draw A Textbox.
	 * @return 1 if return/enter is pressed.
	 * @return 0 if not.
	*/
	uint8_t jl_gr_draw_textbox(jl_gr_t* jl_gr, float x, float y, float w,
		float h, strt *string)
	{
		uint8_t bytetoinsert = 0;

		if(*string == NULL) *string = jl_me_strt_make(1);
		jl_gr->gr.textbox_string = *string;
		if((bytetoinsert = jl_ct_typing_get(jl_gr))) {
			if(bytetoinsert == '\b') {
				if((*string)->curs == 0) return 0;
				(*string)->curs--;
				jl_me_strt_delete_byte(jl_gr->jlc, *string);
			}else if(bytetoinsert == '\02') {
				jl_me_strt_delete_byte(jl_gr->jlc, *string);
			}else if(bytetoinsert == '\n') {
				return 1;
			}else{
				jl_me_strt_insert_byte(jl_gr->jlc, *string,
					 bytetoinsert);
			}
//			printf("inserting %1s\n", &bytetoinsert);
		}
		jl_ct_run_event(jl_gr,JL_CT_MAINLT,_jl_gr_textbox_lt,
			jl_gr_dont);
		jl_ct_run_event(jl_gr,JL_CT_MAINRT,_jl_gr_textbox_rt,
			jl_gr_dont);
//		jl_gr_draw_image(jlc, 0, 0, x, y, w, h, ' ', 255);
		jl_gr_draw_text(jl_gr, (char*)((*string)->data),
			(jl_vec3_t) {x, y, 0.},
			(jl_font_t) {0,JL_IMGI_ICON,0,jl_gr->jlc->fontcolor,h});
//		jl_gr_draw_image(jlc, 0, 0,
//			x + (h*((float)(*string)->curs-.5)), y, h, h, 252, 255);
		return 0;
	}

	/**
	 * Toggle whether or not to show the menu bar.
	 *
	 * @param jlc: the libary context
	**/
	void jl_gr_togglemenubar(jl_gr_t* jl_gr) {
		if(jl_gr->gr.menuoverlay == jl_gr_sp_dont)
			jl_gr->gr.menuoverlay = _jl_gr_taskbar_loop;
		else
			jl_gr->gr.menuoverlay = _jl_gr_taskbar_dont;
	}

	/**
	 * Add an icon to the menubar
	 *
	 * @param jlc: the libary context
	 * @param fno: the function to run when the icon isn't pressed.
	 * @param fnc: the function to run when the icon is pressed.
	 * @param rdr: the function to run when redraw is called.
	**/
	void jl_gr_addicon(jl_gr_t* jl_gr, jl_gr_fnct fno, jl_gr_fnct fnc,
		jl_gr_fnct rdr)
	{
		jl_taskbar_t* ctx = jl_gr->gr.taskbar->data.ctx;
		m_u8_t i;

		ctx->redraw = 2;
		for(i = 0; i < 10; i++) if(!ctx->func[0][i]) break;
		// Set functions for: draw, press, not press
		ctx->func[JL_GRTP_NOT][i] = fno;
		ctx->func[JL_GRTP_YES][i] = fnc;
		ctx->func[JL_GRTP_RDR][i] = rdr;
	}

	/**
	 * Add the flip screen icon to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_flip(jl_gr_t* jl_gr) {
		jl_gr_addicon(jl_gr, jl_gr_dont, jl_gr_menu_flip_press__,
			jl_gr_menu_flip_draw__);	
	}

	/**
	 * Add slowness detector to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_slow(jl_gr_t* jl_gr) {
		jl_gr_addicon(jl_gr, _jl_gr_menu_slow_loop, jl_gr_dont,
			_jl_gr_menu_slow_draw);
	}

	/**
	 * Add program title to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_name(jl_gr_t* jl_gr) {
		int i;
		for(i = 0; i < 4; i++) {
			jl_gr_addicon(jl_gr, jl_gr_dont, jl_gr_dont,
				jl_gr_menu_name_draw2__);
		}
		jl_gr_addicon(jl_gr, jl_gr_dont, jl_gr_dont,
			jl_gr_menu_name_draw__);
	}

/***      @cond       ***/
/************************/
/***  ETOM Functions  ***/
/************************/

	void _jl_gr_loopb(jl_gr_t* jl_gr) {
		//Message Display
		if(timeTilMessageVanish > 0.f) {
			if(timeTilMessageVanish > 4.25) {
				uint8_t color[] = { 255, 255, 255, 255 };
				jl_gr_draw_ctxt(jl_gr,
					// TODO: Depend on which screen: up/down
					GMessage[0],
					0, color);
			}else{
				uint8_t color[] = { 255, 255, 255, (uint8_t)
					(timeTilMessageVanish * 255.f / 4.25)};
				jl_gr_draw_ctxt(jl_gr,
					// TODO: Depend on which screen: up/down
					GMessage[0],
					0, color);
			}
			timeTilMessageVanish-=jl_gr->jlc->time.psec;
		}
	}

	void _jl_gr_loopa(jl_gr_t* jl_gr) {
		jvct_t* _jlc = jl_gr->jlc->_jlc;
		
		// Menu Bar
		if(!_jlc->fl.inloop) jl_gr->gr.menuoverlay(jl_gr, NULL);
		// Update mouse
		jl_gr_sp_rnl(jl_gr, jl_gr->jlc->mouse);
		// Update messages.
		_jl_gr_loopb(jl_gr);
	}
	
	void jl_gr_init__(jl_gr_t* jl_gr) {
		jl_sprite_t* mouse = NULL;

		_jl_gr_init_vos(jl_gr);
		// Set the menu loop.
		jl_gr_menu_init__(jl_gr);
		jl_gr->gr.menuoverlay = _jl_gr_taskbar_loop;
		// Resize screen
		main_resz(jl_gr->jlc->_jlc, jl_gr->dl.full_w, jl_gr->dl.full_h);
		// Draw Loading Screen
		jl_gr_draw_msge(jl_gr, 0, 0, 0, NULL);
		// Create Font
		jl_gr->jlc->fontcolor[0] = 0;
		jl_gr->jlc->fontcolor[1] = 0;
		jl_gr->jlc->fontcolor[2] = 0;
		jl_gr->jlc->fontcolor[3] = 255;
		jl_gr->jlc->font = (jl_font_t)
			{ 0, JL_IMGI_FONT, 0, jl_gr->jlc->fontcolor, .04 };
		jl_sg_add_some_imgs_(jl_gr, 1);
		//
		jl_gr_draw_msge(jl_gr,0,0,0,"LOADING JL_LIB GRAPHICS...");
		// Load the other images.
		jl_sg_add_some_imgs_(jl_gr, 2);
		jl_gr_draw_msge(jl_gr, 0, 0, 0, "LOADED JL_LIB GRAPHICS!");
		// Create the Mouse
		_jl_gr_mouse_init(jl_gr);
		mouse = jl_gr->jlc->mouse;
		jl_gr_sp_rsz(jl_gr, mouse);
		// Set the mouse's collision width and height to 0
		mouse->data.cb.w = 0.f;
		mouse->data.cb.h = 0.f;
		// Draw message on the screen
		jl_gr_draw_msge(jl_gr, 0, 0, 0, "LOADING JLLIB....");
		jl_io_print(jl_gr->jlc, "started up display %dx%d",
			jl_gr->dl.full_w, jl_gr->dl.full_h);
	}

/**      @endcond      **/
/***   #End of File   ***/

strt jl_vi_make_jpeg(jl_t* jlc,i32_t quality,m_u8_t* pxdata,u16_t w,u16_t h) {
	return jl_vi_make_jpeg_(jlc, quality, pxdata, w, h);
}

m_u8_t* jl_gr_load_image(jl_t* jlc, strt data, m_u16_t* w, m_u16_t* h) {
	return jl_vi_load_(jlc, data, w, h);
}
