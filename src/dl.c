/*
 * dl: upper level SDL2
 *	LSDL AKA. SDL or Simple Direct Media Layer:  SDL is a popular library
 *	for making games.
*/

#include "header/jl_pr.h"

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
static void _jl_dl_fscreen(jvct_t* _jlc, uint8_t a) {
	// Make sure the fullscreen value is either a 1 or a 0.
	_jlc->dl.fullscreen = !!a;
	// Actually set whether fullscreen or not.
	if(SDL_SetWindowFullscreen(_jlc->dl.displayWindow,
		JL_DL_FULLSCREEN * _jlc->dl.fullscreen))
	{
		_jl_fl_errf(_jlc, SDL_GetError());
		jl_sg_kill(_jlc->jlc, "\n");
	}
}

static inline void jlvmpi_ini_sdl(jvct_t* _jlc) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "ISDL"); // {
	jl_io_printc(_jlc->jlc, "Starting up...\n");
	SDL_Init(JL_DL_INIT);
	jl_io_printc(_jlc->jlc, "input...\n");
	#if JL_PLAT == JL_PLAT_COMPUTER
	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	#endif
	jl_io_close_block(_jlc->jlc); // }
}

//Update the SDL_displayMode structure
static void _jlvm_curd_mode(jvct_t *_jlc) {
	if(SDL_GetCurrentDisplayMode(0, &_jlc->dl.current)) {
		_jl_fl_errf(_jlc, ":failed to get current display mode:\n:");
		_jl_fl_errf(_jlc, (char *)SDL_GetError());
		jl_sg_kill(_jlc->jlc, "\n");
	}
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "CURD"); // {
	jl_io_printc(_jlc->jlc,
		jl_me_string_fnum(_jlc->jlc, _jlc->dl.current.w));
	jl_io_printc(_jlc->jlc, ",");
	jl_io_printc(_jlc->jlc,
		jl_me_string_fnum(_jlc->jlc, _jlc->dl.current.h));
	jl_io_printc(_jlc->jlc, "\n");
	jl_io_close_block(_jlc->jlc); // }
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

	_jlc->dl.displayWindow = SDL_CreateWindow(
		"¡SDL2 Window!",		// window title
		SDL_WINDOWPOS_UNDEFINED,// initial x position
		SDL_WINDOWPOS_UNDEFINED,// initial y position
		_jlc->dl.current.w, _jlc->dl.current.h, //width and height
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP |
			SDL_WINDOW_RESIZABLE
    		);

	if(_jlc->dl.displayWindow == NULL)
	{
		_jl_fl_errf(_jlc, ":Failed to create display window:\n:");
		_jl_fl_errf(_jlc, (char *)SDL_GetError());
		jl_sg_kill(_jlc->jlc, "\n");
	}
	_jlc->dl.glcontext = SDL_GL_CreateContext(_jlc->dl.displayWindow);
	_jlc->dl.fullscreen = 1;
}

//NON-STATIC FUNCTIONS

void _jl_dl_loop(jvct_t* _jlc) {
	//Update Screen
	SDL_GL_SwapWindow(_jlc->dl.displayWindow); //end current draw
	// Clear the screen of anything wierd.
	jl_gl_clear(_jlc->jlc, 2, 5, 255, 255);
}

void _jl_dl_resz(jvct_t *_jlc, uint16_t x, uint16_t y) {
	uint16_t offx = 0;
	uint16_t offy = 0;

	_jlc->dl.full_w = x;
	_jlc->dl.full_h = y;
	jl_gl_viewport_screen(_jlc);
	jl_gl_clear(_jlc->jlc, 2, 5, 255, 255);
	_jlc->dl.multiplyy = -2.*((float)x)/((float)y);
	_jlc->dl.multiplyx = 2.;
	_jlc->dl.shiftx = 0.;
	_jlc->dl.shifty = 0.;
	if(y < x * .5625) {
		_jlc->jlc->smde = 0;
		offx = x;
		x = y * 16./9.; //Widesceen
		_jlc->dl.multiplyy = -2.*((float)x)/((float)y);
		_jlc->dl.multiplyx = 2.*((float)x)/((float)offx);
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
	_jlc->gl.current_pr.w = _jlc->dl.full_w;
	_jlc->gl.current_pr.h = _jlc->dl.full_h;
	_jlc->dl.aspect = ((double)y) / ((double)x);
	_jlc->gl.current_pr.aspect_y = _jlc->dl.aspect;
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
	 SDL_SetWindowTitle(_jlc->dl.displayWindow, _jlc->dl.windowTitle[0]);
#endif
}

void _jl_dl_inita(jvct_t* _jlc) {
	jlvmpi_ini_sdl(_jlc);
	_jlvm_curd_mode(_jlc); //Get Information On How Big To Make Window
	_jlvm_crea_wind(_jlc); //Create Window With SDL
	#if JL_PLAT == JL_PLAT_PHONE //If Phone (For Reorientation)
//	_jlvm_curd_mode(_jlc);
	#endif
}

void _jl_dl_initb(jvct_t* _jlc) {
	//Update screensize to fix any rendering glitches
	_jl_dl_resz(_jlc, _jlc->dl.current.w, _jlc->dl.current.h);
	_jl_dl_loop(_jlc);
}

void _jl_dl_kill(jvct_t* _jlc) {
	jl_io_printc(_jlc->jlc, "killing SDL...\n");
	if (_jlc->dl.glcontext != NULL) {
		SDL_free(_jlc->dl.glcontext);
	}
	jl_io_printc(_jlc->jlc, "killed SDL!\n");
}
