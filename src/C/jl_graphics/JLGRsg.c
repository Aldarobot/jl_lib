/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRsg.c
 *	sg AKA. Simple Graphics does the window handling.
**/

#include "jl_pr.h"
#include "JLGRinternal.h"

// SG Prototypes
void jl_gl_draw_prendered(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gl_pr_scr_set(jl_gr_t* jl_gr, jl_vo_t* vo);

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
	
//Functions:

//Get a pixels RGBA values from a surface and xy
uint32_t _jl_sg_gpix(/*in */ SDL_Surface* surface, int32_t x, int32_t y) {
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

void _jl_sg_load_jlpx(jl_gr_t* jl_gr,data_t* data,void **pixels,int *w,int *h) {
	if(data == NULL) {
		jl_print(jl_gr->jl, "NULL DATA!");
		jl_sg_kill(jl_gr->jl);
	}else if(data->data[jl_gr->sg.init_image_location] == 0) {
		return;
	}

	jl_print_function(jl_gr->jl, "SG_Jlpx"); // {
	
	//Check If File Is Of Correct Format
	char *testing = malloc(strlen(JL_IMG_HEADER)+1);
	int32_t i, j;

	jl_mem_copyto(data->data + jl_gr->sg.init_image_location, testing,
		strlen(JL_IMG_HEADER));
	testing[strlen(JL_IMG_HEADER)] = '\0';
	JL_PRINT_DEBUG(jl_gr->jl, "header=\"%s\" @%d", testing,
		jl_gr->sg.init_image_location);

	if(strcmp(testing, JL_IMG_HEADER) != 0) {
		jl_print(jl_gr->jl, "error: bad file type:");
		jl_print(jl_gr->jl, ":%s", testing);
		jl_print(jl_gr->jl, "!=");
		jl_print(jl_gr->jl, ":%s", JL_IMG_HEADER);
		jl_print(jl_gr->jl, "couldn't load image #%d",
			jl_gr->sg.image_id);
		jl_sg_kill(jl_gr->jl);
	}
	uint8_t tester = data->data[jl_gr->sg.init_image_location+strlen(JL_IMG_HEADER)];
	uint32_t FSIZE;
	if(tester == JL_IMG_FORMAT_FLS) {
		SDL_Surface *image;
		SDL_RWops *rw;
		void* img_file = NULL;
		uint32_t color = 0;
		data_t* pixel_data;

		JL_PRINT_DEBUG(jl_gr->jl, "png/gif/jpeg etc.");
		data->curs = jl_gr->sg.init_image_location+strlen(JL_IMG_HEADER)+1;
		jl_data_loadto(data, 4, &FSIZE);
		JL_PRINT_DEBUG(jl_gr->jl, "File Size = %d", FSIZE);
		img_file = jl_memi(jl_gr->jl, FSIZE);
		jl_data_loadto(data, FSIZE, img_file);
		rw = SDL_RWFromMem(img_file, FSIZE);
		if ((image = IMG_Load_RW(rw, 1)) == NULL) {
			jl_print(jl_gr->jl, "Couldn't load image: %s",
				IMG_GetError());
			jl_sg_kill(jl_gr->jl);
		}
		// Covert SDL_Surface.
		pixel_data = jl_data_make(image->w * image->h * 4);
		for(i = 0; i < image->h; i++) {
			for(j = 0; j < image->w; j++) {
				color = _jl_sg_gpix(image, j, i);
				jl_data_saveto(pixel_data, 4, &color);
			}
		}
		jl_gr->sg.init_image_location += FSIZE + 6;
		//Set Return values
		*pixels = jl_data_tostring(jl_gr->jl, pixel_data);
		*w = image->w;
		*h = image->h;
		// Clean-up
		SDL_free(image);
	}else{
		jl_print(jl_gr->jl, "bad file type(must be 4) is: %d", tester);
		jl_sg_kill(jl_gr->jl);
	}
	jl_print_return(jl_gr->jl, "SG_Jlpx");
}

//loads next image in the currently loaded file.
static inline uint8_t _jl_sg_load_next_img(jl_gr_t* jl_gr) {
	void *fpixels = NULL;
	int fw;
	int fh;
	jl_print_function(jl_gr->jl, "SG_Imgs");
	_jl_sg_load_jlpx(jl_gr, jl_gr->sg.image_data, &fpixels, &fw, &fh);
	if(fpixels == NULL) {
		JL_PRINT_DEBUG(jl_gr->jl, "loaded %d", jl_gr->sg.image_id);
		jl_gr->jl->info = jl_gr->sg.image_id;
		jl_print(jl_gr->jl, "IL");
		jl_print_return(jl_gr->jl, "SG_Imgs");
		return 0;
	}else{
		JL_PRINT_DEBUG(jl_gr->jl, "creating image #%d....", jl_gr->sg.igid);
		jl_gl_maketexture(jl_gr, jl_gr->sg.igid,
			jl_gr->sg.image_id, fpixels, fw, fh, 0);
		JL_PRINT_DEBUG(jl_gr->jl, "created image #%d:%d!", jl_gr->sg.igid,
			jl_gr->sg.image_id);
//		#endif
		jl_gr->sg.image_id++;
		jl_print_return(jl_gr->jl, "SG_Imgs");
		return 1;
	}
}

void jl_sg_add_some_imgs_(jl_gr_t* jl_gr, u16_t x) {
	m_u16_t i;
	for(i = 0; i < x; i++) {
		if(!_jl_sg_load_next_img(jl_gr)) break;
	}
}

//Load the images in the image file
static inline void _jl_sg_init_images(jl_gr_t* jl_gr,data_t* data,u16_t gi,u16_t x){
	jl_gr->sg.init_image_location = 0;
	jl_gr->sg.image_id= 0; //Reset Image Id
	jl_gr->sg.igid = gi;
	jl_gr->sg.image_data = data;

	jl_print_function(jl_gr->jl, "SG_InitImgs");
	JL_PRINT_DEBUG(jl_gr->jl, "loading images....");
	JL_PRINT_DEBUG(jl_gr->jl, "stringlength = %d", data->size);
//load textures
	if(x) jl_sg_add_some_imgs_(jl_gr, x);
	else while(_jl_sg_load_next_img(jl_gr));
	jl_print_return(jl_gr->jl, "SG_InitImgs");
}

/**
 * Show Error On Screen if screen is available, otherwise print it out.
 * Quit, And Return -1 to show there was an error.
*/
void jl_sg_kill(jl_t* jl) {
	//TODO: Make Screen With Window Saying Error Message Followed By Prompt.
	//	Also, don't automatically quit, like it does now!  ERQT can be
	//	inline at that point.
	jl_print(jl, "Quitting On Error....");
	jl_print_stacktrace(jl);
	jl->mdec = 0;
	exit(-1);
	// Program is stopped at this point.
}

/**
 * Go to exit mode or quit if in exit mode.
 * @param jl: The library context.
 */
void jl_sg_exit(jl_t* jl) {
	if(jl->loop == JL_MODE_EXIT)
		jl->mdec = 0;
	else
		jl->loop = JL_MODE_EXIT;
}

static void jl_sg_add_image__(jl_t* jl, str_t pzipfile, u16_t pigid, u8_t x) {
	jl_print_function(jl, "SG_LImg");
	//Load Graphics
	data_t* img = jl_file_media(jl, "jlex/2/_img", pzipfile, jl_gem(),
		jl_gem_size());

	JL_PRINT_DEBUG(jl, "Loading Images....");
	if(img != NULL)
		_jl_sg_init_images(jl->jl_gr, img, pigid, x);
	else
		JL_PRINT_DEBUG(jl, "Loaded 0 images!");
	JL_PRINT_DEBUG(jl, "Loaded Images...");
	jl_print_return(jl, "SG_LImg"); // }
}

/**
 * Load all images from a zipfile and give them ID's.
 * info: info is set to number of images loaded.
 * @param jl: library context
 * @param pzipfile: full file name of a zip file.
 * @param pigid: which image group to load the images into.
*/
void jl_sg_add_image(jl_t* jl, str_t pzipfile, u16_t pigid) {
	jl_sg_add_image__(jl, pzipfile, pigid, 0);
}

static void _jl_sg_screen_draw(jl_t* jl, f32_t ytrans, jl_vo_t* bg, jl_fnct f){
	jl_gr_t* jl_gr = jl->jl_gr;
	jl_vec3_t translate = { jl_gr->sg.offsx, jl_gr->sg.offsy + ytrans, 0. };
	jl_vec3_t transform = { 1., -1., 1. };

	// Turn off all pre-renderers.
	jl_gl_pr_off(jl_gr);
	// Use the pr for this screen.
	jl_gl_pr_scr_set(jl_gr, bg);
	jl_gl_pr_scr(jl_gr);
	// Clear the screen.
	jl_gl_clear(jl_gr, (ytrans > 0.1 ) * 255, (ytrans > 0.1 ) * 255,
		(ytrans > 0.1 ) * 255, 255);
	// Run the screen's redraw function
	f(jl);
	// If BG is lower screen: Draw Menu Bar & Mouse - on lower screen
	if(bg == jl_gr->sg.bg.dn) _jl_gr_loopa(jl_gr);
	// Turn off the pre-renderer.
	jl_gl_pr_off(jl_gr);
	// Draw the prendered texture.
	jl_gl_pr_draw(jl_gr, bg->pr, &translate, &transform);
}

// Double screen loop
static void _jl_sg_loop_ds(jl_gr_t* jl_gr) {
	// Draw upper screen - alternate screen
	_jl_sg_screen_draw(jl_gr->jl, 0.f, jl_gr->sg.bg.up,
		(jl_gr->sg.cscreen == JL_SCR_UP) ? jl_gr->draw.redraw.lower :
			 jl_gr->draw.redraw.upper);
	// Draw lower screen - default screen
	_jl_sg_screen_draw(jl_gr->jl, jl_gr->sg.screen_height, jl_gr->sg.bg.dn,
		(jl_gr->sg.cscreen == JL_SCR_UP) ? jl_gr->draw.redraw.upper :
			 jl_gr->draw.redraw.lower);
}

// Single screen loop
static void _jl_sg_loop_ss(jl_gr_t* jl_gr) {
	// Draw lower screen - default screen
	_jl_sg_screen_draw(jl_gr->jl, 0.f, jl_gr->sg.bg.dn,
		jl_gr->draw.redraw.single);
}

void _jl_sg_loop(jl_gr_t* jl_gr) {
	jl_print_function(jl_gr->jl, "SG_LP");
	jl_gr_fnct loop = jl_gr->sg.loop;
	// Run the current loop.
	loop(jl_gr);
	jl_print_return(jl_gr->jl, "SG_LP");
}

static void jl_sg_init_ds_(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;
	const float shifty = jl_gr->dl.shifty / 2.;
	const float rcrdw = 1. - jl_gr->dl.shiftx;
	const float rcrdh = .5 - shifty;
	jl_rect_t rcrd = {
		0.f, 0.f,
		rcrdw, rcrdh * jl_gl_ar(jl_gr)
	};
	uint8_t rclr_up[4] = { 127,	127,	127,	255 };
	uint8_t rclr_dn[4] = { 0,	127,	0,	255 };
	
	// Update the rectangle backgrounds.
	jl_gr_vos_rec(jl_gr, jl_gr->sg.bg.up, rcrd, rclr_up, 0);
	jl_gr_pr_new(jl_gr, jl_gr->sg.bg.up, jl_gr->dl.current.w);
	jl_gr_vos_rec(jl_gr, jl_gr->sg.bg.dn, rcrd, rclr_dn, 0);
	jl_gr_pr_new(jl_gr, jl_gr->sg.bg.dn, jl_gr->dl.current.w);
	// Set double screen loop.
	jl_gr->sg.loop = _jl_sg_loop_ds;
	if(jl_gr->sg.cscreen == JL_SCR_SS) jl_gr->sg.cscreen = JL_SCR_DN;
	//
	jl_gr->sg.bg.up->pr->ar = jl_gr->dl.aspect / 2.;
	jl_gr->sg.bg.dn->pr->ar = jl_gr->dl.aspect / 2.;
	
	jl_gr->sg.screen_height = rcrd.h;
}

static void jl_sg_init_ss_(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;
	const float shifty = jl_gr->dl.shifty;
	const float rcrdw = 1. - jl_gr->dl.shiftx;
	const float rcrdh = 1. - shifty;
	jl_rect_t rcrd = {
		0.f, 0.f,
		rcrdw, rcrdh * jl_gl_ar(jl_gr)
	};
	uint8_t rclr_bg[4] = { 0,	255,	0,	255 };
	
	// Update the rectangle backgrounds.
	jl_gr_vos_rec(jl_gr, jl_gr->sg.bg.dn, rcrd, rclr_bg, 0);
	jl_gr_pr_new(jl_gr, jl_gr->sg.bg.dn, jl_gr->dl.current.w);
	// Set single screen loop.
	jl_gr->sg.loop = _jl_sg_loop_ss;
	jl_gr->sg.cscreen = JL_SCR_SS;
	//
	jl_gr->sg.bg.dn->pr->ar = jl_gr->dl.aspect;
	
	jl_gr->sg.screen_height = rcrd.h;
}

void jl_sg_resz__(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;
	const float isDoubleScreen = (double)(jl->smde);

	// Turn Off Pre-renderer.
	jl_gl_pr_off(jl_gr);
	// Check screen count.
	if(jl->smde)
		jl_sg_init_ds_(jl);
	else
		jl_sg_init_ss_(jl);
	// Set screen buffer space
	jl_gr->sg.offsx = jl_gr->dl.shiftx/2.;
	jl_gr->sg.offsy = (((jl_gr->dl.shifty / (1. + isDoubleScreen)) *
		(1. + isDoubleScreen))/2.) * jl_gl_ar(jl_gr);
	// Use the screen's pre-renderer
	jl_gl_pr_scr_set(jl_gr, jl_gr->sg.bg.dn);
	jl_gl_pr_scr(jl_gr);
}

void jl_sg_inita__(jl_gr_t* jl_gr) {
	jlgr_redraw_t redraw = { jl_dont, jl_dont, jl_dont, jl_dont };

	//Set Up Variables
	jl_gr->gl.textures = NULL;
	jl_gr->gl.tex.uniforms.textures = NULL;
	jl_gr->sg.image_id = 0; //Reset Image Id
	jl_gr->sg.igid = 0; //Reset Image Group Id
	// Initialize redraw routines to do nothing.
	jl_mem_copyto(&redraw, &(jl_gr->draw.redraw), sizeof(jlgr_redraw_t));
	// Load Graphics
	jl_gr->gl.allocatedg = 0;
	jl_gr->gl.allocatedi = 0;
	jl_sg_add_image__(jl_gr->jl,
		(void*)jl_file_get_resloc(jl_gr->jl, JL_MAIN_DIR, JL_MAIN_MEF),
		0, 1);
	// Create upper and lower screens
	jl_gr->sg.bg.up = jl_gl_vo_make(jl_gr, 1);
	jl_gr->sg.bg.dn = jl_gl_vo_make(jl_gr, 1);
	jl_gl_pr_off(jl_gr);
	// Default to lower screen
	jl_gl_pr_scr_set(jl_gr, jl_gr->sg.bg.dn);
	jl_gl_pr_scr(jl_gr);
	// Resize for 2 screen Default - so they initialize.
	jl_gr->jl->smde = 1;
	jl_sg_init_ds_(jl_gr->jl);
}
