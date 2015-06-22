/*
 * dl: upper level SDL2
 *	LSDL AKA. SDL or Simple Direct Media Layer:  SDL is a popular library
 *	for making games.
*/

#include "header/jl_pr.h"

//PROTOTYPES
void _jl_gl_setmatrix(jvct_t *pjlc);

static void _jl_dl_fscreen(jvct_t* pjlc, uint8_t a);
static void _jlvm_curd_mode(jvct_t *pjlc);

//EXPORT FUNCTIONS
void jl_dl_setfullscreen(jl_t *pusr, uint8_t is) {
	jvct_t* pjlc = pusr->pjlc;
	_jl_dl_fscreen(pjlc, is);
}

void jl_dl_togglefullscreen(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	_jl_dl_fscreen(pjlc, !pjlc->dl.fullscreen);
}

uint16_t jl_dl_getw(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	return pjlc->dl.current.w;
}

uint16_t jl_dl_geth(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	return pjlc->dl.current.h;
}

float jl_dl_p(jl_t *pusr) {
	jvct_t* pjlc = pusr->pjlc;
	return (((float)pjlc->dl.currenty) / (((float)pjlc->dl.current.w) *
		(1 + pusr->smde)));
}

//STATIC FUNCTIONS
static void _jl_dl_fscreen(jvct_t* pjlc, uint8_t a) {
	pjlc->dl.fullscreen = a;
	SDL_SetWindowFullscreen(pjlc->dl.displayWindow,
		SDL_WINDOW_FULLSCREEN_DESKTOP*a);
}

static inline void jlvmpi_ini_sdl(jvct_t* pjlc) {
	jl_io_offset(pjlc->sg.usrd, JL_IO_MINIMAL, "ISDL"); // {
	jl_io_printc(pjlc->sg.usrd, "Starting up...\n");
	SDL_Init(JLVM_INIT);
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "ISDL"); // =
	jl_io_printc(pjlc->sg.usrd, "input...\n");
	#if JL_PLAT == JL_PLAT_COMPUTER
	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	#endif
	jl_io_close_block(pjlc->sg.usrd); // }
}

//Update the SDL_displayMode structure
static void _jlvm_curd_mode(jvct_t *pjlc) {
	if(SDL_GetCurrentDisplayMode(0, &pjlc->dl.current)) {
		_jl_fl_errf(pjlc, ":failed to get current display mode:\n:");
		_jl_fl_errf(pjlc, (char *)SDL_GetError());
		jl_sg_kill(pjlc->sg.usrd, "\n");
	}
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "CURD"); // {
	jl_io_printc(pjlc->sg.usrd,
		jl_me_string_fnum(pjlc->sg.usrd, pjlc->dl.current.w));
	jl_io_printc(pjlc->sg.usrd, ",");
	jl_io_printc(pjlc->sg.usrd,
		jl_me_string_fnum(pjlc->sg.usrd, pjlc->dl.current.h));
	jl_io_printc(pjlc->sg.usrd, "\n");
	jl_io_close_block(pjlc->sg.usrd); // }
}

//This is the code that actually creates the window by accessing SDL
static inline void _jlvm_crea_wind(jvct_t *pjlc) {
	#if defined(__ANDROID__)
	pjlc->dl.current.w = 640;
	pjlc->dl.current.h = 480;
	#elif defined(__PSP__)
	pjlc->dl.current.w = 480;
	pjlc->dl.current.h = 272;
	#endif

	pjlc->dl.displayWindow = SDL_CreateWindow(
		"¡SDL2 Window!",		// window title
		SDL_WINDOWPOS_UNDEFINED,// initial x position
		SDL_WINDOWPOS_UNDEFINED,// initial y position
		pjlc->dl.current.w, pjlc->dl.current.h, //width and height
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP |
			SDL_WINDOW_RESIZABLE
    		);

	if(pjlc->dl.displayWindow == NULL)
	{
		_jl_fl_errf(pjlc, ":Failed to create display window:\n:");
		_jl_fl_errf(pjlc, (char *)SDL_GetError());
		jl_sg_kill(pjlc->sg.usrd, "\n");
	}
	pjlc->dl.glcontext = SDL_GL_CreateContext(pjlc->dl.displayWindow);
	pjlc->dl.fullscreen = 1;
}

static inline void jlvmpi_upd(uint8_t r, uint8_t g, uint8_t b) {

}

//NON-STATIC FUNCTIONS

//Function is available to user: set window's resolution
void jlvm_sres(jvct_t *pjlc, uint16_t w, uint16_t h) {
	SDL_SetWindowSize(pjlc->dl.displayWindow,w,h);
	_jal5_lsdl_glpt_view(pjlc, w,h);
}

void _jl_dl_loop(jvct_t* pjlc) {
	//Update Screen
	SDL_GL_SwapWindow(pjlc->dl.displayWindow); //end current draw
	//start next draw
	glClearColor(2.f/255.f, 5./255.f, 255./255.f, 1.f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void _jal5_lsdl_glpt_view(jvct_t *pjlc, uint16_t x, uint16_t y) {
	uint16_t offx = 0;
	uint16_t offy = 0;
	glViewport( 0, 0, x, y);
	pjlc->dl.multiplyy = 2.*((float)x)/((float)y);
	pjlc->dl.multiplyx = 2.;
	pjlc->dl.shiftx = -1.;
	pjlc->dl.shifty = 1.;
	if(y < x * .5625) {
		offx = x;
		x = y * 16./9.; //Widesceen
		pjlc->dl.multiplyy = 2.*((float)x)/((float)y);
		pjlc->dl.multiplyx = 2.*((float)x)/((float)offx);
		pjlc->dl.shiftx += ((float)offx - (float)x)/((float)offx);
		offx = (offx - x) / 2;
		pjlc->sg.usrd->smde = 0;
		pjlc->dl.window.x = offx;
		pjlc->dl.window.y = 0.;
		pjlc->dl.window.w = x;
		pjlc->dl.window.h = y;
	}else if(y > x * 1.125) {//DOUBLE SCREEN
		if(y > x * 1.5) {
			offy = y;
			y = x * 1.5; //Standard
			pjlc->dl.shifty -= ((float)offy-(float)y)/((float)offy);
			pjlc->dl.window.x = 0.;
			pjlc->dl.window.y = (offy) / 2.;
			pjlc->dl.window.w = x;
			pjlc->dl.window.h = y / 2.;
		}else{
			offy = y;
			offy = (offy - y) / 2;
			pjlc->dl.window.x = 0.;
			pjlc->dl.window.y = y / 2.;
			pjlc->dl.window.w = x;
			pjlc->dl.window.h = y / 2.;
		}
		pjlc->sg.usrd->smde = 1;
	}else if(y > x * .75) {
		offy = y;
		y = x * .75; //Standard
		pjlc->dl.shifty -= ((float)offy-(float)y)/((float)offy);
		offy = (offy - y) / 2;
		pjlc->sg.usrd->smde = 0;
		pjlc->dl.window.x = 0.;
		pjlc->dl.window.y = offy;
		pjlc->dl.window.w = x;
		pjlc->dl.window.h = y;
	}else{
		pjlc->sg.usrd->smde = 0;
		pjlc->dl.window.x = 0.;
		pjlc->dl.window.y = 0.;
		pjlc->dl.window.w = x;
		pjlc->dl.window.h = y;
	}
	if(glGetError() != GL_NO_ERROR ) {
		jl_sg_kill(pjlc->sg.usrd, "Couldn't initialize(Reshape)");
	}
	pjlc->dl.current.w = x;
	pjlc->dl.current.h = y + (x - y);
	pjlc->dl.currenty = y;
	_jl_gl_setmatrix(pjlc);
}

/*
 * Set the program title.  Used as window name, and as resource
 * directory.
*/
void jl_dl_progname(jl_t* pusr, strt name) {
	jvct_t* pjlc = pusr->pjlc;
	int ii;
	for(ii = 0; ii < 16; ii++) {
		pjlc->dl.windowTitle[0][ii] = name->data[ii];
		if(name->data[ii] == '\0') { break; }
	}
	pjlc->dl.windowTitle[0][15] = '\0';
#if JL_PLAT == JL_PLAT_COMPUTER
	if(pjlc->dl.displayWindow)
	 SDL_SetWindowTitle(pjlc->dl.displayWindow, pjlc->dl.windowTitle[0]);
#endif
}

void _jl_dl_inita(jvct_t* pjlc) {
	jlvmpi_ini_sdl(pjlc);
	_jlvm_curd_mode(pjlc); //Get Information On How Big To Make Window
	_jlvm_crea_wind(pjlc); //Create Window With SDL
	#if JL_PLAT == JL_PLAT_PHONE //If Phone (For Reorientation)
	_jlvm_curd_mode(pjlc);
	#endif
}

void _jl_dl_initb(jvct_t* pjlc) {
	//Update viewport to fix any rendering glitches
	_jal5_lsdl_glpt_view(pjlc, pjlc->dl.current.w, pjlc->dl.current.h);
	_jl_dl_loop(pjlc);
}

void _jl_dl_kill(jvct_t* pjlc) {
	jl_io_printc(pjlc->sg.usrd, "killing SDL...\n");
	if (pjlc->dl.glcontext != NULL) {
		SDL_free(pjlc->dl.glcontext);
	}
	jl_io_printc(pjlc->sg.usrd, "killed SDL!\n");
}
