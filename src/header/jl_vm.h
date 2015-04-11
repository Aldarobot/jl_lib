#include <stdint.h>
#include <stdlib.h>
#include "../lib/clump/clump.h"

//IO:

	typedef struct{
		uint8_t offs;
		int8_t ofs2;
		char head[16][5];
		jl_io_print_fnt *printfn;
		uint16_t maxtag;
		int16_t tag;
	}_io_t;

	typedef struct{
		u32t var_set_count;
		void **vars;
	}_sg_sprt_t;

	//Standard Mode Class
	typedef struct{
		//Sprites
		u32t sprite_count;
		_sg_sprt_t *sprites;

		//Standard Functions
		jl_simple_fnt tclp[JL_SG_WM_MAX];
	}__sg_mode_t;

	typedef struct{
		jl_t * usrd;
		__sg_mode_t *mdes; //Array Sizof Number Of Modes
		int32_t xmse;
		int32_t ymse;
		
		uint32_t taskbar[5];
		uint32_t init_image_location;
		uint32_t prev_tick;
		uint32_t this_tick;
		uint32_t processingTimeMillis;
		
		//For loading images
		uint16_t image_id;
		uint16_t igid;
		void *image_data;
	}_sg_t;

//INPT:
	typedef struct{
		uint16_t CtrC; //Control Count: how many events to check for
		jl_ct_onevent_fnt *function; //1 function for each event
		uint8_t *type; //1 event type for each event
	}_ct_user_events;

	typedef struct{
		jl_ct_event_fnt getEvents[JL_CT_MAXX];

		//User Events For Each Mode & Window
		_ct_user_events *userevents[JL_SG_WM_MAX];

		float msx, msy;
		int msxi, msyi;

		SDL_Event event;
		
		#if PLATFORM == 1 //PHONE
		uint8_t menu;
		#elif PLATFORM == 0
		const Uint8 *keys;
		#endif
		
		uint8_t heldDown;
		uint8_t keyDown[255];
		uint32_t sd; //NYI: stylus delete
	}_ct_t;

//AU:
	typedef struct{
		Mix_Music *_MUS;
		char _VOL;
	}__mixr_jmus;

	typedef struct{
		uint32_t idis; //Which music to play next
		uint8_t sofi; //Seconds Of Fade In
		uint8_t sofo; //Seconds Of Fade Out

		int smax; //Music Stack Maximum Music pieces
		__mixr_jmus *jmus; //Pointer Of "smax" Music Pieces
		double pofr; //Point Of Return (Where Music Should Start)
	}_au_t;

	typedef struct {
		GLuint temp_buff_vrtx, temp_buff_txtr;
		GLuint **textures;
		GLuint vertex_shader, fragment_shader, program;
		
		uint16_t allocatedg;
		uint16_t allocatedi;
		
		struct {
			GLint **textures;
		} uniforms;

		struct {
			GLint position;
			GLint texpos;
		} attributes;

		float buff_vert[255*3];
	}_gl_t;
	
	typedef struct {
		struct cl_list *filelist; //List of all files in working dir.
		int8_t cursor;
		uint8_t cpage;
		char *dirname;
		char *selecteditem;
		uint8_t returnit;
	}_fl_t;
	
	typedef struct {
		jl_simple_fnt menuoverlay;
	}_gr_t;

//OTHER:
typedef struct{
	void (*duu)(void *param);
}jlvm_t;

//JLVM Context Structure
typedef struct{
	_io_t io; //Terminal Data
	_sg_t sg; //Window Info
	_ct_t ct; //Input Information
	_au_t au; //Audio Info
	_gl_t gl; //Opengl Data
	_fl_t fl; //File Manager
	_gr_t gr; //Graphics
//	_amem_t me; NYI

	uint64_t cprg; //current program ID
	_sg_sprt_t fncs; //Functions that change
}jvct_t;
