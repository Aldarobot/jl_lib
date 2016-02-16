// JL_LIB
	#include "../include/jl.h"
// LIBZIP
	#define ZIP_DISABLE_DEPRECATED //Don't allow the old functions.
	#if JL_PLAT == JL_PLAT_COMPUTER
		#include "../../deps/libzip-1.0.1/lib/zip.h"
	#elif JL_PLAT == JL_PLAT_PHONE
		#include <zip.h>
	#endif

// Standard Libraries
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
// Variable Map
	#include "jl_vm.h"

#define MAXFILELEN 1000 * 100000 //100,000 kb

//resolutions
#define JGR_STN 0 //standard 1280 by 960
#define JGR_LOW 1 //Low Res: 640 by 480
#define JGR_DSI 2 //DSi res: 256 by 192

#define VAR_POSITION 0
#define VAR_COLORS 1
#define VAR_TEXTUREI 2

//Files
#define JL_FILE_SEPARATOR "/"
#define JL_ROOT_DIRNAME "JL_Lib"
#define JL_ROOT_DIR JL_ROOT_DIRNAME JL_FILE_SEPARATOR
#define JL_MAIN_DIR "PlopGrizzly_JLL"
#define JL_MAIN_MEF "media.zip"

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
// target frames per second
#define JL_FPS 60

//USER's Functions
void hack_user_init(jl_t* jlc);
// Media To Include
char *jl_gem(void);
uint32_t jl_gem_size(void);

//OTHER LIB STUFF
void main_resz(jvct_t* _jlc, u16_t x, u16_t y);
void _jl_fl_errf(jvct_t * _jlc, const char *msg);
//GL
void jl_gl_viewport_screen(jvct_t *_jlc);
//
void jl_gl_poly(jvct_t *_jlc, jl_vo_t* pv, uint8_t vertices, const float *xyzw);
void jl_gl_vect(jvct_t *_jlc, jl_vo_t* pv, uint8_t vertices, const float *xyzw);
void jl_gl_clrc(jvct_t *_jlc, jl_vo_t* pv, jl_ccolor_t* cc);
jl_ccolor_t* jl_gl_clrcs(jvct_t *_jlc, u8_t *rgba, uint32_t vc);
jl_ccolor_t* jl_gl_clrcg(jvct_t *_jlc, u8_t *rgba, uint32_t vc);
void jl_gl_clrg(jvct_t *_jlc, jl_vo_t* pv, u8_t *rgba);
void jl_gl_clrs(jvct_t *_jlc, jl_vo_t* pv, u8_t *rgba);
void jl_gl_txtr(jvct_t *_jlc, jl_vo_t* pv, u8_t map, u8_t a, u16_t pgid, u16_t pi);
void jl_gl_txtr_(jvct_t* _jlc, jl_vo_t* pv, u8_t map, u8_t a, u32_t tx);
void jl_gl_transform_pr_(jvct_t *_jlc, jl_pr_t* pr, float x, float y, float z,
	float xm, float ym, float zm);
void jl_gl_transform_vo_(jvct_t *_jlc, jl_vo_t* vo, float x, float y, float z,
	float xm, float ym, float zm);
void jl_gl_transform_chr_(jvct_t *_jlc, jl_vo_t* vo, float x, float y, float z,
	float xm, float ym, float zm);
void jl_gl_draw(jvct_t *_jlc, jl_vo_t* pv);
void jl_gl_draw_chr(jvct_t *_jlc, jl_vo_t* pv,
	m_f32_t r, m_f32_t g, m_f32_t b, m_f32_t a);
void jl_gl_draw_pr_(jl_t* jlc, jl_pr_t* pr);
uint8_t jl_gl_pr_isi_(jvct_t *_jlc, jl_pr_t* pr);
void jl_gl_pr_use(jvct_t *_jlc, jl_vo_t* pv);
void jl_gl_pr_off(jvct_t *_jlc);
void jl_gl_pr_scr(jvct_t *_jlc);

//DL
void _jl_dl_resz(jvct_t *_jlc, uint16_t x, uint16_t y);
void _jl_dl_loop(jvct_t* _jlc);
void _jl_gr_resz(jvct_t* _jlc);
void jl_sg_add_some_imgs_(jvct_t * _jlc, u16_t x);
void _jl_sg_loop(jvct_t *_jlc);
float jl_sg_seconds_past_(jvct_t* _jlc);
void jl_ct_quickloop_(jvct_t* _jlc);
