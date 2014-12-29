/*
 * (c) Jeron A. Lau
 * jlvm_sgrp is a library for handling windows
*/

// [WINDOW MODE]
#define SGRP_WUPS 0//Run When In Upper Graphical Window
#define SGRP_WDNS 1//Run When In Lower Graphical Window
#define SGRP_TERM 2//Run When In Terminal Window
#define SGRP_EXIT 3//Run When Not In This Program's Window
#define SGRP_MAKS 4

typedef struct{
	float psec; //seconds since last loop
	uint8_t mdec; //Number of Modes
	uint8_t mode; //Current Mode
	uint8_t loop; //[WINDOW MODE] ( terminal,up,down)
	void* pjct; //The library's context
}sgrp_user_t;
