#include "bin/jlib/jlvm.h"

#define FIELD_USERNAME 0
#define FIELD_PRGM_NAME 1
#define FIELD_COMPANY_NAME 2
#define FIELD_COMPANY_CITY 3
#define FIELD_COMPANY_PROVINCE 4
#define FIELD_COMPANY_COUNTRY 5
#define FIELD_ANDROID ALIAS 6
#define FIELD_ANDROID_PASSPRHASE 7
#define FIELD_ANDROID_ACTIVIY 8
#define FIELD_MAX 9

#define INFO_DESKTOP "umed/dat.text"

void hack_user_init(sgrp_user_t* pusr) {
	siop_prnt_cplo(0,"ANDROID","Setting up...");
	
	strt fields[FIELD_MAX] = {
		Strt("username"),
		Strt("prgmname"),
		Strt("company_name"),
		Strt("company_city"),
		Strt("company_province"),
		Strt("company_country"),
		Strt("android_alias"),
		Strt("android_passphrase"),
		Strt("android_activity")
	};

	siop_prnt_cplo(0,"ANDROID","...");

	strt list = file_file_load(pusr, INFO_DESKTOP);
	strl prpt = List(FIELD_MAX);
	int i;
	while(1) {
		if(amem_strt_byte(list) == '\0') {
			break;
		}
		for(i = 0; i < FIELD_MAX; i++) {
			if(amem_test_next(list, fields[i])) {
				list->curs += fields[i]->size;
				while(1) {
					list->curs++;
					if(amem_strt_byte(list) == '=') break;
				}
				list->curs++;
				while(1) {
					if(amem_strt_byte(list) != ' ' &&
						amem_strt_byte(list) != '\t')
					{
						break;
					}
					list->curs++;	
				}
				prpt[i] = amem_read_upto(list, '\n', 64);
				siop_prnt_lwst(0,Strt("ANDROID/"),prpt[i]);
				break;
			}
		}
		list->curs++;
	}
	for(i = 0; i < FIELD_MAX; i++) {
		siop_prnt_lwst(0,Strt("ANDROID"),fields[i]);
		siop_prnt_lwst(0,Strt("ANDROID"),prpt[i]);
	}
}
