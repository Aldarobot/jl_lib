/*
 * simple example program that starts up and immediately quits.  Good for making
 * script-like programs.
*/
/*#include "../../../bin/jlvm/jlib/jl.h"

void hack_user_init(void) {
}*/

/*----------------------------------------------------------------------------*/

/*
 * Simple example program that creates a loop.  Good for most programs.
*/
#include "header/main.h"
#include <string.h>

void ex_down(jl_t* pusr) {
	if(pusr->ctrl.h == 1) jl_gr_togglemenubar(pusr);
}

void ex_loop(jl_t* pusr) {
	jl_ct_run_event(pusr,
		JL_CT_ALLP(JL_CT_GAME_STRT, JL_CT_COMP_MENU, JL_CT_ANDR_MENU),
		ex_down, jl_dont);
}

void ex_wdns(jl_t* pusr) {
	ex_loop(pusr);
	jl_gr_draw_text(pusr, "testing...1234567890", 0.f, 1.02f, .0625f, 255);
}

void ex_wups(jl_t* pusr) {
	if(!pusr->smde) ex_loop(pusr);
	jl_gr_draw_text(pusr, "this IS upper", 0.f, 0.f, .0625f, 255);
}

void ex_term(jl_t* pusr) {

}

void ex_exit(jl_t* pusr) {
	jl_sg_exit(pusr);
}

static inline void ex_init_modes(jl_t* pusr) {
	//Set mode data
	jl_sg_mode_set(pusr,EX_MODE_EDIT, ex_exit, ex_wups, ex_wdns, ex_term);
	jl_sg_mode_switch(pusr, EX_MODE_EDIT, JL_SG_WM_DN); //Leave terminal mode
}

static inline void ex_init_tasks(jl_t* pusr) {
	jl_gr_addicon_slow(pusr);
	jl_gr_addicon_flip(pusr);
	jl_gr_addicon_name(pusr);
}

void test_jl_me_strt_strt(jl_t* pusr, strt test_a, strt test_b) {
	jl_io_printc(pusr,"Checking jl_me_strt_strt()....");
	test_a->curs = 2;
	test_b->curs = 2;
	jl_me_strt_strt(pusr, test_a, test_b, 1);
	char *ctest_a = jl_me_string_fstrt(pusr, test_a);
	if(strcmp(ctest_a, "HERLO")) {
		jl_io_printc(pusr,"=jl_me_strt_strt(pusr, { data=\"HELLO\", curs=2 },"
		" { \"WORLD\", curs=2 }, 2);");
		jl_io_printc(pusr, "Should be \"HERLO\" is");
		jl_io_printc(pusr, "\"");
		jl_io_printc(pusr, ctest_a);
		jl_io_printc(pusr, "\"\n");
		jl_sg_kill(pusr, "jl_me_strt_strt failed!");
	}else{
		jl_io_printc(pusr, "success=");
		jl_io_printc(pusr, ctest_a);
		jl_io_printc(pusr, "\n");
	}
}

void hack_user_init(jl_t* pusr) {
	strt test_a;
	strt test_b;

	jl_io_tag_set(pusr, 0, 1, NULL);
	jl_io_offset(pusr, 0, "EXMP");
	jl_gr_draw_msge(pusr, "Initializing....");
	jl_io_printc(pusr,"Initializing....\n");
	ex_init_modes(pusr);
	ex_init_tasks(pusr);
	jl_io_close_block(pusr);
	jl_io_printc(pusr,"Checking jl_me_strt_c8ts()....\n");
	test_a = jl_me_strt_c8ts("HELLO");
	test_b = jl_me_strt_c8ts("WORLD");
	jl_io_printc(pusr,"Checking jl_me_string_fstrt()....\n");
	test_jl_me_strt_strt(pusr, test_a, test_b);
}
