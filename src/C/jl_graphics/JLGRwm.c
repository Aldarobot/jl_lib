/*
 * JLGRwm.c: upper level SDL2
 *	Window Manager - Manages creating / destroying / redrawing windows.
*/

#include "JLGRinternal.h"
#include "jl_opengl.h"

#define JL_DL_INIT SDL_INIT_AUDIO|SDL_INIT_VIDEO
#define JL_DL_FULLSCREEN SDL_WINDOW_FULLSCREEN_DESKTOP

//PROTOTYPES
static void _jl_dl_fscreen(jlgr_t* jlgr, uint8_t a);

//EXPORT FUNCTIONS
void jlgr_wm_setfullscreen(jlgr_t* jlgr, uint8_t is) {
	_jl_dl_fscreen(jlgr, is);
}

void jlgr_wm_togglefullscreen(jlgr_t* jlgr) {
	_jl_dl_fscreen(jlgr, !jlgr->dl.fullscreen);
}

uint16_t jlgr_wm_getw(jlgr_t* jlgr) {
	return jlgr->dl.current.w;
}

uint16_t jlgr_wm_geth(jlgr_t* jlgr) {
	return jlgr->dl.current.h;
}

void jlgr_wm_setwindowname(jlgr_t* jlgr, str_t window_name) {
	
}

//STATIC FUNCTIONS
static void jl_dl_killedit(jl_t* jl, char *str) {
	jl_print(jl, str);
	jl_print(jl, SDL_GetError());
	exit(-1);
}

static SDL_Window* jl_dl_mkwindow(jlgr_t* jlgr) {
	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if(jlgr->dl.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	SDL_Window* rtn = SDL_CreateWindow(
		"SDL2 Window",				// window title
		SDL_WINDOWPOS_UNDEFINED,		// initial x position
		SDL_WINDOWPOS_UNDEFINED,		// initial y position
		// width, height, flags
		jlgr->dl.current.w, jlgr->dl.current.h, flags
    	);
	if(rtn == NULL) jl_dl_killedit(jlgr->jl, "SDL_CreateWindow");
	return rtn;
}

static SDL_GLContext* jl_dl_gl_context(jlgr_t* jlgr) {
	SDL_GLContext* rtn = SDL_GL_CreateContext(jlgr->dl.displayWindow->w);
	if(rtn == NULL) jl_dl_killedit(jlgr->jl, "SDL_GL_CreateContext");
	return rtn;
}

// Set fullscreen or not.
static void _jl_dl_fscreen(jlgr_t* jlgr, uint8_t a) {
	// Make sure the fullscreen value is either a 1 or a 0.
	jlgr->dl.fullscreen = !!a;
	// Actually set whether fullscreen or not.
	if(SDL_SetWindowFullscreen(jlgr->dl.displayWindow->w,
	 JL_DL_FULLSCREEN * jlgr->dl.fullscreen))
		jl_dl_killedit(jlgr->jl, "SDL_SetWindowFullscreen");
	// Resize window
	jl_wm_updatewh_(jlgr);
	jlgr_resz(jlgr, jlgr->dl.current.w, jlgr->dl.current.h);
}

static inline void jlvmpi_ini_sdl(jlgr_t* jlgr) {
	jl_print_function(jlgr->jl, "InitSDL"); // {
	JL_PRINT_DEBUG(jlgr->jl, "Starting up....");
	SDL_Init(JL_DL_INIT);
	JL_PRINT_DEBUG(jlgr->jl, "input....");
	#if JL_PLAT == JL_PLAT_COMPUTER
	SDL_ShowCursor(SDL_DISABLE);
	#endif
	jl_print_return(jlgr->jl, "InitSDL"); // }
}

//Update the SDL_displayMode structure
void jl_wm_updatewh_(jlgr_t* jlgr) {
	if(SDL_GetCurrentDisplayMode(0, &jlgr->dl.current)) {
		jl_print(jlgr->jl, "failed to get current display mode:%s",
			(char *)SDL_GetError());
		jl_sg_kill(jlgr->jl);
	}
	JL_PRINT_DEBUG(jlgr->jl, "%d,%d", jlgr->dl.current.w,
		jlgr->dl.current.h);
}

//This is the code that actually creates the window by accessing SDL
static inline void _jlvm_crea_wind(jlgr_t* jlgr) {
	#if defined(__ANDROID__)
	jlgr->dl.current.w = 640;
	jlgr->dl.current.h = 480;
	#elif defined(__PSP__)
	jlgr->dl.current.w = 480;
	jlgr->dl.current.h = 272;
	#endif
	
	// Allocate space for "displayWindow"
	jlgr->dl.displayWindow = jl_memi(jlgr->jl, sizeof(jl_window_t));
	//
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	// Create window.
	jlgr->dl.displayWindow->w = jl_dl_mkwindow(jlgr);
//	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
	
	jlgr->dl.displayWindow->c = jl_dl_gl_context(jlgr);
	// Clear and update
	jl_gl_clear(jlgr, 2, 255, 5, 255);
	jl_dl_loop__(jlgr);
}

// ETOM FUNCTIONS

void jl_dl_loop__(jlgr_t* jlgr) {
	//Update Screen
	SDL_GL_SwapWindow(jlgr->dl.displayWindow->w); //end current draw
	// Clear the screen of anything wierd.
	jl_gl_clear(jlgr, 2, 5, 255, 255);
}

void jl_dl_resz__(jlgr_t* jlgr, uint16_t x, uint16_t y) {
	uint16_t offx = 0;
	uint16_t offy = 0;

	jlgr->dl.full_w = x;
	jlgr->dl.full_h = y;
	jl_gl_viewport_screen(jlgr);
	jlgr->dl.shiftx = 0.;
	jlgr->dl.shifty = 0.;
	if(y < x * .5625) {
		jlgr->jl->smde = 0;
		offx = x;
		x = y * 16./9.; //Widesceen
		jlgr->dl.shiftx += ((float)offx - (float)x)/((float)offx);
		offx = (offx - x) / 2;
		jlgr->dl.window.x = offx;
		jlgr->dl.window.y = 0.;
		jlgr->dl.window.w = x;
		jlgr->dl.window.h = y;
	}else if(y > x * 1.125) {//DOUBLE SCREEN
		jlgr->jl->smde = 1;
		if(y > x * 1.5) {
			offy = y;
			y = x * 1.5; //Standard
			jlgr->dl.shifty += ((float)offy-(float)y)/((float)offy);
			jlgr->dl.window.x = 0.;
			jlgr->dl.window.y = (offy) / 2.;
			jlgr->dl.window.w = x;
			jlgr->dl.window.h = y / 2.;
		}else{
			offy = y;
			offy = (offy - y) / 2;
			jlgr->dl.window.x = 0.;
			jlgr->dl.window.y = y / 2.;
			jlgr->dl.window.w = x;
			jlgr->dl.window.h = y / 2.;
		}
	}else if(y > x * .75) {
		jlgr->jl->smde = 0;
		offy = y;
		y = x * .75; //Standard
		jlgr->dl.shifty += ((float)offy-(float)y)/((float)offy);
		offy = (offy - y) / 2;
		jlgr->dl.window.x = 0.;
		jlgr->dl.window.y = offy;
		jlgr->dl.window.w = x;
		jlgr->dl.window.h = y;
	}else{
		jlgr->jl->smde = 0;
		jlgr->dl.window.x = 0.;
		jlgr->dl.window.y = 0.;
		jlgr->dl.window.w = x;
		jlgr->dl.window.h = y;
	}
	jlgr->dl.current.w = x;
	jlgr->dl.current.h = y + (x - y);
	// Set GL aspect.
	jlgr->dl.aspect = ((double)y) / ((double)x);
	// Clear the screen of anything wierd.
	jl_gl_clear(jlgr, 122, 255, 125, 255);
}

// TODO: Make not exported, but called in jlgr_init()
/**
 * Set the program title.  Used as window name, and as resource
 * directory.
 * @param jl: The library context.
 * @param name: The name of the program.
*/
void jl_dl_progname(jl_t* jl, data_t* name) {
	jlgr_t* jlgr = jl->jlgr;
	int ii;
	for(ii = 0; ii < 16; ii++) {
		jlgr->dl.windowTitle[0][ii] = name->data[ii];
		if(name->data[ii] == '\0') { break; }
	}
	jlgr->dl.windowTitle[0][15] = '\0';
#if JL_PLAT == JL_PLAT_COMPUTER
	if(jlgr->dl.displayWindow)
	 SDL_SetWindowTitle(jlgr->dl.displayWindow->w, jlgr->dl.windowTitle[0]);
#endif
}

void jl_dl_init__(jlgr_t* jlgr) {
	jlvmpi_ini_sdl(jlgr);
	//Get Information On How Big To Make Window
	jl_wm_updatewh_(jlgr);
	//Create Window With SDL
	_jlvm_crea_wind(jlgr);
	//Get Window Size
	jl_wm_updatewh_(jlgr);
	//Update screensize to fix any rendering glitches
	jl_dl_resz__(jlgr, jlgr->dl.current.w, jlgr->dl.current.h);
	// Update The Screen
	jl_dl_loop__(jlgr);
}

void jl_dl_kill__(jlgr_t* jlgr) {
	JL_PRINT_DEBUG(jlgr->jl, "killing SDL....");
	if (jlgr->dl.displayWindow->c != NULL) {
//		SDL_free(jlgr->dl.displayWindow->c);
		SDL_free(jlgr->dl.displayWindow->w);
	}
	JL_PRINT_DEBUG(jlgr->jl, "killed SDL!");
}
