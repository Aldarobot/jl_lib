/*
 * dl: upper level SDL2
*/

#include "header/jl_pr.h"

//VARS
char windowTitle[2][16];

SDL_Window *displayWindow; //where everything is blitted to
SDL_GLContext *glcontext;
SDL_DisplayMode current;

//void SDL_Init(int a) {
//}

//STATIC FUNCTIONS
static inline void jlvmpi_ini_sdl(void) {
	#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
	printf("[JLVM] starting up...\n");
	#endif
	SDL_Init(JLVM_INIT);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM] input...\n");
	#endif
	#if PLATFORM == 0
	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	#endif
}

//Update the SDL_displayMode structure
static void _jlvm_curd_mode(jvct_t *pjct) {
	if(SDL_GetCurrentDisplayMode(0, &current)) {
		jlvm_dies(pjct,
			jl_me_strt_merg(
				Strt("failed to get current display mode! "),
				Strt((char *)SDL_GetError()),
				STRT_TEMP
			)
		);
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/SRES]%d,%d\n", current.w, current.h);
	#endif
}

//This is the code that actually creates the window by accessing SDL
static inline void _jlvm_crea_wind(jvct_t *pjct) {
	#if defined(__ANDROID__)
	current.w = 640;
	current.h = 480;
	#elif defined(__PSP__)
	current.w = 480;
	current.h = 272;
	#endif

	if((displayWindow = SDL_CreateWindow(
        "¡SDL2 Window!",		// window title
        SDL_WINDOWPOS_UNDEFINED,// initial x position
        SDL_WINDOWPOS_UNDEFINED,// initial y position
		current.w, current.h, //width and height
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE
    )) == NULL)
	{
		jlvm_dies(pjct,
			jl_me_strt_merg(
				Strt("Failed to create display window"),
				Strt((char *)SDL_GetError()),
				STRT_TEMP
			)
		);
	}
	glcontext = SDL_GL_CreateContext(displayWindow);	
}

static inline void jlvmpi_upd(uint8_t r, uint8_t g, uint8_t b) {

}

//NON-STATIC FUNCTIONS

//Function is available to user: set window's resolution
void jlvm_sres(jvct_t *pjct, uint16_t w, uint16_t h) {
	SDL_SetWindowSize(displayWindow,w,h);
	_jal5_lsdl_glpt_view(pjct, w,h);
}

uint16_t _jal5_lsdl_sres_getw(void) {
	return current.w;
}

uint16_t _jal5_lsdl_sres_geth(void) {
	return current.h;
}

void _jal5_lsdl_glpt_view(jvct_t *pjct, uint16_t x, uint16_t y) {
	glViewport( 0, 0, x, y );
	if(x > y) { //WIDESCREEN
		
	}else{//DOUBLE SCREEN
		
	}
	/*reshape*/
/*	if(current.w >= current.h) { //Landscape View
		if(current.w <= 16./9. * current.h) {//Lesser Width, so use Width
			glViewport( 0, 0, current.w, current.w * .75 );
			printf("aght\n");
		}else{ //Lesser Height, so use height
			glViewport( 0, 0, (current.h * 16./9.)+1, current.h ); //Widescren
			printf("waerw\n");
		}
		glViewport( 0, 0, current.w, current.h );
	}else{ //Portrait View
		glViewport( 0, 0, current.w, current.h );
	}*/
	if(glGetError() != GL_NO_ERROR ) {
		jlvm_dies(pjct, Strt("Couldn't initialize(Reshape)"));
	}
	SDL_GL_SwapWindow(displayWindow);
}

/*
 * Set the program title.  Used as window name, and as resource
 * directory.
*/
void lsdl_prog_name(strt name) {
	int ii;
	for(ii = 0; ii < 16; ii++) {
		windowTitle[0][ii] = name->data[ii];
		if(name->data[ii] == '\0') { break; }
	}
	windowTitle[0][15] = '\0';
#if PLATFORM == 0
	SDL_SetWindowTitle(displayWindow, windowTitle[0]);
#endif
}

void _jl_dl_init(jvct_t* pjlc) {
	jlvmpi_ini_sdl();
	_jlvm_curd_mode(pjlc); //Get Information On How Big To Make Window
	_jlvm_crea_wind(pjlc); //Create Window With SDL
	#if PLATFORM == 1 //If Phone (For Reorientation)
	_jlvm_curd_mode(pjlc);
	#endif
	//Update viewport to fix any rendering glitches
	_jal5_lsdl_glpt_view(pjlc, current.w, current.h); 
}

void _jl_dl_loop(void) {
	//Update Screen
	SDL_GL_SwapWindow(displayWindow); //end current draw
	//start next draw
	glClearColor(2.f/255.f, 5./255.f, 255./255.f, 1.f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void _jl_dl_kill(jvct_t* pjlc) {
	if (glcontext != NULL) {
		SDL_free(glcontext);
	}
}
