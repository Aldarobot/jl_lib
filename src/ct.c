/*
 * (c) Jeron A. Lau
 *
 * jl/ct is a library for making input controllers compatible between different
 * devices.
*/

#include "header/jlvm_pr.h"

#if PLATFORM == 0
	void jl_ct_key(sgrp_user_t *pusr, jlvm_evnt_t * PMode, uint8_t key) {
		uint8_t a;
		if((a = jl_ct_key_pressed(pusr, key))) {
			PMode->OnEvent(pusr, 255, a);
		}
	}

	void jl_ct_key_retn(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_D);
	}
	
	void jl_ct_key_menu(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jvct_t* pjct = pusr->pjct;
		if(
		#if PLATFORM == 0 //COMPUTER
			pjct->Inpt.keys[SDL_SCANCODE_APPLICATION]
		#elif PLATFORM == 1 //PHONE
			pjct->Inpt.menu
		#endif
		) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}
	
	void jl_ct_left_click(sgrp_user_t *pusr, jlvm_evnt_t * PMode) {
		jvct_t* pjct = pusr->pjct;
		if(pjct->Inpt.heldDown &&
			((pjct->Inpt.msx <= .9) || (pjct->Inpt.msy >= .1)))
		{
			PMode->OnEvent(pusr,
				inpt_gets_xmse(pusr), inpt_gets_ymse(pusr));
		}
	}
	
//	void mous_over(jlvm_evnt_t * PMode) {
//		PMode->OnEvent(255, 0);
//	}
#elif PLATFORM == 1 //if Android
	void tuch_cntr(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //touch center
		if(((msy>.4f) && (msy<6.f)) && ((msx>.4f) && (msx<6.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}
	//
	void tuch_nrrt(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //near right
		if(((msx>.6f) && (msx<.8f)) && ((msy>.2f) && (msy<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_nrlt(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //near left
		if(((msx<.4f) && (msx>.2f)) && ((msy>.2f) && (msy<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_nrup(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //near up
		if(((msy<.4f) && (msy>.2f)) && ((msx>.2f) && (msx<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_nrdn(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //near down
		if(((msy>.6f) && (msy<.8f)) && ((msx>.2f) && (msx<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}
	//

	void tuch_frrt(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //far right
		if(msx>.8f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_frlt(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //far left
		if(msx<.2f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_frup(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //far up
		if(msy<.2f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_frdn(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //far down
		if(msy>.8f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch(sgrp_user_t *pusr, jlvm_evnt_t * PMode) { //Any touch
		PMode->OnEvent(pusr,inpt_gets_xmse(pusr), inpt_gets_ymse(pusr));
	}
#endif

void INPT_txty(void) {
	SDL_StartTextInput();
}

void INPT_txtn(void) {
	SDL_StopTextInput();
}

float inpt_gets_xmse(sgrp_user_t *pusr) {
	jvct_t* pjct = pusr->pjct;
	return pjct->Inpt.msx;
}

float inpt_gets_ymse(sgrp_user_t *pusr) {
	jvct_t* pjct = pusr->pjct;
	return pjct->Inpt.msy;
}

static inline void _jal5_inpt_hndl(jvct_t *pjct) {
	#if PLATFORM == 1 //PHONE
		pjct->Inpt.menu = 0;
		if(pjct->Inpt.event.type==SDL_WINDOWEVENT) {
			if(pjct->Inpt.event.window.event ==
				SDL_WINDOWEVENT_RESIZED)
			{
				_jal5_lsdl_glpt_view(pjct,
					pjct->Inpt.event.window.data1,
					pjct->Inpt.event.window.data2);
				jal5_file_errf(pjct, "windoweventresize....");
			}
		}else if( pjct->Inpt.event.type==SDL_FINGERDOWN ) {
			msx = event.tfinger.x;
			msy = event.tfinger.y;
			pjct->Inpt.heldDown = 1;
		}else if( pjct->Inpt.event.type==SDL_FINGERUP ) {
			pjct->Inpt.heldDown = 0;
		}else if( pjct->Inpt.event.type==SDL_KEYDOWN ) {
			if( pjct->Inpt.event.key.keysym.scancode ==
				SDL_SCANCODE_AC_BACK)
			{ //Back Key
				#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
				jal5_siop_cplo(0,"JLVM","JLVM sêȳ'ēŋ: \"a'kwyt'ēŋ\"");
				#endif
				exit(0);
			}else{ //Menu Key
				pjct->Inpt.menu = 1;
			}
		}
	#elif PLATFORM == 0 //PC
		if ( pjct->Inpt.event.wheel.y > 0 &&
			 pjct->Inpt.event.wheel.type == SDL_MOUSEWHEEL)
		{
			//Grscrup = JL_PP;
		}else if ( pjct->Inpt.event.wheel.y < 0 &&
			 pjct->Inpt.event.wheel.type == SDL_MOUSEWHEEL )
		{
			//Glscrdn = JL_PP;
		}
	#endif
}

//Run Pre-set Events currently being activated
static inline void _jal5_inpt_evnt_runa(jvct_t * pjct) {
	int i;

	if( (pjct->Inpt.msx > .9) && (pjct->Inpt.msy < .1) &&
		(pjct->Inpt.heldDown == 1))
	{
		_grph_flip_scrn(pjct);
		pjct->Inpt.heldDown = 2;
		return;
	}
	if( pjct->Inpt.events
		#if PLATFORM == 1 //PHONE
			&& pjct->Inpt.heldDown
		#endif
	)
	{
		jlvm_evnt_t *Mode = pjct->Inpt.events[pjct->Sgrp.usrd->mode];
		if(pjct->Inpt.getEvents == NULL) {
			jlvm_dies(pjct, Strt(
				"[ERR] Null Pointer: pjct->Inpt.getEvents\n"));
		}
		for(i = 0; i < pjct->Inpt.CtrC[pjct->Sgrp.usrd->mode]; i++) {
			if(pjct->Inpt.getEvents[Mode[i].Type].Event == NULL) {
				jlvm_dies(pjct,
					amem_strt_merg(
					Strt(
						"[INPT/ERR] Null Pointer: pjct\
						->Inpt.getEvents.Event"),
					amem_strt_fnum(Mode[i].Type),
					STRT_TEMP));
			}
			pjct->Inpt.getEvents[Mode[i].Type].Event(
				pjct->Sgrp.usrd, &Mode[i]);
		}
		#if PLATFORM == 1 //PHONE
		pjct->Inpt.heldDown = 2;
		#endif
	}
}

static inline void _jal5_inpt_evnt_updt(jvct_t * pjct) {
	while(SDL_PollEvent(&pjct->Inpt.event)) _jal5_inpt_hndl(pjct);
	#if PLATFORM == 0 //PC
		pjct->Inpt.keys = SDL_GetKeyboardState(NULL);
	#endif
}

//Main Input Loop
void _jal5_inpt_loop(jvct_t* pjct) {
	_jal5_inpt_evnt_updt(pjct);
	#if PLATFORM == 0 //PC
		//Get Whether mouse is down or not and xy coordinates
		if(
			SDL_GetMouseState(
				&pjct->Inpt.msxi,&pjct->Inpt.msyi)
			&SDL_BUTTON(1))
		{
			if(pjct->Inpt.heldDown) {
				pjct->Inpt.heldDown = 2;
			}else{
				pjct->Inpt.heldDown = 1;
			}
		}else{
			pjct->Inpt.heldDown = 0;
		}
		//translate integer into float by clipping [0-1]
		pjct->Inpt.msx =
			((float)(pjct->Inpt.msxi-2)) / _jal5_lsdl_sres_getw();
		pjct->Inpt.msy =
			((float)pjct->Inpt.msyi) / _jal5_lsdl_sres_geth();
		//If Escape key is pressed, then quit the program
		if(jl_ct_key_pressed(pjct->Sgrp.usrd, SDL_SCANCODE_ESCAPE) == 1)
			jl_sg_kill(pjct->Sgrp.usrd);
	#endif
	_jal5_inpt_evnt_runa(pjct); //Run Events currently being activated
}

void _jal5_inpt_init(jvct_t* pjct) {
#if PLATFORM == 0
	pjct->Inpt.getEvents[INPT_COMP_RETN].Event = jl_ct_key_retn;
	pjct->Inpt.getEvents[INPT_COMP_KEYW].Event = jl_ct_key_keyw;
	pjct->Inpt.getEvents[INPT_COMP_KEYA].Event = jl_ct_key_keya;
	pjct->Inpt.getEvents[INPT_COMP_KEYS].Event = jl_ct_key_keys;
	pjct->Inpt.getEvents[INPT_COMP_KEYD].Event = jl_ct_key_keyd;
	pjct->Inpt.getEvents[INPT_COMP_ARUP].Event = jl_ct_key_arup;
	pjct->Inpt.getEvents[INPT_COMP_ARDN].Event = jl_ct_key_ardn;
	pjct->Inpt.getEvents[INPT_COMP_ARRT].Event = jl_ct_key_arrt;
	pjct->Inpt.getEvents[INPT_COMP_ARLT].Event = jl_ct_key_arlt;
//	pjct->Inpt.getEvents[INPT_COMP_MSXY].Event = mous_over;
	pjct->Inpt.getEvents[INPT_COMP_CLLT].Event = jl_ct_left_click;
	pjct->Inpt.getEvents[INPT_COMP_MENU].Event = jl_ct_key_menu;
#elif PLATFORM == 1
	pjct->Inpt.getEvents[INPT_ANDR_TCCR].Event = tuch_cntr;
	pjct->Inpt.getEvents[INPT_ANDR_TFUP].Event = tuch_frup;
	pjct->Inpt.getEvents[INPT_ANDR_TFDN].Event = tuch_frdn;
	pjct->Inpt.getEvents[INPT_ANDR_TFRT].Event = tuch_frrt;
	pjct->Inpt.getEvents[INPT_ANDR_TFLT].Event = tuch_frlt;
	pjct->Inpt.getEvents[INPT_ANDR_TNUP].Event = tuch_nrup;
	pjct->Inpt.getEvents[INPT_ANDR_TNDN].Event = tuch_nrdn;
	pjct->Inpt.getEvents[INPT_ANDR_TNRT].Event = tuch_nrrt;
	pjct->Inpt.getEvents[INPT_ANDR_TNLT].Event = tuch_nrlt;
	pjct->Inpt.getEvents[INPT_ANDR_TOUC].Event = tuch;
#endif
}

void inpt_mode_init(sgrp_user_t *pusr) {
	jvct_t* pjct = pusr->pjct;
	uint8_t modecount = pusr->mdec;
	pjct->Inpt.events = malloc(sizeof(void *) * modecount);
	pjct->Inpt.CtrC = malloc(modecount);
}

void inpt_mode_setm(sgrp_user_t *pusr, uint8_t mode, uint8_t controlCount) {
	jvct_t* pjct = pusr->pjct;
//	printf("mde:%d\n",FMode);
	if(controlCount)
		pjct->Inpt.events[mode] = malloc(sizeof(jlvm_evnt_t) * controlCount);
	pjct->Inpt.CtrC[mode] = controlCount;
	pusr->mode = mode;
}

void inpt_mode_addi(sgrp_user_t *pusr, uint8_t libevent, uint8_t usrevent,
	void (*fn)(sgrp_user_t *pusr, float x, float y))
{
	jvct_t* pjct = pusr->pjct;
	jlvm_evnt_t *Mode = pjct->Inpt.events[pusr->mode];
	Mode[usrevent].Type = libevent;
	Mode[usrevent].OnEvent = fn;
}

void INPT_addr(uint8_t controlNum,
	void (*up)(uint8_t p), void (*down)(uint8_t p),
	void (*right)(uint8_t p), void (*left)(uint8_t p)
	)
{
}

void INPT_adds(void) {
}

void INPT_adde(void) {
}

void INPT_addn(void) {
}

void INPT_adda(void) {
}

/*
//Keyboard functions

uint8_t jlvmpi_key(void) {
	uint8_t j;
	uint8_t k = 1;
	for(j = ' '; j <= '~'; j++) {
		if(1/\*pjct->Inpt.keys[j]*\/) {
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
uint8_t jl_ct_key_pressed(sgrp_user_t *pusr, uint8_t key) {
	jvct_t* pjct = pusr->pjct;
	if(pjct->Inpt.keyDown == 0) {
		pjct->Inpt.keyDown = pjct->Inpt.keys[key];
		return pjct->Inpt.keyDown; //1: Just Pressed, 0: Not pressed
	}else if(pjct->Inpt.keyDown == 2 && !pjct->Inpt.keys[key]) {
		//If Was Held Down And Now Isnt | 3: Release
		pjct->Inpt.keyDown = 0;
		return 3;
	}else{
		pjct->Inpt.keyDown = 2; //2: Held Down
		return 2;
	}
}
