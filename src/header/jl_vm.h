#include <stdint.h>
#include <stdlib.h>
#include "../lib/clump/clump.h"

//SIOP:

	typedef struct{
		uint8_t offs;
		int8_t ofs2;
		char head[16][5];
		strt lines[45]; //length 90
	}_siop_t;

	typedef struct{
		void (* func)(jl_t* pusr);
	}__sg_func_t;

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
		__sg_func_t tclp[JL_SG_WM_MAX];
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
		uint16_t image_id;
		uint16_t igid;
	}_sg_t;

//INPT:
	typedef struct{
		void ( *OnEvent )(jl_t *pusr, float x, float y);
		uint8_t Type;
	}jlvm_evnt_t;

	typedef struct{
		void ( *Event )(jl_t *pusr, jlvm_evnt_t * PMode);
	}jlvm_slib_evnt_t;

	typedef struct{
		uint8_t *CtrC;
		#if PLATFORM == 1 //PHONE
		uint8_t menu;
		#endif
		jlvm_slib_evnt_t getEvents[JL_CT_MAXX];
		void **events;

		float msx, msy;
		int msxi, msyi;
		uint8_t heldDown;
		uint32_t sd; //NYI: stylus delete
		SDL_Event event;

		#if PLATFORM == 0
		const Uint8 *keys;
		#endif
		
		uint8_t keyDown[255];
	}_inpt_t;

//AUDI:
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
	}_audi_t;

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

//OTHER:
typedef struct{
	void (*duu)(void *param);
}jlvm_t;

//JLVM Context Structure
typedef struct{
	_siop_t io; //Terminal Data
	_sg_t sg; //Window Info
	_inpt_t ct; //Input Information
	_audi_t au; //Audio Info
	_gl_t gl; //Opengl Data
//	_amem_t me; NYI

	uint64_t cprg; //current program ID
	_sg_sprt_t fncs; //Functions that change
}jvct_t;
