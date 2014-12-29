/*
 * (c) Jeron A. Lau
 *
 * jlvm_blib_inpt is a library for making input compatible between different
 * devices.
*/

#include "header/jlvmpu.h"
#define INPT_MAXX INPT_ALLP(\
		INPT_NINT_MAXX,\
		INPT_COMP_MAXX,\
		INPT_ANDR_MAXX)

typedef struct{
	void ( *OnEvent )(float x, float y);
	uint8_t Type;
}jlvm_evnt_t;

void **GJlvmEvnt;

typedef struct{
	void ( *Event )(jlvm_evnt_t * PMode);
}jlvm_slib_evnt_t;

jlvm_slib_evnt_t GetEvents[INPT_MAXX];

sgrp_user_t *g_sgrp_user;

uint8_t GMode = 0;
float msx, msy;
int msxi, msyi;
uint8_t heldDown = 0;
uint32_t Gsd = 0; //NYI: stylus delete
SDL_Event event;

#if PLATFORM == 0
const Uint8 *keys;
#endif

#if PLATFORM == 0
	void keys_retn(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_RETURN]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_arup(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_UP]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_ardn(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_DOWN]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_arlt(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_LEFT]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_arrt(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_RIGHT]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_keyw(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_W]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_keya(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_A]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_keys(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_S]) {
			PMode->OnEvent(255, 0);
		}
	}

	void keys_keyd(jlvm_evnt_t * PMode) {
		if(keys[SDL_SCANCODE_D]) {
			PMode->OnEvent(255, 0);
		}
	}
	
	void mous_cllt(jlvm_evnt_t * PMode) {
		if(heldDown && ((msx <= .9) || (msy >= .1))) {
			PMode->OnEvent(inpt_gets_xmse(), inpt_gets_ymse());
		}
	}
	
//	void mous_over(jlvm_evnt_t * PMode) {
//		PMode->OnEvent(255, 0);
//	}
#elif PLATFORM == 1 //if Android
	void tuch_cntr(jlvm_evnt_t * PMode) { //touch center
		if(((msy>.4f) && (msy<6.f)) && ((msx>.4f) && (msx<6.f))) {
			PMode->OnEvent(255, 0);
		}
	}
	//
	void tuch_nrrt(jlvm_evnt_t * PMode) { //near right
		if(((msx>.6f) && (msx<.8f)) && ((msy>.2f) && (msy<8.f))) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch_nrlt(jlvm_evnt_t * PMode) { //near left
		if(((msx<.4f) && (msx>.2f)) && ((msy>.2f) && (msy<8.f))) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch_nrup(jlvm_evnt_t * PMode) { //near up
		if(((msy<.4f) && (msy>.2f)) && ((msx>.2f) && (msx<8.f))) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch_nrdn(jlvm_evnt_t * PMode) { //near down
		if(((msy>.6f) && (msy<.8f)) && ((msx>.2f) && (msx<8.f))) {
			PMode->OnEvent(255, 0);
		}
	}
	//

	void tuch_frrt(jlvm_evnt_t * PMode) { //far right
		if(msx>.8f) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch_frlt(jlvm_evnt_t * PMode) { //far left
		if(msx<.2f) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch_frup(jlvm_evnt_t * PMode) { //far up
		if(msy<.2f) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch_frdn(jlvm_evnt_t * PMode) { //far down
		if(msy>.8f) {
			PMode->OnEvent(255, 0);
		}
	}

	void tuch(jlvm_evnt_t * PMode) { //Any touch
		PMode->OnEvent(msx, msy);
	}
#endif

void INPT_txty(void) {
	SDL_StartTextInput();
}

void INPT_txtn(void) {
	SDL_StopTextInput();
}

float inpt_gets_xmse(void) {
	return msx;
}

float inpt_gets_ymse(void) {
	return msy;
}

static inline void _jal5_inpt_hndl(jvct_t *pjct) {
	#if PLATFORM == 1 //PHONE
		if(event.type==SDL_WINDOWEVENT) {
			if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
				_jal5_lsdl_glpt_view(pjct, event.window.data1,
					event.window.data2);
				jal5_file_errf(pjct, "windoweventresize....");
			}
		}else if( event.type==SDL_FINGERDOWN ) {
			msx = event.tfinger.x;
			msy = event.tfinger.y;
			heldDown = 1;
		}else if( event.type==SDL_FINGERUP ) {
			heldDown = 0;
		}else if( event.type==SDL_KEYDOWN ) {
			if( event.key.keysym.scancode == SDL_SCANCODE_AC_BACK) { //Back Key
				#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
				jal5_siop_cplo(0,"JLVM","JLVM sêȳ'ēŋ: \"a'kwyt'ēŋ\"");
				#endif
				exit(0);
			}else{ //Menu Key
			}
		}
	#elif PLATFORM == 0 //PC
		if ( event.wheel.y > 0 &&
			 event.wheel.type == SDL_MOUSEWHEEL)
		{
			//Grscrup = JL_PP;
		}else if ( event.wheel.y < 0 &&
					 event.wheel.type == SDL_MOUSEWHEEL )
		{
			//Glscrdn = JL_PP;
		}
	#endif
}

//Run Pre-set Events currently being activated
static inline void _jal5_inpt_evnt_runa(jvct_t * pjct) {
	int i;

	if( (msx > .9) && (msy < .1) && (heldDown == 1)) {
		_grph_flip_scrn(pjct);
		heldDown = 2;
		return;
	}
	if( GJlvmEvnt
		#if PLATFORM == 1 //PHONE
			&& heldDown
		#endif
	)
	{
		jlvm_evnt_t *Mode = GJlvmEvnt[GMode];
		if(GetEvents == NULL) {
			jlvm_dies(pjct, Strt(
				"[ERR] Null Pointer: GetEvents\n"));
		}
		for(i = 0; i < pjct->Inpt.CtrC[GMode]; i++) {
			if(GetEvents[Mode[i].Type].Event == NULL) {
				jlvm_dies(pjct,
					amem_strt_merg(
					Strt(
						"[INPT/ERR] Null Pointer: GetEvents.Event"),
					amem_strt_fnum(Mode[i].Type),
					STRT_TEMP));
			}
			GetEvents[Mode[i].Type].Event(&Mode[i]);
		}
		#if PLATFORM == 1 //PHONE
		heldDown = 2;
		#endif
	}
}

static inline void _jal5_inpt_evnt_updt(jvct_t * pjct) {
	while(SDL_PollEvent(&event)) _jal5_inpt_hndl(pjct);
	#if PLATFORM == 0 //PC
		keys = SDL_GetKeyboardState(NULL);
	#endif
}

//Main Input Loop
void _jal5_inpt_loop(jvct_t* pjct) {
	GMode = pjct->Sgrp.usrd->mode; //Get Mode
	_jal5_inpt_evnt_updt(pjct);
	#if PLATFORM == 0 //PC
		//Get Whether mouse is down or not and xy coordinates
		if(SDL_GetMouseState(&msxi,&msyi)&SDL_BUTTON(1)) {
			if(heldDown) {
				heldDown = 2;
			}else{
				heldDown = 1;
			}
		}else{
			heldDown = 0;
		}
		//translate integer into float by clipping [0-1]
		msx = ((float)(msxi-2)) / _jal5_lsdl_sres_getw();
		msy = ((float)msyi) / _jal5_lsdl_sres_geth();
		//If Escape key is pressed, then quit
		if(keys[SDL_SCANCODE_ESCAPE]) sgrp_wind_kill(pjct->Sgrp.usrd);
	#endif
	_jal5_inpt_evnt_runa(pjct); //Run Events currently being activated
}

void _jal5_inpt_init(jvct_t* pjct) {
#if PLATFORM == 0
	GetEvents[INPT_COMP_RETN].Event = keys_retn;
	GetEvents[INPT_COMP_KEYW].Event = keys_keyw;
	GetEvents[INPT_COMP_KEYA].Event = keys_keya;
	GetEvents[INPT_COMP_KEYS].Event = keys_keys;
	GetEvents[INPT_COMP_KEYD].Event = keys_keyd;
	GetEvents[INPT_COMP_ARUP].Event = keys_arup;
	GetEvents[INPT_COMP_ARDN].Event = keys_ardn;
	GetEvents[INPT_COMP_ARRT].Event = keys_arrt;
	GetEvents[INPT_COMP_ARLT].Event = keys_arlt;
//	GetEvents[INPT_COMP_MSXY].Event = mous_over;
	GetEvents[INPT_COMP_CLLT].Event = mous_cllt;
#elif PLATFORM == 1
	GetEvents[INPT_ANDR_TCCR].Event = tuch_cntr;
	GetEvents[INPT_ANDR_TFUP].Event = tuch_frup;
	GetEvents[INPT_ANDR_TFDN].Event = tuch_frdn;
	GetEvents[INPT_ANDR_TFRT].Event = tuch_frrt;
	GetEvents[INPT_ANDR_TFLT].Event = tuch_frlt;
	GetEvents[INPT_ANDR_TNUP].Event = tuch_nrup;
	GetEvents[INPT_ANDR_TNDN].Event = tuch_nrdn;
	GetEvents[INPT_ANDR_TNRT].Event = tuch_nrrt;
	GetEvents[INPT_ANDR_TNLT].Event = tuch_nrlt;
	GetEvents[INPT_ANDR_TOUC].Event = tuch;
#endif
}

void inpt_mode_init(sgrp_user_t *pusr) {
	jvct_t* pjct = pusr->pjct;
	uint8_t modecount = pusr->mdec;
	GJlvmEvnt = malloc(sizeof(void *) * modecount);
	pjct->Inpt.CtrC = malloc(modecount);
}

void inpt_mode_setm(sgrp_user_t *pusr, uint8_t mode, uint8_t controlCount) {
	jvct_t* pjct = pusr->pjct;
//	printf("mde:%d\n",FMode);
	if(controlCount)
		GJlvmEvnt[mode] = malloc(sizeof(jlvm_evnt_t) * controlCount);
	pjct->Inpt.CtrC[mode] = controlCount;
	pusr->mode = mode;
}

void inpt_mode_addi(sgrp_user_t *pusr, uint8_t libevent, uint8_t usrevent,
	void (*fn)(float x, float y))
{
	jlvm_evnt_t *Mode = GJlvmEvnt[pusr->mode];
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
		if(1/\*keys[j]*\/) {
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

uint8_t jlvmpi_spk(uint8_t key) {
	return 0/\*keys[key]*\/;
}
*/
