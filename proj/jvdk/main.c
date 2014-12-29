#include "jlvm/jlvm.h"

#define JVDK_MODE_EDIT 0
#define JVDK_MODE_MAXX 1

u08t jvdk_loop(dect pf) {
	return 1;
}

u08t jvdk_wind(dect pf) {
	return 1;
}

u08t jvdk_term(dect pf) {
	return 1;
}

void hack_user_init(void) {
	siop_prnt_cplo(0,"JVDK","Initializing...\n");
	sgrp_mode_init(JVDK_MODE_MAXX); //Allocate 1 Mode
	sgrp_mode_sett(JVDK_MODE_EDIT); //Set Mode To 0
	sgrp_mode_iset(jvdk_loop, jvdk_wind, jvdk_term);
//	gjdd_init_inpt(); //initialize input methods
}
