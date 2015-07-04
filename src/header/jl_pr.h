//Jal5 Standard Libraries
	#include "jl.h"
	#if JL_PLAT == JL_PLAT_COMPUTER
		//#define JLVM_USEL_COGL
		#define JLVM_USEL_GLES
	#elif JL_PLAT == JL_PLAT_PHONE
		#define JLVM_USEL_GLES
	#else
	#endif
	
//SDL
	#if JL_PLAT == JL_PLAT_COMPUTER
//		#include "../lib/SDL/header/SDL.h"
		#include <SDL2/SDL.h>
		#ifdef JLVM_USEL_COGL
//			#include "../lib/SDL/header/SDL_opengl.h"
			#include "../lib/glew/glew.h"
		#endif
		#ifdef JLVM_USEL_GLES
			#include <SDL2/SDL_opengles2.h>
		#endif
	#elif JL_PLAT == JL_PLAT_PHONE
		#include "../lib/sdl/header/SDL_test_common.h"
		#include "../lib/sdl/header/SDL_opengles2.h"
		#include "../lib/sdl/header/SDL_main.h"
	#endif
	#include "../lib/SDL_mixer.h"
//LIBZIP
	#define ZIP_DISABLE_DEPRECATED //Don't allow the old functions.
	#if JL_PLAT == JL_PLAT_COMPUTER
		#include "../../../../obj/lb/zip.h"
	#elif JL_PLAT == JL_PLAT_PHONE
		#include "../lib/libzip-0.11.2/lib/zip.h"
	#endif
//OPENGL
	#ifdef JLVM_USEL_COGL
		#ifdef APPLE
			#include <OpenGL/glext.h>
		#elif JL_PLAT == JL_PLAT_COMPUTER
//			#include <GL/glext.h>
			#include "../lib/glext.h"
		#else
			#error "cogl ain't supported by non-pc comps, man!"
		#endif
	#endif
	#ifdef JLVM_USEL_GLES
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
	#endif
//Standard Libraries
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <string.h>
	#include <stdlib.h>
	#include <math.h>
	#include <dirent.h>
	
	#include "jl_vm.h" //Variable Map

#define MAXFILELEN 1000 * 100000 //100,000 kb

//resolutions
#define JGR_STN 0 //standard 1280 by 960
#define JGR_LOW 1 //Low Res: 640 by 480
#define JGR_DSI 2 //DSi res: 256 by 192

//Buffer ID's
//permanent
#define BUFFER_TEXTURE 1
//changeable
#define BUFFER_CHARECTER_MAP_TEXTURE 2
#define BUFFER_SPRITE 3
#define BUFFER_TEXT 4
#define BUFFER_OBJECT 5 //3D
#define BUFFER_MAX 6

#define VAR_POSITION 0
#define VAR_COLORS 1
#define VAR_TEXTUREI 2

//Files
#if JL_PLAT == JL_PLAT_COMPUTER
	#define JLVM_FILEBASE "../\0"
#elif JL_PLAT == JL_PLAT_PHONE
	#define JLVM_FILEBASE "/storage/sdcard0/JLVM/\0"
#endif
#define JLVM_INIT SDL_INIT_AUDIO|SDL_INIT_VIDEO

#ifdef SHADERS

//SHADER DATA
	#define IMGID_MAX 16

	//RENDER DATA
	#define TEX_BRIGHT 0 //Bright colors
	#define TEX_COLORFUL 1 //Colorful blending
	#define TEX_DARK 2 //Darkenned blending
	#define TEX_MOVE 3 //Animated version of bright
	#define TEX_SIMPLE 4 //Displays one color
	#define TEX_NO 5 //Don't Draw
	GLfloat *GOne;
	GLfloat *GColor;
	uint32_t *GTex;

#endif
#ifdef VIRTUAL_MACH_DAT
//VIRTUAL MACHINE DATA
	typedef struct{
		//type(port/program/library) and Identification
		uint8_t processType;
		uint64_t processId;
		uint8_t *processTitle;//25 letters
		uint8_t processTid[5];//1 byte lang/4 byte(charecter) tid
		//shouldn't change
		uint32_t StartOfLoadedMemory;
		uint32_t LocationOfKillInFile;//to load class when quiting
		//for main loop(never out of memory unless quitting)
		uint32_t LocationOfLoopInLoadedMemory;
		uint32_t LocationOfRdUpInLoadedMemory;
		uint32_t LocationOfRdDnInLoadedMemory;
		//
		uint32_t EndOfLoadedMemory;
	}process_info_t;

	int i = 0;
	//uint8_t memory[] = { x, y, z, ... };
	process_info_t processInfos[32];
	uint8_t processCount = 1;
	uint8_t GTerminal = 1;
#endif

#define STRT_NULL "(NULL)"

//screen frames per second
#define JL_MAIN_SFPS 30
//Allowed Processing Time
#define JL_MAIN_SAPT 1000/JL_MAIN_SFPS

typedef struct{
	
}_jal5_sgrp_main_data_t;

typedef struct{
	u32t size; //Size of "data" array
	_jal5_sgrp_main_data_t *data;
}jal5_sgrp_main_data_t;

//USER's Functions
void hack_user_init(jl_t* pusr);
//USER's MAIN
char *jal5_head_jlvm(void);
uint32_t jal5_head_size(void);

//OTHER LIB STUFF
void _jl_fl_errf(jvct_t * pjct, char *msg);

void jl_gl_poly(jvct_t *pjlc, jl_vo* pv, uint8_t vertices, float *xyzw);
void jl_gl_clrg(jvct_t *pjlc, jl_vo* pv, uint8_t *rgba);
void jl_gl_clrs(jvct_t *pjlc, jl_vo* pv, uint8_t *rgba);
void jl_gl_txtr(jvct_t *pjlc, jl_vo* pv, u08t map, u08t a, u16t pgid, u16t pi);
void jl_gl_draw(jvct_t *pjlc, jl_vo* pv);
void jl_gl_set_clippane(jvct_t *pjlc, float xmin, float xmax, float ymin, float ymax);
void jl_gl_default_clippane(jvct_t *pjlc);

//void jal5_eogl_draw(uint8_t vertices, uint8_t map, float *xyz);

void _jal5_lsdl_glpt_view(jvct_t *pjct, u16t x, u16t y);

float jal5_inpt_xmse(void);
float jal5_inpt_ymse(void);

void _jl_gr_flip_scrn(jvct_t *pjct);
