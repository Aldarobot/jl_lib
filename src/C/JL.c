#include "jl_pr.h"

#if JL_PLAT == JL_PLAT_PHONE
	#include <jni.h>
#endif

//Initialize The Libraries Needed At Very Beginning: The Base Of It All
static inline jvct_t* jl_init_essential__(void) {
	// Memory
	jvct_t* _jlc = _jl_me_init(); // Create The Library Context
	// Printing to terminal
	_jl_io_init(_jlc->jlc);
	return _jlc;
}

static inline void jl_init_libs__(jvct_t *_jlc) {
	jl_io_print(_jlc->jlc, "Initializing DL....");
	_jl_dl_inita(_jlc); //create the window.
	jl_io_print(_jlc->jlc, "Initializing FL....");
	_jl_fl_inita(_jlc); //prepare for loading media packages.
	jl_io_print(_jlc->jlc, "Initializing SG....");
	jl_sg_inita_(_jlc); //Load default graphics from package.
	jl_io_print(_jlc->jlc, "Initializing GL....");
	_jl_gl_init(_jlc); //Drawing Set-up
	jl_io_print(_jlc->jlc, "Initialized GL! / Initializing GR....");
	jl_gr_inita_(_jlc); //Set-up sprites & menubar
	jl_io_print(_jlc->jlc, "Initialized GR! / Initializing CT....");
	jl_gr_draw_msge(_jlc->jlc, 0, 0, 0, "Initializing input....");
	_jl_ct_init(_jlc); //Prepare to read input.
	jl_gr_draw_msge(_jlc->jlc, 0, 0, 0, "Initialized input!");
	jl_io_print(_jlc->jlc, "Initialized CT!");
	jl_sg_initb_(_jlc);
	jl_io_print(_jlc->jlc, "Initialized SG!");
	_jl_fl_initb(_jlc);
	jl_io_print(_jlc->jlc, "Initialized FL! / Initializing AU....");
	_jl_au_init(_jlc); //Load audiostuffs from packages
	jl_io_print(_jlc->jlc, "Initialized AU!");
	jl_gr_draw_msge(_jlc->jlc, 0, 0, 0, "INITIALIZATION COMPLETE!");
}

static inline void _jl_ini(jvct_t *_jlc, jl_simple_fnt _fnc_init_) {
	jl_io_print(_jlc->jlc, "STARTING JL_lib Version "JL_VERSION"....");

	jl_init_libs__(_jlc);
	_fnc_init_(_jlc->jlc);
	_jlc->sg.mode.tclp[JL_SG_WM_RESZ](_jlc->jlc);

	jl_io_print(_jlc->jlc, "Initialized!");
}

static void jl_main_resz_jl(jvct_t* _jlc, u16_t x, u16_t y) {
	// Reset aspect ratio stuff.
	jl_gl_resz_(_jlc);
	// Update the actual window.
	_jl_dl_resz(_jlc, x, y);
	// Update the size of the background.
	_jl_sg_resz(_jlc->jlc);
	// Update the size of foreground objects.
	_jl_gr_resz(_jlc);	
}

void main_resz(jvct_t* _jlc, u16_t x, u16_t y) {
	// Allow subsystems to adjust to the new window.
	jl_main_resz_jl(_jlc, x, y);
	// Allow the user to update their graphics.
	_jlc->sg.mode.tclp[JL_SG_WM_RESZ](_jlc->jlc);
}

static void sg_time_reset__(jvct_t* _jlc, u8_t on_time) {
	_jlc->sg.prev_tick = _jlc->sg.this_tick;
	if((_jlc->sg.changed = ( _jlc->sg.on_time != on_time)))
		_jlc->sg.on_time = on_time;
}

//return how many seconds passed since last call
static inline void jl_seconds_passed__(jvct_t* _jlc) {
	_jlc->jlc->psec = jl_sg_seconds_past_(_jlc);
	_jlc->sg.fps = (_jlc->sg.fps +
		(1000/(_jlc->sg.this_tick - _jlc->sg.prev_tick)))/2;
	// Tell if fps is 60 fps or better
	sg_time_reset__(_jlc, _jlc->sg.fps >= JL_FPS);
}

static inline void main_loop__(jvct_t* _jlc) {
	jl_io_print(_jlc->jlc, "going into loop....");
	while(1) {
		//Update events.
		_jl_ct_loop(_jlc);
		// Deselect any pre-renderer.
		_jlc->gl.cp = NULL;
		//Check the amount of time passed since last frame.
		jl_seconds_passed__(_jlc);
		//Redraw screen.
		_jl_sg_loop(_jlc);
		//Update Screen.
		_jl_dl_loop(_jlc);
		//Play Audio.
		_jl_au_loop(_jlc);
	}
}

// EXPORT FUNCTIONS

/**
 * Do Nothing
**/
void jl_dont(jl_t* jlc) { }

void jl_kill(jl_t* jlc, int rc) {
	jvct_t* _jlc = jlc->_jlc;

	jl_io_print(jlc, "Quitting...."); //Exited properly
	jl_io_function(jlc, "SG_Kill");
	jl_gr_draw_msge(jlc, 0, 0, 0, "Quiting JL-Lib....");
	if(_jlc->me.status == JL_STATUS_EXIT) {
		rc = JL_RTN_FAIL_IN_FAIL_EXIT;
		printf("\n!! double error!\n");
		printf("!! exiting with return value %d\n", rc);
		exit(rc);
	}
	// Set status to Exiting
	_jlc->me.status = JL_STATUS_EXIT;
	jl_io_print(jlc, "Quitting...."); //Exited properly
	_jl_au_kill(_jlc);
	_jl_fl_kill(_jlc);
	_jl_sg_kill(jlc);
	_jl_dl_kill(_jlc);
	jl_io_print(jlc, ":Quit successfully!"); //Exited properly
	_jl_io_kill(jlc);
	_jl_me_kill(_jlc);
	printf("[\\JL_Lib] ");
	if(!rc) printf("| No errors ");
	printf("| Exiting with return value %d |\n", rc);
	exit(rc);
	jl_io_print(jlc, ":What The Hell?  This is an impossible error!");
}

/**
 * Initialize JL_Lib.
**/
void jl_init(jl_simple_fnt _fnc_init_) {
	//Set Up Memory And Logging
	jvct_t* _jlc = jl_init_essential__();

	// Initialize JL_lib!
	_jl_ini(_jlc, _fnc_init_);
	// Run the Loop
	if(_jlc->jlc->mdec) main_loop__(_jlc);
	// Kill the program
	jl_kill(_jlc->jlc, JL_RTN_SUPER_IMPOSSIBLE);
	// Impossible to reach this point
	return;
}

#if JL_PLAT == JL_PLAT_PHONE

JNIEXPORT void JNICALL
Java_org_libsdl_app_SDLActivity_nativeJlSendData(
	JNIEnv *env, jobject obj, jstring data)
{
	JL_FL_BASE = (*env)->GetStringUTFChars(env, data, 0);
	SDL_Log("nativeJlSendData \"%s\"\n", JL_FL_BASE);
}

#endif
