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
	return jlgr->dl.full_w;
}

uint16_t jlgr_wm_geth(jlgr_t* jlgr) {
	return jlgr->dl.full_h;
}

void jlgr_wm_setwindowname(jlgr_t* jlgr, str_t window_name) {
	
}

//STATIC FUNCTIONS
static void jl_dl_killedit(jl_t* jl, char *str) {
	jl_print(jl, str);
	jl_print(jl, SDL_GetError());
	exit(-1);
}

static SDL_Window* jl_dl_mkwindow(jlgr_t* jlgr, u32_t width, u32_t height) {
	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if(jlgr->dl.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	SDL_Window* rtn = SDL_CreateWindow(
		"SDL2 Window",				// window title
		SDL_WINDOWPOS_UNDEFINED,		// initial x position
		SDL_WINDOWPOS_UNDEFINED,		// initial y position
		width, height, flags
    	);
	#if JL_PLAT == JL_PLAT_COMPUTER
	SDL_ShowCursor(SDL_DISABLE);
	#endif
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
	JL_PRINT_DEBUG(jlgr->jl, "Switched fullscreen on/off");
	// Resize window
	jlgr_resz(jlgr, 0, 0);
}

//Update the SDL_displayMode structure
void jl_wm_updatewh_(jlgr_t* jlgr) {
	// Get Window Size
	SDL_GetWindowSize(jlgr->dl.displayWindow->w, &jlgr->dl.full_w,
		&jlgr->dl.full_h);
}

//This is the code that actually creates the window by accessing SDL
static inline void jlgr_wm_create__(jlgr_t* jlgr, u32_t w, u32_t h) {
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
	jlgr->dl.displayWindow->w = jl_dl_mkwindow(jlgr, w, h);
//	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
	jlgr->dl.displayWindow->c = jl_dl_gl_context(jlgr);
	// Clear and update
	jl_gl_clear(jlgr, 2, 255, 5, 255);
	jl_dl_loop__(jlgr);
}

// ETOM FUNCTIONS

int oldtick = 0;
int newtick = 0;

void jl_dl_loop__(jlgr_t* jlgr) {
	//Update Screen
	SDL_GL_SwapWindow(jlgr->dl.displayWindow->w); //end current draw
	// Clear the screen of anything wierd.
	jl_gl_clear(jlgr, 2, 5, 255, 255);
//
	oldtick = newtick;
	newtick = SDL_GetTicks();
	// milliseconds / 1000 to get seconds
	jlgr->psec = ((double)(newtick - oldtick)) / 1000.;
}

void jl_dl_resz__(jlgr_t* jlgr, uint16_t x, uint16_t y) {
	jlgr->dl.full_w = x;
	jlgr->dl.full_h = y;
	jl_gl_viewport_screen(jlgr);
	jlgr->dl.shiftx = 0.;
	jlgr->dl.shifty = 0.;
	jlgr->jl->smde = 0;
//	jlgr->jl->smde = 1; //DOUBLE SCREEN
	jlgr->dl.window.x = 0.;
	jlgr->dl.window.y = 0.;
	jlgr->dl.window.w = x;
	jlgr->dl.window.h = y;
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
	SDL_DisplayMode current;

	SDL_Init(JL_DL_INIT);
	// Get Information On How Big To Make Window
	if(SDL_GetCurrentDisplayMode(0, &current)) {
		jl_print(jlgr->jl, "failed to get current display mode:%s",
			(char *)SDL_GetError());
		jl_sg_kill(jlgr->jl);
	}
	JL_PRINT_DEBUG(jlgr->jl, "Got wh: %d,%d", current.w, current.h);
	// Create Window
	jlgr_wm_create__(jlgr, current.w, current.h);
	// Get Window Size
	jl_wm_updatewh_(jlgr);
	JL_PRINT_DEBUG(jlgr->jl, "size = %dx%d\n", jlgr->dl.full_w,
		jlgr->dl.full_h);
	//Update screensize to fix any rendering glitches
	jl_dl_resz__(jlgr, jlgr->dl.full_w, jlgr->dl.full_h);
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
