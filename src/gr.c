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

//Upper OpenGL prototypes
jl_vo *jl_gl_vo_make(jvct_t *_jlc, float *xyzw, uint8_t vertices);
void jl_gl_vo_free(jvct_t *_jlc, jl_vo *pv);

//Upper SDL prototype
void _jl_dl_loop(jvct_t* _jlc);

//Graphics Prototypes
static void _jl_gr_menubar(jl_t* jlc);
static void _jl_gr_flipscreen(jl_t* jlc);
static void _jl_gr_menubar_slow(jl_t* jlc);
static void _jl_gr_menubar_name(jl_t* jlc);
static void _jl_gr_draw_icon(jl_t* jlc);
static void _jl_gr_textbox_rt(jl_t* jlc);
static void _jl_gr_textbox_lt(jl_t* jlc);
static void _jl_gr_popup_loop(jl_t* jlc);

/*EXPORTED FUNCTIONS*/

	/**
	 * Set the clipping pane for the screen.
	 * @param jlc: The library context.
  	 * @param xmin: anything drawn left of this line is disregarded.
  	 * @param xmax: anything drawn right of this line is disregarded.
	 * @param ymin: if something is drawn below this line it is disregarded.
 	 * @param ymax: if something is drawn above this line it is disregarded.
	**/
	void jl_gr_set_clippane(jl_t* jlc, float xmin, float xmax,
		float ymin, float ymax)
	{
		jl_gl_set_clippane(jlc->_jlc, xmin, xmax, ymin, ymax);
	}
	
	/**
	 * Reset the clipping pane for the screen.
 	 * @param jlc: The library context.
	**/
	void jl_gr_default_clippane(jl_t* jlc) {
		jl_gl_default_clippane(jlc->_jlc);
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
	 * Draw a vertex object.
  	 * @param jlc: The library context.
  	 * @param pv: the vertex object to draw.
	**/
	void jl_gr_draw_vo(jl_t* jlc, jl_vo* pv) {
		jl_gl_translate(jlc->_jlc, pv, 0.f, 0.f, 0.f);
		jl_gl_draw(jlc->_jlc, pv);
	}
	
	/**
	 * Draw a vertex object with offset by translation.
  	 * @param jlc: The library context.
  	 * @param pv: The vertex object to draw.
  	 * @param vec: The vector of offset/translation.
	**/
	void jl_gr_draw_tvo(jl_t* jlc, jl_vo* pv, jl_vec3_t* vec) {
		jl_gl_translate(jlc->_jlc, pv, vec->x, vec->y, vec->z);
		jl_gl_draw(jlc->_jlc, pv);
	}
	
	/**
	 * Set a Vertex object to vector graphics.
	 * 
	**/
	void jl_gr_vos_vec(jl_t* jlc, jl_vo *pv, uint16_t tricount,
		float* triangles, uint8_t* colors, uint8_t multicolor)
	{
		// Set the vertex object
		jl_gl_vect(jlc->_jlc, pv, tricount * 3, triangles);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jlc->_jlc, pv, colors);
		else jl_gl_clrs(jlc->_jlc, pv, colors);
	}
	
	/**
	 * Set a vertex object to a rectangle.
	**/
	void jl_gr_vos_rec(jl_t* jlc, jl_vo *pv, jl_rect_t rc, uint8_t* colors,
		uint8_t multicolor)
	{
		float rectangle_coords[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w + rc.x,	rc.y,		0.f,
			rc.w + rc.x,	rc.h+rc.y,	0.f };

		// Set the vertex object
		jl_gl_poly(jlc->_jlc, pv, 4, rectangle_coords);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jlc->_jlc, pv, colors);
		else jl_gl_clrs(jlc->_jlc, pv, colors);
	}
	
	/**
	 * Get a Vertex Object for vector graphics.
	**/
	jl_vo* jl_gr_vof_vec(jl_t* jlc, uint16_t tricount, float* triangles,
		uint8_t* colors, uint8_t multicolor)
	{
		// Make the vertex object
		jl_vo *rtn = jl_gl_vo_make(jlc->_jlc, NULL, 0);
		// Set the vertex object
		jl_gr_vos_vec(jlc,rtn,tricount,triangles,colors,multicolor);
		// Return the vertex object
		return rtn;
	}
	
	/**
	 * Get a a Vertex Object for a rectangle.
	 * @param jlc: The library context.
	 * @param rc: The rectangle coordinates.
	 * @param colors: The color(s) to use - [ R, G, B, A ]
	 * @param multicolor: If 0: Then 1 color is used.
	 *	If 1: Then 1 color per each vertex is used.
	**/
	jl_vo* jl_gr_vof_rec(jl_t* jlc, jl_rect_t rc, uint8_t* colors,
		uint8_t multicolor)
	{
		float rectangle_coords[] = {
			rc.x,		rc.h+rc.y,	0.f,
			rc.x,		rc.y,		0.f,
			rc.w + rc.x,	rc.y,		0.f,
			rc.w + rc.x,	rc.h+rc.y,	0.f };
		// Make the vertex object
		jl_vo *rtn = jl_gl_vo_make(jlc->_jlc, NULL, 0);
		// Set the vertex object
		jl_gl_poly(jlc->_jlc, rtn, 4, rectangle_coords);
		// Texture the vertex object
		if(multicolor) jl_gl_clrg(jlc->_jlc, rtn, colors);
		else jl_gl_clrs(jlc->_jlc, rtn, colors);
		// Return the vertex object
		return rtn;
	}

	/**
	 * Draw An Image.
	 *
	 * @param 'jlc': The library context
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
	void jl_gr_draw_image(jl_t* jlc, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t c, u08t a)
	{
		//From bottom left & clockwise
		float Oone[] = {
			x,	h+y,	0.f,
			x,	y,	0.f,
			w+x,	y,	0.f,
			w+x,	h+y,	0.f };
		jl_gl_txtr(jlc->_jlc, NULL, c, a, g, i);
		jl_gl_poly(jlc->_jlc, NULL, 4, Oone);
		jl_gr_draw_vo(jlc, NULL);
	}
	
	/**
	 * 
	**/
	void jl_gr_vof_image(jl_t* jlc, jl_vo* pv, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t c, u08t a)
	{
		
	}

	/**
	 * Create an empty vertex object & return it.
	 * @param jlc: The library context
	 * @returns: A new vertex object with 0 vertices.
	**/
	jl_vo* jl_gr_vo_new(jl_t* jlc) {
		return jl_gl_vo_make(jlc->_jlc, NULL, 0);
	}
	
	/**
	 * Free a vertex object.
	 * @param jlc: The library context
	 * @param pv: The vertex object to free
	**/
	void jl_gr_vo_old(jl_t* jlc, jl_vo* pv) {
		jl_gl_vo_free(jlc->_jlc, pv);
	}

	/**
	 * Draw the sprite.
	 *
	 * @param jlc: The library context
	 * @param psprite: Which sprite to draw.
	**/
	void jl_gr_sprite_draw(jl_t* jlc, jl_sprite_t *psprite) {
		jl_gr_draw_image(jlc, psprite->g, psprite->i,
			psprite->r.x, psprite->r.y, psprite->r.w, psprite->r.h,
			psprite->c, psprite->a);
	}
	
	/**
	 * Create a sprite
	 *
	 * @param 'jlc': library context
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
		jl_t* jlc, u16t g, u16t i, u08t c, u08t a,
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
	 * @param 'jlc': library context
	 * @param 'sprite1': sprite 1
	 * @param 'sprite2': sprite 2
	 * @return 0: if the sprites don't collide in their bounding boxes.
	 * @return 1: if the sprites do collide in their bounding boxes.
	**/
	u08t jl_gr_sprite_collide(jl_t* jlc,
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
	 * @param 'jlc': library context
	 * @param 'str': the text to draw
	 * @param 'x': the x position to draw it at
	 * @param 'y': the y position to draw it at
	 * @param 'size': how big to draw the text
	 * @param 'a': transparency of the text, 255=Opaque 0=Transparent
	**/
	void jl_gr_draw_text(jl_t* jlc, char *str, float x, float y,
		float size, uint8_t a)
	{
		if(str == NULL) return;
		const void *Str = str;
		const uint8_t *STr = Str;
		uint32_t i;
		for(i = 0; i < strlen(str); i++) {
			jl_gr_draw_image(jlc, 0, 0, //Font 0:0
				x, y, size, size,
				STr[i], a);
			x = x + size;
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
	 * @param 'psprite': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_area(jl_t* jlc, jl_sprite_t * psprite, char *txt){
		float fontsize = (psprite->r.w - .1) / strlen(txt);
		jl_gr_draw_text(jlc, txt,
			psprite->r.x + .05,
			psprite->r.y + (.5 * (psprite->r.h - fontsize)),
			fontsize, 255);
	}

	/**
	 * Draw a sprite, then draw text within the boundary of a sprite
 	 * @param 'jlc': library context
	 * @param 'psprite': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_sprite(jl_t* jlc,jl_sprite_t * psprite,char *txt){
		jl_gr_sprite_draw(jlc, psprite);
		jl_gr_draw_text_area(jlc, psprite, txt);
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
		jl_gr_draw_image(jlc, 0, 1, 0., 0., 1., jl_dl_p(jlc), 1, 127);
		jl_gr_draw_ctxt(jlc, message, (9./16.)/2, 255);
		_jl_dl_loop(jlc->_jlc); //Update Screen
	}

	/**
	 * Print a message on the screen and then terminate the program
 	 * @param 'jlc': library context
 	 * @param 'message': the message 
	 */
	void jl_gr_term_msge(jl_t* jlc, char *message) {
		jl_gr_draw_msge(jlc, message);
		jl_sg_kill(jlc, message);
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
 	 * @param 'psprite': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'defaultx': the default x position of the button.
 	 * @param 'slidex': how much the x should change when hovered above.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_slide_button(
		jl_t* jlc, jl_sprite_t * psprite, char *txt, float defaultx,
		float slidex, jl_simple_fnt prun)
	{
		jl_gr_draw_text_sprite(jlc, psprite, txt);
		psprite->r.x = defaultx;
		if(jl_gr_sprite_collide(jlc, jlc->mouse, psprite)) {
			jl_ct_run_event(jlc, JL_CT_PRESS, prun, jl_dont);
			psprite->r.x = defaultx + slidex;
		}
	}
	
	/**
	 * Draw a glow button, and activate if it is pressed.
	 * @param 'jlc': the libary context
 	 * @param 'psprite': the sprite to draw
 	 * @param 'txt': the text to draw on the button.
 	 * @param 'prun': the function to run when pressed.
	**/
	void jl_gr_draw_glow_button(jl_t* jlc, jl_sprite_t * psprite,
		char *txt, jl_simple_fnt prun)
	{
		jl_gr_sprite_draw(jlc, psprite);
		if(jl_gr_sprite_collide(jlc, jlc->mouse, psprite)) {
			jl_ct_run_event(jlc, JL_CT_PRESS, prun, jl_dont);
//			jl_gr_draw_rect(jlc, psprite->r.x, psprite->r.y,
//				psprite->r.w, psprite->r.h,
//				1., 1., 1., 64);
			jl_gr_draw_text(jlc, txt, 0, jl_dl_p(jlc) - .0625,
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

		if(*string == NULL) {
			*string = jl_me_strt_make(1, STRT_KEEP);
		}
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
		jl_gr_draw_image(jlc, 0, 0, x, y, w, h, ' ', 255);
		jl_gr_draw_text(jlc, (char*)((*string)->data), x, y, h, 255);
		jl_gr_draw_image(jlc, 0, 0,
			x + (h*((float)(*string)->curs-.5)), y, h, h, 252, 255);
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
			_jlc->gr.menuoverlay = _jl_gr_menubar;
		else
			_jlc->gr.menuoverlay = jl_dont;
	}
	
	/**
	 * Add an icon to the menubar
	 *
	 * @param jlc: the libary context
	 * @param grp: the image group of the image to display
	 * @param iid: the id of the image in the image group
	 * @param chr: the character of the image to display.  0 means stop.
	 * @param fno: the function to run when the icon isn't pressed.
	 * @param fnc: the function to run when the icon is pressed.
	**/
	void jl_gr_addicon(jl_t* jlc, uint16_t grp, uint8_t iid,
		uint8_t chr, jl_simple_fnt fno, jl_simple_fnt fnc)
	{
		jvct_t *_jlc = jlc->_jlc;
		uint8_t i;

		for(i = 0; i < 10; i++) {
			if(!_jlc->gr.menubar.chr[i]) break;
		}
		_jlc->gr.menubar.grp[i] = grp;
		_jlc->gr.menubar.iid[i] = iid;
		_jlc->gr.menubar.chr[i] = chr;
		_jlc->gr.menubar.func[0][i] = fno;
		_jlc->gr.menubar.func[1][i] = fnc;
	}

	/**
	 * Add the flip screen icon to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_flip(jl_t* jlc) {
		jl_gr_addicon(jlc, 0, 1, JLGR_ID_FLIP_IMAGE,
			_jl_gr_draw_icon, _jl_gr_flipscreen);	
	}
	
	/**
	 * Add slowness detector to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_slow(jl_t* jlc) {
		jl_gr_addicon(jlc, 0, 1, JLGR_ID_GOOD_IMAGE,
			_jl_gr_menubar_slow, _jl_gr_draw_icon);
	}
	
	/**
	 * Add program title to the menubar.
	 * @param jlc: the libary context
	**/
	void jl_gr_addicon_name(jl_t* jlc) {
		int i;
		for(i = 0; i < 4; i++) {
			jl_gr_addicon(jlc, 0, 1, JLGR_ID_UNKNOWN,
				_jl_gr_draw_icon, _jl_gr_draw_icon);
		}
		jl_gr_addicon(jlc, 0, 1, JLGR_ID_UNKNOWN,
			_jl_gr_menubar_name, _jl_gr_draw_icon);
	}

/** @cond **/
/*BACKGROUND FUNCTIONS*/

	static void _jl_gr_popup_loop(jl_t* jlc) {
//		jl_gr_draw_rect(jlc, .1, .1, .8, .2, 127, 127, 255, 255);
//		jl_gr_draw_rect(jlc, .1, .3, .8, .8, 64, 127, 127, 255);
	}

	static void _jl_gr_textbox_lt(jl_t* jlc) {
//		if((int)y != 1) return;
		jvct_t *_jlc = jlc->_jlc;
		
		jl_ct_typing_disable(jlc);
		if(_jlc->gr.textbox_string->curs)
			_jlc->gr.textbox_string->curs--;
	}
	
	static void _jl_gr_textbox_rt(jl_t* jlc) {
//		if((int)y != 1) return;
		jvct_t *_jlc = jlc->_jlc;

		jl_ct_typing_disable(jlc);
		if(_jlc->gr.textbox_string->curs < _jlc->gr.textbox_string->size)
			_jlc->gr.textbox_string->curs++;
	}

	static void _jl_gr_draw_icon(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

//		jl_gr_draw_rect(jlc, _jlc->gr.menubar.iconx - .01,
//			.01,
//			.1, .1, 0., 0., 0., 64);
		jl_gr_draw_image(jlc, 0, 1, _jlc->gr.menubar.iconx,
			0.,
			.1, .1,
			_jlc->gr.menubar.chr[_jlc->gr.menubar.cursor],
			255);
	}

	void _jl_gr_flip_scrn(jvct_t *_jlc) {
		if(_jlc->jlc->loop == JL_SG_WM_UP) {
			_jlc->jlc->loop = JL_SG_WM_DN;
			timeTilMessageVanish = 8.5f;
		}else{
			_jlc->jlc->loop = JL_SG_WM_UP;
			timeTilMessageVanish = 8.5f;
		}
	}
	
	static void _jl_gr_flipscreen(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;
		_jl_gr_flip_scrn(_jlc);
		_jl_gr_draw_icon(jlc);
	}
	
	static void _jl_gr_menubar_name(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

		_jl_gr_draw_icon(jlc);
		jl_gr_draw_text(jlc, _jlc->dl.windowTitle[0],
			_jlc->gr.menubar.iconx, 0., .05, 255);
		jl_gr_draw_text(jlc, _jlc->dl.windowTitle[1],
			_jlc->gr.menubar.iconx, .05, .05, 255);
	}
	
	static void _jl_gr_menubar_slow(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;
		if(_jlc->sg.processingTimeMillis > JL_MAIN_SAPT) {
			_jlc->gr.menubar.chr[_jlc->gr.menubar.cursor] =
				JLGR_ID_SLOW_IMAGE;
/*			sprintf(_jlc->dl.windowTitle[1], "%d/%d",
				_jlc->sg.processingTimeMillis, JL_MAIN_SAPT);*/
			sprintf(_jlc->dl.windowTitle[1], "%f", jlc->psec);
		}else{
			_jlc->gr.menubar.chr[_jlc->gr.menubar.cursor] =
				JLGR_ID_GOOD_IMAGE;
		}
		_jl_gr_draw_icon(jlc);
	}
	
	static void _jl_gr_menubar(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;

		for(_jlc->gr.menubar.cursor = 0; _jlc->gr.menubar.cursor < 10;
			_jlc->gr.menubar.cursor++)
		{
			if(!_jlc->gr.menubar.chr[_jlc->gr.menubar.cursor])break;
			_jlc->gr.menubar.iconx =.9-(.1*_jlc->gr.menubar.cursor);

			if( (_jlc->ct.msx > _jlc->gr.menubar.iconx) &&
				(_jlc->ct.msy < .1) &&
				(_jlc->ct.heldDown == 1))
			{
				_jlc->gr.menubar.func
					[1][_jlc->gr.menubar.cursor](jlc);
				_jlc->ct.heldDown = 2;
			}else{
				_jlc->gr.menubar.func
					[0][_jlc->gr.menubar.cursor](jlc);
			}
		}
	}
	
	static void _jl_gr_mouse_loop(jl_t* jlc) {
		jvct_t *_jlc = jlc->_jlc;
	//Update Mouse
		jlc->mouse->r.x = jl_ct_gmousex(jlc);
		jlc->mouse->r.y = jl_ct_gmousey(jlc);
		jlc->mouse->cb.x = jlc->mouse->r.x;
		jlc->mouse->cb.y = jlc->mouse->r.y;
	//if computer, draw mouse
	#if JL_PLAT == JL_PLAT_COMPUTER
		jl_gr_sprite_draw(jlc, _jlc->jlc->mouse);
	#endif
	}

	void _jl_gr_loop(jl_t* jlc) {
	//Menu Bar
		jvct_t *_jlc = jlc->_jlc;
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
		jlc->mouse->loop((void*)jlc);
	}

	void _jl_gr_init(jvct_t *_jlc) {
		jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "GRIN");
		_jlc->gr.menuoverlay = _jl_gr_menubar;
		_jlc->jlc->mouse = jl_gr_sprite_make(
			_jlc->jlc, 0, 0, 254, 255, //G,I,C,A
			0.f, 0.f, .075f, .075f, //XYWH
			_jl_gr_mouse_loop, 0);
		_jlc->jlc->mouse->cb.w = 0.f;
		_jlc->jlc->mouse->cb.h = 0.f;
		//Taskbar
		jl_io_printc(_jlc->jlc, "loading taskbar...\n");
		int i;
		for(i = 0; i < 10; i++) {
			_jlc->gr.menubar.chr[i] = JLGR_ID_NULL;
		}
		jl_io_printc(_jlc->jlc, "loaded taskbar!\n");
		jl_io_close_block(_jlc->jlc); //Close Block "GRIN"
	}
/** @endcond **/
