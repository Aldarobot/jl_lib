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
char * fields[FIELD_MAX] = {
	"username",
	"prgmname",
	"company_name",
	"company_city",
	"company_province",
	"company_country",
	"android_alias",
	"android_passphrase",
	"android_activity"
};

void hack_user_init(sgrp_user_t* pusr) {
	siop_prnt_cplo(0,"ANDROID","Setting up...");
	s08t *list = file_file_load(pusr, INFO_DESKTOP);
	s08t prpt[FIELD_MAX][64];
	int i, j;
	while(1) {
		for(i = 0; i < FIELD_MAX; i++) {
			if(amem_test_next(list, fields[i])) {
				list+=strlen(fields[i]);
				while(1) {
					list++;
					if(list[0] == '=') { break; }
				}
				j = 0;
				while(1) {
					list++;
					if(list[0] == ' ') {
						continue;
					}else{
						prpt[i][j] = list[0];
						j++;
					} 
					if(list[0] == '\n') {
						list[0] = '\0';
						break;
					}
				}
			}
		}
	}
	for(i = 0; i < FIELD_MAX; i++) {
		siop_prnt_cplo(0,"ANDROID",fields[i]);
		siop_prnt_cplo(0,"ANDROID",prpt[i]);
	}
}
