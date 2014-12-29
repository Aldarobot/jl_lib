#ifdef __ANDROID__
#include "header/jlvm.h"
#else
#include "jlvm/jlvm.h"
#endif

#define JLVM_MODE_EDIT 0
#define JLVM_MODE_MAXX 1

jal5_jlvm_t * jal5;

u08t jlvm_loop(dect pf) {
	return 1;
}

u08t jlvm_wind(dect pf) {
	return 1;
}

u08t jlvm_term(dect pf) {
	return 1;
}

void hack_user_init(void) {
	jal5 = jal5_vmap_jlvm(); //Get JLVM function pointers
	jal5->siop.cplo(0,"JVDK","Initializing...\n");
	jal5->sgrp.init(JLVM_MODE_MAXX); //Allocate 1 Mode
	jal5->sgrp.mode(JLVM_MODE_EDIT); //Set Mode To 0
	jal5->sgrp.iset(jlvm_loop, jlvm_wind, jlvm_term);
//	gjdd_init_inpt(); //initialize input methods
}
