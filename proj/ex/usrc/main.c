/*
 * simple example program that starts up and immediately quits.  Good for making
 * script-like programs.
*/
/*#include "header/jlvm.h"

void hack_user_init(void) {
}*/

/*----------------------------------------------------------------------------*/

/*
 * simple example program that creates a loop.  Good for most programs.
*/
#include "bin/jlib/jlvm.h"

#define EX_MODE_EDIT 0
#define EX_MODE_MAXX 1

void ex_loop(sgrp_user_t* pusr) {
}

void ex_wdns(sgrp_user_t* pusr) {
	grph_draw_text(pusr, "testing...1234567890", 0.f, 0.f, .0625f, 255);
}

void ex_wups(sgrp_user_t* pusr) {
	grph_draw_text(pusr, "this IS upper", 0.f, 0.f, .0625f, 255);
}

void ex_term(sgrp_user_t* pusr) {

}

void hack_user_init(sgrp_user_t* pusr) {
	siop_prnt_cplo(0,"EXAMPLE","Initializing...");
	sgrp_mode_init(pusr, EX_MODE_MAXX); //Allocate 1 Mode
	pusr->mode = EX_MODE_EDIT; //Set Mode To 0
	sgrp_mode_iset(pusr, ex_loop, ex_wups, ex_wdns, ex_term); //Set mode data
	sgrp_wind_sett(pusr, SGRP_WDNS); //Leave terminal mode
//	sgrp_spri_addt();
//	gjdd_init_inpt(); //initialize input methods
}
