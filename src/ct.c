/*
 * (c) Jeron A. Lau
 *
 * jl/ct is a library for making input controllers compatible between different
 * devices.
*/

#include "header/jl_pr.h"

uint8_t jl_ct_key_pressed(jl_t *pusr, uint8_t key);

static void _jl_ct_press(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	pusr->ctrl.h = pjlc->ct.heldDown;
	pusr->ctrl.r = 0.;
	pusr->ctrl.x = jl_ct_gmousex(pusr);
	pusr->ctrl.y = jl_ct_gmousey(pusr);
}

static void _jl_ct_not_down(jl_t* pusr) {
	pusr->ctrl.p = 0.;
	pusr->ctrl.k = 0;
}

static void _jl_ct_is_down(jl_t* pusr) {
	pusr->ctrl.k = 1;
	pusr->ctrl.p = 1.;
}

#if JL_PLAT == JL_PLAT_COMPUTER

	void jl_ct_key(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno, uint8_t key)
	{
		uint8_t a = jl_ct_key_pressed(pusr, key);
		pusr->ctrl.h = a;
		pusr->ctrl.r = 0.;
		pusr->ctrl.x = 0.;
		pusr->ctrl.y = 0.;
		if(a && (a!=3)) {
			pusr->ctrl.p = 1.;
			pusr->ctrl.k = key;
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr);
			pno(pusr);
		}
	}

	void jl_ct_key_retn(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_D);
	}
	
	void jl_ct_left_click(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(pjlc->ct.heldDown &&
			((pjlc->ct.msx <= .9) || (pjlc->ct.msy >= .1)))
		{
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}
	
//	void mous_over(jl_simple_fnt prun) {
//		prun(255, 0);
//	}
#elif JL_PLAT == JL_PLAT_PHONE //if Android
	void tuch_cntr(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //touch center
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(((pjlc->ct.msy>.4f * jl_dl_p(pusr)) &&
			(pjlc->ct.msy<.6f * jl_dl_p(pusr))) &&
			((pjlc->ct.msx>.4f) && (pjlc->ct.msx<.6f)))
		{
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}
	//
	void tuch_nrrt(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near right
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(((pjlc->ct.msx>.6f) && (pjlc->ct.msx<.8f)) &&
			((pjlc->ct.msy * jl_dl_p(pusr)>.2f) &&
			(pjlc->ct.msy * jl_dl_p(pusr)<.8f)))
		{
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch_nrlt(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near left
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(((pjlc->ct.msx<.4f) && (pjlc->ct.msx>.2f)) &&
			((pjlc->ct.msy>.2f * jl_dl_p(pusr)) &&
			(pjlc->ct.msy<.8f * jl_dl_p(pusr))))
		{
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch_nrup(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near up
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(((pjlc->ct.msy<.4f * jl_dl_p(pusr)) &&
			(pjlc->ct.msy>.2f * jl_dl_p(pusr))) &&
			((pjlc->ct.msx>.2f) && (pjlc->ct.msx<.8f)))
		{
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch_nrdn(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{ //near down
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(((pjlc->ct.msy>.6f * jl_dl_p(pusr)) &&
			(pjlc->ct.msy<.8f * jl_dl_p(pusr))) &&
			((pjlc->ct.msx>.2f) && (pjlc->ct.msx<.8f)))
		{
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}
	//

	void tuch_frrt(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far right
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(pjlc->ct.msx>.8f) {
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch_frlt(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far left
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(pjlc->ct.msx<.2f) {
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch_frup(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far up
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(pjlc->ct.msy<.2f * jl_dl_p(pusr)) {
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch_frdn(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//far down
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(pjlc->ct.msy>.8f * jl_dl_p(pusr)) {
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}

	void tuch(jl_t *pusr, jl_simple_fnt prun,
		jl_simple_fnt pno)
	{//Any touch
		jvct_t* pjlc = pusr->pjlc;
		_jl_ct_press(pusr); //hrxy
		if(pjlc->ct.heldDown) {
			_jl_ct_is_down(pusr); //pk
			prun(pusr);
		}else{
			_jl_ct_not_down(pusr); //pk
			pno(pusr);
		}
	}
#endif

void jl_ct_key_menu(jl_t *pusr, jl_simple_fnt prun,
	jl_simple_fnt pno)
{
	#if JL_PLAT == JL_PLAT_COMPUTER
	jl_ct_key(pusr, prun, pno, SDL_SCANCODE_APPLICATION); //xyrhpk
	#elif JL_PLAT == JL_PLAT_PHONE
	jvct_t* pjlc = pusr->pjlc;
	pusr->ctrl.x = 0.;
	pusr->ctrl.y = 0.;
	pusr->ctrl.r = 0.;
	pusr->ctrl.h = pjlc->ct.menu;
	if(pjlc->ct.menu) {
		pusr->ctrl.p = 1.;
		pusr->ctrl.k = SDL_SCANCODE_APPLICATION;
		prun(pusr);
	}else{
		pusr->ctrl.p = 0.;
		pusr->ctrl.k = 0;
		pno(pusr);
	}
	#endif
}

void jl_ct_txty(void) {
	SDL_StartTextInput();
}

void jl_ct_txtn(void) {
	SDL_StopTextInput();
}

float jl_ct_gmousex(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	return pjlc->ct.msx;
}

float jl_ct_gmousey(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	return pjlc->ct.msy;
}

static inline void _jal5_jl_ct_hndl(jvct_t *pjlc) {
	#if JL_PLAT == JL_PLAT_PHONE
		pjlc->ct.menu = 0;
	#endif
	if ( pjlc->ct.event.type == SDL_TEXTINPUT) {
		printf("%1s\n", &(pjlc->ct.event.text.text[0]));
		int i;
		for(i = 0; i < 32; i++)
			pjlc->ct.text_input[i] = pjlc->ct.event.text.text[i];
		pjlc->ct.read_cursor = 0;
	}else if(pjlc->ct.event.type==SDL_WINDOWEVENT) {
		if(pjlc->ct.event.window.event ==
			SDL_WINDOWEVENT_RESIZED)
		{
			_jal5_lsdl_glpt_view(pjlc,
				pjlc->ct.event.window.data1,
				pjlc->ct.event.window.data2);
		}
	}
	#if JL_PLAT == JL_PLAT_PHONE
		if( pjlc->ct.event.type==SDL_FINGERDOWN ) {
			pjlc->ct.msx = pjlc->ct.event.tfinger.x;

			pjlc->ct.heldDown = 1;
			pjlc->ct.msy = pjlc->ct.event.tfinger.y * jl_dl_p(pjlc->sg.usrd);
			if(pjlc->sg.usrd->smde) {
				pjlc->ct.msy = pjlc->ct.msy * 2.;
				pjlc->ct.msy -= jl_dl_p(pjlc->sg.usrd);
				if(pjlc->ct.msy < 0.) pjlc->ct.heldDown = 0;
			}

		}else if( pjlc->ct.event.type==SDL_FINGERUP ) {
			pjlc->ct.heldDown = 0;
		}else if( pjlc->ct.event.type==SDL_KEYDOWN ) {
			if( pjlc->ct.event.key.keysym.scancode ==
				SDL_SCANCODE_AC_BACK)
			{ //Back Key
				jl_io_printc(pjlc->sg.usrd,"JLVM sêȳ'ēŋ: \"a'kwyt'ēŋ\"");
				exit(0);
			}else{ //Menu Key
				pjlc->ct.menu = 1;
			}
		}
	#elif JL_PLAT == JL_PLAT_COMPUTER
		if( pjlc->ct.event.wheel.y > 0 &&
			 pjlc->ct.event.wheel.type == SDL_MOUSEWHEEL)
		{
			//Grscrup = JL_PP;
		}else if ( pjlc->ct.event.wheel.y < 0 &&
			 pjlc->ct.event.wheel.type == SDL_MOUSEWHEEL )
		{
			//Glscrdn = JL_PP;
		}
	#endif
}

static void _jl_ct_run_event(jvct_t * pjlc, uint8_t pevent,
	jl_simple_fnt prun, jl_simple_fnt pno)
{
	if(pjlc->ct.getEvents[pevent] == NULL) {
		_jl_fl_errf(pjlc, ":[INPT/ERR] Null Pointer: pjlc\
				->ct.getEvents.Event:\n:");
		_jl_fl_errf(pjlc, (void*)jl_me_strt_fnum(pevent)->data);
		jl_sg_kill(pjlc->sg.usrd, "\n");
	}
	pjlc->ct.getEvents[pevent](pjlc->sg.usrd, prun, pno);
}

void jl_ct_run_event(jl_t *pusr, uint8_t pevent,
	jl_simple_fnt prun, jl_simple_fnt pno)
{
	_jl_ct_run_event(pusr->pjlc, pevent, prun, pno);
}

static inline void _jal5_jl_ct_evnt_updt(jvct_t * pjlc) {
	while(SDL_PollEvent(&pjlc->ct.event)) _jal5_jl_ct_hndl(pjlc);
	#if JL_PLAT == JL_PLAT_COMPUTER
		pjlc->ct.keys = SDL_GetKeyboardState(NULL);
	#endif
}

//Main Input Loop
void _jl_ct_loop(jvct_t* pjlc) {
	#if JL_PLAT == JL_PLAT_PHONE
	if(pjlc->ct.heldDown) pjlc->ct.heldDown = 2;
	if(pjlc->ct.menu) pjlc->ct.menu = 2;
	#endif
	_jal5_jl_ct_evnt_updt(pjlc); //Get the information on current events
	#if JL_PLAT == JL_PLAT_COMPUTER
		//Get Whether mouse is down or not and xy coordinates
		if(
			SDL_GetMouseState(&pjlc->ct.msxi,&pjlc->ct.msyi)
			&SDL_BUTTON(1))
		{
			if(pjlc->ct.heldDown)
				pjlc->ct.heldDown = 2;
			else
				pjlc->ct.heldDown = 1;
		}else{
			pjlc->ct.heldDown = 0;
		}
		if(pjlc->ct.msxi < pjlc->dl.window.x ||
			pjlc->ct.msxi > pjlc->dl.window.x + pjlc->dl.window.w ||
			pjlc->ct.msyi < pjlc->dl.window.y ||
			pjlc->ct.msyi > pjlc->dl.window.y + pjlc->dl.window.h)
		{
			if(!pjlc->ct.sc) {
				SDL_ShowCursor(SDL_ENABLE);
				pjlc->ct.sc = 1;
			}
		}else{
			if(pjlc->ct.sc) {
				SDL_ShowCursor(SDL_DISABLE);
				pjlc->ct.sc = 0;
			}
		}
		int32_t mousex = pjlc->ct.msxi - pjlc->dl.window.x;
		int32_t mousey = (pjlc->ct.msyi - pjlc->dl.window.y) *
			(1 + pjlc->sg.usrd->smde);
		//translate integer into float by clipping [0-1]
		pjlc->ct.msx =
			((float)(mousex-2)) / jl_dl_getw(pjlc->sg.usrd);
		pjlc->ct.msy =
			((float)mousey) /
			(jl_dl_geth(pjlc->sg.usrd) * (1 + pjlc->sg.usrd->smde));
		if(pjlc->sg.usrd->smde && pjlc->ct.msy < 0.) {
			pjlc->ct.msy = 0.;
			pjlc->ct.heldDown = 0;
		}
		//If Escape key is pressed, then quit the program
		if(jl_ct_key_pressed(pjlc->sg.usrd, SDL_SCANCODE_ESCAPE) == 1)
			jl_sg_exit(pjlc->sg.usrd);
		if(jl_ct_key_pressed(pjlc->sg.usrd, SDL_SCANCODE_F11) == 1)
			jl_dl_togglefullscreen(pjlc->sg.usrd);
	#endif
}

static inline void _jl_ct_fn_init(jvct_t* pjlc) {
#if JL_PLAT == JL_PLAT_COMPUTER
	pjlc->ct.getEvents[JL_CT_COMP_RETN] = jl_ct_key_retn;
	pjlc->ct.getEvents[JL_CT_COMP_KEYW] = jl_ct_key_keyw;
	pjlc->ct.getEvents[JL_CT_COMP_KEYA] = jl_ct_key_keya;
	pjlc->ct.getEvents[JL_CT_COMP_KEYS] = jl_ct_key_keys;
	pjlc->ct.getEvents[JL_CT_COMP_KEYD] = jl_ct_key_keyd;
	pjlc->ct.getEvents[JL_CT_COMP_ARUP] = jl_ct_key_arup;
	pjlc->ct.getEvents[JL_CT_COMP_ARDN] = jl_ct_key_ardn;
	pjlc->ct.getEvents[JL_CT_COMP_ARRT] = jl_ct_key_arrt;
	pjlc->ct.getEvents[JL_CT_COMP_ARLT] = jl_ct_key_arlt;
//	pjlc->ct.getEvents[JL_CT_COMP_MSXY] = mous_over;
	pjlc->ct.getEvents[JL_CT_COMP_CLLT] = jl_ct_left_click;
	pjlc->ct.getEvents[JL_CT_COMP_MENU] = jl_ct_key_menu;
#elif JL_PLAT == JL_PLAT_PHONE
	pjlc->ct.getEvents[JL_CT_ANDR_TCCR] = tuch_cntr;
	pjlc->ct.getEvents[JL_CT_ANDR_TFUP] = tuch_frup;
	pjlc->ct.getEvents[JL_CT_ANDR_TFDN] = tuch_frdn;
	pjlc->ct.getEvents[JL_CT_ANDR_TFRT] = tuch_frrt;
	pjlc->ct.getEvents[JL_CT_ANDR_TFLT] = tuch_frlt;
	pjlc->ct.getEvents[JL_CT_ANDR_TNUP] = tuch_nrup;
	pjlc->ct.getEvents[JL_CT_ANDR_TNDN] = tuch_nrdn;
	pjlc->ct.getEvents[JL_CT_ANDR_TNRT] = tuch_nrrt;
	pjlc->ct.getEvents[JL_CT_ANDR_TNLT] = tuch_nrlt;
	pjlc->ct.getEvents[JL_CT_ANDR_TOUC] = tuch;
	pjlc->ct.getEvents[JL_CT_ANDR_MENU] = jl_ct_key_menu;
#endif
}

static inline void _jl_ct_var_init(jvct_t* pjlc) {
	int i;
	for(i = 0; i < 255; i++)
		pjlc->ct.keyDown[i] = 0;
	pjlc->ct.read_cursor = 0;
	for(i = 0; i < 32; i++)
		pjlc->ct.text_input[i] = 0;
	pjlc->ct.sc = 0;
}

void _jl_ct_init(jvct_t* pjlc) {
	_jl_ct_fn_init(pjlc);
	_jl_ct_var_init(pjlc);
}

/**
 * Get any keys that are currently being typed.  Output in Aski.
 * If phone, pops up keyboard if not already up.  If nothing is being typed,
 * returns 0.
*/
uint8_t jl_ct_typing_get(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;

	if(!SDL_IsTextInputActive()) SDL_StartTextInput();
	uint8_t rtn = pjlc->ct.text_input[pjlc->ct.read_cursor];
	if(jl_ct_key_pressed(pusr, SDL_SCANCODE_BACKSPACE) == 1) return '\b';
	if(jl_ct_key_pressed(pusr, SDL_SCANCODE_DELETE) == 1) return '\02';
	if(jl_ct_key_pressed(pusr, SDL_SCANCODE_RETURN) == 1) return '\n';
	if(!rtn) return 0;
	pjlc->ct.read_cursor++;
	return rtn;
}

/**
 * If phone, hides keyboard.
*/
void jl_ct_typing_disable(jl_t *pusr) {
	SDL_StopTextInput();
}

/**
 * Returns 0 if key isn't pressed
 * Returns 1 if key is just pressed
 * Returns 2 if key is held down
 * Returns 3 if key is released
*/

uint8_t jl_ct_key_pressed(jl_t *pusr, uint8_t key) {
#if JL_PLAT == JL_PLAT_COMPUTER
	jvct_t* pjlc = pusr->pjlc;
	if(pjlc->ct.keyDown[key] == 0) {
		pjlc->ct.keyDown[key] = pjlc->ct.keys[key];
		return pjlc->ct.keyDown[key]; //1: Just Pressed, 0: Not pressed
	}else if(pjlc->ct.keyDown[key] && !pjlc->ct.keys[key]) {
		//If Was Held Down And Now Isnt | 3: Release
		pjlc->ct.keyDown[key] = 0;
		return 3;
	}else{
		pjlc->ct.keyDown[key] = 2; //2: Held Down
		return 2;
	}
#else
	return 0; //Unimplemented
#endif
}
