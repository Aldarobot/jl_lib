#include "jl_pr.h"

#if JL_PLAT == JL_PLAT_PHONE
	#include <jni.h>
#endif

//Initialize The Libraries Needed At Very Beginning: The Base Of It All
static inline jvct_t* jl_init_essential__(void) {
	// Memory
	jvct_t* _jl = jl_mem_init__(); // Create The Library Context
	// Printing to terminal
	jl_print_init__(_jl->jl);
	return _jl;
}

static inline void jl_init_libs__(jvct_t *_jl) {
	JL_PRINT_DEBUG(_jl->jl, "Initializing file system....");
	jl_file_init__(_jl);
	JL_PRINT_DEBUG(_jl->jl, "Initializing threads....");
	jl_thread_init__(_jl);
	JL_PRINT_DEBUG(_jl->jl, "Initializing modes....");
	jl_mode_init__(_jl->jl);
	JL_PRINT_DEBUG(_jl->jl, "Initializing time....");
	jl_sdl_init__(_jl->jl);
	JL_PRINT_DEBUG(_jl->jl, "Initialized!");
//	jl_gr_draw_msge(_jl->jl, 0, 0, 0, "INITIALIZATION COMPLETE!");
}

static inline void _jl_ini(jvct_t *_jl, jl_fnct _fnc_init_) {
	jl_print(_jl->jl, "Starting JL_Lib - Version "JL_VERSION"....");
	// Run the library's init function.
	jl_init_libs__(_jl);
	// Run the program's init function.
	_fnc_init_(_jl->jl);
	jl_print(_jl->jl, "Started JL_Lib!");
}

static void jl_time_reset__(jl_t* jl, u8_t on_time) {
	jl->time.prev_tick = jl->time.this_tick;
	if(jl->jl_gr) {
		jl_gr_t* jl_gr = jl->jl_gr;

		if((jl_gr->sg.changed = ( jl_gr->sg.on_time != on_time)))
			jl_gr->sg.on_time = on_time;
	}
}

//return how many seconds passed since last call
static inline void jl_seconds_passed__(jvct_t* _jl) {
	int diff;

	_jl->jl->time.psec = jl_sdl_seconds_past__(_jl->jl);
	diff = _jl->jl->time.this_tick - _jl->jl->time.prev_tick;
	if(diff) _jl->jl->time.fps = round(1000./((double)diff));
	else _jl->jl->time.fps = 25000;
	// Tell if fps is 60 fps or better
	jl_time_reset__(_jl->jl, _jl->jl->time.fps >= JL_FPS);
}

static inline void main_loop__(jvct_t* _jl) {
	// Check the amount of time passed since last frame.
	jl_seconds_passed__(_jl);
	// Run the user's mode loop.
	_jl->mode.mode.tclp[_jl->jl->loop](_jl->jl);
}

// EXPORT FUNCTIONS

/**
 * Do Nothing
**/
void jl_dont(jl_t* jl) { }

static inline int jl_kill__(jl_t* jl, int rc) {
	jvct_t* _jl = jl->_jl;

	jl_print(jl, "Quitting...."); //Exited properly
//	jl_gr_draw_msge(jl, 0, 0, 0, "Quiting JL-Lib....");
	if(_jl->me.status == JL_STATUS_EXIT) {
		rc = JL_RTN_FAIL_IN_FAIL_EXIT;
		printf("\n!! double error!\n");
		printf("!! exiting with return value %d\n", rc);
		exit(rc);
	}
	// Set status to Exiting
	_jl->me.status = JL_STATUS_EXIT;
	jl_file_kill__(_jl);
	jl_print_kill__(jl);
	jl_mem_kill__(_jl);
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
	jvct_t* _jl = jl_init_essential__();

	// Initialize JL_lib!
	_jl_ini(_jl, _fnc_init_);
	// Run the Loop
	while(_jl->jl->mdec) main_loop__(_jl);
	// Run Users Kill Routine
	_fnc_kill_(_jl->jl);
	// Kill the program
	return jl_kill__(_jl->jl, JL_RTN_SUCCESS);
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

/**
 * @mainpage
 * @section Library Description
 * 
*/
