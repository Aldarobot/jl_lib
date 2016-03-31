/*
 * (c) Jeron A. Lau
 *
 * jl/input is a library for making input controllers compatible between
 * different
 * devices.
*/

#include "JLGRinternal.h"

//Prototypes
	// ct.c
	uint8_t jl_ct_key_pressed(jlgr_t *jlgr, uint8_t key);

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

static void _jl_ct_press(jl_t *jl) {
	jlgr_t* jlgr = jl->jlgr;
	jl->ctrl.h = jlgr->main.ct.heldDown;
	jl->ctrl.r = 0.;
	jl->ctrl.x = jl_ct_gmousex(jlgr);
	jl->ctrl.y = jl_ct_gmousey(jlgr);
}

static void _jl_ct_not_down(jlgr_t *jlgr) {
	jlgr->jl->ctrl.p = 0.;
	jlgr->jl->ctrl.k = 0;
}

static void _jl_ct_is_down(jlgr_t *jlgr) {
	jlgr->jl->ctrl.k = 1;
	jlgr->jl->ctrl.p = 1.;
}

#if JL_PLAT == JL_PLAT_COMPUTER

	void jl_ct_key(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno, uint8_t key)
	{
		uint8_t a = jl_ct_key_pressed(jlgr, key);
		jl_t* jl = jlgr->jl;
		jl->ctrl.h = a;
		jl->ctrl.r = 0.;
		jl->ctrl.x = 0.;
		jl->ctrl.y = 0.;
		if(a && (a!=3)) {
			jl->ctrl.p = 1.;
			jl->ctrl.k = key;
			prun(jl);
		}else{
			_jl_ct_not_down(jlgr);
			pno(jl);
		}
	}

	void jl_ct_key_retn(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_D);
	}
	
	void jl_ct_left_click(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {
		_jl_ct_press(jlgr->jl); //hrxy
		if(jlgr->main.ct.heldDown) {
			_jl_ct_is_down(jlgr); //pk
			prun(jlgr->jl);
		}else{
			_jl_ct_not_down(jlgr); //pk
			pno(jlgr->jl);
		}
	}
	
//	void mous_over(jl_fnct prun) {
//		prun(255, 0);
//	}
#elif JL_PLAT == JL_PLAT_PHONE //if Android
	void tuch_cntr(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) { //touch center
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown && (((jlgr->main.ct.msy>.4f * jl_gl_ar(jl)) &&
			(jlgr->main.ct.msy<.6f * jl_gl_ar(jl))) &&
			((jlgr->main.ct.msx>.4f) && (jlgr->main.ct.msx<.6f))))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}
	//
	void tuch_nrrt(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) { //near right
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown && (((jlgr->main.ct.msx>.6f) && (jlgr->main.ct.msx<.8f)) &&
			((jlgr->main.ct.msy * jl_gl_ar(jl)>.2f) &&
			(jlgr->main.ct.msy * jl_gl_ar(jl)<.8f))))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch_nrlt(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) { //near left
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown && (((jlgr->main.ct.msx<.4f) && (jlgr->main.ct.msx>.2f)) &&
			((jlgr->main.ct.msy>.2f * jl_gl_ar(jl)) &&
			(jlgr->main.ct.msy<.8f * jl_gl_ar(jl)))))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch_nrup(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) { //near up
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown && (((jlgr->main.ct.msy<.4f * jl_gl_ar(jl)) &&
			(jlgr->main.ct.msy>.2f * jl_gl_ar(jl))) &&
			((jlgr->main.ct.msx>.2f) && (jlgr->main.ct.msx<.8f))))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch_nrdn(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) { //near down
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown &&
			(((jlgr->main.ct.msy>.6f * jl_gl_ar(jl)) &&
			(jlgr->main.ct.msy<.8f * jl_gl_ar(jl))) &&
			((jlgr->main.ct.msx>.2f) &&
			(jlgr->main.ct.msx<.8f))))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}
	//

	void tuch_frrt(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {//far right
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown &&
			(jlgr->main.ct.msx>.8f))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch_frlt(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {//far left
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown &&
			(jlgr->main.ct.msx<.2f))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch_frup(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {//far up
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown &&
			(jlgr->main.ct.msy<.2f * jl_gl_ar(jl)))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch_frdn(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {//far down
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown &&
			(jlgr->main.ct.msy>.8f * jl_gl_ar(jl)))
		{
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}

	void tuch(jlgr_t *jlgr, jl_fnct prun, jl_fnct pno) {//Any touch
		jlgr_t* jlgr = jl->jlgr;
		_jl_ct_press(jl); //hrxy
		if(jlgr->main.ct.heldDown) {
			_jl_ct_is_down(jl); //pk
			prun(jl);
		}else{
			_jl_ct_not_down(jl); //pk
			pno(jl);
		}
	}
#endif

void jl_ct_key_menu(jlgr_t *jlgr, jl_fnct prun,
	jl_fnct pno)
{
	#if JL_PLAT == JL_PLAT_COMPUTER
	jl_ct_key(jlgr, prun, pno, SDL_SCANCODE_APPLICATION); //xyrhpk
	#elif JL_PLAT == JL_PLAT_PHONE
	jlgr_t* jlgr = jl->jlgr;
	jl->ctrl.x = 0.;
	jl->ctrl.y = 0.;
	jl->ctrl.r = 0.;
	jl->ctrl.h = jlgr->main.ct.menu;
	if(jlgr->main.ct.menu) {
		jl->ctrl.p = 1. * (jlgr->main.ct.menu != 3);
		jl->ctrl.k = SDL_SCANCODE_APPLICATION;
		prun(jl);
	}else{
		jl->ctrl.p = 0.;
		jl->ctrl.k = 0;
		pno(jl);
	}
	#endif
}

void jl_ct_txty(void) {
	SDL_StartTextInput();
}

void jl_ct_txtn(void) {
	SDL_StopTextInput();
}

float jl_ct_gmousex(jlgr_t* jlgr) {
	return jlgr->main.ct.msx;
}

float jl_ct_gmousey(jlgr_t* jlgr) {
	return jlgr->main.ct.msy;
}

static inline void _jl_ct_handle_events_platform_dependant(jlgr_t* jlgr) {
#if JL_PLAT == JL_PLAT_PHONE
	if( jlgr->main.ct.event.type==SDL_FINGERDOWN ) {
		jlgr->main.ct.msx = jlgr->main.ct.event.tfinger.x;
		jlgr->main.ct.input.finger = 1;
		jlgr->main.ct.msy = jlgr->main.ct.event.tfinger.y * jl_gl_ar(jlgr->jl);
		if(jlgr->jl->smde) {
			jlgr->main.ct.msy = jlgr->main.ct.msy * 2.;
			jlgr->main.ct.msy -= jl_gl_ar(jlgr->jl);
			if(jlgr->main.ct.msy < 0.) jlgr->main.ct.input.finger = 0;
		}

	}else if( jlgr->main.ct.event.type==SDL_FINGERUP ) {
		jlgr->main.ct.input.finger = 0;
	}else if( jlgr->main.ct.event.type==SDL_KEYDOWN || jlgr->main.ct.event.type==SDL_KEYUP) {
		if( jlgr->main.ct.event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
			jlgr->main.ct.input.back = (jlgr->main.ct.event.type==SDL_KEYDOWN); //Back Key
		else
			jlgr->main.ct.input.menu = (jlgr->main.ct.event.type==SDL_KEYDOWN); //Menu Key
	}
#elif JL_PLAT == JL_PLAT_COMPUTER
	uint8_t isNowDown = jlgr->main.ct.event.type == SDL_MOUSEBUTTONDOWN;
	uint8_t isNowUp = jlgr->main.ct.event.type == SDL_MOUSEBUTTONUP;
	if( isNowDown || isNowUp) {
		if(jlgr->main.ct.event.button.button == SDL_BUTTON_LEFT)
			jlgr->main.ct.input.click_left = isNowDown;
		else if(jlgr->main.ct.event.button.button == SDL_BUTTON_RIGHT)
			jlgr->main.ct.input.click_right = isNowDown;
		else if(jlgr->main.ct.event.button.button == SDL_BUTTON_MIDDLE)
			jlgr->main.ct.input.click_middle = isNowDown;
	}else if(jlgr->main.ct.event.wheel.type == SDL_MOUSEWHEEL) {
		//SDL 2.0.4 NYI
		uint8_t flip = 1;
			/*(direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1*/;
		int32_t x = flip * jlgr->main.ct.event.wheel.x;
		int32_t y = flip * jlgr->main.ct.event.wheel.y;
		if(jlgr->main.ct.event.wheel.y > 0)
			jlgr->main.ct.input.scroll_up = (y > 0) ? y : -y;
		else if(jlgr->main.ct.event.wheel.y < 0)
			jlgr->main.ct.input.scroll_down = (y > 0) ? y : -y;
		if(jlgr->main.ct.event.wheel.x > 0)
			jlgr->main.ct.input.scroll_right = (x > 0) ? x : -x;
		else if(jlgr->main.ct.event.wheel.x < 0)
			jlgr->main.ct.input.scroll_left = (x > 0) ? x : -x;
	}

#endif
}

static void jl_ct_handle_resize__(jlgr_t* jlgr) {
	if(jlgr->main.ct.event.type==SDL_WINDOWEVENT) { //Resize
		if((jlgr->main.ct.event.window.event ==
				SDL_WINDOWEVENT_RESIZED) &&
			(SDL_GetWindowFromID(
				jlgr->main.ct.event.window.windowID) ==
					jlgr->dl.displayWindow->w))
		{
			jlgr_resz(jlgr,
				jlgr->main.ct.event.window.data1,
				jlgr->main.ct.event.window.data2);
		}
	}
}

static inline void _jl_ct_handle_events(jlgr_t* jlgr) {
	if ( jlgr->main.ct.event.type == SDL_TEXTINPUT) {
//		printf("%1s\n", &(jlgr->main.ct.event.text.text[0]));
		int i;
		for(i = 0; i < 32; i++)
			jlgr->main.ct.text_input[i] =
				jlgr->main.ct.event.text.text[i];
		jlgr->main.ct.read_cursor = 0;
	}else{
		jl_ct_handle_resize__(jlgr);
	}
	_jl_ct_handle_events_platform_dependant(jlgr);
}

static inline void _jl_ct_process_events(jlgr_t* jlgr) {
#if JL_PLAT == JL_PLAT_PHONE
	_jl_ct_state(&jlgr->main.ct.heldDown, jlgr->main.ct.input.finger);
	_jl_ct_state(&jlgr->main.ct.menu, jlgr->main.ct.input.menu);
#elif JL_PLAT == JL_PLAT_COMPUTER
	_jl_ct_state(&jlgr->main.ct.heldDown, jlgr->main.ct.input.click_left);
#endif
}

static void jl_ct_testquit__(jlgr_t* jlgr) {
	if(jlgr->main.ct.back == 1) jl_mode_exit(jlgr->jl); // Back Button/Key
}

/**
 * Run prun if event pevent is active.
 * @param jlgr: The library context.
 * @param pevent: The event id
 * @param prun: The function to run when event is active.
 * @param pno: The function to run when event is inactive.
*/
void jl_ct_run_event(jlgr_t *jlgr, uint8_t pevent, jl_fnct prun, jl_fnct pno){
	jl_ct_event_fnct FunctionToRun_ = jlgr->main.ct.getEvents[pevent];

	if(jlgr->main.ct.getEvents[pevent] == NULL) {
		jl_print(jlgr->jl,"Null Pointer: jlgr->main.ct.getEvents.Event");
		jl_print(jlgr->jl,"pevent=%d", pevent);
		jl_sg_kill(jlgr->jl);
	}
	FunctionToRun_(jlgr, prun, pno);
}

void jl_ct_getevents_(jlgr_t* jlgr) {
	while(SDL_PollEvent(&jlgr->main.ct.event)) _jl_ct_handle_events(jlgr);
	//If Back key is pressed, then quit the program
#if JL_PLAT == JL_PLAT_COMPUTER
	jlgr->main.ct.keys = SDL_GetKeyboardState(NULL);
	jlgr->main.ct.back = jl_ct_key_pressed(jlgr, SDL_SCANCODE_ESCAPE);
#elif JL_PLAT == JL_PLAT_PHONE
	_jl_ct_state(&jlgr->main.ct.back, jlgr->main.ct.input.back);
#endif
}

void jl_ct_quickloop_(jlgr_t* jlgr) {
	jvct_t* _jl = jlgr->jl->_jl;

	jl_print_function(jlgr->jl, "INPUT_QUICKLOOP");
	_jl->has.quickloop = 1;
	if(_jl->has.input) {
		jl_ct_getevents_(jlgr);
		if(jlgr->main.ct.back == 1) jl_mode_exit(jlgr->jl);
	}else{
		while(SDL_PollEvent(&jlgr->main.ct.event))
			jl_ct_handle_resize__(jlgr);
	}
	jl_print_return(jlgr->jl, "INPUT_QUICKLOOP");
}

//Main Input Loop
void jl_ct_loop__(jlgr_t* jlgr) {
	jvct_t* _jl = jlgr->jl->_jl;

	_jl->has.quickloop = 0;
	//Get the information on current events
	jl_ct_getevents_(jlgr);
	_jl_ct_process_events(jlgr);
	#if JL_PLAT == JL_PLAT_COMPUTER
		//Get Whether mouse is down or not and xy coordinates
		SDL_GetMouseState(&jlgr->main.ct.msxi,&jlgr->main.ct.msyi);
		if(jlgr->main.ct.msxi < jlgr->dl.window.x ||
			jlgr->main.ct.msxi > jlgr->dl.window.x + jlgr->dl.window.w ||
			jlgr->main.ct.msyi < jlgr->dl.window.y ||
			jlgr->main.ct.msyi > jlgr->dl.window.y + jlgr->dl.window.h)
		{
			if(!jlgr->main.ct.sc) {
				SDL_ShowCursor(SDL_ENABLE);
				jlgr->main.ct.sc = 1;
			}
		}else{
			if(jlgr->main.ct.sc) {
				SDL_ShowCursor(SDL_DISABLE);
				jlgr->main.ct.sc = 0;
			}
		}
		int32_t mousex = jlgr->main.ct.msxi - jlgr->dl.window.x;
		int32_t mousey = (jlgr->main.ct.msyi - jlgr->dl.window.y) *
			(1 + jlgr->jl->smde);
		//translate integer into float by clipping [0-1]
		jlgr->main.ct.msx =
			((float)(mousex-2)) / jlgr_wm_getw(jlgr);
		jlgr->main.ct.msy =
			((float)mousey) /
			(jlgr_wm_geth(jlgr) * (1 + jlgr->jl->smde));
		if(jlgr->jl->smde && jlgr->main.ct.msy < 0.) {
			jlgr->main.ct.msy = 0.;
			jlgr->main.ct.heldDown = 0;
		}
		if(jl_ct_key_pressed(jlgr, SDL_SCANCODE_F11) == 1)
			jlgr_wm_togglefullscreen(jlgr);
	#endif
	jl_ct_testquit__(jlgr);
}

static inline void _jl_ct_fn_init(jlgr_t* jlgr) {
#if JL_PLAT == JL_PLAT_COMPUTER
	jlgr->main.ct.getEvents[JL_CT_COMP_RETN] = jl_ct_key_retn;
	jlgr->main.ct.getEvents[JL_CT_COMP_KEYW] = jl_ct_key_keyw;
	jlgr->main.ct.getEvents[JL_CT_COMP_KEYA] = jl_ct_key_keya;
	jlgr->main.ct.getEvents[JL_CT_COMP_KEYS] = jl_ct_key_keys;
	jlgr->main.ct.getEvents[JL_CT_COMP_KEYD] = jl_ct_key_keyd;
	jlgr->main.ct.getEvents[JL_CT_COMP_ARUP] = jl_ct_key_arup;
	jlgr->main.ct.getEvents[JL_CT_COMP_ARDN] = jl_ct_key_ardn;
	jlgr->main.ct.getEvents[JL_CT_COMP_ARRT] = jl_ct_key_arrt;
	jlgr->main.ct.getEvents[JL_CT_COMP_ARLT] = jl_ct_key_arlt;
//	jlgr->main.ct.getEvents[JL_CT_COMP_MSXY] = mous_over;
	jlgr->main.ct.getEvents[JL_CT_COMP_CLLT] = jl_ct_left_click;
	jlgr->main.ct.getEvents[JL_CT_COMP_MENU] = jl_ct_key_menu;
#elif JL_PLAT == JL_PLAT_PHONE
	jlgr->main.ct.getEvents[JL_CT_ANDR_TCCR] = tuch_cntr;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TFUP] = tuch_frup;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TFDN] = tuch_frdn;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TFRT] = tuch_frrt;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TFLT] = tuch_frlt;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TNUP] = tuch_nrup;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TNDN] = tuch_nrdn;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TNRT] = tuch_nrrt;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TNLT] = tuch_nrlt;
	jlgr->main.ct.getEvents[JL_CT_ANDR_TOUC] = tuch;
	jlgr->main.ct.getEvents[JL_CT_ANDR_MENU] = jl_ct_key_menu;
#endif
}

static inline void _jl_ct_var_init(jlgr_t* jlgr) {
	int i;
	for(i = 0; i < 255; i++)
		jlgr->main.ct.keyDown[i] = 0;
	jlgr->main.ct.read_cursor = 0;
	for(i = 0; i < 32; i++)
		jlgr->main.ct.text_input[i] = 0;
	jlgr->main.ct.sc = 0;
}

void jl_ct_init__(jlgr_t* jlgr) {
	jvct_t* _jl = jlgr->jl->_jl;
	_jl_ct_fn_init(jlgr);
	_jl_ct_var_init(jlgr);
	_jl->has.input = 1;
}

/**
 * Get any keys that are currently being typed.  Output in Aski.
 * If phone, pops up keyboard if not already up.  If nothing is being typed,
 * returns 0.
*/
uint8_t jl_ct_typing_get(jlgr_t *jlgr) {
	if(!SDL_IsTextInputActive()) SDL_StartTextInput();
	uint8_t rtn = jlgr->main.ct.text_input[jlgr->main.ct.read_cursor];
	if(jl_ct_key_pressed(jlgr, SDL_SCANCODE_BACKSPACE) == 1) return '\b';
	if(jl_ct_key_pressed(jlgr, SDL_SCANCODE_DELETE) == 1) return '\02';
	if(jl_ct_key_pressed(jlgr, SDL_SCANCODE_RETURN) == 1) return '\n';
	if(!rtn) return 0;
	jlgr->main.ct.read_cursor++;
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
uint8_t jl_ct_key_pressed(jlgr_t *jlgr, uint8_t key) {
#if JL_PLAT == JL_PLAT_COMPUTER
	return _jl_ct_state(&jlgr->main.ct.keyDown[key], jlgr->main.ct.keys[key]);
#else
	return 0; //Unimplemented
#endif
}
