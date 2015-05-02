/*
 * JL_lib(c) Jeron A. Lau
 * The header to be included within your programs you make with JL_lib.
*/

#include <stdint.h>
#include <stdlib.h>
#include "jl_sg.h"
#include "jl_me.h"
#include "jl_ct.h"
#include "clump.h"

	void jl_dont(jl_t* pusr);

/*
	JAL5_jl_me

		A simple memory library.  Includes creating variables, setting and
		getting variables, and doing simple and complicated math functions on
		the variables.  Has a specialized string type.
*/
//me.c
	//Allocate space of "size" bytes
	void * jl_me_alloc(u32t size);
	#define List(x) jl_me_list_allc(sizeof(void*)*x)

	// Clears an already existing string and resets it's cursor value.
	void jl_me_strt_clear(strt pa);

	// allocates a strt of size "size" and returns it.
	strt jl_me_strt_make(u32t size, u08t type);
	
	// frees a strt ("pstr")
	void jl_me_strt_free(strt pstr);
	
	// convert "string" into a (temporary) strt and return it.
	strt jl_me_strt_c8ts(const char *string);
	#define Strt(x) jl_me_strt_c8ts((const void*)x)
	
	strt jl_me_strt_mkfrom_data(uint32_t size, void *data);
	void jl_me_strt_merg(jl_t *pusr, strt a, strt b);
	void jl_me_strt_trunc(jl_t *pusr, strt a, uint32_t size);
	
	//Print a number out as a string and return it (Type=STRT_TEMP)
	char* jl_me_string_fnum(jl_t* pusr, int32_t a);
	strt jl_me_strt_fnum(s32t a);
	
	//Returns the byte at the cursor
	u08t jl_me_strt_byte(strt pstr);
	
	//Add a byte ( "pvalue" ) at the cursor in "pstr", then increment the
	// cursor value [ truncated to the string size ]
	void jl_me_strt_add_byte(strt pstr, u08t pvalue);
	
	//Returns a random integer from 0 to "a"
	u32t jl_me_random_int(u32t a);
	
	// Tests if the next thing in array script is equivalent to particle
	u08t jl_me_test_next(strt script, strt particle);
	
	// Returns string "script" truncated to "psize" or to the byte "end" in
	// "script".  It is dependant on which happens first. (Type=STRT_KEEP)
	strt jl_me_read_upto(strt script, u08t end, u32t psize);

// "cl.c"
	void jl_cl_list_alphabetize(struct cl_list *list);
// "dl.c"
	void jl_dl_setfullscreen(jl_t *pusr, uint8_t is);
	void jl_dl_togglefullscreen(jl_t *pusr);
	uint16_t jl_dl_getw(jl_t *pusr);
	uint16_t jl_dl_geth(jl_t *pusr);
	float jl_dl_p(jl_t *pusr);
// "gl.c"
	void jl_gl_maketexture(jl_t* pusr, uint16_t gid, uint16_t id,
		void *pixels, int width, int height);
// "sg.c"
	void jl_sg_mode_init(jl_t* pusr, u08t mdec);
	// Exit unsuccessfully with an error message.  "prop" is used for
	// customizing the error message
		//	void (* dies)(char *message, sgrp_t *prop);
	void jl_sg_smode_fncs(jl_t* pusr, uint8_t mode, jl_simple_fnt exit,
		jl_simple_fnt wups, jl_simple_fnt wdns, jl_simple_fnt term);
	void jl_sg_setlm(jl_t* pusr, uint8_t mode, jl_sg_wm_t loop);
	void jl_sg_kill(jl_t* pusr);
	void jl_sg_add_image(jl_t* pusr, char *pzipfile, uint16_t pigid);

/*
	_NKL_LSDL
		LSDL AKA. SDL or Simple Direct Media Layer:  LSDL is a popular library
		for making games.
*/
	void jl_dl_progname(jl_t* pusr, strt name);
// "gr.c"
	void jl_gr_draw_rect(jl_t* pusr, float x, float y, float w, float h,
		uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void jl_gr_draw_image(jl_t* pusr, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t c, u08t a);
	void jl_gr_sprite_draw(jl_t* pusr, jl_sprite_t *psprite);
	jl_sprite_t * jl_gr_sprite_make(
		jl_t* pusr, u16t g, u16t i, u08t c, u08t a,
		dect x, dect y, dect w, dect h,
		jl_simple_fnt loop, u32t ctxs);
	u08t jl_gr_sprite_collide(jl_t* pusr,
		jl_sprite_t *sprite1, jl_sprite_t *sprite2);
	void jl_gr_draw_text(jl_t* pusr, char *str, dect x, dect y, dect size,
		uint8_t a);
	void jl_gr_draw_numi(jl_t* pusr, uint32_t num, dect x, dect y, dect size,
		uint8_t a);
	void jl_gr_draw_text_area(jl_t* pusr, jl_sprite_t * psprite, char *txt);
	void jl_gr_draw_text_sprite(jl_t* pusr,jl_sprite_t * psprite,char *txt);
	void jl_gr_draw_ctxt(jl_t* pusr, char *str, dect p_y, uint8_t p_a);
	void jl_gr_draw_msge(jl_t* pusr, char* message);
	void jl_gr_term_msge(jl_t* pusr, char* message);
	void jl_gr_draw_slide_button(
		jl_t* pusr, jl_sprite_t * psprite, char *txt, float defaultx,
		float slidex, jl_ct_onevent_fnt prun);
	void jl_gr_togglemenubar(jl_t* pusr);
	void jl_gr_addicon(jl_t* pusr, uint16_t grp, uint8_t iid,
		uint8_t chr, jl_simple_fnt fno, jl_simple_fnt fnc);
	void jl_gr_addicon_flip(jl_t* pusr);
	void jl_gr_addicon_slow(jl_t* pusr);
	void jl_gr_addicon_name(jl_t* pusr);

/*
	JAL5_INPT

		INPT AKA JL_input is a library for grabbing input and attaching it to
		functions.
*/

/*
	JL/IO

		IO is like <stdio>.
*/

	//Change offset header to "this"
	void jl_io_offset(jl_t* pusr, char * this, int16_t tag);
	void jl_io_close_block(jl_t* pusr);
	void jl_io_tag_set(jl_t* pusr,
		int16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn);
	void jl_io_printc(jl_t* pusr, const char * print);
	void jl_io_printt(jl_t *pusr, uint8_t a, const char *print);
	void jl_io_printi(jl_t *pusr, int print);
	void jl_io_printd(jl_t *pusr, double print);

//FL
	// Save A File To The File System.  Save Data of "bytes" bytes in "file" to
	// file "name"
	void jl_fl_save(jl_t* pusr, void *file, const char *name,
		uint32_t bytes);

	// Load a File from the file system.  Returns bytes loaded from "file_name"
	strt jl_fl_load(jl_t* pusr, char *file_name);

	// Save file "filename" with contents "data" of size "dataSize" to package
	// "packageFileName"
	char jl_fl_pk_save(jl_t* pusr, char *packageFileName,
		char *fileName,	void *data, uint64_t dataSize);

	// Load file "filename" in package "packageFileName" & Return contents
	//-ERRF:
	//	ERRF_NONE	can't find filename in packageFileName
	//	ERRF_FIND	can't find packageFileName
	uint8_t *jl_fl_pk_load(jl_t* pusr, const char *packageFileName,
		const char *filename);
	
	//Load file "Fname" in default package & Return contents.
	uint8_t *jl_fl_media(jl_t* pusr, char *Fname, char *pzipfile,
		void *pdata, uint64_t psize);

	/**
	 * Create a folder (directory)
	*/
	void jl_fl_mkdir(jl_t* pusr, strt pfilebase);
	/**
	 * Return the location of the resource pack for program with name
	 * "pprg_name"
	*/
	strt jl_fl_get_resloc(jl_t* pusr, strt pprg_name, strt pfilename);
	/**
	 * Open directory for file viewer.
	**/
	void jl_fl_user_select_init(jl_t* pusr, char *program_name);
	/**
	 * Run the file viewer.
	**/
	void jl_fl_user_select_loop(jl_t* pusr);
	
	/**
	 * Get the results from the file viewer.
	**/
	char *jl_fl_user_select_get(jl_t* pusr);
	

/*
	JAL5_COMM

		COMM lets you send network packets.  It uses SDL_net.
*/

/*
	JAL5_VIDE

		VIDE is for editing sounds and graphics.
*/

/*
	JAL5_AUDI

		AUDI: This library can play and record / music or sound effects.
*/
	// fade out any previously playing music (If there is any) for
	// "p_secondsOfFadeOut" seconds, then fade in music with ID "p_IDinStack"
	// for "p_secondsOfFadeIn" seconds
	void audi_musi_play(jl_t* pusr, u32t p_IDinStack,
		u08t p_secondsOfFadeOut, u08t p_secondsOfFadeIn);

	// fade out any previously playing music (if there is any) for
	// "p_secondsOfFadeOut" seconds
	void audi_musi_halt(u08t p_secondsOfFadeOut);

	// returns 1 if music is playing otherwise 0
	uint8_t audi_musi_spli(void);

	// set where the music is coming from.  255 is left 0 is right
	void audi_sdir_setm(uint8_t left, uint8_t toCenter);

	// sets where music is comming from to center ( resets where music comes from )
	void audi_sdir_setd(void);

/*
	This a Jeron Lau project. JL_lib (c) 2014 
*/
