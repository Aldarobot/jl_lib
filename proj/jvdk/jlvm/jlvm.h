/*
 * JLVM(c) Jeron A. Lau
 * The header to be included within your programs you make with JLVM.
*/

#include <stdint.h>
#include "jlvm_sgrp.h"
#include "jlvm_inpt.h"

//Fixed point numbers go from -8.f to 8.f
#define JFXD(x) ((int16_t)(x*4000)) //create fixed number from float
#define JFLT(x) (((float)x)/4000.f) //create float from fixed number

//Do Nothing
void dont(void);

//JLGR (Prev: grph)
	/* pre defined screen locations */
	#define JAL5_GRPH_YDEP (JFXD(9./16.))
	/* screens */
	#define JL_SCREEN_UP 0 //upper screen
	#define JL_SCREEN_DN 1 //lower screen
	/* functions */
	void jal5_jlgr_draw_msge(char * str);

//Image ID's
#define IMGID_TEXT_IMAGE 1
#define IMGID_TERMINAL_TEXT 2
#define IMGID_LANDSCAPE 3
#define IMGID_TASK_BUTTON 4

//Variable Types
#define u08t uint8_t	//Small int
#define u16t uint16_t	//Short int
#define u32t uint32_t	//Normal int
#define u64t uint64_t	//Large int
#define s08t int8_t	//Small int
#define s16t int16_t	//Short int
#define s32t int32_t	//Normal int
#define s64t int64_t	//Large int
#define dect float	//floating point decimal
#define f08t s08t	//8-bit fixed point decimal
#define f16t s16t	//16-bit fixed point decimal
#define f32t s32t	//32-bit fixed point decimal
#define f64t s64t	//64-bit fixed point decimal
#define strt strt_t *

//5 bytes of information about the string are included
typedef struct{
	u08t* data; //Actual String
	u32t size; //Allocated Space In String
	u32t curs; //Cursor In String
	u08t type;
}strt_t;

//strt_t.Type:
	//TEMPORARY - Free after used
	#define STRT_TEMP 0 
	//NOT TEMPORARY
	#define STRT_KEEP 1 

typedef struct{
}sgrp_t;

/*
	JAL5_AMEM

		A simple memory library.  Includes creating variables, setting and
		getting variables, and doing simple and complicated math functions on
		the variables.  Has a specialized string type.
*/
	// convert "string" into a (temporary) sstrt and return it.
	strt amem_strt_c8ts(char *string);
	#define Strt(x) amem_strt_c8ts((void*)x)

	// allocates a strt of size "size" and returns it.
	strt amem_strt_make(u32t size, u08t type);
	
	strt amem_strt_merg(strt a, strt b, u08t type);
	
	strt amem_strt_fnum(s32t a);
	
	u32t amem_rand_nint(u32t a);
	
	// Tests if the next thing in array script is equivalent to particle
	u08t amem_test_next(char * script, char * particle);

/*
	DPL7_CLMP

		Clump is a handy memory library that includes various variable
		structures which are: Bitarrays, Linked List, Hash Tables,
		Data Trees and Huffman Codecs.
	
*/

/*
	JAL5_SGRP

		SGRP AKA. Simple JLVM Graphics Library is a window handling library.
		It is needed for handling the 2 screens that JLVM provides.  It also
		has support for things called modes.  An example is: your title screen
		of a game and the actual game would be on different modes.
*/
	//Create a window.  Allocate modes of count "mdec"
	void sgrp_mode_init(sgrp_user_t* pusr, u08t mdec);

	// Exit unsuccessfully with an error message.  "prop" is used for
	// customizing the error message
		//	void (* dies)(char *message, sgrp_t *prop);

	//Set the current mode loop functions
	void sgrp_mode_iset(
		sgrp_user_t* pusr,
		void (* exit)(sgrp_user_t* pusr),
		void (* wups)(sgrp_user_t* pusr),
		void (* wdns)(sgrp_user_t* pusr),
		void (* term)(sgrp_user_t* pusr));

	//Set Which Window
	void sgrp_wind_sett(sgrp_user_t* pusr, uint8_t window);

	//Kill The Program
	void sgrp_wind_kill(sgrp_user_t* pusr);

/*
	_NKL_LSDL
		LSDL AKA. SDL or Simple Direct Media Layer:  LSDL is a popular library
		for making games.
*/

/*
	JAL5_GRPH

		A High Level Graphics Library that supports sprites, texture loading,
		2D rendering & 3D rendering.
*/
	/*
	 * Draw An Image, 'jvct' is the library context, 'i' is the ID of the
	 * image.  'xywh' is where it is and how big it is drawn.  'chr' is 0
	 * unless you want to use the image as a charecter map, then it will
	 * zoom into charecter 'chr'.  'a' is the transparency each pixel is
	 * multiplied by; 255 is solid and 0 is totally invisble
	*/
	void grph_draw_simg(sgrp_user_t* pusr, s32t i,
		float x,float y,float w,float h,
		u08t chr, u08t a);
	//Draw "str" at 'x','y', size 'size', transparency 'a', [jvct=context]
	void grph_draw_text(sgrp_user_t* pusr, char *str, dect x, dect y, dect size,
		uint8_t a);
	//draw a number at "x","y" size: "size" transparency "a"
	void grph_draw_numi(sgrp_user_t* pusr, uint32_t num, dect x, dect y, dect size,
		uint8_t a);
	//Draw centered text on screen saying "strt" at y coordinate "p_y"
	void grph_draw_ctxt(sgrp_user_t* pusr, char *str, dect p_y);
	//Print message "str" on the screen
	void grph_draw_msge(char* str);
	//Print a message on the screen and then terminate the program
	void grph_term_msge(sgrp_user_t* pusr, char* message);

/*
	JAL5_INPT

		INPT AKA JL_input is a library for grabbing input and attaching it to
		functions.
*/

/*
	JAL5_SIOP

		SIOP is like stdio.
*/
	// Print "pstr" to the lowest level terminal [the one not drawn with
	// SDL/OpenGL]
	void siop_prnt_lwst(int8_t offs, strt this, strt print);

	// Print "pstr" to the lowest level terminal [the one not drawn with
	// SDL/OpenGL]
	void jal5_siop_cplo(int8_t offs, char * this, char * print);
	#define siop_prnt_cplo(x,y,z) jal5_siop_cplo(x,y,z)

/*
	JAL5_FILE

		FILE allows you to modify the file system.  It uses libzip.
*/
	// Save A File To The File System.  Save Data of "bytes" bytes in "file" to
	// file "name"
	void file_file_save(sgrp_user_t* pusr, char *file, char *name,
		uint32_t bytes);

	// Load a File from the file system.  Returns bytes loaded from "file_name"
	uint8_t *file_file_load(sgrp_user_t* pusr, char *file_name);

	// Load a file "filename" in package "packageFileName". Return bytes loaded
	uint8_t *file_pkdj_load(sgrp_user_t* pusr, char *Fname);

	// Save file "filename" with contents "data" of size "dataSize" to package
	// "packageFileName"
	char file_pkdj_save(sgrp_user_t* pusr, char *packageFileName,
		char *fileName,	void *data, uint64_t dataSize);

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
	void audi_musi_play(sgrp_user_t* pusr, u32t p_IDinStack,
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
	This a Jeron Lau project. JLVM (c) 2014 
*/
