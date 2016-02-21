/*
 * (c) Jeron A. Lau
 *
 * jl/ct is a library for making input controllers compatible between different
 * devices.
*/

#include "jl_pr.h"

//Prototypes
	// ct.c
	uint8_t jl_ct_key_pressed(jl_gr_t *jl_gr, uint8_t key);

/*
 * Returns 0 if key isn't pressed
 * Returns 1 if key is just pressed
 * Returns 2 if key is held down
 * Returns 3 if key is released
 * "Preval" is a pointer to previous key pressed value.
 * "read" is the value currently active
*/
static uint8_t _jl_ct_state(uint8_t *preval, const uint8_t read) {
	if(*preval == 0) {
		*preval = read;
		return *preval; //1: Just Pressed, 0: Not pressed
	}else if(*preval && !read) {
		//If Was Held Down And Now Isnt | 3: Release
		*preval = 0;
		return 3;
	}else{
		*preval = 2; //2: Held Down
		return 2;
	}
}

static void _jl_ct_press(jl_t *jlc) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	jlc->ctrl.h = jl_gr->ct.heldDown;
	jlc->ctrl.r = 0.;
	jlc->ctrl.x = jl_ct_gmousex(jl_gr);
	jlc->ctrl.y = jl_ct_gmousey(jl_gr);
}

static void _jl_ct_not_down(jl_gr_t *jl_gr) {
	jl_gr->jlc->ctrl.p = 0.;
	jl_gr->jlc->ctrl.k = 0;
}

static void _jl_ct_is_down(jl_gr_t *jl_gr) {
	jl_gr->jlc->ctrl.k = 1;
	jl_gr->jlc->ctrl.p = 1.;
}

#if JL_PLAT == JL_PLAT_COMPUTER

	void jl_ct_key(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno, uint8_t key)
	{
		uint8_t a = jl_ct_key_pressed(jl_gr, key);
		jl_t* jlc = jl_gr->jlc;
		jlc->ctrl.h = a;
		jlc->ctrl.r = 0.;
		jlc->ctrl.x = 0.;
		jlc->ctrl.y = 0.;
		if(a && (a!=3)) {
			jlc->ctrl.p = 1.;
			jlc->ctrl.k = key;
			prun(jlc);
		}else{
			_jl_ct_not_down(jl_gr);
			pno(jlc);
		}
	}

	void jl_ct_key_retn(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_D);
	}
	
	void jl_ct_left_click(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {
		_jl_ct_press(jl_gr->jlc); //hrxy
		if(jl_gr->ct.heldDown) {
			_jl_ct_is_down(jl_gr); //pk
			prun(jl_gr->jlc);
		}else{
			_jl_ct_not_down(jl_gr); //pk
			pno(jl_gr->jlc);
		}
	}
	
//	void mous_over(jl_fnct prun) {
//		prun(255, 0);
//	}
#elif JL_PLAT == JL_PLAT_PHONE //if Android
	void tuch_cntr(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) { //touch center
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (((jl_gr->ct.msy>.4f * jl_gl_ar(jlc)) &&
			(jl_gr->ct.msy<.6f * jl_gl_ar(jlc))) &&
			((jl_gr->ct.msx>.4f) && (jl_gr->ct.msx<.6f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
	//
	void tuch_nrrt(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) { //near right
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (((jl_gr->ct.msx>.6f) && (jl_gr->ct.msx<.8f)) &&
			((jl_gr->ct.msy * jl_gl_ar(jlc)>.2f) &&
			(jl_gr->ct.msy * jl_gl_ar(jlc)<.8f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_nrlt(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) { //near left
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (((jl_gr->ct.msx<.4f) && (jl_gr->ct.msx>.2f)) &&
			((jl_gr->ct.msy>.2f * jl_gl_ar(jlc)) &&
			(jl_gr->ct.msy<.8f * jl_gl_ar(jlc)))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_nrup(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) { //near up
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (((jl_gr->ct.msy<.4f * jl_gl_ar(jlc)) &&
			(jl_gr->ct.msy>.2f * jl_gl_ar(jlc))) &&
			((jl_gr->ct.msx>.2f) && (jl_gr->ct.msx<.8f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_nrdn(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) { //near down
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (((jl_gr->ct.msy>.6f * jl_gl_ar(jlc)) &&
			(jl_gr->ct.msy<.8f * jl_gl_ar(jlc))) &&
			((jl_gr->ct.msx>.2f) && (jl_gr->ct.msx<.8f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
	//

	void tuch_frrt(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {//far right
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (jl_gr->ct.msx>.8f)) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_frlt(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {//far left
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (jl_gr->ct.msx<.2f)) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_frup(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {//far up
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (jl_gr->ct.msy<.2f * jl_gl_ar(jlc))) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_frdn(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {//far down
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown && (jl_gr->ct.msy>.8f * jl_gl_ar(jlc))) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch(jl_gr_t *jl_gr, jl_fnct prun, jl_fnct pno) {//Any touch
		jl_gr_t* jl_gr = jlc->jl_gr;
		_jl_ct_press(jlc); //hrxy
		if(jl_gr->ct.heldDown) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
#endif

void jl_ct_key_menu(jl_gr_t *jl_gr, jl_fnct prun,
	jl_fnct pno)
{
	#if JL_PLAT == JL_PLAT_COMPUTER
	jl_ct_key(jl_gr, prun, pno, SDL_SCANCODE_APPLICATION); //xyrhpk
	#elif JL_PLAT == JL_PLAT_PHONE
	jl_gr_t* jl_gr = jlc->jl_gr;
	jlc->ctrl.x = 0.;
	jlc->ctrl.y = 0.;
	jlc->ctrl.r = 0.;
	jlc->ctrl.h = jl_gr->ct.menu;
	if(jl_gr->ct.menu) {
		jlc->ctrl.p = 1. * (jl_gr->ct.menu != 3);
		jlc->ctrl.k = SDL_SCANCODE_APPLICATION;
		prun(jlc);
	}else{
		jlc->ctrl.p = 0.;
		jlc->ctrl.k = 0;
		pno(jlc);
	}
	#endif
}

void jl_ct_txty(void) {
	SDL_StartTextInput();
}

void jl_ct_txtn(void) {
	SDL_StopTextInput();
}

float jl_ct_gmousex(jl_gr_t* jl_gr) {
	return jl_gr->ct.msx;
}

float jl_ct_gmousey(jl_gr_t* jl_gr) {
	return jl_gr->ct.msy;
}

static inline void _jl_ct_handle_events_platform_dependant(jl_gr_t* jl_gr) {
#if JL_PLAT == JL_PLAT_PHONE
	if( jl_gr->ct.event.type==SDL_FINGERDOWN ) {
		jl_gr->ct.msx = jl_gr->ct.event.tfinger.x;
		jl_gr->ct.input.finger = 1;
		jl_gr->ct.msy = jl_gr->ct.event.tfinger.y * jl_gl_ar(jl_gr->jlc);
		if(jl_gr->jlc->smde) {
			jl_gr->ct.msy = jl_gr->ct.msy * 2.;
			jl_gr->ct.msy -= jl_gl_ar(jl_gr->jlc);
			if(jl_gr->ct.msy < 0.) jl_gr->ct.input.finger = 0;
		}

	}else if( jl_gr->ct.event.type==SDL_FINGERUP ) {
		jl_gr->ct.input.finger = 0;
	}else if( jl_gr->ct.event.type==SDL_KEYDOWN || jl_gr->ct.event.type==SDL_KEYUP) {
		if( jl_gr->ct.event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
			jl_gr->ct.input.back = (jl_gr->ct.event.type==SDL_KEYDOWN); //Back Key
		else
			jl_gr->ct.input.menu = (jl_gr->ct.event.type==SDL_KEYDOWN); //Menu Key
	}
#elif JL_PLAT == JL_PLAT_COMPUTER
	uint8_t isNowDown = jl_gr->ct.event.type == SDL_MOUSEBUTTONDOWN;
	uint8_t isNowUp = jl_gr->ct.event.type == SDL_MOUSEBUTTONUP;
	if( isNowDown || isNowUp) {
		if(jl_gr->ct.event.button.button == SDL_BUTTON_LEFT)
			jl_gr->ct.input.click_left = isNowDown;
		else if(jl_gr->ct.event.button.button == SDL_BUTTON_RIGHT)
			jl_gr->ct.input.click_right = isNowDown;
		else if(jl_gr->ct.event.button.button == SDL_BUTTON_MIDDLE)
			jl_gr->ct.input.click_middle = isNowDown;
	}else if(jl_gr->ct.event.wheel.type == SDL_MOUSEWHEEL) {
		//SDL 2.0.4 NYI
		uint8_t flip = 1;
			/*(direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1*/;
		int32_t x = flip * jl_gr->ct.event.wheel.x;
		int32_t y = flip * jl_gr->ct.event.wheel.y;
		if(jl_gr->ct.event.wheel.y > 0)
			jl_gr->ct.input.scroll_up = (y > 0) ? y : -y;
		else if(jl_gr->ct.event.wheel.y < 0)
			jl_gr->ct.input.scroll_down = (y > 0) ? y : -y;
		if(jl_gr->ct.event.wheel.x > 0)
			jl_gr->ct.input.scroll_right = (x > 0) ? x : -x;
		else if(jl_gr->ct.event.wheel.x < 0)
			jl_gr->ct.input.scroll_left = (x > 0) ? x : -x;
	}

#endif
}

static void jl_ct_handle_resize__(jl_gr_t* jl_gr) {
	if(jl_gr->ct.event.type==SDL_WINDOWEVENT) { //Resize
		if((jl_gr->ct.event.window.event == SDL_WINDOWEVENT_RESIZED) &&
			(SDL_GetWindowFromID(jl_gr->ct.event.window.windowID) ==
				jl_gr->dl.displayWindow->w))
		{
			main_resz(jl_gr->jlc->_jlc, jl_gr->ct.event.window.data1,
				jl_gr->ct.event.window.data2);
		}
	}
}

static inline void _jl_ct_handle_events(jl_gr_t* jl_gr) {
	if ( jl_gr->ct.event.type == SDL_TEXTINPUT) {
//		printf("%1s\n", &(jl_gr->ct.event.text.text[0]));
		int i;
		for(i = 0; i < 32; i++)
			jl_gr->ct.text_input[i] = jl_gr->ct.event.text.text[i];
		jl_gr->ct.read_cursor = 0;
	}else{
		jl_ct_handle_resize__(jl_gr);
	}
	_jl_ct_handle_events_platform_dependant(jl_gr);
}

static inline void _jl_ct_process_events(jl_gr_t* jl_gr) {
#if JL_PLAT == JL_PLAT_PHONE
	_jl_ct_state(&jl_gr->ct.heldDown, jl_gr->ct.input.finger);
	_jl_ct_state(&jl_gr->ct.menu, jl_gr->ct.input.menu);
#elif JL_PLAT == JL_PLAT_COMPUTER
	_jl_ct_state(&jl_gr->ct.heldDown, jl_gr->ct.input.click_left);
#endif
}

static void jl_ct_testquit__(jl_gr_t* jl_gr) {
	if(jl_gr->ct.back == 1) jl_sg_exit(jl_gr->jlc); // Back Button/Key
}

/**
 * Run prun if event pevent is active.
*/
void jl_ct_run_event(jl_gr_t *jl_gr, uint8_t pevent, jl_gr_fnct prun,
	jl_gr_fnct pno)
{
	jl_ct_event_fnct FunctionToRun_ = jl_gr->ct.getEvents[pevent];

	if(jl_gr->ct.getEvents[pevent] == NULL) {
		jl_io_print(jl_gr->jlc,"Null Pointer: jl_gr->ct.getEvents.Event");
		jl_io_print(jl_gr->jlc,"pevent=%d", pevent);
		jl_sg_kill(jl_gr->jlc);
	}
	FunctionToRun_(jl_gr->jlc, prun, pno);
}

void jl_ct_getevents_(jl_gr_t* jl_gr) {
	while(SDL_PollEvent(&jl_gr->ct.event)) _jl_ct_handle_events(jl_gr);
	//If Back key is pressed, then quit the program
#if JL_PLAT == JL_PLAT_COMPUTER
	jl_gr->ct.keys = SDL_GetKeyboardState(NULL);
	jl_gr->ct.back = jl_ct_key_pressed(jl_gr, SDL_SCANCODE_ESCAPE);
#elif JL_PLAT == JL_PLAT_PHONE
	_jl_ct_state(&jl_gr->ct.back, jl_gr->ct.input.back);
#endif
}

void jl_ct_quickloop_(jl_gr_t* jl_gr) {
	jvct_t* _jlc = jl_gr->jlc->_jlc;

	_jlc->has.quickloop = 1;
	if(_jlc->has.input) {
		jl_ct_getevents_(jl_gr);
		if(jl_gr->ct.back == 1) jl_sg_exit(jl_gr->jlc);
	}else{
		while(SDL_PollEvent(&jl_gr->ct.event))
			jl_ct_handle_resize__(jl_gr);
	}
}

//Main Input Loop
void jl_ct_loop__(jl_gr_t* jl_gr) {
	jvct_t* _jlc = jl_gr->jlc->_jlc;

	_jlc->has.quickloop = 0;
	//Get the information on current events
	jl_ct_getevents_(jl_gr);
	_jl_ct_process_events(jl_gr);
	#if JL_PLAT == JL_PLAT_COMPUTER
		//Get Whether mouse is down or not and xy coordinates
		SDL_GetMouseState(&jl_gr->ct.msxi,&jl_gr->ct.msyi);
		if(jl_gr->ct.msxi < jl_gr->dl.window.x ||
			jl_gr->ct.msxi > jl_gr->dl.window.x + jl_gr->dl.window.w ||
			jl_gr->ct.msyi < jl_gr->dl.window.y ||
			jl_gr->ct.msyi > jl_gr->dl.window.y + jl_gr->dl.window.h)
		{
			if(!jl_gr->ct.sc) {
				SDL_ShowCursor(SDL_ENABLE);
				jl_gr->ct.sc = 1;
			}
		}else{
			if(jl_gr->ct.sc) {
				SDL_ShowCursor(SDL_DISABLE);
				jl_gr->ct.sc = 0;
			}
		}
		int32_t mousex = jl_gr->ct.msxi - jl_gr->dl.window.x;
		int32_t mousey = (jl_gr->ct.msyi - jl_gr->dl.window.y) *
			(1 + jl_gr->jlc->smde);
		//translate integer into float by clipping [0-1]
		jl_gr->ct.msx =
			((float)(mousex-2)) / jl_dl_getw(jl_gr->jlc);
		jl_gr->ct.msy =
			((float)mousey) /
			(jl_dl_geth(jl_gr->jlc) * (1 + jl_gr->jlc->smde));
		if(jl_gr->jlc->smde && jl_gr->ct.msy < 0.) {
			jl_gr->ct.msy = 0.;
			jl_gr->ct.heldDown = 0;
		}
		if(jl_ct_key_pressed(jl_gr, SDL_SCANCODE_F11) == 1)
			jl_dl_togglefullscreen(jl_gr->jlc);
	#endif
	jl_ct_testquit__(jl_gr);
}

static inline void _jl_ct_fn_init(jl_gr_t* jl_gr) {
#if JL_PLAT == JL_PLAT_COMPUTER
	jl_gr->ct.getEvents[JL_CT_COMP_RETN] = jl_ct_key_retn;
	jl_gr->ct.getEvents[JL_CT_COMP_KEYW] = jl_ct_key_keyw;
	jl_gr->ct.getEvents[JL_CT_COMP_KEYA] = jl_ct_key_keya;
	jl_gr->ct.getEvents[JL_CT_COMP_KEYS] = jl_ct_key_keys;
	jl_gr->ct.getEvents[JL_CT_COMP_KEYD] = jl_ct_key_keyd;
	jl_gr->ct.getEvents[JL_CT_COMP_ARUP] = jl_ct_key_arup;
	jl_gr->ct.getEvents[JL_CT_COMP_ARDN] = jl_ct_key_ardn;
	jl_gr->ct.getEvents[JL_CT_COMP_ARRT] = jl_ct_key_arrt;
	jl_gr->ct.getEvents[JL_CT_COMP_ARLT] = jl_ct_key_arlt;
//	jl_gr->ct.getEvents[JL_CT_COMP_MSXY] = mous_over;
	jl_gr->ct.getEvents[JL_CT_COMP_CLLT] = jl_ct_left_click;
	jl_gr->ct.getEvents[JL_CT_COMP_MENU] = jl_ct_key_menu;
#elif JL_PLAT == JL_PLAT_PHONE
	jl_gr->ct.getEvents[JL_CT_ANDR_TCCR] = tuch_cntr;
	jl_gr->ct.getEvents[JL_CT_ANDR_TFUP] = tuch_frup;
	jl_gr->ct.getEvents[JL_CT_ANDR_TFDN] = tuch_frdn;
	jl_gr->ct.getEvents[JL_CT_ANDR_TFRT] = tuch_frrt;
	jl_gr->ct.getEvents[JL_CT_ANDR_TFLT] = tuch_frlt;
	jl_gr->ct.getEvents[JL_CT_ANDR_TNUP] = tuch_nrup;
	jl_gr->ct.getEvents[JL_CT_ANDR_TNDN] = tuch_nrdn;
	jl_gr->ct.getEvents[JL_CT_ANDR_TNRT] = tuch_nrrt;
	jl_gr->ct.getEvents[JL_CT_ANDR_TNLT] = tuch_nrlt;
	jl_gr->ct.getEvents[JL_CT_ANDR_TOUC] = tuch;
	jl_gr->ct.getEvents[JL_CT_ANDR_MENU] = jl_ct_key_menu;
#endif
}

static inline void _jl_ct_var_init(jl_gr_t* jl_gr) {
	int i;
	for(i = 0; i < 255; i++)
		jl_gr->ct.keyDown[i] = 0;
	jl_gr->ct.read_cursor = 0;
	for(i = 0; i < 32; i++)
		jl_gr->ct.text_input[i] = 0;
	jl_gr->ct.sc = 0;
}

void jl_ct_init__(jl_gr_t* jl_gr) {
	jvct_t* _jlc = jl_gr->jlc->_jlc;
	_jl_ct_fn_init(jl_gr);
	_jl_ct_var_init(jl_gr);
	_jlc->has.input = 1;
}

/**
 * Get any keys that are currently being typed.  Output in Aski.
 * If phone, pops up keyboard if not already up.  If nothing is being typed,
 * returns 0.
*/
uint8_t jl_ct_typing_get(jl_gr_t *jl_gr) {
	if(!SDL_IsTextInputActive()) SDL_StartTextInput();
	uint8_t rtn = jl_gr->ct.text_input[jl_gr->ct.read_cursor];
	if(jl_ct_key_pressed(jl_gr, SDL_SCANCODE_BACKSPACE) == 1) return '\b';
	if(jl_ct_key_pressed(jl_gr, SDL_SCANCODE_DELETE) == 1) return '\02';
	if(jl_ct_key_pressed(jl_gr, SDL_SCANCODE_RETURN) == 1) return '\n';
	if(!rtn) return 0;
	jl_gr->ct.read_cursor++;
	return rtn;
}

/**
 * If phone, hides keyboard.
*/
void jl_ct_typing_disable(void) {
	SDL_StopTextInput();
}


/**
 * Returns 0 if key isn't pressed
 * Returns 1 if key is just pressed
 * Returns 2 if key is held down
 * Returns 3 if key is released
*/
uint8_t jl_ct_key_pressed(jl_gr_t *jl_gr, uint8_t key) {
#if JL_PLAT == JL_PLAT_COMPUTER
	return _jl_ct_state(&jl_gr->ct.keyDown[key], jl_gr->ct.keys[key]);
#else
	return 0; //Unimplemented
#endif
}
