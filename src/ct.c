/*
 * (c) Jeron A. Lau
 *
 * jl/ct is a library for making input controllers compatible between different
 * devices.
*/

#include "header/jl_pr.h"

#if PLATFORM == 0
	void jl_ct_key(jl_t *pusr, jlvm_evnt_t * PMode, uint8_t key) {
		uint8_t a;
		if((a = jl_ct_key_pressed(pusr, key))) {
			PMode->OnEvent(pusr, 255, a + .25);
		}
	}

	void jl_ct_key_retn(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_RETURN);
	}

	void jl_ct_key_arup(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_UP);
	}

	void jl_ct_key_ardn(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_DOWN);
	}

	void jl_ct_key_arlt(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_LEFT);
	}

	void jl_ct_key_arrt(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_RIGHT);
	}

	void jl_ct_key_keyw(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_W);
	}

	void jl_ct_key_keya(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_A);
	}

	void jl_ct_key_keys(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_S);
	}

	void jl_ct_key_keyd(jl_t *pusr, jlvm_evnt_t * PMode) {
		jl_ct_key(pusr, PMode, SDL_SCANCODE_D);
	}
	
	void jl_ct_key_menu(jl_t *pusr, jlvm_evnt_t * PMode) {
		jvct_t* pjlc = pusr->pjlc;
		if(
		#if PLATFORM == 0 //COMPUTER
			pjlc->ct.keys[SDL_SCANCODE_APPLICATION]
		#elif PLATFORM == 1 //PHONE
			pjlc->ct.menu
		#endif
		) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}
	
	void jl_ct_left_click(jl_t *pusr, jlvm_evnt_t * PMode) {
		jvct_t* pjlc = pusr->pjlc;
		if(pjlc->ct.heldDown &&
			((pjlc->ct.msx <= .9) || (pjlc->ct.msy >= .1)))
		{
			PMode->OnEvent(pusr,
				jl_ct_gmousex(pusr), jl_ct_gmousey(pusr));
		}
	}
	
//	void mous_over(jlvm_evnt_t * PMode) {
//		PMode->OnEvent(255, 0);
//	}
#elif PLATFORM == 1 //if Android
	void tuch_cntr(jl_t *pusr, jlvm_evnt_t * PMode) { //touch center
		if(((msy>.4f) && (msy<6.f)) && ((msx>.4f) && (msx<6.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}
	//
	void tuch_nrrt(jl_t *pusr, jlvm_evnt_t * PMode) { //near right
		if(((msx>.6f) && (msx<.8f)) && ((msy>.2f) && (msy<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_nrlt(jl_t *pusr, jlvm_evnt_t * PMode) { //near left
		if(((msx<.4f) && (msx>.2f)) && ((msy>.2f) && (msy<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_nrup(jl_t *pusr, jlvm_evnt_t * PMode) { //near up
		if(((msy<.4f) && (msy>.2f)) && ((msx>.2f) && (msx<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_nrdn(jl_t *pusr, jlvm_evnt_t * PMode) { //near down
		if(((msy>.6f) && (msy<.8f)) && ((msx>.2f) && (msx<8.f))) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}
	//

	void tuch_frrt(jl_t *pusr, jlvm_evnt_t * PMode) { //far right
		if(msx>.8f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_frlt(jl_t *pusr, jlvm_evnt_t * PMode) { //far left
		if(msx<.2f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_frup(jl_t *pusr, jlvm_evnt_t * PMode) { //far up
		if(msy<.2f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch_frdn(jl_t *pusr, jlvm_evnt_t * PMode) { //far down
		if(msy>.8f) {
			PMode->OnEvent(pusr, 255, 0);
		}
	}

	void tuch(jl_t *pusr, jlvm_evnt_t * PMode) { //Any touch
		PMode->OnEvent(pusr,jl_ct_gmousex(pusr), jl_ct_gmousey(pusr));
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

//Run Pre-set Events currently being activated
static inline void _jal5_jl_ct_evnt_runa(jvct_t * pjlc) {
	int i;

	if( (pjlc->ct.msx > .9) && (pjlc->ct.msy < .1) &&
		(pjlc->ct.heldDown == 1))
	{
		_jl_gr_flip_scrn(pjlc);
		pjlc->ct.heldDown = 2;
		return;
	}
	if( pjlc->ct.events
		#if PLATFORM == 1 //PHONE
			&& pjlc->ct.heldDown
		#endif
	)
	{
		jlvm_evnt_t *Mode = pjlc->ct.events[pjlc->sg.usrd->mode];
		if(pjlc->ct.getEvents == NULL) {
			jlvm_dies(pjlc, Strt(
				"[ERR] Null Pointer: pjlc->ct.getEvents\n"));
		}
		for(i = 0; i < pjlc->ct.CtrC[pjlc->sg.usrd->mode]; i++) {
			if(pjlc->ct.getEvents[Mode[i].Type].Event == NULL) {
				jlvm_dies(pjlc,
					jl_me_strt_merg(
					Strt(
						"[INPT/ERR] Null Pointer: pjlc\
						->ct.getEvents.Event"),
					jl_me_strt_fnum(Mode[i].Type),
					STRT_TEMP));
			}
			pjlc->ct.getEvents[Mode[i].Type].Event(
				pjlc->sg.usrd, &Mode[i]);
		}
		#if PLATFORM == 1 //PHONE
		pjlc->ct.heldDown = 2;
		#endif
	}
}

static inline void _jal5_jl_ct_evnt_updt(jvct_t * pjlc) {
	while(SDL_PollEvent(&pjlc->ct.event)) _jal5_jl_ct_hndl(pjlc);
	#if PLATFORM == 0 //PC
		pjlc->ct.keys = SDL_GetKeyboardState(NULL);
	#endif
}

//Main Input Loop
void _jl_ct_loop(jvct_t* pjlc) {
	_jal5_jl_ct_evnt_updt(pjlc);
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
	#endif
	_jal5_jl_ct_evnt_runa(pjlc); //Run Events currently being activated
}

void _jl_ct_init(jvct_t* pjlc) {
#if PLATFORM == 0
	pjlc->ct.getEvents[JL_CT_COMP_RETN].Event = jl_ct_key_retn;
	pjlc->ct.getEvents[JL_CT_COMP_KEYW].Event = jl_ct_key_keyw;
	pjlc->ct.getEvents[JL_CT_COMP_KEYA].Event = jl_ct_key_keya;
	pjlc->ct.getEvents[JL_CT_COMP_KEYS].Event = jl_ct_key_keys;
	pjlc->ct.getEvents[JL_CT_COMP_KEYD].Event = jl_ct_key_keyd;
	pjlc->ct.getEvents[JL_CT_COMP_ARUP].Event = jl_ct_key_arup;
	pjlc->ct.getEvents[JL_CT_COMP_ARDN].Event = jl_ct_key_ardn;
	pjlc->ct.getEvents[JL_CT_COMP_ARRT].Event = jl_ct_key_arrt;
	pjlc->ct.getEvents[JL_CT_COMP_ARLT].Event = jl_ct_key_arlt;
//	pjlc->ct.getEvents[JL_CT_COMP_MSXY].Event = mous_over;
	pjlc->ct.getEvents[JL_CT_COMP_CLLT].Event = jl_ct_left_click;
	pjlc->ct.getEvents[JL_CT_COMP_MENU].Event = jl_ct_key_menu;
#elif PLATFORM == 1
	pjlc->ct.getEvents[JL_CT_ANDR_TCCR].Event = tuch_cntr;
	pjlc->ct.getEvents[JL_CT_ANDR_TFUP].Event = tuch_frup;
	pjlc->ct.getEvents[JL_CT_ANDR_TFDN].Event = tuch_frdn;
	pjlc->ct.getEvents[JL_CT_ANDR_TFRT].Event = tuch_frrt;
	pjlc->ct.getEvents[JL_CT_ANDR_TFLT].Event = tuch_frlt;
	pjlc->ct.getEvents[JL_CT_ANDR_TNUP].Event = tuch_nrup;
	pjlc->ct.getEvents[JL_CT_ANDR_TNDN].Event = tuch_nrdn;
	pjlc->ct.getEvents[JL_CT_ANDR_TNRT].Event = tuch_nrrt;
	pjlc->ct.getEvents[JL_CT_ANDR_TNLT].Event = tuch_nrlt;
	pjlc->ct.getEvents[JL_CT_ANDR_TOUC].Event = tuch;
#endif
}

void jl_ct_mode_init(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	uint8_t modecount = pusr->mdec;
	pjlc->ct.events = malloc(sizeof(void *) * modecount);
	pjlc->ct.CtrC = malloc(modecount);
}

void jl_ct_mode_setm(jl_t *pusr, uint8_t mode, uint8_t controlCount) {
	jvct_t* pjlc = pusr->pjlc;
//	printf("mde:%d\n",FMode);
	if(controlCount)
		pjlc->ct.events[mode] = malloc(sizeof(jlvm_evnt_t) * controlCount);
	pjlc->ct.CtrC[mode] = controlCount;
	pusr->mode = mode;
}

void jl_ct_mode_addi(jl_t *pusr, uint8_t libevent, uint8_t usrevent,
	void (*fn)(jl_t *pusr, float x, float y))
{
	jvct_t* pjlc = pusr->pjlc;
	jlvm_evnt_t *Mode = pjlc->ct.events[pusr->mode];
	Mode[usrevent].Type = libevent;
	Mode[usrevent].OnEvent = fn;
}

void jl_ct_addr(uint8_t controlNum,
	void (*up)(uint8_t p), void (*down)(uint8_t p),
	void (*right)(uint8_t p), void (*left)(uint8_t p)
	)
{
}

void jl_ct_adds(void) {
}

void jl_ct_adde(void) {
}

void jl_ct_addn(void) {
}

void jl_ct_adda(void) {
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
		if(pjlc->ct.keyDown[key]) printf("pressed\n");
		return pjlc->ct.keyDown[key]; //1: Just Pressed, 0: Not pressed
	}else if(pjlc->ct.keyDown[key] == 2 && !pjlc->ct.keys[key]) {
		//If Was Held Down And Now Isnt | 3: Release
		pjlc->ct.keyDown[key] = 0;
		printf("released\n");
		return 3;
	}else{
		pjlc->ct.keyDown[key] = 2; //2: Held Down
		printf("held down\n");
		return 2;
	}
}
