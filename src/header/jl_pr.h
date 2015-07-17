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
	
// Return Values
enum {
	JL_RTN_SUCCESS, // 0
	JL_RTN_FAIL, // 1
	JL_RTN_IMPOSSIBLE, // 2
	JL_RTN_SUPER_IMPOSSIBLE, // 3
	JL_RTN_COMPLETE_IMPOSSIBLE, // 4
} JL_RTN;

#define MAXFILELEN 1000 * 100000 //100,000 kb

//resolutions
#define JGR_STN 0 //standard 1280 by 960
#define JGR_LOW 1 //Low Res: 640 by 480
#define JGR_DSI 2 //DSi res: 256 by 192

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

//Replacement for NULL
#define STRT_NULL "(NULL)"
//screen frames per second
#define JL_MAIN_SFPS 30
//Allowed Processing Time
#define JL_MAIN_SAPT 1000/JL_MAIN_SFPS

//USER's Functions
void hack_user_init(jl_t* jlc);
//USER's MAIN
char *jal5_head_jlvm(void);
uint32_t jal5_head_size(void);

//OTHER LIB STUFF
void _jl_fl_errf(jvct_t * pjct, char *msg);

//GL
void jl_gl_poly(jvct_t *_jlc, jl_vo* pv, uint8_t vertices, const float *xyzw);
void jl_gl_vect(jvct_t *_jlc, jl_vo* pv, uint8_t vertices, const float *xyzw);
void jl_gl_clrc(jvct_t *_jlc, jl_vo* pv, jl_ccolor_t* cc);
jl_ccolor_t* jl_gl_clrcs(jvct_t *_jlc, uint8_t *rgba, uint32_t vc);
jl_ccolor_t* jl_gl_clrcg(jvct_t *_jlc, uint8_t *rgba, uint32_t vc);
void jl_gl_clrg(jvct_t *_jlc, jl_vo* pv, uint8_t *rgba);
void jl_gl_clrs(jvct_t *_jlc, jl_vo* pv, uint8_t *rgba);
void jl_gl_txtr(jvct_t *_jlc, jl_vo* pv, u08t map, u08t a, u16t pgid, u16t pi);
void jl_gl_translate(jvct_t *_jlc, jl_vo* pv, float x, float y, float z);
void jl_gl_draw(jvct_t *_jlc, jl_vo* pv);
void jl_gl_set_clippane(jvct_t *_jlc, float xmin, float xmax, float ymin, float ymax);
void jl_gl_default_clippane(jvct_t *_jlc);

//DL
void _jl_dl_resize(jvct_t *_jlc, uint16_t x, uint16_t y);

void _jl_gr_flip_scrn(jvct_t *pjct);
