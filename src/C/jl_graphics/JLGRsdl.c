/*
 * JLGRsdl.c: upper level SDL2
 *	LSDL AKA. SDL or Simple Direct Media Layer:  SDL is a popular library
 *	for making games.
*/

#include "jl_pr.h"
#include "jl_opengl.h"

#define JL_DL_INIT SDL_INIT_AUDIO|SDL_INIT_VIDEO
#define JL_DL_FULLSCREEN SDL_WINDOW_FULLSCREEN_DESKTOP

//PROTOTYPES
static void _jl_dl_fscreen(jvct_t* _jlc, uint8_t a);
static void _jlvm_curd_mode(jvct_t *_jlc);

//EXPORT FUNCTIONS
void jl_dl_setfullscreen(jl_t *jlc, uint8_t is) {
	jvct_t* _jlc = jlc->_jlc;
	_jl_dl_fscreen(_jlc, is);
}

void jl_dl_togglefullscreen(jl_t *jlc) {
	jvct_t* _jlc = jlc->_jlc;
	_jl_dl_fscreen(_jlc, !_jlc->dl.fullscreen);
}

uint16_t jl_dl_getw(jl_t *jlc) {
	jvct_t* _jlc = jlc->_jlc;
	return _jlc->dl.current.w;
}

uint16_t jl_dl_geth(jl_t *jlc) {
	jvct_t* _jlc = jlc->_jlc;
	return _jlc->dl.current.h;
}

//STATIC FUNCTIONS
static void jl_dl_killedit(jl_t* jlc, char *str) {
	jl_io_print(jlc, str);
	jl_io_print(jlc, SDL_GetError());
	jl_sg_kill(jlc);
}

static SDL_Window* jl_dl_mkwindow(jvct_t* _jlc) {
	SDL_Window* rtn = SDL_CreateWindow(
		"SDL2 Window",				// window title
		SDL_WINDOWPOS_UNDEFINED,		// initial x position
		SDL_WINDOWPOS_UNDEFINED,		// initial y position
		_jlc->dl.current.w, _jlc->dl.current.h,	// width and height
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    	);
	if(rtn == NULL) jl_dl_killedit(_jlc->jlc, "SDL_CreateWindow");
	return rtn;
}

static SDL_GLContext* jl_dl_gl_context(jvct_t* _jlc) {
	SDL_GLContext* rtn = SDL_GL_CreateContext(_jlc->dl.displayWindow->w);
	if(rtn == NULL) jl_dl_killedit(_jlc->jlc, "SDL_GL_CreateContext");
	return rtn;
}

static void _jl_dl_fscreen(jvct_t* _jlc, uint8_t a) {
	// Make sure the fullscreen value is either a 1 or a 0.
	_jlc->dl.fullscreen = !!a;
	// Actually set whether fullscreen or not.
	if(SDL_SetWindowFullscreen(_jlc->dl.displayWindow->w,
	 JL_DL_FULLSCREEN * _jlc->dl.fullscreen))
		jl_dl_killedit(_jlc->jlc, "SDL_SetWindowFullscreen");
}

static inline void jlvmpi_ini_sdl(jvct_t* _jlc) {
	jl_io_function(_jlc->jlc, "InitSDL"); // {
	jl_io_print(_jlc->jlc, "Starting up....");
	SDL_Init(JL_DL_INIT);
	jl_io_print(_jlc->jlc, "input....");
	#if JL_PLAT == JL_PLAT_COMPUTER
	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	#endif
	jl_io_return(_jlc->jlc, "InitSDL"); // }
}

//Update the SDL_displayMode structure
static void _jlvm_curd_mode(jvct_t *_jlc) {
	if(SDL_GetCurrentDisplayMode(0, &_jlc->dl.current)) {
		jl_io_print(_jlc->jlc, "failed to get current display mode:%s",
			(char *)SDL_GetError());
		jl_sg_kill(_jlc->jlc);
	}
	jl_io_function(_jlc->jlc, "SDL_cdm");
	jl_io_print(_jlc->jlc, "%d,%d", _jlc->dl.current.w, _jlc->dl.current.h);
	jl_io_return(_jlc->jlc, "SDL_cdm");
}

//This is the code that actually creates the window by accessing SDL
static inline void _jlvm_crea_wind(jvct_t *_jlc) {
	#if defined(__ANDROID__)
	_jlc->dl.current.w = 640;
	_jlc->dl.current.h = 480;
	#elif defined(__PSP__)
	_jlc->dl.current.w = 480;
	_jlc->dl.current.h = 272;
	#endif
	
	// Allocate space for "displayWindow"
	jl_me_alloc(_jlc->jlc, (void**)&_jlc->dl.displayWindow,
		sizeof(jl_window_t), 0);
	//
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Create window.
	_jlc->dl.displayWindow->w = SDL_CreateWindow(
		"¡SDL2 Window!",			// window title
		SDL_WINDOWPOS_UNDEFINED,		// initial x position
		SDL_WINDOWPOS_UNDEFINED,		// initial y position
		_jlc->dl.current.w, _jlc->dl.current.h,	// width and height
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN
    	);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	// Create window.
	_jlc->dl.displayWindow->w = jl_dl_mkwindow(_jlc);
//	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
	
	_jlc->dl.displayWindow->c = jl_dl_gl_context(_jlc);
	// Clear and update
	jl_gl_clear(_jlc->jlc, 2, 255, 5, 255);
	_jl_dl_loop(_jlc);
#if JL_GLRTEX == JL_GLRTEX_SDL
	_jlc->dl.whichwindow = 0;
#endif
	_jlc->dl.fullscreen = 0;
}

//NON-STATIC FUNCTIONS
// ETOM FUNCTIONS

#if JL_GLRTEX == JL_GLRTEX_SDL

void jl_dl_screen_(jvct_t* _jlc, jl_window_t* which) {
	if(_jlc->dl.whichwindow != which) {
		SDL_GL_MakeCurrent(which->w, _jlc->dl.displayWindow->c);
		_jlc->dl.whichwindow = which;
	}
}

jl_window_t* jl_dl_screen_new_(jvct_t* _jlc, u16_t w, u16_t h) {
	jl_window_t* window = NULL;

	// Allocate space for "window"
	jl_me_alloc(_jlc->jlc, (void**)&window, sizeof(jl_window_t), 0);
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
	SDL_RaiseWindow(_jlc->dl.displayWindow->w);
	// Return the new window.
	return window;
}

#endif

void _jl_dl_loop(jvct_t* _jlc) {
	//Update Screen
	SDL_GL_SwapWindow(_jlc->dl.displayWindow->w); //end current draw
	// Clear the screen of anything wierd.
	jl_gl_clear(_jlc->jlc, 2, 5, 255, 255);
}

void _jl_dl_resz(jvct_t *_jlc, uint16_t x, uint16_t y) {
	uint16_t offx = 0;
	uint16_t offy = 0;

	_jlc->dl.full_w = x;
	_jlc->dl.full_h = y;
	jl_gl_viewport_screen(_jlc);
	_jlc->dl.shiftx = 0.;
	_jlc->dl.shifty = 0.;
	if(y < x * .5625) {
		_jlc->jlc->smde = 0;
		offx = x;
		x = y * 16./9.; //Widesceen
		_jlc->dl.shiftx += ((float)offx - (float)x)/((float)offx);
		offx = (offx - x) / 2;
		_jlc->dl.window.x = offx;
		_jlc->dl.window.y = 0.;
		_jlc->dl.window.w = x;
		_jlc->dl.window.h = y;
	}else if(y > x * 1.125) {//DOUBLE SCREEN
		_jlc->jlc->smde = 1;
		if(y > x * 1.5) {
			offy = y;
			y = x * 1.5; //Standard
			_jlc->dl.shifty += ((float)offy-(float)y)/((float)offy);
			_jlc->dl.window.x = 0.;
			_jlc->dl.window.y = (offy) / 2.;
			_jlc->dl.window.w = x;
			_jlc->dl.window.h = y / 2.;
		}else{
			offy = y;
			offy = (offy - y) / 2;
			_jlc->dl.window.x = 0.;
			_jlc->dl.window.y = y / 2.;
			_jlc->dl.window.w = x;
			_jlc->dl.window.h = y / 2.;
		}
	}else if(y > x * .75) {
		_jlc->jlc->smde = 0;
		offy = y;
		y = x * .75; //Standard
		_jlc->dl.shifty += ((float)offy-(float)y)/((float)offy);
		offy = (offy - y) / 2;
		_jlc->dl.window.x = 0.;
		_jlc->dl.window.y = offy;
		_jlc->dl.window.w = x;
		_jlc->dl.window.h = y;
	}else{
		_jlc->jlc->smde = 0;
		_jlc->dl.window.x = 0.;
		_jlc->dl.window.y = 0.;
		_jlc->dl.window.w = x;
		_jlc->dl.window.h = y;
	}
	_jlc->dl.current.w = x;
	_jlc->dl.current.h = y + (x - y);
	// Set GL aspect.
	_jlc->dl.aspect = ((double)y) / ((double)x);
	// Clear the screen of anything wierd.
	jl_gl_clear(_jlc->jlc, 122, 255, 125, 255);
}

/**
 * Set the program title.  Used as window name, and as resource
 * directory.
 * @param jlc: The library context.
 * @param name: The name of the program.
*/
void jl_dl_progname(jl_t* jlc, strt name) {
	jvct_t* _jlc = jlc->_jlc;
	int ii;
	for(ii = 0; ii < 16; ii++) {
		_jlc->dl.windowTitle[0][ii] = name->data[ii];
		if(name->data[ii] == '\0') { break; }
	}
	_jlc->dl.windowTitle[0][15] = '\0';
#if JL_PLAT == JL_PLAT_COMPUTER
	if(_jlc->dl.displayWindow)
	 SDL_SetWindowTitle(_jlc->dl.displayWindow->w, _jlc->dl.windowTitle[0]);
#endif
}

void _jl_dl_inita(jvct_t* _jlc) {
	jlvmpi_ini_sdl(_jlc);
	//Get Information On How Big To Make Window
	_jlvm_curd_mode(_jlc);
	//Create Window With SDL
	_jlvm_crea_wind(_jlc);
	//Get Window Size
	_jlvm_curd_mode(_jlc);
	//Update screensize to fix any rendering glitches
	_jl_dl_resz(_jlc, _jlc->dl.current.w, _jlc->dl.current.h);
	// Update The Screen
	_jl_dl_loop(_jlc);
}

void _jl_dl_kill(jvct_t* _jlc) {
	jl_io_print(_jlc->jlc, "killing SDL....");
	if (_jlc->dl.displayWindow->c != NULL) {
		SDL_free(_jlc->dl.displayWindow->c);
		SDL_free(_jlc->dl.displayWindow->w);
	}
	jl_io_print(_jlc->jlc, "killed SDL!");
}
