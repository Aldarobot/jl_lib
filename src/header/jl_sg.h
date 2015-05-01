/*
 * (c) Jeron A. Lau
 * jl/sg is a library for handling windows
*/

//Platform Declarations
#define JL_PLAT_COMPUTER 0 //PC/MAC
#define JL_PLAT_PHONE 1 //ANDROID/IPHONE
#if defined(__IPHONEOS__) || defined(__ANDROID__)
        #define JL_PLAT JL_PLAT_PHONE
#else
        #define JL_PLAT JL_PLAT_COMPUTER
#endif


// [WINDOW MODE]
typedef enum{
	JL_SG_WM_UP, //Run When In Upper Graphical Window
	JL_SG_WM_DN, //Run When In Lower Graphical Window
	JL_SG_WM_TERM, //Run When In Terminal Window
	JL_SG_WM_EXIT, //Run When Not In This Program's Window
	JL_SG_WM_MAX,
}jl_sg_wm_t;

//ERROR MESSAGES
typedef enum{
	JL_ERR_NERR, //NO ERROR
	JL_ERR_NONE, //Something requested is Non-existant
	JL_ERR_FIND, //Can not find the thing requested
	JL_ERR_NULL, //Something requested is empty/null
}jl_err_t;

typedef struct{
	float x, y, w, h;
}jl_rect_t;

typedef struct{
	uint16_t g; //Image group ID
	uint16_t i; //Image Id in group
	jl_rect_t r; //Where & how big to draw sprite
	jl_rect_t cb; //Collision Box
	uint8_t c, a; //How to texture the sprite
	void(* loop)(void * pusr); //Loop function
	void* ctx; //The sprite's context [ for variables]
}jl_sprite_t;

typedef struct{
	uint8_t smde; //Whether 2 or 1 Screens are showing.
	uint32_t info; //@startup:# images loaded from jlvm.zip.Set by others.
	jl_err_t errf; //Set if error
	float psec; //seconds since last loop
	uint8_t mdec; //Number of Modes
	uint8_t mode; //Current Mode
	jl_sg_wm_t loop; //[WINDOW MODE] ( terminal,up,down,exit screen)
	jl_sprite_t *mouse; //Sprite to represent mouse pointer
	void* pjlc; //The library's context
	void* uctx; //The program's context
}jl_t;
