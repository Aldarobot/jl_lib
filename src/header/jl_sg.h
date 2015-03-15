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
#define JL_SG_WM_UP 0//Run When In Upper Graphical Window
#define JL_SG_WM_DN 1//Run When In Lower Graphical Window
#define JL_SG_WM_TERM 2//Run When In Terminal Window
#define JL_SG_WM_EXIT 3//Run When Not In This Program's Window
#define JL_SG_WM_MAX 4

typedef struct{
	uint32_t info; //@startup:# images loaded from jlvm.zip.Set by others.
	uint8_t errf; //Set if error
	float psec; //seconds since last loop
	uint8_t mdec; //Number of Modes
	uint8_t mode; //Current Mode
	uint8_t loop; //[WINDOW MODE] ( terminal,up,down,exit screen)
	void* pjlc; //The library's context
	void* uctx; //The program's context
}jl_t;

//ERROR MESSAGES
#define JL_ERRF_NERR 0 //NO ERROR
#define JL_ERRF_NONE 1 //Something requested is Non-existant
#define JL_ERRF_FIND 2 //Can not find the thing requested
//#define JL_ERRF_NULL //Something requested is empty/null
