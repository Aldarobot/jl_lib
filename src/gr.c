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

/*screen being displayed ( on two screens which one on bottom, if keyboard is
	being displayed on bottom screen then this will be displayed on top )*/
float timeTilMessageVanish = 0.f;

char *GMessage[2] = {
	"SWITCH SCREEN: UPPER",
	"SWITCH SCREEN: LOWER"
};

/*_jal5_jl_gr_t* g_grph;*/

//Upper OpenGL prototypes
jl_vo *jl_gl_vo_make(jvct_t *pjlc, float *xyzw, uint8_t vertices);
void jl_gl_vo_free(jvct_t *pjlc, jl_vo *pv);

//Upper SDL prototype
void _jl_dl_loop(jvct_t* pjlc);

//Graphics Prototypes
static void _jl_gr_menubar(jl_t* pusr);
static void _jl_gr_flipscreen(jl_t* pusr);
static void _jl_gr_menubar_slow(jl_t* pusr);
static void _jl_gr_menubar_name(jl_t* pusr);
static void _jl_gr_draw_icon(jl_t* pusr);
static void _jl_gr_textbox_rt(jl_t* pusr);
static void _jl_gr_textbox_lt(jl_t* pusr);
static void _jl_gr_popup_loop(jl_t* pusr);

/*EXPORTED FUNCTIONS*/

	/**
	 * Set the clipping pane for the screen.
	 * @param pusr: The library context.
  	 * @param xmin: anything drawn left of this line is disregarded.
  	 * @param xmax: anything drawn right of this line is disregarded.
	 * @param ymin: if something is drawn below this line it is disregarded.
 	 * @param ymax: if something is drawn above this line it is disregarded.
	**/
	void jl_gr_set_clippane(jl_t* pusr, float xmin, float xmax,
		float ymin, float ymax)
	{
		jl_gl_set_clippane(pusr->pjlc, xmin, xmax, ymin, ymax);
	}
	
	/**
	 * Reset the clipping pane for the screen.
 	 * @param pusr: The library context.
	**/
	void jl_gr_default_clippane(jl_t* pusr) {
		jl_gl_default_clippane(pusr->pjlc);
	}

	/**
	 * Convert a color.
	 * @param pusr: The library context.
	 * @param rgba: The color to convert ( Not freed - Reusable ).
	 * @param vc: How many vertices to acount for.
	 * @param gradient: 1 if "rgba" is a gradient array, 0 if solid color.
	 * @returns: The converted color
	**/
	jl_ccolor_t* jl_gr_convert_color(jl_t* pusr, uint8_t *rgba, uint32_t vc,
		uint8_t gradient)
	{
		if(gradient) return jl_gl_clrcg(pusr->pjlc, rgba, vc);
		else return jl_gl_clrcs(pusr->pjlc, rgba, vc);
	}
	
	/**
	 * Change the coloring scheme for a vertex object.
 	 * @param pusr: The library context.
 	 * @param pv: The Vertex Object
 	 * @param cc: The Converted Color Object to use on the Vertex Object.
 	 *	The library takes care of freeing this variable.
	**/
	void jl_gr_vo_color(jl_t* pusr, jl_vo* pv, jl_ccolor_t* cc) {
		jl_gl_clrc(pusr->pjlc, pv, cc);
	}

	/**
	 * Draw a vertex object.
  	 * @param pusr: The library context.
  	 * @param pv: the vertex object to draw.
	**/
	void jl_gr_draw_vo(jl_t* pusr, jl_vo* pv) {
		jl_gl_translate(pusr->pjlc, pv, 0.f, 0.f, 0.f);
		jl_gl_draw(pusr->pjlc, pv);
	}
	
	/**
	 * Draw a vertex object with offset by translation.
	 *
	**/
	void jl_gr_draw_tvo(jl_t* pusr, jl_vo* pv, jl_vec3_t* vec) {
		jl_gl_translate(pusr->pjlc, pv, vec->x, vec->y, vec->z);
		jl_gl_draw(pusr->pjlc, pv);
	}

	/**
	 * Draw Vector Graphics.
	**/
	void jl_gr_draw_vect(jl_t* pusr, uint16_t tricount, float* triangles,
		uint8_t* colors, uint8_t multicolor)
	{
		jl_gl_vect(pusr->pjlc, NULL, tricount * 3, triangles);
		if(multicolor) jl_gl_clrg(pusr->pjlc, NULL, colors);
		else jl_gl_clrs(pusr->pjlc, NULL, colors);
		jl_gl_translate(pusr->pjlc, NULL, 0.f, 0.f, 0.f);
		jl_gl_draw(pusr->pjlc, NULL);
	}
	
	/**
	 * Get a a Vertex Object From vector graphics.
	**/
	jl_vo* jl_gr_vof_vec(jl_t* pusr, uint16_t tricount, float* triangles,
		uint8_t* colors, uint8_t multicolor)
	{
		jl_vo *rtn = jl_gl_vo_make(pusr->pjlc, NULL, 0);
		jl_gl_vect(pusr->pjlc, rtn, tricount * 3, triangles);
		if(multicolor) jl_gl_clrg(pusr->pjlc, rtn, colors);
		else jl_gl_clrs(pusr->pjlc, rtn, colors);
		return rtn;
	}

	/**
	 * Draw a solid rectangle.
	 *
	 * @param 'pusr': library context
 	 * @param 'x': the x location of the sprite
 	 * @param 'y': the y location of the sprite
 	 * @param 'w': how wide to draw the sprite
 	 * @param 'h': how tall to draw the sprite.
	 * @param 'r': the red value of the color of the rectangle.
 	 * @param 'g': the green value of the color of the rectangle.
 	 * @param 'b': the blue value of the color of the rectangle.
	 * @param 'a': the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
	**/
	void jl_gr_draw_rect(jl_t* pusr, float x, float y, float w, float h,
		uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		float Oone[] = {
			x,	h+y,	0.f,
			x,	y,	0.f,
			w+x,	y,	0.f,
			w+x,	h+y,	0.f };
		uint8_t Otwo[] = { r, g, b, a };
		jl_gl_clrs(pusr->pjlc, NULL, Otwo);
		jl_gl_poly(pusr->pjlc, NULL, 4, Oone);
		jl_gl_translate(pusr->pjlc, NULL, 0.f, 0.f, 0.f);
		jl_gl_draw(pusr->pjlc, NULL);
	}
	/**
	 * Draw An Image.
	 *
	 * @param 'pusr': library context
  	 * @param 'g': the image group that the image pointed to by 'i' is in.
	 * @param 'i':  the ID of the image.
 	 * @param 'x': the x location of the sprite
 	 * @param 'y': the y location of the sprite
 	 * @param 'w': how wide to draw the sprite
 	 * @param 'h': how tall to draw the sprite.
	 * @param 'c': is 0 unless you want to use the image as
	 * 	a charecter map, then it will zoom into charecter 'chr'.
	 * @param 'a': the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
	**/
	void jl_gr_draw_image(jl_t* pusr, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t c, u08t a)
	{
		//From bottom left & clockwise
		float Oone[] = {
			x,	h+y,	0.f,
			x,	y,	0.f,
			w+x,	y,	0.f,
			w+x,	h+y,	0.f };
		jl_gl_txtr(pusr->pjlc, NULL, c, a, g, i);
		jl_gl_poly(pusr->pjlc, NULL, 4, Oone);
		jl_gl_translate(pusr->pjlc, NULL, 0.f, 0.f, 0.f);
		jl_gl_draw(pusr->pjlc, NULL);
	}
	
	/**
	 * 
	**/
	void jl_gr_vof_image(jl_t* pusr, jl_vo* pv, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t c, u08t a)
	{
		
	}

	/**
	 * Create an empty vertex object & return it.
	 * @param pusr: The library context
	 * @returns: A new vertex object with 0 vertices.
	**/
	jl_vo* jl_gr_vo_new(jl_t* pusr) {
		return jl_gl_vo_make(pusr->pjlc, NULL, 0);
	}
	
	/**
	 * Free a vertex object.
	 * @param pusr: The library context
	 * @param pv: The vertex object to free
	**/
	void jl_gr_vo_old(jl_t* pusr, jl_vo* pv) {
		jl_gl_vo_free(pusr->pjlc, pv);
	}

	/**
	 * Draw the sprite.
	 *
	 * @param pusr: The library context
	 * @param psprite: Which sprite to draw.
	**/
	void jl_gr_sprite_draw(jl_t* pusr, jl_sprite_t *psprite) {
		jl_gr_draw_image(pusr, psprite->g, psprite->i,
			psprite->r.x, psprite->r.y, psprite->r.w, psprite->r.h,
			psprite->c, psprite->a);
	}
	
	/**
	 * Create a sprite
	 *
	 * @param 'pusr': library context
 	 * @param 'g': the image group that the image pointed to by 'i' is in.
 	 * @param 'i': the ID of the image.
 	 * @param 'c': set to 0 unless you want to use the image as a character
 	 * 	map, then it will zoom into charecter 'chr'.
 	 * @param 'a' the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
 	 * @param 'x': the x location of the sprite
 	 * @param 'y': the y location of the sprite
 	 * @param 'w': how wide to draw the sprite
 	 * @param 'h': how tall to draw the sprite.
 	 * @param 'loop': the loop function.
 	 * @param 'ctxs': how many bytes to allocate for the sprite's context.
 	 * @return x: the new sprite
	**/
	jl_sprite_t * jl_gr_sprite_make(
		jl_t* pusr, u16t g, u16t i, u08t c, u08t a,
		float x, float y, float w, float h,
		jl_simple_fnt loop, u32t ctxs)
	{
		jl_sprite_t *rtn;
		rtn = malloc(sizeof(jl_sprite_t));
		rtn->g = g;
		rtn->i = i;
		rtn->c = c;
		rtn->a = a;
		rtn->r.x = x; rtn->cb.x = x;
		rtn->r.y = y; rtn->cb.y = y;
		rtn->r.w = w; rtn->cb.w = w;
		rtn->r.h = h; rtn->cb.h = h;
		rtn->loop = (void*)loop;
		if(ctxs) rtn->ctx = malloc(ctxs);
		return rtn;
	}
	
	/**
	 * test if 2 sprites collide.
	 *
	 * @param 'pusr': library context
	 * @param 'sprite1': sprite 1
	 * @param 'sprite2': sprite 2
	 * @return 0: if the sprites don't collide in their bounding boxes.
	 * @return 1: if the sprites do collide in their bounding boxes.
	**/
	u08t jl_gr_sprite_collide(jl_t* pusr,
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

	/**
	 * Draw text in the window
	 * @param 'pusr': library context
	 * @param 'str': the text to draw
	 * @param 'x': the x position to draw it at
	 * @param 'y': the y position to draw it at
	 * @param 'size': how big to draw the text
	 * @param 'a': transparency of the text, 255=Opaque 0=Transparent
	**/
	void jl_gr_draw_text(jl_t* pusr, char *str, float x, float y,
		float size, uint8_t a)
	{
		if(str == NULL) return;
		const void *Str = str;
		const uint8_t *STr = Str;
		uint32_t i;
		for(i = 0; i < strlen(str); i++) {
			jl_gr_draw_image(pusr, 0, 0, //Font 0:0
				x, y, size, size,
				STr[i], a);
			x = x + size;
		}
	}

	/**
	 * draw a number on the screen
 	 * @param 'pusr': library context
	 * @param 'num': the number to draw
	 * @param 'x': the x position to draw it at
	 * @param 'y': the y position to draw it at
	 * @param 'size': how big to draw the text
	 * @param 'a': transparency of the text, 255=Opaque 0=Transparent
	 */
	void jl_gr_draw_numi(jl_t* pusr, uint32_t num, float x, float y, float size,
		uint8_t a)
	{
		char display[10];
		sprintf(display, "%d", num);
		jl_gr_draw_text(pusr, display, x, y, size, a);
	}

	/**
	 * Draw text within the boundary of a sprite
	 * @param 'pusr': library context
	 * @param 'psprite': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_area(jl_t* pusr, jl_sprite_t * psprite, char *txt){
		float fontsize = (psprite->r.w - .1) / strlen(txt);
		jl_gr_draw_text(pusr, txt,
			psprite->r.x + .05,
			psprite->r.y + (.5 * (psprite->r.h - fontsize)),
			fontsize, 255);
	}

	/**
	 * Draw a sprite, then draw text within the boundary of a sprite
 	 * @param 'pusr': library context
	 * @param 'psprite': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_sprite(jl_t* pusr,jl_sprite_t * psprite,char *txt){
		jl_gr_sprite_draw(pusr, psprite);
		jl_gr_draw_text_area(pusr, psprite, txt);
	}

	/**
	 * Draw centered text across screen
  	 * @param 'pusr': library context.
	 * @param 'str': the text to draw
	 * @param 'p_y': y coordinate to draw it at
	 * @param 'p_a': 255 = opaque, 0 = invisible
	 */
	void jl_gr_draw_ctxt(jl_t* pusr, char *str, float p_y, uint8_t p_a) {
		jl_gr_draw_text(pusr, str, 0, p_y,
			1. / ((float)(strlen(str))), p_a);
	}

	/**
	 * Print message on the screen.
   	 * @param 'pusr': library context.
	 * @param 'message': the message 
	 */
	void jl_gr_draw_msge(jl_t* pusr, char * message) {
		jl_gr_draw_image(pusr, 0, 1, 0., 0., 1., jl_dl_p(pusr), 1, 127);
		jl_gr_draw_ctxt(pusr, message, (9./16.)/2, 255);
		_jl_dl_loop(pusr->pjlc); //Update Screen
	}

	/**
	 * Print a message on the screen and then terminate the program
 	 * @param 'pusr': library context
 	 * @param 'message': the message 
	 */
	void jl_gr_term_msge(jl_t* pusr, char *message) {
		jl_gr_draw_msge(pusr, message);
		jl_sg_kill(pusr, message);
	}
	
	/**
	 * Create a popup window.
	 */
	void jl_gr_popup(jl_t* pusr, char *name, char *message,
		jl_popup_button_t *btns, uint8_t opt_count)
	{
		jvct_t *pjlc = pusr->pjlc;
		pjlc->gr.popup.window_name = name;
		pjlc->gr.popup.message = message;
		pjlc->gr.popup.btns = btns;
		jl_sg_mode_override(pusr, JL_SG_WM_EXIT, jl_sg_exit);
		jl_sg_mode_override(pusr, JL_SG_WM_UP, _jl_gr_popup_loop);
		jl_sg_mode_override(pusr, JL_SG_WM_DN, _jl_gr_popup_loop);
		jl_sg_mode_override(pusr, JL_SG_WM_TERM, jl_dont);
		jl_sg_mode_override(pusr, JL_SG_WM_RESZ, jl_dont);
	}
	
	/**
	 * Draw a slide button, and activate if it is pressed.
	 * @param 'pusr': the libary context
 	 * @param 'psprite': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'defaultx': the default x position of the button.
 	 * @param 'slidex': how much the x should change when hovered above.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_slide_button(
		jl_t* pusr, jl_sprite_t * psprite, char *txt, float defaultx,
		float slidex, jl_simple_fnt prun)
	{
		jl_gr_draw_text_sprite(pusr, psprite, txt);
		psprite->r.x = defaultx;
		if(jl_gr_sprite_collide(pusr, pusr->mouse, psprite)) {
			jl_ct_run_event(pusr, JL_CT_PRESS, prun, jl_dont);
			psprite->r.x = defaultx + slidex;
		}
	}
	
	/**
	 * Draw a glow button, and activate if it is pressed.
	 * @param 'pusr': the libary context
 	 * @param 'psprite': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_glow_button(jl_t* pusr, jl_sprite_t * psprite,
		char *txt, jl_simple_fnt prun)
	{
		jl_gr_sprite_draw(pusr, psprite);
		if(jl_gr_sprite_collide(pusr, pusr->mouse, psprite)) {
			jl_ct_run_event(pusr, JL_CT_PRESS, prun, jl_dont);
			jl_gr_draw_rect(pusr, psprite->r.x, psprite->r.y,
				psprite->r.w, psprite->r.h,
				1., 1., 1., 64); 
			jl_gr_draw_text(pusr, txt, 0, jl_dl_p(pusr) - .0625,
				.05, 255);
		}
	}
	
	/**
	 * Draw A Textbox.
	 * @return 1 if return/enter is pressed.
	 * @return 0 if not.
	*/
	uint8_t jl_gr_draw_textbox(jl_t*pusr, float x, float y, float w,
		float h, strt *string)
	{
		jvct_t *pjlc = pusr->pjlc;
		uint8_t bytetoinsert = 0;

		if(*string == NULL) {
			*string = jl_me_strt_make(1, STRT_KEEP);
		}
		pjlc->gr.textbox_string = *string;
		if((bytetoinsert = jl_ct_typing_get(pusr))) {
			if(bytetoinsert == '\b') {
				if((*string)->curs == 0) return 0;
				(*string)->curs--;
				jl_me_strt_delete_byte(pusr, *string);
			}else if(bytetoinsert == '\02') {
				jl_me_strt_delete_byte(pusr, *string);
			}else if(bytetoinsert == '\n') {
				return 1;
			}else{
				jl_me_strt_insert_byte(pusr, *string, bytetoinsert);
			}
//			printf("inserting %1s\n", &bytetoinsert);
		}
		jl_ct_run_event(pusr,JL_CT_MAINLT,_jl_gr_textbox_lt,jl_dont);
		jl_ct_run_event(pusr,JL_CT_MAINRT,_jl_gr_textbox_rt,jl_dont);
		jl_gr_draw_image(pusr, 0, 0, x, y, w, h, ' ', 255);
		jl_gr_draw_text(pusr, (char*)((*string)->data), x, y, h, 255);
		jl_gr_draw_image(pusr, 0, 0,
			x + (h*((float)(*string)->curs-.5)), y, h, h, 252, 255);
		return 0;
	}
	
	/**
	 * Toggle whether or not to show the menu bar.
	 *
	 * @param pusr: the libary context
	**/
	void jl_gr_togglemenubar(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;

		if(pjlc->gr.menuoverlay == jl_dont)
			pjlc->gr.menuoverlay = _jl_gr_menubar;
		else
			pjlc->gr.menuoverlay = jl_dont;
	}
	
	/**
	 * Add an icon to the menubar
	 *
	 * @param pusr: the libary context
	 * @param grp: the image group of the image to display
	 * @param iid: the id of the image in the image group
	 * @param chr: the character of the image to display.  0 means stop.
	 * @param fno: the function to run when the icon isn't pressed.
	 * @param fnc: the function to run when the icon is pressed.
	**/
	void jl_gr_addicon(jl_t* pusr, uint16_t grp, uint8_t iid,
		uint8_t chr, jl_simple_fnt fno, jl_simple_fnt fnc)
	{
		jvct_t *pjlc = pusr->pjlc;
		uint8_t i;

		for(i = 0; i < 10; i++) {
			if(!pjlc->gr.menubar.chr[i]) break;
		}
		pjlc->gr.menubar.grp[i] = grp;
		pjlc->gr.menubar.iid[i] = iid;
		pjlc->gr.menubar.chr[i] = chr;
		pjlc->gr.menubar.func[0][i] = fno;
		pjlc->gr.menubar.func[1][i] = fnc;
	}

	/**
	 * Add the flip screen icon to the menubar.
	 * @param pusr: the libary context
	**/
	void jl_gr_addicon_flip(jl_t* pusr) {
		jl_gr_addicon(pusr, 0, 1, JLGR_ID_FLIP_IMAGE,
			_jl_gr_draw_icon, _jl_gr_flipscreen);	
	}
	
	/**
	 * Add slowness detector to the menubar.
	 * @param pusr: the libary context
	**/
	void jl_gr_addicon_slow(jl_t* pusr) {
		jl_gr_addicon(pusr, 0, 1, JLGR_ID_GOOD_IMAGE,
			_jl_gr_menubar_slow, _jl_gr_draw_icon);
	}
	
	/**
	 * Add program title to the menubar.
	 * @param pusr: the libary context
	**/
	void jl_gr_addicon_name(jl_t* pusr) {
		int i;
		for(i = 0; i < 4; i++) {
			jl_gr_addicon(pusr, 0, 1, JLGR_ID_UNKNOWN,
				_jl_gr_draw_icon, _jl_gr_draw_icon);
		}
		jl_gr_addicon(pusr, 0, 1, JLGR_ID_UNKNOWN,
			_jl_gr_menubar_name, _jl_gr_draw_icon);
	}

/** @cond **/
/*BACKGROUND FUNCTIONS*/

	static void _jl_gr_popup_loop(jl_t* pusr) {
		jl_gr_draw_rect(pusr, .1, .1, .8, .2, 127, 127, 255, 255);
		jl_gr_draw_rect(pusr, .1, .3, .8, .8, 64, 127, 127, 255);
	}

	static void _jl_gr_textbox_lt(jl_t* pusr) {
//		if((int)y != 1) return;
		jvct_t *pjlc = pusr->pjlc;
		
		jl_ct_typing_disable(pusr);
		if(pjlc->gr.textbox_string->curs)
			pjlc->gr.textbox_string->curs--;
	}
	
	static void _jl_gr_textbox_rt(jl_t* pusr) {
//		if((int)y != 1) return;
		jvct_t *pjlc = pusr->pjlc;

		jl_ct_typing_disable(pusr);
		if(pjlc->gr.textbox_string->curs < pjlc->gr.textbox_string->size)
			pjlc->gr.textbox_string->curs++;
	}

	static void _jl_gr_draw_icon(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;

		jl_gr_draw_rect(pusr, pjlc->gr.menubar.iconx - .01,
			.01,
			.1, .1, 0., 0., 0., 64);
		jl_gr_draw_image(pusr, 0, 1, pjlc->gr.menubar.iconx,
			0.,
			.1, .1,
			pjlc->gr.menubar.chr[pjlc->gr.menubar.cursor],
			255);
	}

	void _jl_gr_flip_scrn(jvct_t *pjlc) {
		if(pjlc->sg.usrd->loop == JL_SG_WM_UP) {
			pjlc->sg.usrd->loop = JL_SG_WM_DN;
			timeTilMessageVanish = 8.5f;
		}else{
			pjlc->sg.usrd->loop = JL_SG_WM_UP;
			timeTilMessageVanish = 8.5f;
		}
	}
	
	static void _jl_gr_flipscreen(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;
		_jl_gr_flip_scrn(pjlc);
		_jl_gr_draw_icon(pusr);
	}
	
	static void _jl_gr_menubar_name(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;

		_jl_gr_draw_icon(pusr);
		jl_gr_draw_text(pusr, pjlc->dl.windowTitle[0],
			pjlc->gr.menubar.iconx, 0., .05, 255);
		jl_gr_draw_text(pusr, pjlc->dl.windowTitle[1],
			pjlc->gr.menubar.iconx, .05, .05, 255);
	}
	
	static void _jl_gr_menubar_slow(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;
		if(pjlc->sg.processingTimeMillis > JL_MAIN_SAPT) {
			pjlc->gr.menubar.chr[pjlc->gr.menubar.cursor] =
				JLGR_ID_SLOW_IMAGE;
/*			sprintf(pjlc->dl.windowTitle[1], "%d/%d",
				pjlc->sg.processingTimeMillis, JL_MAIN_SAPT);*/
			sprintf(pjlc->dl.windowTitle[1], "%f", pusr->psec);
		}else{
			pjlc->gr.menubar.chr[pjlc->gr.menubar.cursor] =
				JLGR_ID_GOOD_IMAGE;
		}
		_jl_gr_draw_icon(pusr);
	}
	
	static void _jl_gr_menubar(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;

		for(pjlc->gr.menubar.cursor = 0; pjlc->gr.menubar.cursor < 10;
			pjlc->gr.menubar.cursor++)
		{
			if(!pjlc->gr.menubar.chr[pjlc->gr.menubar.cursor])break;
			pjlc->gr.menubar.iconx =.9-(.1*pjlc->gr.menubar.cursor);

			if( (pjlc->ct.msx > pjlc->gr.menubar.iconx) &&
				(pjlc->ct.msy < .1) &&
				(pjlc->ct.heldDown == 1))
			{
				pjlc->gr.menubar.func
					[1][pjlc->gr.menubar.cursor](pusr);
				pjlc->ct.heldDown = 2;
			}else{
				pjlc->gr.menubar.func
					[0][pjlc->gr.menubar.cursor](pusr);
			}
		}
	}
	
	static void _jl_gr_mouse_loop(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;
	//Update Mouse
		pusr->mouse->r.x = jl_ct_gmousex(pusr);
		pusr->mouse->r.y = jl_ct_gmousey(pusr) + pjlc->gl.ytrans;
		pusr->mouse->cb.x = pusr->mouse->r.x;
		pusr->mouse->cb.y = pusr->mouse->r.y;
	//if computer, draw mouse
	#if JL_PLAT == JL_PLAT_COMPUTER
		jl_gr_sprite_draw(pusr, pjlc->sg.usrd->mouse);
	#endif
	}

	void _jl_gr_loop(jl_t* pusr) {
	//Menu Bar
		jvct_t *pjlc = pusr->pjlc;
		if(!pjlc->fl.inloop) pjlc->gr.menuoverlay(pusr);
	//Message Display
		if(timeTilMessageVanish > 0.f) {
			if(timeTilMessageVanish > 4.25)
				jl_gr_draw_ctxt(pusr,
					GMessage[pusr->loop == JL_SG_WM_DN],
					0, 255);
			else
				jl_gr_draw_ctxt(pusr,
					GMessage[pusr->loop == JL_SG_WM_DN],
					0, (uint8_t)
					(timeTilMessageVanish * 255.f / 4.25));	
			timeTilMessageVanish-=pusr->psec;
		}
	//Update mouse
		pusr->mouse->loop((void*)pusr);
	}

	void _jl_gr_init(jvct_t *pjlc) {
		jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "GRIN");
		pjlc->gr.menuoverlay = _jl_gr_menubar;
		pjlc->sg.usrd->mouse = jl_gr_sprite_make(
			pjlc->sg.usrd, 0, 0, 254, 255, //G,I,C,A
			0.f, 0.f, .075f, .075f, //XYWH
			_jl_gr_mouse_loop, 0);
		pjlc->sg.usrd->mouse->cb.w = 0.f;
		pjlc->sg.usrd->mouse->cb.h = 0.f;
		//Taskbar
		jl_io_printc(pjlc->sg.usrd, "loading taskbar...\n");
		int i;
		for(i = 0; i < 10; i++) {
			pjlc->gr.menubar.chr[i] = JLGR_ID_NULL;
		}
		jl_io_printc(pjlc->sg.usrd, "loaded taskbar!\n");
		jl_io_close_block(pjlc->sg.usrd); //Close Block "GRIN"
	}
/** @endcond **/
