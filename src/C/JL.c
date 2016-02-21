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
	jl_io_print(_jlc->jlc, "Initializing file system....");
	jl_fl_init__(_jlc);
	jl_io_print(_jlc->jlc, "Initialized file system!");
//	jl_gr_draw_msge(_jlc->jlc, 0, 0, 0, "INITIALIZATION COMPLETE!");
}

static inline void _jl_ini(jvct_t *_jlc, jl_fnct _fnc_init_) {
	jl_io_print(_jlc->jlc, "STARTING JL_lib Version "JL_VERSION"....");

	jl_init_libs__(_jlc);
	// Run the users loop & resize functions
	_fnc_init_(_jlc->jlc);
	_jlc->mode.mode.tclp[JL_SG_WM_RESZ](_jlc->jlc);
	//
	jl_io_print(_jlc->jlc, "Initialized!");
}

static void jl_main_resz_jl(jvct_t* _jlc, u16_t x, u16_t y) {
	if(_jlc->jlc->jl_gr) {
		jl_gr_t* jl_gr = _jlc->jlc->jl_gr;

		jl_gr_resz(jl_gr, x, y);
	}
}

void main_resz(jvct_t* _jlc, u16_t x, u16_t y) {
	// Allow subsystems to adjust to the new window.
	jl_main_resz_jl(_jlc, x, y);
	// Allow the user to update their graphics.
	_jlc->mode.mode.tclp[JL_SG_WM_RESZ](_jlc->jlc);
}

static void jl_time_reset__(jl_t* jlc, u8_t on_time) {
	jlc->time.prev_tick = jlc->time.this_tick;
	if(jlc->jl_gr) {
		jl_gr_t* jl_gr = jlc->jl_gr;

		if((jl_gr->sg.changed = ( jl_gr->sg.on_time != on_time)))
			jl_gr->sg.on_time = on_time;
	}
}

//return how many seconds passed since last call
static inline void jl_seconds_passed__(jvct_t* _jlc) {
	int diff = _jlc->jlc->time.this_tick - _jlc->jlc->time.prev_tick;
	_jlc->jlc->time.psec = jl_sdl_seconds_past__(_jlc->jlc);
	if(diff) _jlc->jlc->time.fps = (_jlc->jlc->time.fps + (1000/ diff))/2;
	else _jlc->jlc->time.fps = 1;
	// Tell if fps is 60 fps or better
	jl_time_reset__(_jlc->jlc, _jlc->jlc->time.fps >= JL_FPS);
}

static inline void main_loop__(jvct_t* _jlc) {
	jl_io_print(_jlc->jlc, "going into loop....");
	while(1) {
		// Check the amount of time passed since last frame.
		jl_seconds_passed__(_jlc);
		// Run the user's mode loop.
		_jlc->mode.mode.tclp[_jlc->jlc->loop](_jlc->jlc);
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
//	jl_gr_draw_msge(jlc, 0, 0, 0, "Quiting JL-Lib....");
	if(_jlc->me.status == JL_STATUS_EXIT) {
		rc = JL_RTN_FAIL_IN_FAIL_EXIT;
		printf("\n!! double error!\n");
		printf("!! exiting with return value %d\n", rc);
		exit(rc);
	}
	// Set status to Exiting
	_jlc->me.status = JL_STATUS_EXIT;
	jl_io_print(jlc, "Quitting...."); //Exited properly
	_jl_fl_kill(_jlc);
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
void jl_init(jl_fnct _fnc_init_) {
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
Java_org_libsdl_app_SDLActivity_nativeJlSendData( JNIEnv *env, jobject obj,
	jstring data)
{
	JL_FL_BASE = (*env)->GetStringUTFChars(env, data, 0);
	SDL_Log("nativeJlSendData \"%s\"\n", JL_FL_BASE);
}

#endif
