/*
 * (c) Jeron A. Lau
 *
 * jl/ct is a library for making input controllers compatible between different
 * devices.
*/

#include "header/jl_pr.h"

#if PLATFORM == 0
	void jl_ct_key(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno, uint8_t key)
	{
		uint8_t a;
		if((a = jl_ct_key_pressed(pusr, key))) {
			prun(pusr, 255, a + .25);
		}else{
			pno(pusr, 255, a + .25);
		}
	}

	void jl_ct_key_retn(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_D);
	}
	
	void jl_ct_key_menu(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		#if PLATFORM == 0 //COMPUTER
		jl_ct_key(pusr, prun, pno, SDL_SCANCODE_APPLICATION);
		#elif PLATFORM == 1 //PHONE
		jvct_t* pjlc = pusr->pjlc;
		if(pjlc->ct.menu)
			prun(pusr, 255, pjlc->ct.menu);
		else
			pno(pusr, 255, pjlc->ct.menu);
		#endif
	}
	
	void jl_ct_left_click(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{
		jvct_t* pjlc = pusr->pjlc;
		if(pjlc->ct.heldDown &&
			((pjlc->ct.msx <= .9) || (pjlc->ct.msy >= .1)))
		{
			prun(pusr, jl_ct_gmousex(pusr), jl_ct_gmousey(pusr));
		}else{
			pno(pusr, jl_ct_gmousex(pusr), jl_ct_gmousey(pusr));
		}
	}
	
//	void mous_over(jl_ct_onevent_fnt prun) {
//		prun(255, 0);
//	}
#elif PLATFORM == 1 //if Android
	void tuch_cntr(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{ //touch center
		if(((msy>.4f) && (msy<6.f)) && ((msx>.4f) && (msx<6.f))) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}
	//
	void tuch_nrrt(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{ //near right
		if(((msx>.6f) && (msx<.8f)) && ((msy>.2f) && (msy<8.f))) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch_nrlt(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{ //near left
		if(((msx<.4f) && (msx>.2f)) && ((msy>.2f) && (msy<8.f))) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch_nrup(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{ //near up
		if(((msy<.4f) && (msy>.2f)) && ((msx>.2f) && (msx<8.f))) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch_nrdn(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{ //near down
		if(((msy>.6f) && (msy<.8f)) && ((msx>.2f) && (msx<8.f))) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}
	//

	void tuch_frrt(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{//far right
		if(msx>.8f) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch_frlt(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{//far left
		if(msx<.2f) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch_frup(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{//far up
		if(msy<.2f) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch_frdn(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{//far down
		if(msy>.8f) {
			prun(pusr, 255, 0);
		}else{
			pno(pusr, 255, 0);
		}
	}

	void tuch(jl_t *pusr, jl_ct_onevent_fnt prun,
		jl_ct_onevent_fnt pno)
	{//Any touch
		prun(pusr,jl_ct_gmousex(pusr), jl_ct_gmousey(pusr));
	}
#endif

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
	#if PLATFORM == 1 //PHONE
		pjlc->ct.menu = 0;
		if(pjlc->ct.event.type==SDL_WINDOWEVENT) {
			if(pjlc->ct.event.window.event ==
				SDL_WINDOWEVENT_RESIZED)
			{
				_jal5_lsdl_glpt_view(pjlc,
					pjlc->ct.event.window.data1,
					pjlc->ct.event.window.data2);
				jal5_file_errf(pjlc, "windoweventresize....");
			}
		}else if( pjlc->ct.event.type==SDL_FINGERDOWN ) {
			msx = event.tfinger.x;
			msy = event.tfinger.y;
			pjlc->ct.heldDown = 1;
		}else if( pjlc->ct.event.type==SDL_FINGERUP ) {
			pjlc->ct.heldDown = 0;
		}else if( pjlc->ct.event.type==SDL_KEYDOWN ) {
			if( pjlc->ct.event.key.keysym.scancode ==
				SDL_SCANCODE_AC_BACK)
			{ //Back Key
				#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
				jl_io_print_lowc(0,"JLVM","JLVM sêȳ'ēŋ: \"a'kwyt'ēŋ\"");
				#endif
				exit(0);
			}else{ //Menu Key
				pjlc->ct.menu = 1;
			}
		}
	#elif PLATFORM == 0 //PC
		if ( pjlc->ct.event.wheel.y > 0 &&
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
	jl_ct_onevent_fnt prun, jl_ct_onevent_fnt pno)
{
	if(pjlc->ct.getEvents[pevent] == NULL) {
		jlvm_dies(pjlc,
			jl_me_strt_merg(
			Strt(
				"[INPT/ERR] Null Pointer: pjlc\
				->ct.getEvents.Event"),
			jl_me_strt_fnum(pevent),
			STRT_TEMP));
	}
	pjlc->ct.getEvents[pevent](pjlc->sg.usrd, prun, pno);
}

/**
 * Input function to do nothing.
*/
void jl_ct_dont(jl_t* pusr, float x, float y) {}

void jl_ct_run_event(jl_t *pusr, uint8_t pevent,
	jl_ct_onevent_fnt prun, jl_ct_onevent_fnt pno)
{
	_jl_ct_run_event(pusr->pjlc, pevent, prun, pno);
}

static inline void _jal5_jl_ct_evnt_updt(jvct_t * pjlc) {
	while(SDL_PollEvent(&pjlc->ct.event)) _jal5_jl_ct_hndl(pjlc);
	#if PLATFORM == 0 //PC
		pjlc->ct.keys = SDL_GetKeyboardState(NULL);
	#endif
}

//Main Input Loop
void _jl_ct_loop(jvct_t* pjlc) {
	_jal5_jl_ct_evnt_updt(pjlc); //Get the information on current events
	#if PLATFORM == 0 //PC
		//Get Whether mouse is down or not and xy coordinates
		if(
			SDL_GetMouseState(
				&pjlc->ct.msxi,&pjlc->ct.msyi)
			&SDL_BUTTON(1))
		{
			if(pjlc->ct.heldDown) {
				pjlc->ct.heldDown = 2;
			}else{
				pjlc->ct.heldDown = 1;
			}
		}else{
			pjlc->ct.heldDown = 0;
		}
		//translate integer into float by clipping [0-1]
		pjlc->ct.msx =
			((float)(pjlc->ct.msxi-2)) / _jal5_lsdl_sres_getw();
		pjlc->ct.msy =
			((float)pjlc->ct.msyi) / _jal5_lsdl_sres_geth();
		//If Escape key is pressed, then quit the program
		if(jl_ct_key_pressed(pjlc->sg.usrd, SDL_SCANCODE_ESCAPE) == 1)
			jl_sg_kill(pjlc->sg.usrd);
	#elif PLATFORM == 1 //PHONE
	if(pjlc->ct.heldDown) {
		pjlc->ct.heldDown = 2;
	}
	#endif
}

void _jl_ct_init(jvct_t* pjlc) {
#if PLATFORM == 0
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
#elif PLATFORM == 1
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
#endif
}

/*
//Keyboard functions

uint8_t jlvmpi_key(void) {
	uint8_t j;
	uint8_t k = 1;
	for(j = ' '; j <= '~'; j++) {
		if(1/\*pjlc->ct.keys[j]*\/) {
			k=0;
			break;
		}
	}
	if(k) {
		return 0;
	}else{
		return j;
	}
}
*/
uint8_t jl_ct_key_pressed(jl_t *pusr, uint8_t key) {
	jvct_t* pjlc = pusr->pjlc;
	if(pjlc->ct.keyDown[key] == 0) {
		pjlc->ct.keyDown[key] = pjlc->ct.keys[key];
		return pjlc->ct.keyDown[key]; //1: Just Pressed, 0: Not pressed
	}else if(pjlc->ct.keyDown[key] == 2 && !pjlc->ct.keys[key]) {
		//If Was Held Down And Now Isnt | 3: Release
		pjlc->ct.keyDown[key] = 0;
		return 3;
	}else{
		pjlc->ct.keyDown[key] = 2; //2: Held Down
		return 2;
	}
}
