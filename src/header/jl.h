/*
 * JL_lib(c) Jeron A. Lau
 * The header to be included within your programs you make with JL_lib.
*/

#include <stdint.h>
#include "jl_me.h"
#include "jl_sg.h"
#include "jl_ct.h"
#include "clump.h"

	//Do Nothing
	void dont(jl_t* pusr);

/*
	JAL5_jl_me

		A simple memory library.  Includes creating variables, setting and
		getting variables, and doing simple and complicated math functions on
		the variables.  Has a specialized string type.
*/
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
	strt jl_me_strt_c8ts(char *string);
	#define Strt(x) jl_me_strt_c8ts((void*)x)
	
	//Return a string that has the contents of "a" followed by "b"
	strt jl_me_strt_merg(strt a, strt b, u08t type);
	
	//Print a number out as a string and return it (Type=STRT_TEMP)
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

/*
	DPL7_CLMP

		Clump is a handy memory library that includes various variable
		structures which are: Bitarrays, Linked List, Hash Tables,
		Data Trees and Huffman Codecs.
	
*/

/*
	JAL5_SG

		SG AKA. Simple JL_lib Graphics Library is a window handling library.
		It is needed for handling the 2 screens that JL_lib provides.  It also
		has support for things called modes.  An example is: your title screen
		of a game and the actual game would be on different modes.
*/
	//Create a window.  Allocate modes of count "mdec"
	void jl_sg_mode_init(jl_t* pusr, u08t mdec);

	// Exit unsuccessfully with an error message.  "prop" is used for
	// customizing the error message
		//	void (* dies)(char *message, sgrp_t *prop);

	//Set the current mode loop functions
	void jl_sg_smode_fncs(
		jl_t* pusr,
		fnct(void, exit, jl_t* pusr),
		fnct(void, wups, jl_t* pusr),
		fnct(void, wdns, jl_t* pusr),
		fnct(void, term, jl_t* pusr));

	//Set Which Window
	void jl_sg_set_window(jl_t* pusr, uint8_t window);

	//Kill The Program
	void jl_sg_kill(jl_t* pusr);
	
	/**
	 * Add images From program "pprg" in file "pfile"
	 *
	 * @info info is set to number of images loaded.
	 * @param pusr:library context
	 * @param pprg: program title
	 * @param pfile: name of file to load within the program's directory
	*/
	void jl_sg_add_image(jl_t* pusr, strt pprg, strt pfile);

/*
	_NKL_LSDL
		LSDL AKA. SDL or Simple Direct Media Layer:  LSDL is a popular library
		for making games.
*/
	/*
	 * Set the program title.  Used as window name, and as resource
	 * directory.
	*/
	void lsdl_prog_name(strt name);

/*
	JAL5_GRPH

		A High Level Graphics Library that supports sprites, texture loading,
		2D rendering & 3D rendering.
*/
	/**
	 * Draw An Image.
	 *
	 * @param 'pusr': library context
	 * @param 'i':  the ID of the image.
	 * @param 'xywh': where and how big it is drawn.
	 * @param 'c': is 0 unless you want to use the image as
	 * 	a charecter map, then it will zoom into charecter 'chr'.
	 * @param 'a': the transparency each pixel is multiplied by; 255 is
	 *	solid and 0 is totally invisble.
	**/
	void jl_gr_draw_image(jl_t* pusr, u16t g, u16t i,
		float x,float y,float w,float h,
		u08t c, u08t a);

	/**
	 * Draw the sprite.
	 *
	 * @param 'pusr': library context
	 * @param 'psprite': the sprite to draw.
	**/
	void jl_gr_sprite_draw(jl_t* pusr, jl_sprite_t *psprite);
	
	jl_sprite_t * jl_gr_sprite_make(
		jl_t* pusr, u16t g, u16t i, u08t c, u08t a,
		dect x, dect y, dect w, dect h,
		fnct(void, loop, jl_t* pusr), u32t ctxs);
	
	/**
	 * test if 2 sprites collide.
	 *
	 * @param 'pusr': library context
	 * @param 'sprite1': sprite 1
	 * @param 'sprite2': sprite 2
	 * @return 0: if the sprites don't collide in their bounding boxes.
	 * @return 1: if the sprites do collide in their bounding boxes.
	**/
	u08t jl_gr_sprite_collide(jl_t* pusr,
		jl_sprite_t *sprite1, jl_sprite_t *sprite2);

	//Draw "str" at 'x','y', size 'size', transparency 'a', [jvct=context]
	void jl_gr_draw_text(jl_t* pusr, char *str, dect x, dect y, dect size,
		uint8_t a);
	//draw a number at "x","y" size: "size" transparency "a"
	void jl_gr_draw_numi(jl_t* pusr, uint32_t num, dect x, dect y, dect size,
		uint8_t a);
	/**
	 * Draw text within the boundary of a sprite
	 * @param 'pusr': library context
	 * @param 'psprite': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_area(jl_t* pusr, jl_sprite_t * psprite, char *txt);
	/**
	 * Draw a sprite, then draw text within the boundary of a sprite
 	 * @param 'pusr': library context
	 * @param 'psprite': the boundary sprite
	 * @param 'txt': the text to draw
	**/
	void jl_gr_draw_text_sprite(jl_t* pusr,jl_sprite_t * psprite,char *txt);
	//Draw centered text on screen saying "strt" at y coordinate "p_y"
	void jl_gr_draw_ctxt(jl_t* pusr, char *str, dect p_y);
	//Print message "str" on the screen
	void jl_gr_draw_msge(char* str);
	//Print a message on the screen and then terminate the program
	void jl_gr_term_msge(jl_t* pusr, char* message);
	/**
	 * Draw a slide button, and activate if it is pressed.
	 * @param 'pusr': the libary context
 	 * @param 'pusr': the libary context
	**/
	void jl_gr_draw_slide_button(
		jl_t* pusr, jl_sprite_t * psprite, char *txt, float defaultx,
		float slidex, fnc_onevent_t(prun));
	/**
	 * toggle whether or not to show the menu bar.
	 *
	 * @param pusr: the libary context
	**/
	void jl_gr_togglemenubar(jl_t* pusr);

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
	void jl_io_offset(jl_t* pusr, char * this);

	// Print "pstr" to the lowest level terminal [the one not drawn with
	// SDL/OpenGL]
	void jl_io_print_lows(jl_t* pusr, strt print);

	// Print "pstr" to the lowest level terminal [the one not drawn with
	// SDL/OpenGL]
	void jl_io_print_lowc(jl_t* pusr, char * print);

/*
	JAL5_FILE

		FILE allows you to modify the file system.  It uses libzip.
*/
	// Save A File To The File System.  Save Data of "bytes" bytes in "file" to
	// file "name"
	void jl_fl_save(jl_t* pusr, void *file, char *name,
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
	uint8_t *jl_fl_pk_load(jl_t* pusr, char *packageFileName,
		char *filename);
	
	//Load file "Fname" in default package & Return contents.
	uint8_t *jl_fl_pk_mnld(jl_t* pusr, char *Fname);

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
