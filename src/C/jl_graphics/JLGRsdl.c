/*
 * JLGRsdl.c: upper level SDL2
 *	LSDL AKA. SDL or Simple Direct Media Layer:  SDL is a popular library
 *	for making games.
*/

#include "jl_pr.h"
#include "jl_opengl.h"
#include "JLGRinternal.h"

#define JL_DL_INIT SDL_INIT_AUDIO|SDL_INIT_VIDEO
#define JL_DL_FULLSCREEN SDL_WINDOW_FULLSCREEN_DESKTOP

//PROTOTYPES
static void _jl_dl_fscreen(jl_gr_t* jl_gr, uint8_t a);
static void _jlvm_curd_mode(jl_gr_t* jl_gr);

//EXPORT FUNCTIONS
void jl_dl_setfullscreen(jl_t *jlc, uint8_t is) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	_jl_dl_fscreen(jl_gr, is);
}

void jl_dl_togglefullscreen(jl_t *jlc) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	_jl_dl_fscreen(jl_gr, !jl_gr->dl.fullscreen);
}

uint16_t jl_dl_getw(jl_t *jlc) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	return jl_gr->dl.current.w;
}

uint16_t jl_dl_geth(jl_t *jlc) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	return jl_gr->dl.current.h;
}

//STATIC FUNCTIONS
static void jl_dl_killedit(jl_t* jlc, char *str) {
	jl_io_print(jlc, str);
	jl_io_print(jlc, SDL_GetError());
	jl_sg_kill(jlc);
}

static SDL_Window* jl_dl_mkwindow(jl_gr_t* jl_gr) {
	SDL_Window* rtn = SDL_CreateWindow(
		"SDL2 Window",				// window title
		SDL_WINDOWPOS_UNDEFINED,		// initial x position
		SDL_WINDOWPOS_UNDEFINED,		// initial y position
		jl_gr->dl.current.w, jl_gr->dl.current.h,	// width and height
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE//| SDL_WINDOW_FULLSCREEN
    	);
	if(rtn == NULL) jl_dl_killedit(jl_gr->jlc, "SDL_CreateWindow");
	return rtn;
}

static SDL_GLContext* jl_dl_gl_context(jl_gr_t* jl_gr) {
	SDL_GLContext* rtn = SDL_GL_CreateContext(jl_gr->dl.displayWindow->w);
	if(rtn == NULL) jl_dl_killedit(jl_gr->jlc, "SDL_GL_CreateContext");
	return rtn;
}

// Set fullscreen or not.
static void _jl_dl_fscreen(jl_gr_t* jl_gr, uint8_t a) {
	// Make sure the fullscreen value is either a 1 or a 0.
	jl_gr->dl.fullscreen = !!a;
	// Actually set whether fullscreen or not.
	if(SDL_SetWindowFullscreen(jl_gr->dl.displayWindow->w,
	 JL_DL_FULLSCREEN * jl_gr->dl.fullscreen))
		jl_dl_killedit(jl_gr->jlc, "SDL_SetWindowFullscreen");
	// Resize window
	_jlvm_curd_mode(jl_gr);
	main_resz(jl_gr->jlc->_jlc, jl_gr->dl.current.w, jl_gr->dl.current.h);
}

static inline void jlvmpi_ini_sdl(jl_gr_t* jl_gr) {
	jl_io_function(jl_gr->jlc, "InitSDL"); // {
	JL_IO_DEBUG(jl_gr->jlc, "Starting up....");
	SDL_Init(JL_DL_INIT);
	JL_IO_DEBUG(jl_gr->jlc, "input....");
	#if JL_PLAT == JL_PLAT_COMPUTER
	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	#endif
	jl_io_return(jl_gr->jlc, "InitSDL"); // }
}

//Update the SDL_displayMode structure
static void _jlvm_curd_mode(jl_gr_t* jl_gr) {
	if(SDL_GetCurrentDisplayMode(0, &jl_gr->dl.current)) {
		jl_io_print(jl_gr->jlc, "failed to get current display mode:%s",
			(char *)SDL_GetError());
		jl_sg_kill(jl_gr->jlc);
	}
	jl_io_function(jl_gr->jlc, "SDL_cdm");
	JL_IO_DEBUG(jl_gr->jlc, "%d,%d", jl_gr->dl.current.w,
		jl_gr->dl.current.h);
	jl_io_return(jl_gr->jlc, "SDL_cdm");
}

//This is the code that actually creates the window by accessing SDL
static inline void _jlvm_crea_wind(jl_gr_t* jl_gr) {
	#if defined(__ANDROID__)
	jl_gr->dl.current.w = 640;
	jl_gr->dl.current.h = 480;
	#elif defined(__PSP__)
	jl_gr->dl.current.w = 480;
	jl_gr->dl.current.h = 272;
	#endif
	
	// Allocate space for "displayWindow"
	jl_me_alloc(jl_gr->jlc, (void**)&jl_gr->dl.displayWindow,
		sizeof(jl_window_t), 0);
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
	jl_gr->dl.displayWindow->w = jl_dl_mkwindow(jl_gr);
//	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
	
	jl_gr->dl.displayWindow->c = jl_dl_gl_context(jl_gr);
	// Clear and update
	jl_gl_clear(jl_gr, 2, 255, 5, 255);
	jl_dl_loop__(jl_gr);
#if JL_GLRTEX == JL_GLRTEX_SDL
	jl_gr->dl.whichwindow = 0;
#endif
	jl_gr->dl.fullscreen = 0;
}

//NON-STATIC FUNCTIONS
// ETOM FUNCTIONS

#if JL_GLRTEX == JL_GLRTEX_SDL

void jl_dl_screen_(jl_gr_t* jl_gr, jl_window_t* which) {
	if(jl_gr->dl.whichwindow != which) {
		SDL_GL_MakeCurrent(which->w, jl_gr->dl.displayWindow->c);
		jl_gr->dl.whichwindow = which;
	}
}

jl_window_t* jl_dl_screen_new_(jl_gr_t* jl_gr, u16_t w, u16_t h) {
	jl_window_t* window = NULL;

	// Allocate space for "window"
	jl_me_alloc(jl_gr->jlc, (void**)&window, sizeof(jl_window_t), 0);
	// Create the window.
	window->w = SDL_CreateWindow(
		"¡Hidden!",				// window title
		10, 10,					// initial (x,y)
		w, h,					// width & height
		SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
	);
	// Create Shared Context
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	window->c = SDL_GL_CreateContext(window->w);
	// Set display window as default.
	SDL_RaiseWindow(jl_gr->dl.displayWindow->w);
	// Return the new window.
	return window;
}

#endif

void jl_dl_loop__(jl_gr_t* jl_gr) {
	//Update Screen
	SDL_GL_SwapWindow(jl_gr->dl.displayWindow->w); //end current draw
	// Clear the screen of anything wierd.
	jl_gl_clear(jl_gr, 2, 5, 255, 255);
}

void jl_dl_resz__(jl_gr_t* jl_gr, uint16_t x, uint16_t y) {
	uint16_t offx = 0;
	uint16_t offy = 0;

	jl_gr->dl.full_w = x;
	jl_gr->dl.full_h = y;
	jl_gl_viewport_screen(jl_gr);
	jl_gr->dl.shiftx = 0.;
	jl_gr->dl.shifty = 0.;
	if(y < x * .5625) {
		jl_gr->jlc->smde = 0;
		offx = x;
		x = y * 16./9.; //Widesceen
		jl_gr->dl.shiftx += ((float)offx - (float)x)/((float)offx);
		offx = (offx - x) / 2;
		jl_gr->dl.window.x = offx;
		jl_gr->dl.window.y = 0.;
		jl_gr->dl.window.w = x;
		jl_gr->dl.window.h = y;
	}else if(y > x * 1.125) {//DOUBLE SCREEN
		jl_gr->jlc->smde = 1;
		if(y > x * 1.5) {
			offy = y;
			y = x * 1.5; //Standard
			jl_gr->dl.shifty += ((float)offy-(float)y)/((float)offy);
			jl_gr->dl.window.x = 0.;
			jl_gr->dl.window.y = (offy) / 2.;
			jl_gr->dl.window.w = x;
			jl_gr->dl.window.h = y / 2.;
		}else{
			offy = y;
			offy = (offy - y) / 2;
			jl_gr->dl.window.x = 0.;
			jl_gr->dl.window.y = y / 2.;
			jl_gr->dl.window.w = x;
			jl_gr->dl.window.h = y / 2.;
		}
	}else if(y > x * .75) {
		jl_gr->jlc->smde = 0;
		offy = y;
		y = x * .75; //Standard
		jl_gr->dl.shifty += ((float)offy-(float)y)/((float)offy);
		offy = (offy - y) / 2;
		jl_gr->dl.window.x = 0.;
		jl_gr->dl.window.y = offy;
		jl_gr->dl.window.w = x;
		jl_gr->dl.window.h = y;
	}else{
		jl_gr->jlc->smde = 0;
		jl_gr->dl.window.x = 0.;
		jl_gr->dl.window.y = 0.;
		jl_gr->dl.window.w = x;
		jl_gr->dl.window.h = y;
	}
	jl_gr->dl.current.w = x;
	jl_gr->dl.current.h = y + (x - y);
	// Set GL aspect.
	jl_gr->dl.aspect = ((double)y) / ((double)x);
	// Clear the screen of anything wierd.
	jl_gl_clear(jl_gr, 122, 255, 125, 255);
}

/**
 * Set the program title.  Used as window name, and as resource
 * directory.
 * @param jlc: The library context.
 * @param name: The name of the program.
*/
void jl_dl_progname(jl_t* jlc, strt name) {
	jl_gr_t* jl_gr = jlc->jl_gr;
	int ii;
	for(ii = 0; ii < 16; ii++) {
		jl_gr->dl.windowTitle[0][ii] = name->data[ii];
		if(name->data[ii] == '\0') { break; }
	}
	jl_gr->dl.windowTitle[0][15] = '\0';
#if JL_PLAT == JL_PLAT_COMPUTER
	if(jl_gr->dl.displayWindow)
	 SDL_SetWindowTitle(jl_gr->dl.displayWindow->w, jl_gr->dl.windowTitle[0]);
#endif
}

void jl_dl_init__(jl_gr_t* jl_gr) {
	jlvmpi_ini_sdl(jl_gr);
	//Get Information On How Big To Make Window
	_jlvm_curd_mode(jl_gr);
	//Create Window With SDL
	_jlvm_crea_wind(jl_gr);
	//Get Window Size
	_jlvm_curd_mode(jl_gr);
	//Update screensize to fix any rendering glitches
	jl_dl_resz__(jl_gr, jl_gr->dl.current.w, jl_gr->dl.current.h);
	// Update The Screen
	jl_dl_loop__(jl_gr);
}

void jl_dl_kill__(jl_gr_t* jl_gr) {
	JL_IO_DEBUG(jl_gr->jlc, "killing SDL....");
	if (jl_gr->dl.displayWindow->c != NULL) {
		SDL_free(jl_gr->dl.displayWindow->c);
		SDL_free(jl_gr->dl.displayWindow->w);
	}
	JL_IO_DEBUG(jl_gr->jlc, "killed SDL!");
}
