/*
 * (c) Jeron A. Lau
 *
 * jl/ct is a library for making input controllers compatible between different
 * devices.
*/

#include "header/jl_pr.h"

//Prototypes
	// ct.c
	uint8_t jl_ct_key_pressed(jl_t *jlc, uint8_t key);
	// main.c
	void main_resz(jvct_t* _jlc);

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
	jvct_t* _jlc = jlc->_jlc;
	jlc->ctrl.h = _jlc->ct.heldDown;
	jlc->ctrl.r = 0.;
	jlc->ctrl.x = jl_ct_gmousex(jlc);
	jlc->ctrl.y = jl_ct_gmousey(jlc);
}

static void _jl_ct_not_down(jl_t* jlc) {
	jlc->ctrl.p = 0.;
	jlc->ctrl.k = 0;
}

static void _jl_ct_is_down(jl_t* jlc) {
	jlc->ctrl.k = 1;
	jlc->ctrl.p = 1.;
}

#if JL_PLAT == JL_PLAT_COMPUTER

	void jl_ct_key(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno, uint8_t key)
	{
		uint8_t a = jl_ct_key_pressed(jlc, key);
		jlc->ctrl.h = a;
		jlc->ctrl.r = 0.;
		jlc->ctrl.x = 0.;
		jlc->ctrl.y = 0.;
		if(a && (a!=3)) {
			jlc->ctrl.p = 1.;
			jlc->ctrl.k = key;
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc);
			pno(jlc);
		}
	}

	void jl_ct_key_retn(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(jlc, prun, pno, SDL_SCANCODE_D);
	}
	
	void jl_ct_left_click(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown &&
			((_jlc->ct.msx <= .9) || (_jlc->ct.msy >= .1)))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
	
//	void mous_over(jl_simple_fnt prun) {
//		prun(255, 0);
//	}
#elif JL_PLAT == JL_PLAT_PHONE //if Android
	void tuch_cntr(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //touch center
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (((_jlc->ct.msy>.4f * jl_gl_ar(jlc)) &&
			(_jlc->ct.msy<.6f * jl_gl_ar(jlc))) &&
			((_jlc->ct.msx>.4f) && (_jlc->ct.msx<.6f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
	//
	void tuch_nrrt(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near right
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (((_jlc->ct.msx>.6f) && (_jlc->ct.msx<.8f)) &&
			((_jlc->ct.msy * jl_gl_ar(jlc)>.2f) &&
			(_jlc->ct.msy * jl_gl_ar(jlc)<.8f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_nrlt(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near left
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (((_jlc->ct.msx<.4f) && (_jlc->ct.msx>.2f)) &&
			((_jlc->ct.msy>.2f * jl_gl_ar(jlc)) &&
			(_jlc->ct.msy<.8f * jl_gl_ar(jlc)))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_nrup(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near up
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (((_jlc->ct.msy<.4f * jl_gl_ar(jlc)) &&
			(_jlc->ct.msy>.2f * jl_gl_ar(jlc))) &&
			((_jlc->ct.msx>.2f) && (_jlc->ct.msx<.8f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_nrdn(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near down
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (((_jlc->ct.msy>.6f * jl_gl_ar(jlc)) &&
			(_jlc->ct.msy<.8f * jl_gl_ar(jlc))) &&
			((_jlc->ct.msx>.2f) && (_jlc->ct.msx<.8f))))
		{
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
	//

	void tuch_frrt(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far right
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (_jlc->ct.msx>.8f)) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_frlt(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far left
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (_jlc->ct.msx<.2f)) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_frup(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far up
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (_jlc->ct.msy<.2f * jl_gl_ar(jlc))) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch_frdn(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far down
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown && (_jlc->ct.msy>.8f * jl_gl_ar(jlc))) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}

	void tuch(jl_t *jlc, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//Any touch
		jvct_t* _jlc = jlc->_jlc;
		_jl_ct_press(jlc); //hrxy
		if(_jlc->ct.heldDown) {
			_jl_ct_is_down(jlc); //pk
			prun(jlc);
		}else{
			_jl_ct_not_down(jlc); //pk
			pno(jlc);
		}
	}
#endif

void jl_ct_key_menu(jl_t *jlc, jl_simple_fnt prun,
	jl_simple_fnt pno)
{
	#if JL_PLAT == JL_PLAT_COMPUTER
	jl_ct_key(jlc, prun, pno, SDL_SCANCODE_APPLICATION); //xyrhpk
	#elif JL_PLAT == JL_PLAT_PHONE
	jvct_t* _jlc = jlc->_jlc;
	jlc->ctrl.x = 0.;
	jlc->ctrl.y = 0.;
	jlc->ctrl.r = 0.;
	jlc->ctrl.h = _jlc->ct.menu;
	if(_jlc->ct.menu) {
		jlc->ctrl.p = 1. * (_jlc->ct.menu != 3);
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

float jl_ct_gmousex(jl_t *jlc) {
	jvct_t* _jlc = jlc->_jlc;
	return _jlc->ct.msx;
}

float jl_ct_gmousey(jl_t *jlc) {
	jvct_t* _jlc = jlc->_jlc;
	return _jlc->ct.msy;
}

static inline void _jl_ct_handle_events_platform_dependant(jvct_t *_jlc) {
#if JL_PLAT == JL_PLAT_PHONE
	if( _jlc->ct.event.type==SDL_FINGERDOWN ) {
		_jlc->ct.msx = _jlc->ct.event.tfinger.x;
		_jlc->ct.input.finger = 1;
		_jlc->ct.msy = _jlc->ct.event.tfinger.y * jl_gl_ar(_jlc->jlc);
		if(_jlc->jlc->smde) {
			_jlc->ct.msy = _jlc->ct.msy * 2.;
			_jlc->ct.msy -= jl_gl_ar(_jlc->jlc);
			if(_jlc->ct.msy < 0.) _jlc->ct.input.finger = 0;
		}

	}else if( _jlc->ct.event.type==SDL_FINGERUP ) {
		_jlc->ct.input.finger = 0;
	}else if( _jlc->ct.event.type==SDL_KEYDOWN || _jlc->ct.event.type==SDL_KEYUP) {
		if( _jlc->ct.event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
			_jlc->ct.input.back = (_jlc->ct.event.type==SDL_KEYDOWN); //Back Key
		else
			_jlc->ct.input.menu = (_jlc->ct.event.type==SDL_KEYDOWN); //Menu Key
	}
#elif JL_PLAT == JL_PLAT_COMPUTER
	uint8_t isNowDown = _jlc->ct.event.type == SDL_MOUSEBUTTONDOWN;
	uint8_t isNowUp = _jlc->ct.event.type == SDL_MOUSEBUTTONUP;
	if( isNowDown || isNowUp) {
		if(_jlc->ct.event.button.button == SDL_BUTTON_LEFT)
			_jlc->ct.input.click_left = isNowDown;
		else if(_jlc->ct.event.button.button == SDL_BUTTON_RIGHT)
			_jlc->ct.input.click_right = isNowDown;
		else if(_jlc->ct.event.button.button == SDL_BUTTON_MIDDLE)
			_jlc->ct.input.click_middle = isNowDown;
	}else if(_jlc->ct.event.wheel.type == SDL_MOUSEWHEEL) {
		//SDL 2.0.4 NYI
		uint8_t flip = 1;
			/*(direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1*/;
		int32_t x = flip * _jlc->ct.event.wheel.x;
		int32_t y = flip * _jlc->ct.event.wheel.y;
		if(_jlc->ct.event.wheel.y > 0)
			_jlc->ct.input.scroll_up = (y > 0) ? y : -y;
		else if(_jlc->ct.event.wheel.y < 0)
			_jlc->ct.input.scroll_down = (y > 0) ? y : -y;
		if(_jlc->ct.event.wheel.x > 0)
			_jlc->ct.input.scroll_right = (x > 0) ? x : -x;
		else if(_jlc->ct.event.wheel.x < 0)
			_jlc->ct.input.scroll_left = (x > 0) ? x : -x;
	}

#endif
}

static inline void _jl_ct_handle_events(jvct_t *_jlc) {
	if ( _jlc->ct.event.type == SDL_TEXTINPUT) {
//		printf("%1s\n", &(_jlc->ct.event.text.text[0]));
		int i;
		for(i = 0; i < 32; i++)
			_jlc->ct.text_input[i] = _jlc->ct.event.text.text[i];
		_jlc->ct.read_cursor = 0;
	}else if(_jlc->ct.event.type==SDL_WINDOWEVENT) { //Resize
		if(_jlc->ct.event.window.event == SDL_WINDOWEVENT_RESIZED)
			main_resz(_jlc);
	}
	_jl_ct_handle_events_platform_dependant(_jlc);
}

static inline void _jl_ct_process_events(jvct_t *_jlc) {
#if JL_PLAT == JL_PLAT_PHONE
	_jl_ct_state(&_jlc->ct.heldDown, _jlc->ct.input.finger);
	_jl_ct_state(&_jlc->ct.menu, _jlc->ct.input.menu);
	_jl_ct_state(&_jlc->ct.back, _jlc->ct.input.back);
#elif JL_PLAT == JL_PLAT_COMPUTER
	_jl_ct_state(&_jlc->ct.heldDown, _jlc->ct.input.click_left);
#endif
}

static void _jl_ct_run_event(jvct_t * _jlc, uint8_t pevent,
	jl_simple_fnt prun, jl_simple_fnt pno)
{
	if(_jlc->ct.getEvents[pevent] == NULL) {
		_jl_fl_errf(_jlc, ":[INPT/ERR] Null Pointer: _jlc\
				->ct.getEvents.Event:\n:");
		_jl_fl_errf(_jlc, (void*)jl_me_strt_fnum(pevent)->data);
		jl_sg_kill(_jlc->jlc, "\n");
	}
	_jlc->ct.getEvents[pevent](_jlc->jlc, prun, pno);
}

void jl_ct_run_event(jl_t *jlc, uint8_t pevent,
	jl_simple_fnt prun, jl_simple_fnt pno)
{
	_jl_ct_run_event(jlc->_jlc, pevent, prun, pno);
}

//Main Input Loop
void _jl_ct_loop(jvct_t* _jlc) {
	//Get the information on current events
	while(SDL_PollEvent(&_jlc->ct.event)) _jl_ct_handle_events(_jlc);
	_jl_ct_process_events(_jlc);
	#if JL_PLAT == JL_PLAT_COMPUTER
		_jlc->ct.keys = SDL_GetKeyboardState(NULL);
		//Get Whether mouse is down or not and xy coordinates
		SDL_GetMouseState(&_jlc->ct.msxi,&_jlc->ct.msyi);
		if(_jlc->ct.msxi < _jlc->dl.window.x ||
			_jlc->ct.msxi > _jlc->dl.window.x + _jlc->dl.window.w ||
			_jlc->ct.msyi < _jlc->dl.window.y ||
			_jlc->ct.msyi > _jlc->dl.window.y + _jlc->dl.window.h)
		{
			if(!_jlc->ct.sc) {
				SDL_ShowCursor(SDL_ENABLE);
				_jlc->ct.sc = 1;
			}
		}else{
			if(_jlc->ct.sc) {
				SDL_ShowCursor(SDL_DISABLE);
				_jlc->ct.sc = 0;
			}
		}
		int32_t mousex = _jlc->ct.msxi - _jlc->dl.window.x;
		int32_t mousey = (_jlc->ct.msyi - _jlc->dl.window.y) *
			(1 + _jlc->jlc->smde);
		//translate integer into float by clipping [0-1]
		_jlc->ct.msx =
			((float)(mousex-2)) / jl_dl_getw(_jlc->jlc);
		_jlc->ct.msy =
			((float)mousey) /
			(jl_dl_geth(_jlc->jlc) * (1 + _jlc->jlc->smde));
		if(_jlc->jlc->smde && _jlc->ct.msy < 0.) {
			_jlc->ct.msy = 0.;
			_jlc->ct.heldDown = 0;
		}
		//If Escape key is pressed, then quit the program
		_jlc->ct.back = jl_ct_key_pressed(_jlc->jlc, SDL_SCANCODE_ESCAPE);
		if(jl_ct_key_pressed(_jlc->jlc, SDL_SCANCODE_F11) == 1)
			jl_dl_togglefullscreen(_jlc->jlc);
	#endif
	if(_jlc->ct.back == 1) jl_sg_exit(_jlc->jlc); // Back Button/Key
}

static inline void _jl_ct_fn_init(jvct_t* _jlc) {
#if JL_PLAT == JL_PLAT_COMPUTER
	_jlc->ct.getEvents[JL_CT_COMP_RETN] = jl_ct_key_retn;
	_jlc->ct.getEvents[JL_CT_COMP_KEYW] = jl_ct_key_keyw;
	_jlc->ct.getEvents[JL_CT_COMP_KEYA] = jl_ct_key_keya;
	_jlc->ct.getEvents[JL_CT_COMP_KEYS] = jl_ct_key_keys;
	_jlc->ct.getEvents[JL_CT_COMP_KEYD] = jl_ct_key_keyd;
	_jlc->ct.getEvents[JL_CT_COMP_ARUP] = jl_ct_key_arup;
	_jlc->ct.getEvents[JL_CT_COMP_ARDN] = jl_ct_key_ardn;
	_jlc->ct.getEvents[JL_CT_COMP_ARRT] = jl_ct_key_arrt;
	_jlc->ct.getEvents[JL_CT_COMP_ARLT] = jl_ct_key_arlt;
//	_jlc->ct.getEvents[JL_CT_COMP_MSXY] = mous_over;
	_jlc->ct.getEvents[JL_CT_COMP_CLLT] = jl_ct_left_click;
	_jlc->ct.getEvents[JL_CT_COMP_MENU] = jl_ct_key_menu;
#elif JL_PLAT == JL_PLAT_PHONE
	_jlc->ct.getEvents[JL_CT_ANDR_TCCR] = tuch_cntr;
	_jlc->ct.getEvents[JL_CT_ANDR_TFUP] = tuch_frup;
	_jlc->ct.getEvents[JL_CT_ANDR_TFDN] = tuch_frdn;
	_jlc->ct.getEvents[JL_CT_ANDR_TFRT] = tuch_frrt;
	_jlc->ct.getEvents[JL_CT_ANDR_TFLT] = tuch_frlt;
	_jlc->ct.getEvents[JL_CT_ANDR_TNUP] = tuch_nrup;
	_jlc->ct.getEvents[JL_CT_ANDR_TNDN] = tuch_nrdn;
	_jlc->ct.getEvents[JL_CT_ANDR_TNRT] = tuch_nrrt;
	_jlc->ct.getEvents[JL_CT_ANDR_TNLT] = tuch_nrlt;
	_jlc->ct.getEvents[JL_CT_ANDR_TOUC] = tuch;
	_jlc->ct.getEvents[JL_CT_ANDR_MENU] = jl_ct_key_menu;
#endif
}

static inline void _jl_ct_var_init(jvct_t* _jlc) {
	int i;
	for(i = 0; i < 255; i++)
		_jlc->ct.keyDown[i] = 0;
	_jlc->ct.read_cursor = 0;
	for(i = 0; i < 32; i++)
		_jlc->ct.text_input[i] = 0;
	_jlc->ct.sc = 0;
}

void _jl_ct_init(jvct_t* _jlc) {
	_jl_ct_fn_init(_jlc);
	_jl_ct_var_init(_jlc);
}

/**
 * Get any keys that are currently being typed.  Output in Aski.
 * If phone, pops up keyboard if not already up.  If nothing is being typed,
 * returns 0.
*/
uint8_t jl_ct_typing_get(jl_t *jlc) {
	jvct_t* _jlc = jlc->_jlc;

	if(!SDL_IsTextInputActive()) SDL_StartTextInput();
	uint8_t rtn = _jlc->ct.text_input[_jlc->ct.read_cursor];
	if(jl_ct_key_pressed(jlc, SDL_SCANCODE_BACKSPACE) == 1) return '\b';
	if(jl_ct_key_pressed(jlc, SDL_SCANCODE_DELETE) == 1) return '\02';
	if(jl_ct_key_pressed(jlc, SDL_SCANCODE_RETURN) == 1) return '\n';
	if(!rtn) return 0;
	_jlc->ct.read_cursor++;
	return rtn;
}

/**
 * If phone, hides keyboard.
*/
void jl_ct_typing_disable(jl_t *jlc) {
	SDL_StopTextInput();
}


/**
 * Returns 0 if key isn't pressed
 * Returns 1 if key is just pressed
 * Returns 2 if key is held down
 * Returns 3 if key is released
*/
uint8_t jl_ct_key_pressed(jl_t *jlc, uint8_t key) {
#if JL_PLAT == JL_PLAT_COMPUTER
	jvct_t* _jlc = jlc->_jlc;
	return _jl_ct_state(&_jlc->ct.keyDown[key], _jlc->ct.keys[key]);
#else
	return 0; //Unimplemented
#endif
}
