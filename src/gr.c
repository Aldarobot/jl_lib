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

//MESSAGES
char *GScreenMesg;
uint8_t GScreenMesgOn = 0;

/*screen being displayed ( on two screens which one on bottom, if keyboard is
	being displayed on bottom screen then this will be displayed on top )*/
uint8_t timeTilMessageVanish = 0;

char *GMessage[2] = {
	"SWITCH SCREEN: UPPER",
	"SWITCH SCREEN: LOWER"
};

/*_jal5_jl_gr_t* g_grph;*/

static void _jl_gr_menubar(jl_t* pusr);

/*EXPORTED FUNCTIONS*/

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
		dect Oone[] = {
			x,	h+y,	0.f,
			x,	y,	0.f,
			w+x,	y,	0.f,
			w+x,	h+y,	0.f };

		eogl_vrtx(pusr->pjlc, 4, Oone);
		eogl_txtr(pusr->pjlc, c, a, g, i);
		eogl_draw(pusr->pjlc);
	}
	
	/**
	 * Draw the sprite.
	 *
	 * @param 'pusr': library context
	 * @param 'psprite': the sprite to draw.
	**/
	void jl_gr_sprite_draw(jl_t* pusr, jl_sprite_t *psprite) {
		jl_gr_draw_image(pusr, psprite->g, psprite->i,
			psprite->x, psprite->y, psprite->w, psprite->h,
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
		dect x, dect y, dect w, dect h,
		jl_simple_fnt loop, u32t ctxs)
	{
		jl_sprite_t *rtn;
		rtn = malloc(sizeof(jl_sprite_t));
		rtn->g = g;
		rtn->i = i;
		rtn->c = c;
		rtn->a = a;
		rtn->x = x; rtn->cx = x;
		rtn->y = y; rtn->cy = y;
		rtn->w = w; rtn->cw = w;
		rtn->h = h; rtn->ch = h;
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
			(sprite1->cy >= (sprite2->cy+sprite2->ch)) ||
			(sprite1->cx >= (sprite2->cx+sprite2->cw)) ||
			(sprite2->cy >= (sprite1->cy+sprite1->ch)) ||
			(sprite2->cx >= (sprite1->cx+sprite1->cw)) )
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
	void jl_gr_draw_text(jl_t* pusr, char *str, dect x, dect y, dect size,
		uint8_t a)
	{
		void *Str = str;
		uint8_t *STr = Str;
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
	void jl_gr_draw_numi(jl_t* pusr, uint32_t num, dect x, dect y, dect size,
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
		float fontsize = (psprite->w - .1) / strlen(txt);
		jl_gr_draw_text(pusr, txt,
			psprite->x + .05,
			psprite->y + (.5 * (psprite->h - fontsize)),
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
	 */
	void jl_gr_draw_ctxt(jl_t* pusr, char *str, dect p_y) {
		jl_gr_draw_text(pusr->pjlc, str, 0, p_y,
			1.f / ((float)(strlen(str))), 255);
	}

	/**
	 * Print message on the screen.
	 * @param 'message': the message 
	 */
	void jl_gr_draw_msge(char * message) {
		GScreenMesg = message;
		GScreenMesgOn = 1;
	}

	/**
	 * Print a message on the screen and then terminate the program
 	 * @param 'pusr': library context
 	 * @param 'message': the message 
	 */
	void jl_gr_term_msge(jl_t* pusr, char *message) {
		jl_gr_draw_msge(message);
		jlvm_dies(pusr->pjlc, Strt(message));
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
		float slidex, jl_ct_onevent_fnt prun)
	{
		jl_gr_draw_text_sprite(pusr, psprite, txt);
		psprite->x = defaultx;
		if(jl_gr_sprite_collide(pusr, pusr->mouse, psprite)) {
			jl_ct_run_event(pusr,
				JL_CT_ALLP(JL_CT_GAME_STYL, JL_CT_COMP_CLLT,
					JL_CT_ANDR_TOUC), prun
				);
			psprite->x = defaultx + slidex;
		}
	}
	
	/**
	 * toggle whether or not to show the menu bar.
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

/** @cond **/
/*BACKGROUND FUNCTIONS*/

	void _jl_gr_flip_scrn(jvct_t *pjlc) {
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("flipping!!%d\n", pjlc->sg.usrd->loop);
		#endif
		if(pjlc->sg.usrd->loop == JL_SG_WM_UP) {
			pjlc->sg.usrd->loop = JL_SG_WM_DN;
			timeTilMessageVanish = 255;
		}else{
			pjlc->sg.usrd->loop = JL_SG_WM_UP;
			timeTilMessageVanish = 255;
		}
	}

	//Print a message on the screen
	void _jl_gr_draw_msge(jl_t* pusr) {
		jl_gr_draw_ctxt(pusr, GScreenMesg, (9./16.)/2);
		GScreenMesgOn = 0;
	}
	
	static void _jl_gr_menubar(jl_t* pusr) {
	/*	flip->x = 255;
		flip->w = 20;
		for(i = 0; i < 5; i++) {
			flip->x-=20;
			if((taskbar[i] == GOOD_IMAGE_ID) && (slow)) {
				flip->g->w = SLOW_IMAGE_ID;
				sprintf(windowTitle[1], "%d/%d", processingTimeMillis,
					TimeProcessingAllowed);
			}else{
				flip->g->w = taskbar[i];
			}
	//		jgr_upd_sprite_pos(flip);
	//		jgr_draw_sprite(flip);
		}
		flip->w = 155;
		flip->x = 0;
		flip->g->w = UNKNOWN_ID;
	//	jgr_upd_sprite_pos(flip);
	//	jgr_draw_sprite(flip);
		jgr_draw_text(windowTitle[0], 0, 0, 10);
		jgr_draw_text(windowTitle[1], 0, 10, 10);
		if(timeTilMessageVanish) {
			jgr_draw_centered_text(GMessage[GScreenDisplayed],0);
			timeTilMessageVanish--;
		}*/
		jl_gr_draw_image(pusr, 0, 1, .9, 0., .1, .1, 2, 255);
		
		jvct_t *pjlc = pusr->pjlc;
		if( (pjlc->ct.msx > .9) && (pjlc->ct.msy < .1) &&
			(pjlc->ct.heldDown == 1))
		{
			_jl_gr_flip_scrn(pjlc);
			pjlc->ct.heldDown = 2;
			return;
		}
	}
	
	static void _jl_gr_mouse_loop(jl_t* pusr) {
		jvct_t *pjlc = pusr->pjlc;
	//Update Mouse
		pusr->mouse->x = jl_ct_gmousex(pusr);
		pusr->mouse->y = jl_ct_gmousey(pusr);
		pusr->mouse->cx = pusr->mouse->x;
		pusr->mouse->cy = pusr->mouse->y;
	//if computer, draw mouse
	#if PLATFORM == 0
		jl_gr_sprite_draw(pusr, pjlc->sg.usrd->mouse);
	#endif
	}

	void _jl_gr_loop(jl_t* pusr) {
	//Menu Bar
		jvct_t *pjlc = pusr->pjlc;
		pjlc->gr.menuoverlay(pusr);
	//Update mouse
		pusr->mouse->loop((void*)pusr);
	}

	void _jl_gr_init(jvct_t *pjlc) {
		pjlc->gr.menuoverlay = _jl_gr_menubar;
		pjlc->sg.usrd->mouse = jl_gr_sprite_make(
			pjlc->sg.usrd, 0, 0, 254, 255, //G,I,C,A
			0.f, 0.f, .075f, .075f, //XYWH
			_jl_gr_mouse_loop, 0);
		pjlc->sg.usrd->mouse->cw = 0.f;
		pjlc->sg.usrd->mouse->ch = 0.f;
		//Taskbar
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("[JLVM/LIM] loading taskbar...\n");
		#endif

	/*	taskbar[0] = FLIP_IMAGE_ID;
		taskbar[1] = GOOD_IMAGE_ID;
		taskbar[2] = UNKNOWN_ID;
		taskbar[3] = UNKNOWN_ID;
		taskbar[4] = UNKNOWN_ID;
	//	jgr_load_image(IMGID_TASK_BUTTON, taskbar_items, sizeof(taskbar_items));
		jgr_grp_t *flipg = jgr_make_graphic(IMGID_TASK_BUTTON,255,0);

		flip = jgr_make_sprite(235,0,20,20,flipg);
		mouse = jgr_make_sprite(0,0,10,10,textg);*/
	}
/** @endcond **/
