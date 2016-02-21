// JL_LIB
	#include "../include/jl.h"
// LIBZIP
	#define ZIP_DISABLE_DEPRECATED //Don't allow the old functions.
	#if JL_PLAT == JL_PLAT_COMPUTER
		#include "zip.h"
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

//Replacement for NULL
#define STRT_NULL "(NULL)"
// target frames per second
#define JL_FPS 60

// Media To Include
char *jl_gem(void);
uint32_t jl_gem_size(void);

//OTHER LIB STUFF
void main_resz(jvct_t* _jlc, u16_t x, u16_t y);

// Main - Prototypes
	float jl_sdl_seconds_past__(jl_t* jlc);
	// RESIZES
	void jl_gr_resz(jl_gr_t* jl_gr, u16_t x, u16_t y);

	// LIB INITIALIZATION fn(Context)
	void _jl_cm_init(jvct_t* _jlc);
	void jl_fl_init__(jvct_t * _jlc);
	jvct_t* _jl_me_init(void);
	void _jl_io_init(jl_t* jlc);

	// LIB KILLS
	void _jl_me_kill(jvct_t* jprg);
	void _jl_fl_kill(jvct_t* _jlc);
	void _jl_io_kill(jl_t* jlc);
