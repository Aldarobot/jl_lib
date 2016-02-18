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
**/

#include "header/jl_pr.h"

// SG Prototypes
void jl_gl_draw_prendered(jvct_t *_jlc, jl_vo_t* pv);
void jl_gl_pr_scr_set(jvct_t *_jlc, jl_vo_t* vo);
void _jl_gr_loopa(jl_t* jlc);

// Constants
	//ALL IMAGES: 1024x1024
	#define TEXTURE_WH 1024*1024 
	//1bpp Bitmap = 1048832 bytes (Color Key(256)*RGBA(4), 1024*1024)
	#define IMG_FORMAT_LOW 1048832 
	//2bpp HQ bitmap = 2097664 bytes (Color Key(256*2=512)*RGBA(4), 2*1024*1024)
	#define IMG_FORMAT_MED 2097664
	//3bpp Picture = 3145728 bytes (No color key, RGB(3)*1024*1024)
	#define IMG_FORMAT_PIC 3145728
	//
	#define IMG_SIZE_LOW (1+strlen(JL_IMG_HEADER)+(256*4)+(1024*1024)+1)

	#if JL_PLAT == JL_PLAT_PHONE
	str_t JL_FL_BASE;
	#endif

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

static void _jl_sg_mode_add(jvct_t* _jlc) {
	// Allocate a new mode.
	jl_me_alloc(_jlc->jlc, (void**)&_jlc->sg.mdes,
		(_jlc->jlc->mdec + 1) * sizeof(__sg_mode_t),
		_jlc->jlc->mdec * sizeof(__sg_mode_t));
	// Set the mode.
	_jlc->sg.mdes[_jlc->jlc->mdec].tclp[JL_SG_WM_EXIT] = jl_sg_exit;
	_jlc->sg.mdes[_jlc->jlc->mdec].tclp[JL_SG_WM_UP] = jl_dont;
	_jlc->sg.mdes[_jlc->jlc->mdec].tclp[JL_SG_WM_DN] = jl_dont;
	_jlc->sg.mdes[_jlc->jlc->mdec].tclp[JL_SG_WM_TERM] = jl_dont;
	_jlc->sg.mdes[_jlc->jlc->mdec].tclp[JL_SG_WM_RESZ] = jl_dont;
	// Add to mode count.
	_jlc->jlc->mdec++;
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
void jl_sg_mode_set(jl_t* jlc, u8_t mode, u8_t wm, jl_simple_fnt loop) {
	jvct_t* _jlc = jlc->_jlc;

	jl_gr_draw_msge(jlc, 0, 0, 0, "Switching Mode...");
	while(mode >= _jlc->jlc->mdec) _jl_sg_mode_add(_jlc);
	_jlc->sg.mdes[mode].tclp[wm] = loop;
	// Reset things
	_jlc->ct.heldDown = 0;
}

/**
 * Temporarily change the mode functions without actually changing the mode.
 * @param jlc: The library context.
 * @param wm: the loop to override
 * @param loop: the overriding function 
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

//Get a pixels RGBA values from a surface and xy
uint32_t _jl_sg_gpix(
	/*in */ SDL_Surface* surface, int32_t x, int32_t y)
{
	i32_t bpp = surface->format->BytesPerPixel;
	u8_t *p = (uint8_t *)surface->pixels + (y * surface->pitch) + (x * bpp);
	uint32_t color_orig;
	uint32_t color;
	uint8_t* out_color = (void*)&color;

	if(bpp == 1) {
		color_orig = *p;
	}else if(bpp == 2) {
		color_orig = *(u16_t *)p;
	}else if(bpp == 3) {
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			color_orig = p[0] << 16 | p[1] << 8 | p[2];
		else
			color_orig = p[0] | p[1] << 8 | p[2] << 16;
	}else{ // 4
		color_orig = *(u32_t *)p;
	}
	SDL_GetRGBA(color_orig, surface->format, &(out_color[0]),
		&(out_color[1]), &(out_color[2]), &(out_color[3]));
	return color;
}

void _jl_sg_load_jlpx(jvct_t* _jlc,strt data,void **pixels,int *w,int *h) {
	if(data == NULL) {
		jl_io_print(_jlc->jlc, "NULL DATA!");
		jl_sg_kill(_jlc->jlc);
	}else if(data->data[_jlc->sg.init_image_location] == 0) {
		return;
	}

	jl_io_function(_jlc->jlc, "SG_Jlpx"); // {
	
	//Check If File Is Of Correct Format
	char *testing = malloc(strlen(JL_IMG_HEADER)+1);
	int32_t i, j;

	jl_me_copyto(data->data + _jlc->sg.init_image_location, testing,
		strlen(JL_IMG_HEADER));
	testing[strlen(JL_IMG_HEADER)] = '\0';
	jl_io_print(_jlc->jlc, "header=\"%s\" @%d", testing,
		_jlc->sg.init_image_location);

	if(strcmp(testing, JL_IMG_HEADER) != 0) {
		jl_io_print(_jlc->jlc, "error: bad file type:");
		jl_io_print(_jlc->jlc, ":%s", testing);
		jl_io_print(_jlc->jlc, "!=");
		jl_io_print(_jlc->jlc, ":%s", JL_IMG_HEADER);
		jl_io_print(_jlc->jlc, "couldn't load image #%d",
			_jlc->sg.image_id);
		jl_sg_kill(_jlc->jlc);
	}
	uint8_t tester = data->data[_jlc->sg.init_image_location+strlen(JL_IMG_HEADER)];
	uint32_t FSIZE;
	if(tester == JL_IMG_FORMAT_IMG) { //Normal Quality[Lowy]
		FSIZE = IMG_SIZE_LOW;
		jl_io_print(_jlc->jlc, "normal quality");
	}else if(tester == JL_IMG_FORMAT_HQB) { //High Quality[Norm]
		FSIZE = IMG_FORMAT_MED;
		jl_io_print(_jlc->jlc, "high quality");
	}else if(tester == JL_IMG_FORMAT_PIC) { //Picture[High]
		FSIZE = IMG_FORMAT_PIC;
		jl_io_print(_jlc->jlc, "pic quality");
	}else if(tester == JL_IMG_FORMAT_FLS) {
		SDL_Surface *image;
		SDL_RWops *rw;
		void* img_file = NULL;
		uint32_t color = 0;
		strt pixel_data;

		jl_io_print(_jlc->jlc, "png/gif/jpeg etc.");
		data->curs = _jlc->sg.init_image_location+strlen(JL_IMG_HEADER)+1;
		jl_me_strt_loadto(data, 4, &FSIZE);
		jl_io_print(_jlc->jlc, "File Size = %d", FSIZE);
		jl_me_alloc(_jlc->jlc, &img_file, FSIZE, 0);
		jl_me_strt_loadto(data, FSIZE, img_file);
		rw = SDL_RWFromMem(img_file, FSIZE);
		if ((image = IMG_Load_RW(rw, 1)) == NULL) {
			jl_io_print(_jlc->jlc, "Couldn't load image: %s",
				IMG_GetError());
			jl_sg_kill(_jlc->jlc);
		}
		// Covert SDL_Surface.
		pixel_data = jl_me_strt_make(image->w * image->h * 4);
		for(i = 0; i < image->h; i++) {
			for(j = 0; j < image->w; j++) {
				color = _jl_sg_gpix(image, j, i);
				jl_me_strt_saveto(pixel_data, 4, &color);
			}
		}
		_jlc->sg.init_image_location += FSIZE + 6;
		//Set Return values
		*pixels = jl_me_string_fstrt(_jlc->jlc, pixel_data);
		*w = image->w;
		*h = image->h;
		// Clean-up
		SDL_free(image);
		jl_io_return(_jlc->jlc, "SG_Jlpx");
		return;
	}else{
		jl_io_print(_jlc->jlc, "bad file type(must be 1-4) is: %d", tester);
		jl_sg_kill(_jlc->jlc);
	}
	jgr_img_t *image = NULL;
	uint32_t ki = strlen(JL_IMG_HEADER)+1;

	jl_me_alloc(_jlc->jlc, (void**)&image, sizeof(jgr_img_t), 0);
	for(i = 0; i < 256; i++) {
		image->key[i].r = data->data[_jlc->sg.init_image_location+ki];
		ki++;
		image->key[i].g = data->data[_jlc->sg.init_image_location+ki];
		ki++;
		image->key[i].b = data->data[_jlc->sg.init_image_location+ki];
		ki++;
		image->key[i].a = data->data[_jlc->sg.init_image_location+ki];
		ki++;
	}
	for(i = 0; i < TEXTURE_WH; i++) {
		image->tex_pixels[i] = data->data[_jlc->sg.init_image_location+ki];
		ki++;
	}
	_jlc->sg.init_image_location+=FSIZE+1;
	jl_io_print(_jlc->jlc, "creating texture...");

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
	*pixels = tex_pixels;
	*w = a[0];
	*h = a[1];
	//Cleanup
	free(image);
	jl_io_return(_jlc->jlc, "SG_Jlpx");
}

//loads next image in the currently loaded file.
static inline uint8_t _jl_sg_load_next_img(jvct_t * _jlc) {
	void *fpixels = NULL;
	int fw;
	int fh;
	jl_io_function(_jlc->jlc, "SG_Imgs");
	_jl_sg_load_jlpx(_jlc, _jlc->sg.image_data, &fpixels, &fw, &fh);
	if(fpixels == NULL) {
		jl_io_print(_jlc->jlc, "loaded %d", _jlc->sg.image_id);
		_jlc->jlc->info = _jlc->sg.image_id;
		jl_io_print(_jlc->jlc, "IL");
		jl_io_return(_jlc->jlc, "SG_Imgs");
		return 0;
	}else{
		jl_io_print(_jlc->jlc, "creating image #%d....", _jlc->sg.igid);
		jl_gl_maketexture(_jlc->jlc, _jlc->sg.igid,
			_jlc->sg.image_id, fpixels, fw, fh, 0);
		jl_io_print(_jlc->jlc, "created image #%d:%d!", _jlc->sg.igid,
			_jlc->sg.image_id);
//		#endif
		_jlc->sg.image_id++;
		jl_io_return(_jlc->jlc, "SG_Imgs");
		return 1;
	}
}

void jl_sg_add_some_imgs_(jvct_t * _jlc, u16_t x) {
	m_u16_t i;
	for(i = 0; i < x; i++) {
		if(!_jl_sg_load_next_img(_jlc)) break;
	}
}

//Load the images in the image file
static inline void _jl_sg_init_images(jvct_t * _jlc,strt data,u16_t gi,u16_t x){
	_jlc->sg.init_image_location = 0;
	_jlc->sg.image_id= 0; //Reset Image Id
	_jlc->sg.igid = gi;
	_jlc->sg.image_data = data;

	jl_io_function(_jlc->jlc, "SG_InitImgs");
	jl_io_print(_jlc->jlc, "loading images....");
	jl_io_print(_jlc->jlc, "stringlength = %d", data->size);
//load textures
	if(x) jl_sg_add_some_imgs_(_jlc, x);
	else while(_jl_sg_load_next_img(_jlc));
	jl_io_return(_jlc->jlc, "SG_InitImgs");
}

/**
 * Show Error On Screen if screen is available, otherwise print it out.
 * Quit, And Return -1 to show there was an error.
*/
void jl_sg_kill(jl_t* jlc) {
	//TODO: Make Screen With Window Saying Error Message Followed By Prompt.
	//	Also, don't automatically quit, like it does now!  ERQT can be
	//	inline at that point.
	jvct_t * _jlc = jlc->_jlc;
	
	jl_io_print(jlc, "Quitting On Error....");
	strt error_string = jl_fl_load(jlc, _jlc->fl.paths.errf);
	jl_io_print(jlc, (void*)error_string->data);
//	jl_io_stacktrace(jlc);
	jl_kill(jlc, JL_RTN_FAIL);
	// Program is stopped at this point.
}

/**
 * Go to exit mode or exit if in exit mode.
 */
void jl_sg_exit(jl_t* jlc) {
	if(jlc->loop == JL_SG_WM_EXIT)
		jl_kill(jlc, JL_RTN_SUCCESS);
	else
		jlc->loop = JL_SG_WM_EXIT;
}

// Delete window size data.
void _jl_sg_kill(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;

	jl_gr_pr_old(jlc, _jlc->sg.bg.up);
	jl_gr_pr_old(jlc, _jlc->sg.bg.dn);
}

//void sgrp_

float jl_sg_seconds_past_(jvct_t* _jlc) {
	_jlc->sg.this_tick = SDL_GetTicks();
	// milliseconds / 1000 to get seconds
	return ((float)(_jlc->sg.this_tick - _jlc->sg.prev_tick)) / 1000.f;
}

static void jl_sg_add_image__(jl_t* jlc, str_t pzipfile, u16_t pigid, u8_t x) {
	jl_io_function(jlc, "SG_LImg");
	//Load Graphics
	strt img = jl_fl_media(jlc, "jlex/2/_img", pzipfile,
		jl_gem(), jl_gem_size());

	jl_io_print(jlc, "Loading Images....");
	if(img != NULL)
		_jl_sg_init_images(jlc->_jlc, img, pigid, x);
	else
		jl_io_print(jlc, "Loaded 0 images!");
	jl_io_print(jlc, "Loaded Images...");
	jl_io_return(jlc, "SG_LImg"); // }
}

/**
 * Load all images from a zipfile and give them ID's.
 * info: info is set to number of images loaded.
 * @param jlc: library context
 * @param pzipfile: full file name of a zip file.
 * @param pigid: which image group to load the images into.
*/
void jl_sg_add_image(jl_t* jlc, str_t pzipfile, u16_t pigid) {
	jl_sg_add_image__(jlc, pzipfile, pigid, 0);
}

static void _jl_sg_screen_draw(jl_t* jlc, f32_t ytrans, jl_vo_t* bg, u8_t lp) {
	jvct_t * _jlc = jlc->_jlc;
	jl_vec3_t translate = { _jlc->sg.offsx, _jlc->sg.offsy + ytrans, 0. };
	jl_vec3_t transform = { 1., -1., 1. };

	// Turn off all pre-renderers.
	jl_gl_pr_off(_jlc);
	// Draw the vertex object.
	jl_gr_draw_vo(jlc, bg, &translate);
	// Use the pr for this screen.
	jl_gl_pr_scr_set(_jlc, bg);
	jl_gl_pr_scr(_jlc);
	// Clear the screen.
	jl_gl_clear(jlc, (ytrans > 0.1 ) * 255, (ytrans > 0.1 ) * 255, (ytrans > 0.1 ) * 255, 255);
	// Run the screen's loop
	_jlc->sg.mode.tclp[lp](jlc);
	// If BG is lower screen: Draw Menu Bar & Mouse - on lower screen
	if(bg == _jlc->sg.bg.dn && _jlc->has.graphics) _jl_gr_loopa(jlc);
	// Turn off the pre-renderer.
	jl_gl_pr_off(_jlc);
	// Draw the prendered texture.
	jl_gl_pr_draw(jlc, bg->pr, &translate, &transform);
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

static void jl_sg_init_ds_(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	const float shifty = _jlc->dl.shifty / 2.;
	const float rcrdw = 1. - _jlc->dl.shiftx;
	const float rcrdh = .5 - shifty;
	jl_rect_t rcrd = {
		0.f, 0.f,
		rcrdw, rcrdh * jl_gl_ar(jlc)
	};
	uint8_t rclr_up[4] = { 127,	127,	127,	255 };
	uint8_t rclr_dn[4] = { 0,	127,	0,	255 };
	
	jl_io_print(_jlc->jlc, "2 Screen init");
	// Update the rectangle backgrounds.
	jl_gr_vos_rec(jlc, _jlc->sg.bg.up, rcrd, rclr_up, 0);
	jl_gr_pr_new(jlc, _jlc->sg.bg.up, _jlc->dl.current.w);
	jl_gr_vos_rec(jlc, _jlc->sg.bg.dn, rcrd, rclr_dn, 0);
	jl_gr_pr_new(jlc, _jlc->sg.bg.dn, _jlc->dl.current.w);
	// Set double screen loop.
	_jlc->sg.loop = _jl_sg_loop_ds;
	//
	_jlc->sg.bg.up->pr->ar = _jlc->dl.aspect / 2.;
	_jlc->sg.bg.dn->pr->ar = _jlc->dl.aspect / 2.;
	
	_jlc->sg.screen_height = rcrd.h;
	jl_io_print(_jlc->jlc, "2 Screen init'd");
}

static void jl_sg_init_ss_(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	const float shifty = _jlc->dl.shifty;
	const float rcrdw = 1. - _jlc->dl.shiftx;
	const float rcrdh = 1. - shifty;
	jl_rect_t rcrd = {
		0.f, 0.f,
		rcrdw, rcrdh * jl_gl_ar(jlc)
	};
	uint8_t rclr_bg[4] = { 0,	255,	0,	255 };
	
	jl_io_print(jlc, "{{{ 1 Screen init");
	// Update the rectangle backgrounds.
	jl_gr_vos_rec(jlc, _jlc->sg.bg.dn, rcrd, rclr_bg, 0);
	jl_gr_pr_new(jlc, _jlc->sg.bg.dn, _jlc->dl.current.w);
	// Set single screen loop.
	_jlc->sg.loop = _jl_sg_loop_ss;
	//
	_jlc->sg.bg.dn->pr->ar = _jlc->dl.aspect;
	
	_jlc->sg.screen_height = rcrd.h;
}

void _jl_sg_resz(jl_t* jlc) {
	jvct_t * _jlc = jlc->_jlc;
	const float isDoubleScreen = (double)(jlc->smde);

	// Turn Off Pre-renderer.
	jl_gl_pr_off(_jlc);
	// Check screen count.
	if(jlc->smde)
		jl_sg_init_ds_(jlc);
	else
		jl_sg_init_ss_(jlc);
	// Set screen buffer space
	_jlc->sg.offsx = _jlc->dl.shiftx/2.;
	_jlc->sg.offsy = (((_jlc->dl.shifty / (1. + isDoubleScreen)) *
		(1. + isDoubleScreen))/2.) * jl_gl_ar(jlc);
	// Use the screen's pre-renderer
	jl_gl_pr_scr_set(_jlc, _jlc->sg.bg.dn);
	jl_gl_pr_scr(_jlc);
}

void jl_sg_initb_(jvct_t * _jlc) {
	// No error
	_jlc->jlc->errf = JL_ERR_NERR;
	_jlc->jlc->psec = 0.f;
	// Set Default Window To Terminal
	_jlc->jlc->loop = JL_SG_WM_TERM;
	_jlc->sg.prev_tick = 0;
	_jlc->sg.fps = JL_FPS;
}

void jl_sg_inita_(jvct_t * _jlc) {
	m_u16_t i;

	//Set Up Variables
	_jlc->gl.textures = NULL;
	_jlc->gl.tex.uniforms.textures = NULL;
	_jlc->sg.image_id = 0; //Reset Image Id
	_jlc->sg.igid = 0; //Reset Image Group Id
	// Load Graphics
	_jlc->gl.allocatedg = 0;
	_jlc->gl.allocatedi = 0;
	jl_sg_add_image__(_jlc->jlc,
		(void*)jl_fl_get_resloc(_jlc->jlc, JL_MAIN_DIR, JL_MAIN_MEF),
		0, 1);
	// Create upper and lower screens
	_jlc->sg.bg.up = jl_gl_vo_make(_jlc->jlc, 1);
	_jlc->sg.bg.dn = jl_gl_vo_make(_jlc->jlc, 1);
	jl_gl_pr_off(_jlc);
	// Default to lower screen
	jl_gl_pr_scr_set(_jlc, _jlc->sg.bg.dn);
	jl_gl_pr_scr(_jlc);
	// Resize for 2 screen Default - so they initialize.
	_jlc->jlc->smde = 1;
	jl_sg_init_ds_(_jlc->jlc);
	// Set up modes:
	_jlc->jlc->mode = 0;
	_jlc->jlc->mdec = 0;
	_jlc->sg.mdes = NULL;
	_jl_sg_mode_add(_jlc);
	// Clear User Loops
	for(i = 0; i < JL_SG_WM_MAX; i++) _jlc->sg.mode.tclp[i] = jl_dont;
}

/**
 * @mainpage
 * @section Library Description
 * 
*/