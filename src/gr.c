/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * gr.c
 *	A High Level Graphics Library that supports sprites, texture loading,
 *	2D rendering & 3D rendering.
 */
#include "header/jl_pr.h"

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

typedef struct{
	// Used for all icons on the taskbar.
	jl_vo* icon;
	// Not Pressed & Pressed & Redraw Functions for 10 icons.
	jl_simple_fnt func[3][10];
	// Redraw? - 0 = no, 1 = yes
	m_u8_t redraw;
	// Cursor
	m_i8_t cursor;
}jl_taskbar_t;

/*screen being displayed ( on two screens which one on bottom, if keyboard is
	being displayed on bottom screen then this will be displayed on top )*/
float timeTilMessageVanish = 0.f;

char *GMessage[2] = {
	"SWITCH SCREEN: UPPER",
	"SWITCH SCREEN: LOWER"
};

//Upper OpenGL prototypes
void jl_gl_vo_free(jvct_t *_jlc, jl_vo *pv);
void jl_gl_draw_pr(jvct_t *_jlc, jl_vo* pv);
void jl_gl_pr_old(jvct_t *_jlc, jl_vo* pv);
uint8_t jl_gl_pr_isi(jvct_t *_jlc, jl_vo* pv);
void jl_gl_pr_old_(jvct_t *_jlc, jl_pr_t** pr);

//Upper SDL prototype
void _jl_dl_loop(jvct_t* _jlc);

//Graphics Prototypes
static void _jl_gr_textbox_rt(jl_t* jlc);
static void _jl_gr_textbox_lt(jl_t* jlc);
static void _jl_gr_popup_loop(jl_t* jlc);

/***      @cond       ***/
/************************/
/*** Static Functions ***/
/************************/

	static inline void _jl_gr_init_vos(jvct_t *_jlc) {
		_jlc->gr.vos.whole_screen = jl_gl_vo_make(_jlc->jlc, 1);
	}
	
	static inline void _jl_gr_flip_scrn(jvct_t *_jlc) {
		if(_jlc->jlc->loop == JL_SG_WM_UP) {
			_jlc->jlc->loop = JL_SG_WM_DN;
			timeTilMessageVanish = 8.5f;
		}else{
			_jlc->jlc->loop = JL_SG_WM_UP;
			timeTilMessageVanish = 8.5f;
		}
	}
	
	static void jl_gr_sprite_draw_to_pr__(jl_t *jlc) {
		jl_sprite_t *spr = jl_me_tmp_ptr(jlc, 0, NULL);

		spr->draw(jlc, &(spr->data));
	}

	static void _jl_gr_popup_loop(jl_t* jlc) {
//		jl_gr_draw_rect(jlc, .1, .1, .8, .2, 127, 127, 255, 255);
//		jl_gr_draw_rect(jlc, .1, .3, .8, .8, 64, 127, 127, 255);
	}

	static void _jl_gr_textbox_lt(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;
		
		jl_ct_typing_disable(jlc);
		if(_jlc->gr.textbox_string->curs)
			_jlc->gr.textbox_string->curs--;
	}

	static void _jl_gr_textbox_rt(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

		jl_ct_typing_disable(jlc);
		if(_jlc->gr.textbox_string->curs < _jlc->gr.textbox_string->size)
			_jlc->gr.textbox_string->curs++;
	}

	static void jl_gr_draw_icon__(jl_t* jlc, uint16_t g, uint16_t i,
		uint8_t c)
	{
		jvct_t *_jlc = jlc->_jlc;
		jl_rect_t rc_icon = { 0., 0., jl_gl_ar(jlc), jl_gl_ar(jlc)};
		jl_taskbar_t* ctx = _jlc->gr.taskbar->data.ctx;
		jl_vec3_t tr = { 1. - (jl_gl_ar(jlc) * (ctx->cursor+1.)),
			0., 0. };

		jl_gr_vos_image(jlc, &(ctx->icon[1]), rc_icon, g, i, c, 255);
		jl_gr_draw_vo(jlc, &(ctx->icon[1]), &tr);
	}
	
	static void jl_gr_menu_flip_draw__(jl_t* jlc) {
		jl_gr_draw_icon__(jlc, 0, 1, JLGR_ID_FLIP_IMAGE);
	}

	static void jl_gr_menu_flip_press__(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

		if(jlc->ctrl.h != 1) return;
		_jl_gr_flip_scrn(_jlc);
	}
	
	static void jl_gr_menu_name_draw2__(jl_t* jlc) {
		jl_gr_draw_icon__(jlc, 0, 1, JLGR_ID_UNKNOWN);
	}

	static void jl_gr_menu_name_draw__(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;
		jl_taskbar_t* ctx = _jlc->gr.taskbar->data.ctx;
		f32_t text_size = jl_gl_ar(jlc) * .5;

		jl_gr_menu_name_draw2__(jlc);
		jl_gr_draw_text(jlc, _jlc->dl.windowTitle[0],
			1. - (jl_gl_ar(jlc) * (ctx->cursor+1.)),
			0., text_size, 255);
		jl_gr_draw_text(jlc, _jlc->dl.windowTitle[1],
			1. - (jl_gl_ar(jlc) * (ctx->cursor+1.)),
			text_size, text_size, 255);
	}

	static void _jl_gr_menu_slow_draw(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

		// If not on time report the seconds that passed.
		if(!_jlc->sg.on_time)
			sprintf(_jlc->dl.windowTitle[1],"%f",(float)jlc->psec);
		// Draw the icon based on whether on time or not.
		jl_gr_draw_icon__(jlc, 0, 1, _jlc->sg.on_time ?
			JLGR_ID_GOOD_IMAGE : JLGR_ID_SLOW_IMAGE);
	}

	static void _jl_gr_menu_slow_loop(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;
		jl_taskbar_t* ctx = _jlc->gr.taskbar->data.ctx;

		if(_jlc->sg.changed) ctx->redraw = 1;
	}

	// Run when the taskbar is left alone.
	static void _jl_gr_taskbar_loop_pass(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;
		jl_taskbar_t *ctx = _jlc->gr.taskbar->data.ctx;

		for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
			// If A NULL function then, stop looping taskbar.
			if( !(ctx->func[0][ctx->cursor]) ) break;
			// Run the not loop.
			ctx->func[JL_GRTP_NOT][ctx->cursor](jlc);
			// If need to redraw run "rdr"
			if(ctx->redraw) jl_gr_sprite_redraw(jlc, _jlc->gr.taskbar);
		}
	}
	
	static void _jl_gr_taskbar_loop_resz(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;
		jl_taskbar_t *ctx = _jlc->gr.taskbar->data.ctx;

		jl_gr_sprite_resz(jlc, _jlc->gr.taskbar);
		for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
			// If A NULL function then, stop looping taskbar.
			if( !(ctx->func[0][ctx->cursor]) ) break;
			// If need to redraw run "rdr"
			jl_gr_sprite_redraw(jlc, _jlc->gr.taskbar);
		}	
	}

	// Run when the taskbar is clicked/pressed
	static void _jl_gr_taskbar_loop_run(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;
		jl_taskbar_t *ctx;

		//If mouse is'nt over the taskbar - dont run pressed.
		if(_jlc->ct.msy >= .1) {
			_jl_gr_taskbar_loop_pass(jlc);
			return;
		}
		// Set context
		ctx = _jlc->gr.taskbar->data.ctx;
		// Figure out what's selected.
		u8_t selected = (m_u8_t)((1. - _jlc->ct.msx) / .1);
		
		for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
			// If A NULL function then, stop looping taskbar.
			if( !(ctx->func[0][ctx->cursor]) ) break;
			// Run the "not" or "yes" loop.
			ctx->func[(ctx->cursor == selected) ?
				JL_GRTP_YES: JL_GRTP_NOT][ctx->cursor](jlc);
			// If need to redraw run "rdr"
			if(ctx->redraw) jl_gr_sprite_redraw(jlc, _jlc->gr.taskbar);
		}
	}

	// Run whenever a redraw is needed for an icon.
	static void _jl_gr_taskbar_draw(jl_t* jlc, jl_sprd_t* spr) {
		jvct_t *_jlc = jlc->_jlc;
		jl_taskbar_t* ctx = _jlc->gr.taskbar->data.ctx;
		
		// Run the selected icon's redraw function
		if(ctx->cursor >= 0 && ctx->func[JL_GRTP_RDR][ctx->cursor])
			ctx->func[JL_GRTP_RDR][ctx->cursor](jlc);
	}

	// Runs every frame.
	static void _jl_gr_taskbar_loop(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;

		// Draw the pre-rendered taskbar.
		jl_gr_sprite_draw_pr(jlc, _jlc->gr.taskbar);
		// Run the proper loops.
		jl_ct_run_event(jlc, JL_CT_PRESS, _jl_gr_taskbar_loop_run,
			_jl_gr_taskbar_loop_pass);
	}

	static void _jl_gr_menu_init(jvct_t* _jlc) {
		jl_rect_t rc = { 0.f, 0.f, 1.f, .1f };
		jl_rect_t rc_icon = { 0., 0., .1, .1};
		jl_rect_t rc_shadow = {-.01, .01, .1, .1 };
		uint8_t shadow_color[] = { 0, 0, 0, 64 };
		jl_taskbar_t *ctx;
		jl_vo *icon = jl_gl_vo_make(_jlc->jlc, 2);

		// Make the taskbar.
		_jlc->gr.taskbar = jl_gr_sprite_make(
			_jlc->jlc, rc, _jl_gr_taskbar_draw,
			_jl_gr_taskbar_loop, sizeof(jl_taskbar_t));
		// Get the context.
		ctx = _jlc->gr.taskbar->data.ctx;
		// Set the icon & Shadow vertex objects
		ctx->icon = icon;
		// Make the shadow vertex object.
		jl_gr_vos_rec(_jlc->jlc, &icon[0], rc_shadow, shadow_color, 0);
		// Make the icon vertex object.
		jl_gr_vos_image(_jlc->jlc, &icon[1], rc_icon, 0, 1,
			JLGR_ID_UNKNOWN, 255);
		// Clear the taskbar & make pre-renderer.
		for( ctx->cursor = 0; ctx->cursor < 10; ctx->cursor++) {
			m_u8_t i;

			for(i = 0; i < 3; i++) ctx->func[i][ctx->cursor] = NULL;
			jl_gr_sprite_redraw(_jlc->jlc, _jlc->gr.taskbar);
		}
		ctx->cursor = -1;
	}

	static void _jl_gr_mouse_loop(jl_t* jlc) {
		jl_sprite_t* mouse = jlc->mouse;
		jl_vo* mouse_vo = mouse->data.ctx;

	//Update Mouse
		mouse->data.tr.x = jl_ct_gmousex(jlc);
		mouse->data.tr.y = jl_ct_gmousey(jlc);
		mouse->data.cb.x = mouse->data.tr.x;
		mouse->data.cb.y = mouse->data.tr.y;
	// Draw mouse
		#if JL_PLAT == JL_PLAT_COMPUTER //if computer
			jl_gr_draw_vo(jlc, mouse_vo, &(mouse->data.tr));
		#endif
	}

	static inline void _jl_gr_mouse_init(jvct_t *_jlc) {
		jl_rect_t rc = { 0.f, 0.f, .075f, .075f };
		jl_vo *mouse = jl_gl_vo_make(_jlc->jlc, 1);

		#if JL_PLAT == JL_PLAT_COMPUTER //if computer
			jl_gr_vos_image(_jlc->jlc, &(mouse[0]), rc, 0, 0, 254,
				255);
		#endif
		_jlc->jlc->mouse = jl_gr_sprite_make(
			_jlc->jlc, rc, jl_gr_sprdr_dont,
			_jl_gr_mouse_loop,
		#if JL_PLAT == JL_PLAT_COMPUTER //if computer
				sizeof(jl_vo*));
			// Set the context to the vertex object.
			((jl_sprite_t*)_jlc->jlc->mouse)->data.ctx = mouse;
		#elif JL_PLAT == JL_PLAT_PHONE // if phone
				0);
			// Set the context to NULL.
			((jl_sprite_t*)_jlc->jlc->mouse)->data.ctx = NULL;
		#endif
	}

/**      @endcond      **/
/************************/
/*** Global Functions ***/
/************************/

	void jl_gr_sprdr_dont(jl_t* jlc, jl_sprd_t* spr) { }

	/**
	 * Delete a pr image for a vertex object.
	 * @param jlc: The library context.
	 * @param pv: The pr's vertex object.
	**/
	void jl_gr_pr_old(jl_t* jlc, jl_vo* pv) {
		jl_gl_pr_old(jlc->_jlc, pv);
	}

	/**
	 * Create or replace a pr image for a vertex object based on
	 * the bounding box.
	 * @param jlc: The library context.
	 * @param pv: The vertex object to create/replace a pr for.
	 * @param xres: The resolution across the x axis for the pre-renderer.
	**/
	void jl_gr_pr_new(jl_t* jlc, jl_vo* pv, u16_t xres) {
		uint8_t isi = jl_gl_pr_isi(jlc->_jlc, pv);
		if(isi == 2) {
			_jl_fl_errf(jlc->_jlc, "jl_gr_pr_new: VO is NULL\n");
			jl_sg_kill(jlc);
		}
		// If prender is already initialized, resize, otherwise init.
		if(isi) jl_gl_pr_rsz(jlc, pv->pr, pv->cb.w, pv->cb.h, xres);
		else pv->pr = jl_gl_pr_new(jlc, pv->cb.w, pv->cb.h, xres);
		// Check if pre-renderer is initialized.
		if(!pv->pr) {
			_jl_fl_errf(jlc->_jlc,"Prerender Failed Allocation.\n");
			jl_sg_kill(jlc);
		}
		if(!jl_gl_pr_isi(jlc->_jlc, pv)) {
			_jl_fl_errf(jlc->_jlc, "jl_gr_pr_new: didn't make!\n");
			jl_sg_kill(jlc);
		}
	}

	/**
	 * Convert a color.
	 * @param jlc: The library context.
	 * @param rgba: The color to convert ( Not freed - Reusable ).
	 * @param vc: How many vertices to acount for.
	 * @param gradient: 1 if "rgba" is a gradient array, 0 if solid color.
	 * @returns: The converted color
	**/
	jl_ccolor_t* jl_gr_convert_color(jl_t* jlc, uint8_t *rgba, uint32_t vc,
		uint8_t gradient)
	{
		if(gradient) return jl_gl_clrcg(jlc->_jlc, rgba, vc);
		else return jl_gl_clrcs(jlc->_jlc, rgba, vc);
	}

	/**
	 * Change the coloring scheme for a vertex object.
 	 * @param jlc: The library context.
 	 * @param pv: The Vertex Object
 	 * @param cc: The Converted Color Object to use on the Vertex Object.
 	 *	The library takes care of freeing this variable.
	**/
	void jl_gr_vo_color(jl_t* jlc, jl_vo* pv, jl_ccolor_t* cc) {
		jl_gl_clrc(jlc->_jlc, pv, cc);
	}

	/**
	 * Draw a vertex object with offset by translation.
  	 * @param jlc: The library context.
  	 * @param pv: The vertex object to draw.
  	 * @param vec: The vector of offset/translation.
	**/
	void jl_gr_draw_vo(jl_t* jlc, jl_vo* pv, jl_vec3_t* vec) {
		if(vec == NULL) {
			jl_gl_transform_vo_(jlc->_jlc, pv,
				0.f, 0.f, 0.f, 1., 1., 1.);
		}else{
			jl_gl_transform_vo_(jlc->_jlc, pv,
				vec->x, vec->y, vec->z, 1., 1., 1.);
		}
		jl_gl_draw(jlc->_jlc, pv);
	}

	/**
	 * Draw a vertex object's pre-rendered texture.
  	 * @param jlc: The library context.
  	 * @param pv: The vertex object to get the pre-rendered texture from.
  	 * @param vec: The vector of offset/translation.
	**/
	void jl_gr_draw_pr(jl_t* jlc, jl_vo* pv, jl_vec3_t* vec) {
		jvct_t *_jlc = jlc->_jlc;

		if(pv == NULL) pv = _jlc->gl.temp_vo;
		if(vec == NULL)
			jl_gl_transform_vo_(jlc->_jlc, pv,
				0.f, 0.f, 0.f, 1., 1., 1.);
		else
			jl_gl_transform_vo_(jlc->_jlc, pv,
				vec->x, vec->y, vec->z, 1., 1., 1.);
		jl_gl_pr_draw(jlc, pv);
	}

	/**
	 * Set a Vertex object to vector graphics.
	 * 
	**/
	void jl_gr_vos_vec(jl_t* jlc, jl_vo *pv, uint16_t tricount,
		float* triangles, uint8_t* colors, uint8_t multicolor)
	{
		int i;

		// Overwrite the vertex object
		jl_gl_vect(jlc->_jlc, pv, tricount * 3, triangles);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jlc->_jlc, pv, colors);
		else jl_gl_clrs(jlc->_jlc, pv, colors);
		// Set collision box.
		pv->cb.x = 1.f, pv->cb.y = jl_gl_ar(jlc), pv->cb.z = 0.f;
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
	void jl_gr_vos_rec(jl_t* jlc, jl_vo *pv, jl_rect_t rc, u8_t* colors,
		uint8_t multicolor)
	{
		float rectangle_coords[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w + rc.x,	rc.y,		0.f,
			rc.w + rc.x,	rc.h+rc.y,	0.f };

		// Overwrite the vertex object
		jl_gl_poly(jlc->_jlc, pv, 4, rectangle_coords);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jlc->_jlc, pv, colors);
		else jl_gl_clrs(jlc->_jlc, pv, colors);
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
	void jl_gr_vos_image(jl_t* jlc, jl_vo *pv, jl_rect_t rc,
		u16_t g, u16_t i, u8_t c, u8_t a)
	{
		//From bottom left & clockwise
		float Oone[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w+rc.x,	rc.y,		0.f,
			rc.w+rc.x,	rc.h+rc.y,	0.f };
		// Overwrite the vertex object
		jl_gl_poly(jlc->_jlc, pv, 4, Oone);
		// Texture the vertex object
		jl_gl_txtr(jlc->_jlc, pv, c, a, g, i);
		// Set collision box.
		pv->cb.x = rc.x, pv->cb.y = rc.y, pv->cb.z = 0.f;
		pv->cb.w = rc.w, pv->cb.h = rc.h, pv->cb.d = 0.f;
	}

	/**
	 * Free a vertex object.
	 * @param jlc: The library context
	 * @param pv: The vertex object to free
	**/
	void jl_gr_vo_old(jl_t* jlc, jl_vo* pv) {
		jl_gl_vo_free(jlc->_jlc, pv);
	}
	
	void jl_gr_sprite_resz(jl_t* jlc, jl_sprite_t *spr) {
		jvct_t* _jlc = jlc->_jlc;
		u16_t res = (_jlc->gl.cp ? _jlc->gl.cp->w : _jlc->dl.full_w)
			* spr->data.rw;
		u8_t isi = jl_gl_pr_isi_(_jlc, spr->pr);
		
		if(isi) jl_gl_pr_rsz(jlc,spr->pr,spr->data.rw,spr->data.rh,res);
		else spr->pr = jl_gl_pr_new(jlc, spr->data.rw,spr->data.rh,res);
	}

	/**
	 * Run a sprite's draw routine to draw on it's pre-rendered texture.
	 *
	 * @param jlc: The library context
	 * @param spr: Which sprite to draw.
	**/
	void jl_gr_sprite_redraw(jl_t* jlc, jl_sprite_t *spr) {
		jl_me_tmp_ptr(jlc, 0, spr);
		if(!spr->pr) jl_gr_sprite_resz(jlc, spr);
		jl_gl_pr_(jlc->_jlc, spr->pr, jl_gr_sprite_draw_to_pr__);
	}

	/**
	 * Run a sprite's loop.
	 * @param jlc: The library context.
	 * @param spr: Which sprite to loop.
	**/
	void jl_gr_sprite_loop(jl_t* jlc, jl_sprite_t *spr) {
		spr->loop(jlc);
	}

	/**
	 * Render a sprite's pre-rendered texture onto the screen.
	 *
	 * @param jlc: The library context.
	 * @param spr: The sprite.
	**/
	void jl_gr_sprite_draw_pr(jl_t* jlc, jl_sprite_t *spr) {
		jl_pr_t *pr = spr->pr;

		if(!pr) {
			_jl_fl_errf(jlc->_jlc,
				"jl_gr_sprite_draw_pr: not init'd!");
			jl_sg_kill(jlc);
		}
		jl_gl_transform_pr_(jlc->_jlc, pr, spr->data.tr.x,
			spr->data.tr.y, spr->data.tr.z, 1., 1., 1.);

		jl_gl_draw_pr_(jlc, pr);
	}

	/**
	 * Create a sprite
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
	jl_sprite_t * jl_gr_sprite_make(jl_t* jlc, jl_rect_t rc,
		jl_gr_sprdr_fnt draw, jl_simple_fnt loop, u32_t ctxs)
	{
		jl_sprite_t *rtn = NULL;

		rtn = malloc(sizeof(jl_sprite_t));
		// Set draw function.
		rtn->draw = draw;
		// Set translations
		rtn->data.tr.x = rc.x; rtn->data.tr.y = rc.y; rtn->data.tr.z = 0.f;
		// Set collision box.
		rtn->data.cb.x = rc.x; rtn->data.cb.y = rc.y;
		rtn->data.cb.w = rc.w; rtn->data.cb.h = rc.h;
		// Set real dimensions
		rtn->data.rw = rc.w;
		rtn->data.rh = rc.h;
		// Set loop
		rtn->loop = (void*)loop;
		// No pr made yet.
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
	u8_t jl_gr_sprite_collide(jl_t* jlc,
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
	 * Draw text in the window
	 * @param 'jlc': library context
	 * @param 'str': the text to draw
	 * @param 'x': the x position to draw it at
	 * @param 'y': the y position to draw it at
	 * @param 'size': how big to draw the text
	 * @param 'a': transparency of the text, 255=Opaque 0=Transparent
	**/
	void jl_gr_draw_text(jl_t* jlc, char *str, f32_t x, f32_t y, f32_t size,
		uint8_t a)
	{
		jvct_t* _jlc = jlc->_jlc;
		const void *Str = str;
		const uint8_t *STr = Str;
		uint32_t i;
		jl_rect_t rc = { x, y, size, size };
		jl_vec3_t tr = { 0., 0., 0. };
		jl_vo* vo = _jlc->gl.temp_vo;

		if(str == NULL) return;
		for(i = 0; i < strlen(str); i++) {
			//Font 0:0
			jl_gr_vos_image(jlc, vo, rc, 0, 0, STr[i], a);
			jl_gr_draw_vo(jlc, vo, &tr);
			tr.x += size;
		}
	}

	/**
	 * draw a number on the screen
 	 * @param 'jlc': library context
	 * @param 'num': the number to draw
	 * @param 'x': the x position to draw it at
	 * @param 'y': the y position to draw it at
	 * @param 'size': how big to draw the text
	 * @param 'a': transparency of the text, 255=Opaque 0=Transparent
	 */
	void jl_gr_draw_numi(jl_t* jlc, uint32_t num, float x, float y, float size,
		uint8_t a)
	{
		char display[10];
		sprintf(display, "%d", num);
		jl_gr_draw_text(jlc, display, x, y, size, a);
	}

	/**
	 * Draw text within the boundary of a sprite
	 * @param 'jlc': library context
	 * @param 'spr': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_area(jl_t* jlc, jl_sprite_t * spr, char *txt){
		float fontsize = (spr->data.cb.w - .1) / strlen(txt);
		jl_gr_draw_text(jlc, txt,
			spr->data.cb.x + .05,
			spr->data.cb.y + (.5 * (spr->data.cb.h - fontsize)),
			fontsize, 255);
	}

	/**
	 * Draw a sprite, then draw text within the boundary of a sprite
 	 * @param 'jlc': library context
	 * @param 'spr': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_sprite(jl_t* jlc,jl_sprite_t * spr,char *txt){
		jl_gr_sprite_redraw(jlc, spr);
		jl_gr_draw_text_area(jlc, spr, txt);
	}

	/**
	 * Draw centered text across screen
  	 * @param 'jlc': library context.
	 * @param 'str': the text to draw
	 * @param 'p_y': y coordinate to draw it at
	 * @param 'p_a': 255 = opaque, 0 = invisible
	 */
	void jl_gr_draw_ctxt(jl_t* jlc, char *str, float p_y, uint8_t p_a) {
		jl_gr_draw_text(jlc, str, 0, p_y,
			1. / ((float)(strlen(str))), p_a);
	}

	/**
	 * Print message on the screen.
   	 * @param 'jlc': library context.
	 * @param 'message': the message 
	 */
	void jl_gr_draw_msge(jl_t* jlc, char * message) {
		jvct_t *_jlc = jlc->_jlc;
		//TODO: Make it so this variable is unneeded to change graphics.
		jl_io_printc(_jlc->jlc, "MKIng RC....\n");
		jl_rect_t rc = { 0., 0., 1., jl_gl_ar(jlc) };
		jl_io_printc(_jlc->jlc, "MKIng VO....\n");
		printf("draw msge\n");
		jl_gr_draw_vo(jlc, _jlc->gr.vos.whole_screen, NULL);
		jl_io_printc(_jlc->jlc, "DRing VO....\n");
		jl_gr_vos_image(jlc, _jlc->gr.vos.whole_screen, rc,
			0, 1, 1, 127);
		jl_io_printc(_jlc->jlc, "DRing CTX....\n");
		jl_gr_draw_ctxt(jlc, message, (9./16.)/2, 255);
		jl_io_printc(_jlc->jlc, "DRing LOOp....\n");
		_jl_dl_loop(jlc->_jlc); //Update Screen
	}

	/**
	 * Print a message on the screen and then terminate the program
 	 * @param 'jlc': library context
 	 * @param 'message': the message 
	 */
	void jl_gr_term_msge(jl_t* jlc, char *message) {
		jl_gr_draw_msge(jlc, message);
		_jl_fl_errf(jlc->_jlc, message);
		jl_sg_kill(jlc);
	}

	/**
	 * Create a popup window.
	 */
	void jl_gr_popup(jl_t* jlc, char *name, char *message,
		jl_popup_button_t *btns, uint8_t opt_count)
	{
		jvct_t *_jlc = jlc->_jlc;
		_jlc->gr.popup.window_name = name;
		_jlc->gr.popup.message = message;
		_jlc->gr.popup.btns = btns;
		jl_sg_mode_override(jlc, JL_SG_WM_EXIT, jl_sg_exit);
		jl_sg_mode_override(jlc, JL_SG_WM_UP, _jl_gr_popup_loop);
		jl_sg_mode_override(jlc, JL_SG_WM_DN, _jl_gr_popup_loop);
		jl_sg_mode_override(jlc, JL_SG_WM_TERM, jl_dont);
		jl_sg_mode_override(jlc, JL_SG_WM_RESZ, jl_dont);
	}

	/**
	 * Draw a slide button, and activate if it is pressed.
	 * @param 'jlc': the libary context
 	 * @param 'spr': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'defaultx': the default x position of the button.
 	 * @param 'slidex': how much the x should change when hovered above.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_slide_button(
		jl_t* jlc, jl_sprite_t * spr, char *txt, float defaultx,
		float slidex, jl_simple_fnt prun)
	{
		jl_gr_draw_text_sprite(jlc, spr, txt);
		spr->data.tr.x = defaultx;
		if(jl_gr_sprite_collide(jlc, jlc->mouse, spr)) {
			jl_ct_run_event(jlc, JL_CT_PRESS, prun, jl_dont);
			spr->data.tr.x = defaultx + slidex;
		}
	}

	/**
	 * Draw a glow button, and activate if it is pressed.
	 * @param 'jlc': the libary context
 	 * @param 'spr': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_glow_button(jl_t* jlc, jl_sprite_t * spr,
		char *txt, jl_simple_fnt prun)
	{
		jl_gr_sprite_redraw(jlc, spr);
		if(jl_gr_sprite_collide(jlc, jlc->mouse, spr)) {
			jl_ct_run_event(jlc, JL_CT_PRESS, prun, jl_dont);
//			jl_gr_draw_rect(jlc, spr->data.cb.x, spr->data.cb.y,
//				spr->data.cb.w, spr->data.cb.h,
//				1., 1., 1., 64);
			jl_gr_draw_text(jlc, txt, 0, jl_gl_ar(jlc) - .0625,
				.05, 255);
		}
	}

	/**
	 * Draw A Textbox.
	 * @return 1 if return/enter is pressed.
	 * @return 0 if not.
	*/
	uint8_t jl_gr_draw_textbox(jl_t*jlc, float x, float y, float w,
		float h, strt *string)
	{
		jvct_t *_jlc = jlc->_jlc;
		uint8_t bytetoinsert = 0;

		if(*string == NULL) *string = jl_me_strt_make(1);
		_jlc->gr.textbox_string = *string;
		if((bytetoinsert = jl_ct_typing_get(jlc))) {
			if(bytetoinsert == '\b') {
				if((*string)->curs == 0) return 0;
				(*string)->curs--;
				jl_me_strt_delete_byte(jlc, *string);
			}else if(bytetoinsert == '\02') {
				jl_me_strt_delete_byte(jlc, *string);
			}else if(bytetoinsert == '\n') {
				return 1;
			}else{
				jl_me_strt_insert_byte(jlc, *string, bytetoinsert);
			}
//			printf("inserting %1s\n", &bytetoinsert);
		}
		jl_ct_run_event(jlc,JL_CT_MAINLT,_jl_gr_textbox_lt,jl_dont);
		jl_ct_run_event(jlc,JL_CT_MAINRT,_jl_gr_textbox_rt,jl_dont);
//		jl_gr_draw_image(jlc, 0, 0, x, y, w, h, ' ', 255);
		jl_gr_draw_text(jlc, (char*)((*string)->data), x, y, h, 255);
//		jl_gr_draw_image(jlc, 0, 0,
//			x + (h*((float)(*string)->curs-.5)), y, h, h, 252, 255);
		return 0;
	}

	/**
	 * Toggle whether or not to show the menu bar.
	 *
	 * @param jlc: the libary context
	**/
	void jl_gr_togglemenubar(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

		if(_jlc->gr.menuoverlay == jl_dont)
			_jlc->gr.menuoverlay = _jl_gr_taskbar_loop;
		else
			_jlc->gr.menuoverlay = jl_dont;
	}

	/**
	 * Add an icon to the menubar
	 *
	 * @param jlc: the libary context
	 * @param fno: the function to run when the icon isn't pressed.
	 * @param fnc: the function to run when the icon is pressed.
	 * @param rdr: the function to run when redraw is called.
	**/
	void jl_gr_addicon(jl_t* jlc, jl_simple_fnt fno, jl_simple_fnt fnc,
		jl_simple_fnt rdr)
	{
		jvct_t *_jlc = jlc->_jlc;
		jl_taskbar_t* ctx = _jlc->gr.taskbar->data.ctx;
		uint8_t i;

		for(i = 0; i < 10; i++) { if(!ctx->func[0][i]) break; }
		ctx->func[JL_GRTP_NOT][i] = fno;
		ctx->func[JL_GRTP_YES][i] = fnc;
		ctx->func[JL_GRTP_RDR][i] = rdr;
	}

	/**
	 * Add the flip screen icon to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_flip(jl_t* jlc) {
		jl_gr_addicon(jlc, jl_dont, jl_gr_menu_flip_press__,
			jl_gr_menu_flip_draw__);	
	}

	/**
	 * Add slowness detector to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_slow(jl_t* jlc) {
		jl_gr_addicon(jlc, _jl_gr_menu_slow_loop, jl_dont,
			_jl_gr_menu_slow_draw);
	}

	/**
	 * Add program title to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_name(jl_t* jlc) {
		int i;
		for(i = 0; i < 4; i++) {
			jl_gr_addicon(jlc, jl_dont, jl_dont,
				jl_gr_menu_name_draw2__);
		}
		jl_gr_addicon(jlc, jl_dont, jl_dont,
			jl_gr_menu_name_draw__);
	}

/***      @cond       ***/
/************************/
/***  ETOM Functions  ***/
/************************/

	void _jl_gr_resz(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;
		jl_rect_t rc = { 0., 0., 1., jl_gl_ar(jlc)};

		jl_gr_vos_image(jlc, _jlc->gr.vos.whole_screen, rc, 0, 0, 0, 255);
	//Menu Bar
		_jl_gr_taskbar_loop_resz(jlc);
	//Mouse
		jl_gr_sprite_resz(jlc, jlc->mouse);
	}

	void _jl_gr_loop(jl_t* jlc) {
		jvct_t* _jlc = jlc->_jlc;
		jl_sprite_t* mouse = jlc->mouse;
		
	//Menu Bar
		if(!_jlc->fl.inloop) _jlc->gr.menuoverlay(jlc);
	//Message Display
		if(timeTilMessageVanish > 0.f) {
			if(timeTilMessageVanish > 4.25)
				jl_gr_draw_ctxt(jlc,
					GMessage[jlc->loop == JL_SG_WM_DN],
					0, 255);
			else
				jl_gr_draw_ctxt(jlc,
					GMessage[jlc->loop == JL_SG_WM_DN],
					0, (uint8_t)
					(timeTilMessageVanish * 255.f / 4.25));	
			timeTilMessageVanish-=jlc->psec;
		}
	//Update mouse
		jl_gr_sprite_loop(jlc, mouse);
	}
	
	void jl_gr_inita_(jvct_t *_jlc) {
		jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "GRIN");
		_jl_gr_init_vos(_jlc);
		jl_io_close_block(_jlc->jlc); //Close Block "GRIN"
	}

	void jl_gr_initb_(jvct_t *_jlc) {
		jl_sprite_t* mouse = NULL;

		jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "GRIN");
		// Create the Mouse
		_jl_gr_mouse_init(_jlc);
		mouse = _jlc->jlc->mouse;
		// Set the mouse's collision width and height to 0
		mouse->data.cb.w = 0.f;
		mouse->data.cb.h = 0.f;
		// Set the menu loop.
		_jl_gr_menu_init(_jlc);
		_jlc->gr.menuoverlay = _jl_gr_taskbar_loop;
		jl_io_close_block(_jlc->jlc); //Close Block "GRIN"
	}

/**      @endcond      **/
/***   #End of File   ***/
