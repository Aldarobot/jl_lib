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
	JL_IO_DEBUG(_jlc->jlc, "Initializing file system....");
	jl_fl_init__(_jlc);
	JL_IO_DEBUG(_jlc->jlc, "Initializing threads....");
	jl_thread_init__(_jlc);
	JL_IO_DEBUG(_jlc->jlc, "Initialized!");
//	jl_gr_draw_msge(_jlc->jlc, 0, 0, 0, "INITIALIZATION COMPLETE!");
}

static inline void _jl_ini(jvct_t *_jlc, jl_fnct _fnc_init_) {
	jl_io_print(_jlc->jlc, "Starting JL_Lib - Version "JL_VERSION"....");
	// Run the library's init function.
	jl_init_libs__(_jlc);
	// Run the program's init function.
	_fnc_init_(_jlc->jlc);
	jl_io_print(_jlc->jlc, "Started JL_Lib!");
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
	int diff;

	_jlc->jlc->time.psec = jl_sdl_seconds_past__(_jlc->jlc);
	diff = _jlc->jlc->time.this_tick - _jlc->jlc->time.prev_tick;
	if(diff) _jlc->jlc->time.fps = round(1000./((double)diff));
	else _jlc->jlc->time.fps = 25000;
	// Tell if fps is 60 fps or better
	jl_time_reset__(_jlc->jlc, _jlc->jlc->time.fps >= JL_FPS);
}

static inline void main_loop__(jvct_t* _jlc) {
	// Check the amount of time passed since last frame.
	jl_seconds_passed__(_jlc);
	// Run the user's mode loop.
	_jlc->mode.mode.tclp[_jlc->jlc->loop](_jlc->jlc);
}

// EXPORT FUNCTIONS

/**
 * Do Nothing
**/
void jl_dont(jl_t* jlc) { }

static inline int jl_kill__(jl_t* jlc, int rc) {
	jvct_t* _jlc = jlc->_jlc;

	jl_io_print(jlc, "Quitting...."); //Exited properly
//	jl_gr_draw_msge(jlc, 0, 0, 0, "Quiting JL-Lib....");
	if(_jlc->me.status == JL_STATUS_EXIT) {
		rc = JL_RTN_FAIL_IN_FAIL_EXIT;
		printf("\n!! double error!\n");
		printf("!! exiting with return value %d\n", rc);
		exit(rc);
	}
	// Set status to Exiting
	_jlc->me.status = JL_STATUS_EXIT;
	_jl_fl_kill(_jlc);
	_jl_io_kill(jlc);
	_jl_me_kill(_jlc);
	printf("[\\JL_Lib] ");
	if(!rc) printf("| No errors ");
	printf("| Exiting with return value %d |\n", rc);
	return rc;
}

/**
 * Start JL_Lib.  Returns when program is closed.
**/
int jl_init(jl_fnct _fnc_init_, jl_fnct _fnc_kill_) {
	//Set Up Memory And Logging
	jvct_t* _jlc = jl_init_essential__();

	// Initialize JL_lib!
	_jl_ini(_jlc, _fnc_init_);
	// Run the Loop
	while(_jlc->jlc->mdec) main_loop__(_jlc);
	// Run Users Kill Routine
	_fnc_kill_(_jlc->jlc);
	// Kill the program
	return jl_kill__(_jlc->jlc, JL_RTN_SUCCESS);
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
