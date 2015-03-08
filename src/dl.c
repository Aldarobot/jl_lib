#include "header/jlvm_pr.h"

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
			amem_strt_merg(
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
			amem_strt_merg(
				Strt("Failed to create display window"),
				Strt((char *)SDL_GetError()),
				STRT_TEMP
			)
		);
	}
	glcontext = SDL_GL_CreateContext(displayWindow);	
}

static inline void jlvmpi_upd(uint8_t r, uint8_t g, uint8_t b) {
	//end current draw
	SDL_GL_SwapWindow(displayWindow);
	//start next draw
	glClearColor(((float)r)/255.f, ((float)g)/255.f, ((float)b)/255.f, 1.f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
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

void _jal5_lsdl_init(jvct_t *jcpt) {
	jlvmpi_ini_sdl();
	_jlvm_curd_mode(jcpt); //Get Information On How Big To Make Window
	_jlvm_crea_wind(jcpt); //Create Window With SDL
	#if PLATFORM == 1 //If Phone (For Reorientation)
	_jlvm_curd_mode(jcpt);
	#endif
	//Update viewport to fix any rendering glitches
	_jal5_lsdl_glpt_view(jcpt, current.w, current.h); 
}

void _jal5_lsdl_loop(void) {
	//	jgr_draw_sprite_layer();

/*    glUseProgram(g_program_tex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GTextures[0].texp);
    glUniform1i(GTextures[0].shdp, 0);

//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, g_resources.textures[1]);
//    glUniform1i(g_resources.uniforms.textures[1], 1);

    GLfloat g_vertex_buffer_data[] = { 
     -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
	};

	//BIND&SET VERTEX BUFFER
	jlvmpi_bind_buffer(BUFFER_SPRITE);
	jlvmpi_check_gl_error(0, "SPR/BIND_BUFF");
	jlvmpi_set_buffer(g_vertex_buffer_data, 8);
	jlvmpi_check_gl_error(0, "SPR/SET_BUFF");
	//USE VERTEX BUFFER
	glEnableVertexAttribArray(TEX_POS);
	jlvmpi_check_gl_error(0, "SPR/VAA");
	glVertexAttribPointer(TEX_POS, 2, GL_FLOAT, GL_FALSE, 
		0, 0);
	jlvmpi_check_gl_error(0, "SPR/VAP");
	//DRAW
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);*/
	//Update Screen
	jlvmpi_upd(2,5,255);

/*	flip->x = 255;
	flip->w = 20;
	for(i = 0; i < 5; i++) {
		flip->x-=20;
		if((taskbar[i] == GOOD_IMAGE_ID) && (slow)) {
			flip->g->w = SLOW_IMAGE_ID;
			sprintf(windowTitle[1], "%d/%d", processingTimeMillis,
				TimeProcessingAllowed);
		}else{
			flip->g->w = taskbar[i];
		}
//		jgr_upd_sprite_pos(flip);
//		jgr_draw_sprite(flip);
	}
	flip->w = 155;
	flip->x = 0;
	flip->g->w = UNKNOWN_ID;
//	jgr_upd_sprite_pos(flip);
//	jgr_draw_sprite(flip);
	jgr_draw_text(windowTitle[0], 0, 0, 10);
	jgr_draw_text(windowTitle[1], 0, 10, 10);
	if(timeTilMessageVanish) {
		jgr_draw_centered_text(GMessage[GScreenDisplayed],0);
		timeTilMessageVanish--;
	}*/
}

void _jal5_lsdl_kill(jvct_t *jcpt) {
	if (glcontext != NULL) {
		SDL_free(glcontext);
	}
}
