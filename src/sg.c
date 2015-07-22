/*
 * JL_lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * sg.c
 *	sg AKA. Simple JL_lib Graphics Library is a window handling library.
 *	It is needed for handling the 2 screens that JL_lib provides.  It also
 *	has support for things called modes.  An example is: your title screen
 *	of a game and the actual game would be on different modes.
*/

#include "header/jl_pr.h"

// SG Prototypes
void jl_gl_prer_use(jvct_t *_jlc, jl_vo* pv);
void jl_gl_prer_off(jvct_t *_jlc);
void jl_gl_draw_prerendered(jvct_t *_jlc, jl_vo* pv);

//Prototypes
	//LIB INITIALIZATION fn(Context)
	void _jl_cm_init(jvct_t* _jlc);
	void _jl_au_init(jvct_t* _jlc);
	void _jl_fl_inita(jvct_t* _jlc);
	void _jl_fl_initb(jvct_t* _jlc);
	void _jl_gr_init(jvct_t* _jlc);
	void _jl_ct_init(jvct_t* _jlc);
	void _jl_gl_init(jvct_t* _jlc);
	void _jl_io_init(jvct_t* _jlc);
	void _jl_dl_inita(jvct_t* _jlc);
	void _jl_dl_initb(jvct_t* _jlc);
	jvct_t* _jl_me_init(void);

	//LIB LOOPS: parameter is context
	void _jl_gr_loop(jl_t* jlc);
	void _jl_ct_loop(jvct_t* _jlc);
	void _jl_sg_loop(jvct_t* _jlc);
	void _jl_dl_loop(jvct_t* _jlc);
	void _jl_au_loop(jvct_t* _jlc);

	//LIB KILLS
	void _jl_dl_kill(jvct_t* _jlc);
	void _jl_me_kill(jvct_t* jprg);
	void _jl_fl_kill(jvct_t* _jlc);
	void _jl_io_kill(jl_t* jlc);
	void _jl_au_kill(jvct_t *_jlc);
	void _jl_sg_kill(jl_t* jlc);

// Constants
	//ALL IMAGES: 1024x1024
	#define TEXTURE_WH 1024*1024 
	#define JL_IMG_HEADER "JLVM0:JYMJ\0"
	//1bpp Bitmap = 1048832 bytes (Color Key(256)*RGBA(4), 1024*1024)
	#define IMG_FORMAT_LOW 1048832 
	//2bpp HQ bitmap = 2097664 bytes (Color Key(256*2=512)*RGBA(4), 2*1024*1024)
	#define IMG_FORMAT_MED 2097664
	//3bpp Picture = 3145728 bytes (No color key, RGB(3)*1024*1024)
	#define IMG_FORMAT_PIC 3145728
	//
	#define IMG_SIZE_LOW (1+strlen(JL_IMG_HEADER)+(256*4)+(1024*1024)+1)

// Struct Types:
	typedef struct{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	}jgr_img_col_t;

	//Photo-Quality Picture
	typedef struct{
		uint8_t tex_pixels[3*1024*1024];//stored as uint8_t(3)
	}jgr_pic_t;

	//High-Quality Bitmap
	typedef struct{
		jgr_img_col_t key[256][256];
		uint8_t tex_pixels[2*1024*1024];//stored as uint8_t(2)
	}jgr_hqb_t;

	//Bitmap
	typedef struct{
		jgr_img_col_t key[256];
		uint8_t tex_pixels[1024*1024];//stored as uint8_t(1)
	}jgr_img_t;
	
//Functions:

void _jl_sg_mode_add(jvct_t* _jlc, uint8_t mode) {
	//Add & Allocate a new mode
	_jlc->jlc->mdec = mode + 1;
	_jlc->sg.mdes = realloc(_jlc->sg.mdes,
		(_jlc->jlc->mdec + 1) * sizeof(__sg_mode_t));
	_jlc->sg.mdes[mode].tclp[JL_SG_WM_EXIT] = jl_sg_exit;
	_jlc->sg.mdes[mode].tclp[JL_SG_WM_UP] = jl_dont;
	_jlc->sg.mdes[mode].tclp[JL_SG_WM_DN] = jl_dont;
	_jlc->sg.mdes[mode].tclp[JL_SG_WM_TERM] = jl_dont;
	_jlc->sg.mdes[mode].tclp[JL_SG_WM_RESZ] = jl_dont;
}

/**
 * Set the loop functions for a mode.
 *
 * @param jlc: The library context.
 * @param mode: The mode to change the loop functions of.
 * @param wm: Which loop to change.
 *	JL_SG_WM_EXIT: Called when "Back Button" Is Pressed.  "Back Button" is:
 *		- 3DS/WiiU: Select
 *		- Android: Back
 *		- Computer: Escape
 *		The default is to quit the program.  If set to something else
 *		then the function will loop forever unless interupted by a
 *		second press of the "Back Button" or unless the mode is changed.
 *	JL_SG_WM_UP: Called when upper screen is visible.  Whatever draw
 *		functions are called in this loop will be displayed on the upper
 *		screen each frame when in this mode.
 *	JL_SG_WM_DN: Like JL_SG_WM_UP but for lower screen
 *	JL_SG_WM_TERM: This loop is called when instead of being in either upper
 *		or lower screen the only thing displayed is the terminal.
 *	JL_SG_WM_RESZ: This loop is called when the window is resized.
 * @param loop: What to change the loop to.
*/
void jl_sg_mode_set(jl_t* jlc, uint8_t mode, uint8_t wm, jl_simple_fnt loop) {
	jl_gr_draw_msge(jlc, "Switching Mode...");
	jvct_t* _jlc = jlc->_jlc;
	
	if(mode > _jlc->jlc->mdec - 1) _jl_sg_mode_add(_jlc, mode);
	_jlc->sg.mdes[mode].tclp[wm] = loop;
	// Reset things
	_jlc->ct.heldDown = 0;
	jl_gr_draw_msge(jlc, "Switched Mode!");
}

/**
 * Temporarily change the mode functions without actually changing the mode.
 * @param jlc: The library context.
 * @param exit: exit loop
 * @param upsl: upper screen loop
 * @param dnsl: lower screen loop
 * @param term: terminal screen loop
 */
void jl_sg_mode_override(jl_t* jlc, uint8_t wm, jl_simple_fnt loop) {
	jvct_t* _jlc = jlc->_jlc;

	_jlc->sg.mode.tclp[wm] = loop;
}

/**
 * Reset any functions overwritten with jl_sg_mode_override().
 * @param jlc: The library context.
 */
void jl_sg_mode_reset(jl_t* jlc) {
	jvct_t* _jlc = jlc->_jlc;
	int i;

	for(i = 0; i < JL_SG_WM_MAX; i++)
		_jlc->sg.mode.tclp[i] = _jlc->sg.mdes[jlc->mode].tclp[i];
}

/**
 * Switch which mode is in use & the current mode loop.
 * @param jlc: The library context.
 * @param mode: The mode to switch to.
 * @param loop: The loop to switch to.
 */
void jl_sg_mode_switch(jl_t* jlc, uint8_t mode, jl_sg_wm_t loop) {
	jlc->mode = mode;
	jlc->loop = loop;
	jl_sg_mode_reset(jlc);
}

void _jl_sg_load_jlpx(jvct_t* _jlc,uint8_t *data,void **pixels,int *w,int *h) {
	if(data == NULL) {
		jl_sg_kill(_jlc->jlc, ":NULL DATA!\n");
	}else if(data[_jlc->sg.init_image_location] == 0) {
		return;
	}
	jgr_img_t *image = NULL;
	jl_me_alloc(_jlc->jlc, (void**)&image, sizeof(jgr_img_t), 0);
	
	jl_io_offset(_jlc->jlc, JL_IO_PROGRESS, "LOAD"); // {
	jl_io_offset(_jlc->jlc, JL_IO_PROGRESS, "JLPX"); // {
	
	//Check If File Is Of Correct Format
	jl_io_printc(_jlc->jlc, "loading image - header@");
	jl_io_printi(_jlc->jlc, _jlc->sg.init_image_location);
	jl_io_printc(_jlc->jlc,":");
	jl_io_printt(_jlc->jlc, 8, (char *)data
		+_jlc->sg.init_image_location);
	jl_io_printc(_jlc->jlc,"\n");

	char *testing = malloc(strlen(JL_IMG_HEADER)+1);
	uint32_t i;

	for(i = 0; i < strlen(JL_IMG_HEADER); i++) {
		testing[i] = data[_jlc->sg.init_image_location+i];
	}
	testing[strlen(JL_IMG_HEADER)] = '\0';
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "JLPX"); // =
	jl_io_printc(_jlc->jlc, "header:");
	jl_io_printc(_jlc->jlc, testing);
	jl_io_printc(_jlc->jlc, "\n");

	if(strcmp(testing, JL_IMG_HEADER) != 0) {
		_jl_fl_errf(_jlc, ":error: bad file type:\n:");
		_jl_fl_errf(_jlc, testing);
		_jl_fl_errf(_jlc, "\n:!=\n:");
		_jl_fl_errf(_jlc, JL_IMG_HEADER);
		jl_sg_kill(_jlc->jlc, "\n");
	}
	uint8_t tester = data[_jlc->sg.init_image_location+strlen(JL_IMG_HEADER)];
	int FSIZE; //The size of the file.
	if(tester == 1) { //Normal Quality[Lowy]
		FSIZE = IMG_SIZE_LOW;
		jl_io_printc(_jlc->jlc, "normal quality\n");
	}else if(tester == 2) { //High Quality[Norm]
		FSIZE = IMG_FORMAT_MED;
		jl_io_printc(_jlc->jlc, "high quality\n");
	}else if(tester == 3) { //Picture[High]
		FSIZE = IMG_FORMAT_PIC;
		jl_io_printc(_jlc->jlc, "pic quality\n");
	}else{
		jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "JLPX");
		jl_io_printc(_jlc->jlc, "bad file type(must be 1,2 or 3) is: ");
		jl_io_printi(_jlc->jlc, tester);
		jl_io_printc(_jlc->jlc, "\n");
		jl_sg_kill(_jlc->jlc, ":bad file type(must be 1,2 or 3)\n");
	}
	uint32_t ki;
	ki = strlen(JL_IMG_HEADER)+1;
	for(i = 0; i < 256; i++) {
		image->key[i].r = data[_jlc->sg.init_image_location+ki];
		ki++;
		image->key[i].g = data[_jlc->sg.init_image_location+ki];
		ki++;
		image->key[i].b = data[_jlc->sg.init_image_location+ki];
		ki++;
		image->key[i].a = data[_jlc->sg.init_image_location+ki];
		ki++;
	}
	jl_io_offset(_jlc->jlc, JL_IO_INTENSE, "JLPX");
	jl_io_printc(_jlc->jlc, "Key: ");
	jl_io_printi(_jlc->jlc, image->key[0].r);
	jl_io_printc(_jlc->jlc, ", ");
	jl_io_printi(_jlc->jlc, image->key[0].g);
	jl_io_printc(_jlc->jlc, ", ");
	jl_io_printi(_jlc->jlc, image->key[0].b);
	jl_io_printc(_jlc->jlc, ", ");
	jl_io_printi(_jlc->jlc, image->key[0].a);
	jl_io_printc(_jlc->jlc, "\n");
	for(i = 0; i < TEXTURE_WH; i++) {
		image->tex_pixels[i] = data[_jlc->sg.init_image_location+ki];
		ki++;
	}
	// Don't comment out this line!!!! it will cause an endless freeze!
	_jlc->sg.init_image_location+=FSIZE+1;
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "JLPX");
	jl_io_printc(_jlc->jlc, "creating texture...\n");

	uint8_t *tex_pixels = NULL;
	//R(1)+G(1)+B(1)+A(1) = 4
	jl_me_alloc(_jlc->jlc, (void**)&tex_pixels, TEXTURE_WH*4, 0);
	for(i = 0; i < TEXTURE_WH; i++) {
		tex_pixels[(i*4)+0] = image->key[image->tex_pixels[i]].r;
		tex_pixels[(i*4)+1] = image->key[image->tex_pixels[i]].g;
		tex_pixels[(i*4)+2] = image->key[image->tex_pixels[i]].b;
		tex_pixels[(i*4)+3] = image->key[image->tex_pixels[i]].a;
	}
	int *a = NULL;
	jl_me_alloc(_jlc->jlc, (void**)&a, 2*sizeof(int), 0);
	a[0] = 1024;
	a[1] = 1024;
	//Set Return values
	pixels[0] = tex_pixels;
	w[0] = a[0];
	h[0] = a[1];
	//Cleanup
	free(image);
	
	jl_io_close_block(_jlc->jlc); // } :Close Block "JLPX"
	jl_io_close_block(_jlc->jlc); // } :Close Block "LOAD"
}

//loads next image in the currently loaded file.
static inline uint8_t _jl_sg_load_next_img(jvct_t * _jlc) {
	void *fpixels = NULL;
	int fw;
	int fh;
	jl_io_offset(_jlc->jlc, JL_IO_PROGRESS, "IMGS");
	_jl_sg_load_jlpx(_jlc, _jlc->sg.image_data, &fpixels, &fw, &fh);
	jl_io_printc(_jlc->jlc, "{IMGS}\n");
	if(fpixels == NULL) {
		jl_io_printc(_jlc->jlc, "loaded ");
		jl_io_printi(_jlc->jlc, _jlc->sg.image_id);
		jl_io_printc(_jlc->jlc, "images!\n");
		_jlc->jlc->info = _jlc->sg.image_id;
		jl_io_close_block(_jlc->jlc); //Close Block "IMGS"
		jl_io_printc(_jlc->jlc, "IL\n");
		return 0;
	}else{
		jl_gl_maketexture(_jlc->jlc, _jlc->sg.igid,
			_jlc->sg.image_id, fpixels, 1024, 1024);
		jl_io_printc(_jlc->jlc, "created image #");
		jl_io_printi(_jlc->jlc, _jlc->sg.igid);
		jl_io_printc(_jlc->jlc, ":");
		jl_io_printi(_jlc->jlc, _jlc->sg.image_id);
		jl_io_printc(_jlc->jlc, "!\n");
//		#endif
		_jlc->sg.image_id++;
		jl_io_close_block(_jlc->jlc); //Close Block "IMGS"
		jl_io_printc(_jlc->jlc, "IL____\n");
		return 1;
	}
}

//Load the images in the image file
static inline void _jl_sg_init_images(jvct_t * _jlc, uint8_t *data, uint16_t p){
	char *stringlength;

	_jlc->sg.init_image_location = 0;
	_jlc->sg.image_id= 0; //Reset Image Id
	_jlc->sg.igid = p;
	_jlc->sg.image_data = data;

	jl_io_offset(_jlc->jlc, JL_IO_PROGRESS, "INIM");
	jl_io_printc(_jlc->jlc, "loading images...\n");
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "INIM");
	stringlength = jl_me_string_fnum(_jlc->jlc, (int)strlen((void *)data));
	jl_io_printc(_jlc->jlc, "lne ");
	jl_io_printc(_jlc->jlc, stringlength);
	jl_io_printc(_jlc->jlc, "\n");
	free(stringlength);
	jl_io_close_block(_jlc->jlc); //Close Block "INIM"
//load textures
	while(_jl_sg_load_next_img(_jlc));
}

static uint32_t _jl_sg_quit(jvct_t* _jlc, int rc) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "KILL"); // {
	if(_jlc->has.graphics)
		jl_gr_draw_msge(_jlc->jlc, "QUITING JLLIB....");
	_jl_fl_errf(_jlc, ":Quitting....\n"); //Exited properly
	_jl_au_kill(_jlc);
	_jl_dl_kill(_jlc);
	_jl_fl_kill(_jlc);
	_jl_sg_kill(_jlc->jlc);
	_jl_fl_errf(_jlc, ":No Error! YAY!\n"); //Exited properly
	_jl_io_kill(_jlc->jlc);
	_jl_me_kill(_jlc);
	printf("exiting with return value %d\n", rc);
	exit(rc);
	_jl_fl_errf(_jlc, ":What The Hell?  This is an impossible error!\n");
	return JL_RTN_IMPOSSIBLE;
}

/**
 * Show Error On Screen if screen is available.  Also store error in errf.txt.
 * Quit, And Return -1 to show there was an error.
*/
void jl_sg_kill(jl_t* jlc, char * msg) {
	//TODO: Make Screen With Window Saying Error Message Followed By Prompt.
	//	Also, don't automatically quit, like it does now!  ERQT can be
	//	inline at that point.
	_jl_fl_errf(jlc->_jlc, ":Quitting On Error...\n");
	jl_io_offset(jlc, JL_IO_INTENSE, "ERQT");
	jl_io_printc(jlc, msg);
	_jl_fl_errf(jlc->_jlc, msg);
	_jl_sg_quit(jlc->_jlc, JL_RTN_FAIL);
	jl_io_close_block(jlc); //Close Block "ERQT"
}

/**
 * Go to exit mode or exit if in exit mode.
 */
void jl_sg_exit(jl_t* jlc) {
	if(jlc->loop == JL_SG_WM_EXIT)
		_jl_sg_quit(jlc->_jlc, JL_RTN_SUCCESS);
	else
		jlc->loop = JL_SG_WM_EXIT;
}

void _jl_sg_kill(jl_t* jlc) {
	// Delete window size data.
	jvct_t * _jlc = jlc->_jlc;
	jl_gr_prer_old(jlc, _jlc->sg.bg.up);
	jl_gr_prer_old(jlc, _jlc->sg.bg.dn);
}

/**
 * Do Nothing
 */
void jl_dont(jl_t* jlc) { }

//void sgrp_

static void _sg_time_reset(jvct_t* _jlc) {
	_jlc->sg.prev_tick = _jlc->sg.this_tick;
}

//return how many frames passed since last call
static inline float _jl_sg_seconds_passed(jvct_t* _jlc) {
	_jlc->sg.processingTimeMillis=
		(_jlc->sg.this_tick=SDL_GetTicks())-_jlc->sg.prev_tick;
	if(_jlc->sg.processingTimeMillis <= JL_MAIN_SAPT) {
		SDL_Delay(JL_MAIN_SAPT-_jlc->sg.processingTimeMillis);
//		printf("[TIMING] fast: 1\n");
		_sg_time_reset(_jlc);
		return 1.f / ((float)JL_MAIN_SFPS);
	}else{
		float frames =
			((float)_jlc->sg.processingTimeMillis)/
			((float)JL_MAIN_SAPT);
/*		printf("[TIMING] slow: %f, %f, %f\n",
			(float)_jlc->sg.processingTimeMillis,
			(float)JL_MAIN_SAPT, frames);*/
		_sg_time_reset(_jlc);
		return frames / ((float)JL_MAIN_SFPS);
	}
}

/**
 * Load all images from a zipfile and give them ID's.
 * info: info is set to number of images loaded.
 * @param jlc: library context
 * @param pzipfile: full file name of a zip file.
 * @param pigid: which image group to load the images into.
*/
void jl_sg_add_image(jl_t* jlc, char *pzipfile, uint16_t pigid) {
	jl_io_offset(jlc, JL_IO_PROGRESS, "LIMG");
	//Load Graphics
	uint8_t *img = jl_fl_media(jlc, "jlex/2/_img", pzipfile,
		jal5_head_jlvm(), jal5_head_size());

	jl_io_printc(jlc, "Loading Images...\n");
	if(img != NULL)
		_jl_sg_init_images(jlc->_jlc, img, pigid);
	else
		jl_io_printc(jlc, "Loaded 0 images!\n");
	jl_io_offset(jlc, JL_IO_PROGRESS, "LIMG"); // {
	jl_io_printc(jlc, "Loaded Images...\n");
	jl_io_close_block(jlc); // }
}

static void _jl_sg_screen_draw(jl_t* jlc, float ytrans, jl_vo* bg, uint8_t lp) {
	jvct_t * _jlc = jlc->_jlc;
	jl_vec3_t translate = { 0., ytrans, 0. };

	_jlc->sg.cbg = bg;

	// Draw the vertex object.
	jl_gr_draw_vo(jlc, bg, &translate);
	// Use the prerenderer for this screen.
	jl_gl_prer_use(_jlc, _jlc->sg.cbg);
	// Run the screen's loop
	_jlc->sg.mode.tclp[lp](jlc);
	// If BG is lower screen: Draw Menu Bar & Mouse - on lower screen
	if(bg == _jlc->sg.bg.dn) _jl_gr_loop(jlc);
	// Turn off the prerenderer.
	jl_gl_prer_off(_jlc);
	// Draw the prerendered texture.
	jl_gr_draw_pr(jlc, bg, &translate);
}

// Double screen loop
static void _jl_sg_loop_ds(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;

	// Draw lower screen - default screen
	_jl_sg_screen_draw(jlc, 0.f, _jlc->sg.bg.up,
		(jlc->loop == JL_SG_WM_UP) ? JL_SG_WM_DN : JL_SG_WM_UP);
	// Draw upper screen - alternate screen
	_jl_sg_screen_draw(jlc, _jlc->sg.screen_height, _jlc->sg.bg.dn,
		jlc->loop);
}

// Single screen loop
static void _jl_sg_loop_ss(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;

	// Draw lower screen - default screen
	_jl_sg_screen_draw(jlc, 0.f, _jlc->sg.bg.dn, jlc->loop);
}

void _jl_sg_loop(jvct_t *_jlc) {
	// Run the current loop.
	_jlc->sg.loop(_jlc->jlc);
}

void _jl_sg_resize(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	const float isDoubleScreen = (double)jlc->smde;
	const float shiftx = _jlc->dl.shiftx;
	const float shifty = _jlc->dl.shifty / (isDoubleScreen+1.);
	const float rcrdx = shiftx/2.;
	const float rcrdy = (shifty * (1. + isDoubleScreen))/2.;
	const float rcrdw = 1. - _jlc->dl.shiftx;
	const float rcrdh = (1./(1.+isDoubleScreen)) - shifty;
	jl_rect_t rcrd = {
		rcrdx, rcrdy * jl_dl_p(jlc),
		rcrdw, rcrdh * jl_dl_p(jlc)
	};
	uint8_t rclr_up[4] = { 127,	127,	127,	255 };
	uint8_t rclr_dn[4] = { 0,	127,	0,	255 };
	uint8_t rclr_bg[4] = { 0,	255,	0,	255 };

	_jlc->sg.screen_height = rcrd.h;
	if(jlc->smde) {
		// Update the rectangle backgrounds.
		jl_gr_vos_rec(jlc, _jlc->sg.bg.up, rcrd, rclr_up, 0);
		jl_gr_prer_new(jlc, _jlc->sg.bg.up);
		jl_gr_vos_rec(jlc, _jlc->sg.bg.dn, rcrd, rclr_dn, 0);
		jl_gr_prer_new(jlc, _jlc->sg.bg.dn);
		// Set double screen loop.
		_jlc->sg.loop = _jl_sg_loop_ds;
	}else{
		// Update the rectangle backgrounds.
		jl_gr_vos_rec(jlc, _jlc->sg.bg.dn, rcrd, rclr_bg, 0);
		jl_gr_prer_new(jlc, _jlc->sg.bg.dn);
		// Set single screen loop.
		_jlc->sg.loop = _jl_sg_loop_ss;
	}
}

static inline void _jl_sg_initb(jvct_t * _jlc) {
	_jlc->jlc->errf = JL_ERR_NERR; //no error
	_jlc->jlc->psec = 0.f;
	_jlc->jlc->mode = 0;
	_jlc->jlc->mdec = 0;
	_jlc->sg.mdes = NULL;
	_jlc->jlc->loop = JL_SG_WM_TERM; //Set Default Window To Terminal
	_jlc->sg.prev_tick = 0;
	_jlc->sg.bg.up = jl_gr_vo_new(_jlc->jlc);
	_jlc->sg.bg.dn = jl_gr_vo_new(_jlc->jlc);
	_jl_sg_resize(_jlc->jlc);
}

static inline void _jl_sg_inita(jvct_t * _jlc) {
	//Set Up Variables
	_jlc->gl.textures = NULL;
	_jlc->gl.tex.uniforms.textures = NULL;
	_jlc->sg.image_id = 0; //Reset Image Id
	_jlc->sg.igid = 0; //Reset Image Group Id
	//Load Graphics
	_jlc->gl.allocatedg = 0;
	_jlc->gl.allocatedi = 0;
	jl_sg_add_image(_jlc->jlc,
		(void*)jl_fl_get_resloc(_jlc->jlc, "JLLB", "media.zip"), 0);
}

/*
	x 0-255
	y 0-191(upper screen), 192-383(lower screen)
*/

//NON_USER FUNCTION

static inline void _jl_sg_init_done(jvct_t *_jlc) {
	_jlc->has.graphics = 1; //Graphics Now Available For Use
	jl_gr_draw_msge(_jlc->jlc, "LOADING JLLIB....");
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "JLLB"); //"JLLB" to SIMPLE
	jl_io_printc(_jlc->jlc, "started up display ");
	jl_io_printc(_jlc->jlc,
		jl_me_string_fnum_tmp(_jlc->jlc, _jlc->dl.full_w));
	jl_io_printc(_jlc->jlc, "x");
	jl_io_printc(_jlc->jlc,
		jl_me_string_fnum_tmp(_jlc->jlc, _jlc->dl.full_h));
	jl_io_printc(_jlc->jlc, "\n");
}

//Initialize The Libraries Needed At Very Beginning: The Base Of It All
static inline jvct_t* _jl_sg_init_blib(void) {
	// Memory
	jvct_t* _jlc = _jl_me_init(); // Create The Library Context
	// Other
	_jl_io_init(_jlc); // Allow Printing
	return _jlc;
}

static inline void _jl_sg_init_libs(jvct_t *_jlc) {
	_jl_dl_inita(_jlc); //create the window.
	_jl_fl_inita(_jlc); //prepare for loading media packages.
	_jl_sg_inita(_jlc); //Load default graphics from package.
	_jl_gl_init(_jlc); //Drawing Set-up
	_jl_dl_initb(_jlc); //update viewport.
	_jl_sg_init_done(_jlc); //Draw "loading jl_lib" on the screen.
	_jl_ct_init(_jlc); //Prepare to read input.
	_jl_sg_initb(_jlc);
	_jl_fl_initb(_jlc);
	_jl_au_init(_jlc); //Load audiostuffs from packages
	_jl_gr_init(_jlc); //Set-up sprites & menubar
}

static inline void _jl_ini(jvct_t *_jlc) {
	jl_io_printc(_jlc->jlc, "STARTING JL_lib Version" JL_VERSION "....\n");

	_jl_sg_init_libs(_jlc);
	hack_user_init(_jlc->jlc);

	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "JLLB"); //"JLLB" to SIMPLE
	jl_io_printc(_jlc->jlc, "Init5...\n");
	
	jl_io_offset(_jlc->jlc, JL_IO_MINIMAL, "JLLB"); //"JLLB" to MINIMAL
	jl_io_printc(_jlc->jlc, "Initialized!\n");
}

// Main.c
// void _jl_sg_resize(jl_t* jlc);

void main_resz(jvct_t* _jlc) {
	// Update the actual window.
	_jl_dl_resize(_jlc, _jlc->ct.event.window.data1,
		_jlc->ct.event.window.data2);
	// Update the size of the background.
	_jl_sg_resize(_jlc->jlc);
	// Allow the user to update their graphics.
	_jlc->sg.mode.tclp[JL_SG_WM_RESZ](_jlc->jlc);
}

static inline void _main_loop(jvct_t* _jlc) {
	_jl_fl_errf(_jlc, ":going into loop....\n");
	while(1) {
		//Update events.
		_jl_ct_loop(_jlc);
		//Check the amount of time passed since last frame.
		_jlc->jlc->psec = _jl_sg_seconds_passed(_jlc);
		//Redraw screen.
		_jl_sg_loop(_jlc);
		//Update Screen.
		_jl_dl_loop(_jlc);
		//Play Audio.
		_jl_au_loop(_jlc);
	}
}

int32_t main(int argc, char *argv[]) {
	jvct_t* _jlc = _jl_sg_init_blib(); //Set Up Memory And Logging

	// Initialize JL_lib!
	_jl_ini(_jlc);
	// Run the Loop
	if(_jlc->jlc->mdec) _main_loop(_jlc);
	// Kill the program
	_jl_sg_quit(_jlc, JL_RTN_SUPER_IMPOSSIBLE);
	// Impossible to reach this point
	return JL_RTN_COMPLETE_IMPOSSIBLE;
}

/**
 * @mainpage
 * @section Library Description
 * 
*/
