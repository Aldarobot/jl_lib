#include <stdint.h>
#include <stdlib.h>
#include "../lib/clump/clump.h"

//SIOP:

	typedef struct{
		uint8_t offs;
		char head[16][5];
		strt lines[45]; //length 90
	}_jal5_siop_t;

	typedef struct{
		void (* func)(sgrp_user_t* pusr);
	}__jal5_sgrp_func_t;

	typedef struct{
		u32t var_set_count;
		void **vars;
	}_jal5_sgrp_sprt_t;

	//Standard Mode Class
	typedef struct{
		//Sprites
		u32t sprite_count;
		_jal5_sgrp_sprt_t *sprites;

		//Standard Functions
		__jal5_sgrp_func_t tclp[SGRP_MAKS];
	}__jal5_sgrp_mode_t;

	typedef struct{
		sgrp_user_t * usrd;
		__jal5_sgrp_mode_t *mdes; //Array Sizof Number Of Modes
		int32_t xmse;
		int32_t ymse;
	}_jal5_sgrp_t;

//INPT:
	typedef struct{
		uint8_t *CtrC;
	}_jal5_inpt_t;

//AUDI:
	typedef struct{
		Mix_Music *_MUS;
		char _VOL;
	}__jal5_mixr_jmus;

	typedef struct{
		uint32_t idis; //Which music to play next
		uint8_t sofi; //Seconds Of Fade In
		uint8_t sofo; //Seconds Of Fade Out

		int smax; //Music Stack Maximum Music pieces
		__jal5_mixr_jmus *jmus; //Pointer Of "smax" Music Pieces
		double pofr; //Point Of Return (Where Music Should Start)
	}_jal5_audi_t;

	typedef struct {
		GLuint temp_buff_vrtx, temp_buff_txtr;
		GLuint textures[1];
		GLuint vertex_shader, fragment_shader, program;
		
		struct {
		    GLint textures[1];
		} uniforms;

		struct {
		    GLint position;
			GLint texpos;
		} attributes;

		float buff_vert[255*3];
	}_jal5_eogl_t;

//OTHER:
typedef struct{
	void (*duu)(void *param);
}jlvm_t;

//JLVM Context Structure
typedef struct{
	_jal5_siop_t Siop; //Terminal Data
	_jal5_sgrp_t Sgrp; //Window Info
	_jal5_inpt_t Inpt; //Input Information
	_jal5_audi_t Audi; //Audio Info
	_jal5_eogl_t Eogl; //Opengl Data
//	_jal5_amem_t Amem; NYI

	uint64_t cprg; //current program ID
	_jal5_sgrp_sprt_t fncs; //Functions that change
}jvct_t;

//For Internal Use Only: DO NOT USE
	jvct_t* _jal5_amem_init(void);
	void _jal5_amem_kill(jvct_t* jprg);
