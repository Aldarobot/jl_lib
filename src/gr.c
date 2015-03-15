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

/*EXPORTED FUNCTIONS*/

	/*
	 * Draw An Image, 'i' is the ID of the image.  'xywh' is where it is and how big
	 * It is drawn.  'chr' is 0 unless you want to use the image as a charecter map,
	 * then it will zoom into charecter 'chr'.  'a' is the transparency each pixel
	 * is multiplied by; 255 is solid and 0 is totally invisible
	*/
	void jl_gr_draw_image(jl_t* pusr, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t chr, u08t a)
	{
		dect Oone[] = {
			x,	h+y,	0.f,
			x,	y,	0.f,
			w+x,	y,	0.f,
			w+x,	h+y,	0.f };

		eogl_vrtx(pusr->pjlc, 4, Oone);
		eogl_txtr(pusr->pjlc, chr, a, g, i);
		eogl_draw(pusr->pjlc);
	}

	//Draw "str" at 'x','y', size 'size', transparency 'a'
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

	//draw a number at "x","y" size: "size" transparency "a"
	void jl_gr_draw_numi(jl_t* pusr, uint32_t num, dect x, dect y, dect size,
		uint8_t a)
	{
		char display[10];
		sprintf(display, "%d", num);
		jl_gr_draw_text(pusr, display, x, y, size, a);
	}

	//Draw centered text on screen saying "strt" at y coordinate "p_y"
	void jl_gr_draw_ctxt(jl_t* pusr, char *str, dect p_y) {
		jl_gr_draw_text(pusr->pjlc, str, 0, p_y,
			1.f / ((float)(strlen(str))), 255);
	}

	//Print message "str" on the screen
	void jl_gr_draw_msge(char * str) {
		GScreenMesg = str;
		GScreenMesgOn = 1;
	}

	//Print a message on the screen and then terminate the program
	void jl_gr_term_msge(jl_t* pusr, char *message) {
		jl_gr_draw_msge(message);
		jlvm_dies(pusr->pjlc, Strt(message));
	}

/*BACKGROUND FUNCTIONS*/

	void _jl_gr_flip_scrn(jvct_t *pjlc) {
//		printf("%p\n", pjlc->sg.usrd);
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("flipping!!%d\n", pjlc->sg.usrd->loop);
		#endif
		if(pjlc->sg.usrd->loop == JL_SG_WM_UP) {
			jl_sg_set_window(pjlc->sg.usrd, JL_SG_WM_DN);
			timeTilMessageVanish = 255;
		}else{
			jl_sg_set_window(pjlc->sg.usrd, JL_SG_WM_UP);
			timeTilMessageVanish = 255;
		}
	}

	//Print a message on the screen
	void _jl_gr_draw_msge(jl_t* pusr) {
		jl_gr_draw_ctxt(pusr, GScreenMesg, JAL5_GRPH_YDEP/2);
		GScreenMesgOn = 0;
	}

	void _jal5_jl_gr_loop(jl_t* pusr) {
	//Menu Bar
		jl_gr_draw_image(pusr, 0, 1, .9, 0., .1, .1, 2, 255);
	//if computer, draw mouse
	#if PLATFORM == 0
		jl_gr_draw_image(pusr, 0, 0,
			jl_ct_gmousex(pusr), jl_ct_gmousey(pusr),
			.075, .075, 254,255);
	#endif
	}

/*
void usr_run(void) {
	bjl_fnc_lst_run(fnc_lst_msg, MSG_OFF);
}*/

	void _jal5_jl_gr_init(jvct_t *pjlc) {
	//	printf("%p\n", pjlc->sg.usrd);
	//	fnc_lst_msg = bjl_fnc_lst_new(MSG_MAX);
	//	bjl_fnc_lst_add(fnc_lst_msg, MSG_ONN, jgr_updn_screen_message);
	//	bjl_fnc_lst_add(fnc_lst_msg, MSG_OFF, do_nothing);
	}
