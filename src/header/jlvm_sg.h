/*
 * (c) Jeron A. Lau
 * jl/sg is a library for handling windows
*/

//Platform Declarations
#if defined(__IPHONEOS__) || defined(__ANDROID__)
        #define PLATFORM 1 //ANDROID/IPHONE
#else
        #define PLATFORM 0 //PC/MAC
#endif


// [WINDOW MODE]
#define SGRP_WUPS 0//Run When In Upper Graphical Window
#define SGRP_WDNS 1//Run When In Lower Graphical Window
#define SGRP_TERM 2//Run When In Terminal Window
#define SGRP_EXIT 3//Run When Not In This Program's Window
#define SGRP_MAKS 4

typedef struct{
	uint8_t errf; //Set if error
	float psec; //seconds since last loop
	uint8_t mdec; //Number of Modes
	uint8_t mode; //Current Mode
	uint8_t loop; //[WINDOW MODE] ( terminal,up,down)
	void* pjct; //The library's context
}sgrp_user_t;

//ERROR MESSAGES
#define ERRF_NERR 0 //NO ERROR
#define ERRF_NONE 1 //Something requested is Non-existant
#define ERRF_FIND 2 //Can not find the thing requested
//#define ERRF_NULL //Something requested is empty/null
