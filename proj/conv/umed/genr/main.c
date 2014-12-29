#include "main.h"
#include <stdint.h>
#include <stdio.h>

char *jal5_head_jlvm(void) {
	return umed_genr_jlvm;
}

uint32_t jal5_head_size(void) {
	printf("aght: %d\n", (int)sizeof(umed_genr_jlvm));
	return sizeof(umed_genr_jlvm);
}
