#include <stdint.h>
#include <stdio.h>
#include "main.h"

char *jal5_head_jlvm(void) {
	return GEN_jlvm;
}

uint32_t jal5_head_size(void) {
	printf("aght: %d\n", (int)sizeof(GEN_jlvm));
	return sizeof(GEN_jlvm);
}
