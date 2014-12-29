/*
 * (c) Jeron A. Lau
 *
 * INCH: INclude C Header
*/

#include "header/cuhc.h"
#include <stdio.h>

void main(int argc, char *argv[]) {
	printf("[INCH] INclude C Header V0.0\n");
	jal5_cuhc_init();
	jal5_cuhc_conv("umed/genr/jlvm.zip");
	jal5_cuhc_save("umed/genr/main.h");
	printf("[INCH] Done!\n");
}
