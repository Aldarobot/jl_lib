#include "header/jlvm_pr.h"

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

/*_jal5_grph_t* g_grph;*/

/*EXPORTED FUNCTIONS*/

	/*
	 * Draw An Image, 'i' is the ID of the image.  'xywh' is where it is and how big
	 * It is drawn.  'chr' is 0 unless you want to use the image as a charecter map,
	 * then it will zoom into charecter 'chr'.  'a' is the transparency each pixel
	 * is multiplied by; 255 is solid and 0 is totally invisible
	*/
	void grph_draw_simg(sgrp_user_t* pusr, s32t i,
		float x,float y,float w,float h,
		u08t chr, u08t a)
	{
		dect Oone[] = {
			x,	h+y,	0.f,
			x,	y,	0.f,
			w+x,	y,	0.f,
			w+x,	h+y,	0.f };

		jal5_eogl_vrtx(pusr->pjct, 4, Oone);
		jal5_eogl_txtr(pusr->pjct, chr, a, i);
		jal5_eogl_draw(pusr->pjct);
	}

	//Draw "str" at 'x','y', size 'size', transparency 'a'
	void grph_draw_text(sgrp_user_t* pusr, char *str, dect x, dect y, dect size,
		uint8_t a)
	{
		void *Str = str;
		uint8_t *STr = Str;
		uint32_t i;
		for(i = 0; i < strlen(str); i++) {
			grph_draw_simg(pusr, 0,
				x, y, size, size,
				STr[i], a);
			x = x + size;
		}
	}

	//draw a number at "x","y" size: "size" transparency "a"
	void grph_draw_numi(sgrp_user_t* pusr, uint32_t num, dect x, dect y, dect size,
		uint8_t a)
	{
		char display[10];
		sprintf(display, "%d", num);
		grph_draw_text(pusr, display, x, y, size, a);
	}

	//Draw centered text on screen saying "strt" at y coordinate "p_y"
	void grph_draw_ctxt(sgrp_user_t* pusr, char *str, dect p_y) {
		grph_draw_text(pusr->pjct, str, 0, p_y,
			1.f / ((float)(strlen(str))), 255);
	}

	//Print message "str" on the screen
	void grph_draw_msge(char * str) {
		GScreenMesg = str;
		GScreenMesgOn = 1;
	}

	//Print a message on the screen and then terminate the program
	void grph_term_msge(sgrp_user_t* pusr, char *message) {
		grph_draw_msge(message);
		jlvm_dies(pusr->pjct, Strt(message));
	}

/*BACKGROUND FUNCTIONS*/

	void _grph_flip_scrn(jvct_t *pjct) {
//		printf("%p\n", pjct->Sgrp.usrd);
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("flipping!!%d\n", pjct->Sgrp.usrd->loop);
		#endif
		if(pjct->Sgrp.usrd->loop == SGRP_WUPS) {
			jl_sg_set_window(pjct->Sgrp.usrd, SGRP_WDNS);
			timeTilMessageVanish = 255;
		}else{
			jl_sg_set_window(pjct->Sgrp.usrd, SGRP_WUPS);
			timeTilMessageVanish = 255;
		}
	}

	//Print a message on the screen
	void _grph_draw_msge(sgrp_user_t* pusr) {
		grph_draw_ctxt(pusr, GScreenMesg, JAL5_GRPH_YDEP/2);
		GScreenMesgOn = 0;
	}

	void _jal5_grph_loop(sgrp_user_t* pusr) {
	//Menu Bar
		grph_draw_simg(pusr, 1, .9, 0., .1, .1, 2, 255);
	//if computer, draw mouse
	#if PLATFORM == 0
		grph_draw_simg(pusr, 0,
			inpt_gets_xmse(pusr), inpt_gets_ymse(pusr),
			.075, .075, 254,255);
	#endif
	}

/*
void usr_run(void) {
	bjl_fnc_lst_run(fnc_lst_msg, MSG_OFF);
}*/

	void _jal5_grph_init(jvct_t *pjct) {
	//	printf("%p\n", pjct->Sgrp.usrd);
	//	fnc_lst_msg = bjl_fnc_lst_new(MSG_MAX);
	//	bjl_fnc_lst_add(fnc_lst_msg, MSG_ONN, jgr_updn_screen_message);
	//	bjl_fnc_lst_add(fnc_lst_msg, MSG_OFF, do_nothing);
	}
