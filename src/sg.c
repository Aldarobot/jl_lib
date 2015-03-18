/*
 * (c) Jeron A. Lau
 * jlvm_blib_sgrp is a library for handling windows.
*/

#include "header/jl_pr.h"

//Prototypes
	//LIB INITIALIZATION fn(Context)
	void _jal5_comm_init(jvct_t* pjlc);
	void _jal5_audi_init(jvct_t* pjlc);
	void _jal5_file_init(jvct_t* pjlc);
	void _jal5_jl_gr_init(jvct_t* pjlc);
	void _jl_ct_init(jvct_t* pjlc);
	void _jal5_sgrp_init(jvct_t* pjlc);
	void _jal5_eogl_init(jvct_t* pjlc);
	void _jl_io_init(jvct_t* pjlc);
	void _jal5_lsdl_init(jvct_t* pjlc);
	jvct_t* _jl_me_init(void);

	//LIB LOOPS: parameter is context
	void _jal5_jl_gr_loop(jl_t* pusr);
	void _jl_ct_loop(jvct_t* pjlc);
	void _jal5_sgrp_loop(jl_t* pusr);
	void _jal5_lsdl_loop(void);
	void _jal5_audi_loop(jvct_t* pjlc);

	//LIB KILLS
	void _jal5_lsdl_kill(jvct_t *jcpt);
	void _jl_me_kill(jvct_t* jprg);

// Constants

	#define TEXTURE_WH 1024*1024
	//screen frames per second
	#define JAL5_SGRP_MAIN_SFPS 30
	//Allowed Processing Time
	#define JAL5_SGRP_MAIN_SAPT 1000/JAL5_SGRP_MAIN_SFPS 

	#define JAL5_SGRP_LSDL_RMSE SDL_BUTTON_RIGHT
	#define JAL5_SGRP_LSDL_LMSE SDL_BUTTON_LEFT
	#define JAL5_SGRP_LSDL_MMSE SDL_BUTTON_MIDDLE

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
}jgr_img_col_t;

//ALL IMAGES: 1024x1024
	//3bpp Picture =	3145728 bytes (No color key, RGB(3)*1024*1024)
	//2bpp HQ bitmap =	2097664 bytes (Color Key(256*2=512)*RGBA(4), 2*1024*1024)
	//1bpp Bitmap =	 	1048832 bytes (Color Key(256)*RGBA(4), 1024*1024)

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

uint8_t jal5_sgrp_lsdl_gmse(uint8_t a) {
	return SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(a);
}

void jl_sg_mode_init(jl_t* pusr, uint8_t mdec) {
	jvct_t* pjlc = pusr->pjlc;
	pjlc->sg.mdes = malloc(mdec * sizeof(__sg_mode_t));
	pjlc->sg.usrd->mdec = mdec;
}

void jl_sg_smode_fncs(
	jl_t* pusr,
	fnct(void, exit, jl_t* pusr),
	fnct(void, wups, jl_t* pusr),
	fnct(void, wdns, jl_t* pusr),
	fnct(void, term, jl_t* pusr))
{
	jvct_t* pjlc = pusr->pjlc;
	pjlc->sg.mdes[pjlc->sg.usrd->mode].tclp[JL_SG_WM_EXIT] = exit;
	pjlc->sg.mdes[pjlc->sg.usrd->mode].tclp[JL_SG_WM_UP] = wups;
	pjlc->sg.mdes[pjlc->sg.usrd->mode].tclp[JL_SG_WM_DN] = wdns;
	pjlc->sg.mdes[pjlc->sg.usrd->mode].tclp[JL_SG_WM_TERM] = term;
}

void jl_sg_set_window(jl_t* pusr, uint8_t window) {
	pusr->loop = window;
}

#define HEADER "JLVM0:JYMJ\0"
void _jlvm_load_jlpx(jvct_t* pjlc,uint8_t *data,void **pixels,int *w,int *h) {
	if(data == NULL) {
		jlvm_dies(pjlc, Strt("NULL DATA!"));
	}else if(data[pjlc->sg.init_image_location] == 0) {
		return;
	}
	jgr_img_t *image = NULL;
	image = _jl_me_hydd_allc(pjlc, image, sizeof(jgr_img_t));
	char *testing = malloc(strlen(HEADER)+1);
	uint32_t i;

	for(i = 0; i < strlen(HEADER); i++) {
		testing[i] = data[pjlc->sg.init_image_location+i];
	}
	testing[strlen(HEADER)] = '\0';        
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("header:%s\n", testing);
	#endif
	if(strcmp(testing, HEADER) != 0) {
		jlvm_dies(pjlc, jl_me_strt_merg(
			jl_me_strt_merg(
				Strt("error: bad file type:\n"),
				Strt(testing), STRT_TEMP
			),
			jl_me_strt_merg(
				Strt("\n!=\n"),
				Strt(HEADER), STRT_TEMP),
			STRT_TEMP)
		);
	}
	uint8_t tester = data[pjlc->sg.init_image_location+strlen(HEADER)];
	int FSIZE;
	if(tester == 1) { //Normal Quality[Lowy]
		FSIZE = 1048832;
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("normal quality\n");
		#endif
	}else if(tester == 2) { //High Quality[Norm]
		FSIZE = 2097664;
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("high quality\n");
		#endif
	}else if(tester == 3) { //Picture[High]
		FSIZE = 3145728;
		#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
		printf("pic quality\n");
		#endif
	}else{
		printf("bad file type(must be 1,2 or 3) is: %d\n", tester);
		jlvm_dies(pjlc, Strt("bad file type(must be 1,2 or 3)"));
	}
	uint32_t ki;
	ki = strlen(HEADER)+1;
	for(i = 0; i < 256; i++) {
		image->key[i].r = data[pjlc->sg.init_image_location+ki];
		ki++;
		image->key[i].g = data[pjlc->sg.init_image_location+ki];
		ki++;
		image->key[i].b = data[pjlc->sg.init_image_location+ki];
		ki++;
		image->key[i].a = data[pjlc->sg.init_image_location+ki];
		ki++;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_INTENSE
	printf("[JLVM/KEYYYY] %d,%d,%d,%d\n", image->key[0].r, image->key[0].g, image->key[0].b, image->key[0].a);
	#endif
	for(i = 0; i < TEXTURE_WH; i++) {
		image->tex_pixels[i] = data[pjlc->sg.init_image_location+ki];
		ki++;
	}
	pjlc->sg.init_image_location+=FSIZE;
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/LIMG/JLPX] creating texture...\n");
	#endif

	uint8_t *tex_pixels = NULL;
	//R(1)+G(1)+B(1)+A(1) = 4
	tex_pixels =_jl_me_hydd_allc(pjlc, tex_pixels, TEXTURE_WH*4);
	for(i = 0; i < TEXTURE_WH; i++) {
		tex_pixels[(i*4)+0] = image->key[image->tex_pixels[i]].r;
		tex_pixels[(i*4)+1] = image->key[image->tex_pixels[i]].g;
		tex_pixels[(i*4)+2] = image->key[image->tex_pixels[i]].b;
		tex_pixels[(i*4)+3] = image->key[image->tex_pixels[i]].a;
	}
	int *a = NULL;
	a = _jl_me_hydd_allc(pjlc, a, 2*sizeof(int));
	a[0] = 1024;
	a[1] = 1024;
	//Set Return values
	pixels[0] = tex_pixels;
	w[0] = a[0];
	h[0] = a[1];
	//Cleanup
	free(image);
}

//loads next image in the file contents pointed to by "data"
static inline u08t jgr_load_jlpx(jvct_t * pjlc, uint8_t *data, uint32_t id) {
//	jlvm_dies(data);
	void *fpixels = NULL;
	int fw;
	int fh;
	_jlvm_load_jlpx(pjlc, data, &fpixels, &fw, &fh);
	if(fpixels == NULL) {
		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		printf("[JLVM/LIMG/JLPX] loaded %d images!\n",
			pjlc->sg.image_id);
		#endif
		pjlc->sg.usrd->info = pjlc->sg.image_id;
		pjlc->sg.image_id=0; //Reset Image Id
		pjlc->sg.igid++; //Next Image Group Id
		return 0;
	}else{
		eogl_make_txtr(pjlc, pjlc->sg.igid, pjlc->sg.image_id,
			fpixels, 1024, 1024);
		#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
		printf("[JLVM/LIMG/JLPX] created image #%d:%d!\n",
			pjlc->sg.igid, pjlc->sg.image_id);
		#endif
		pjlc->sg.image_id++;
		return 1;
	}
}

//Load the images in the image file
static inline void jlvm_ini_images(jvct_t * pjlc, uint8_t *data) {
//	jl_io_offset(pjlc->sg.usrd, "INIM");
	#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
	printf("[JLVM/LIM] loading images...\n");
	printf("lne %d\n", (int)strlen((void *)data));
	#endif
//load textures
	while(1) {
		if(jgr_load_jlpx(pjlc, data, IMGID_TEXT_IMAGE))
			continue;
		else
			break;
	}
//Move To GRPH
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("[JLVM/LIM] loading taskbar...\n");
	#endif

/*	taskbar[0] = FLIP_IMAGE_ID;
	taskbar[1] = GOOD_IMAGE_ID;
	taskbar[2] = UNKNOWN_ID;
	taskbar[3] = UNKNOWN_ID;
	taskbar[4] = UNKNOWN_ID;
//	jgr_load_image(IMGID_TASK_BUTTON, taskbar_items, sizeof(taskbar_items));
	jgr_grp_t *flipg = jgr_make_graphic(IMGID_TASK_BUTTON,255,0);

	flip = jgr_make_sprite(235,0,20,20,flipg);
	mouse = jgr_make_sprite(0,0,10,10,textg);*/
}

static uint32_t jlvm_quit(jvct_t* pjlc, int rc) {
	#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
	_jl_fl_errf(pjlc, "Quitting...."); //Exited properly
	#endif
	_jal5_lsdl_kill(pjlc);
	_jl_me_kill(pjlc);
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	_jl_fl_errf(pjlc, "No Error! YAY!"); //Exited properly
	#endif
	exit(rc);
	_jl_fl_errf(pjlc, "What The Hell?  This is an impossible error!"); //??
	return 105;
}

//Quit, And Return -1 to show there was an error, also save message in errf.text
static void jlvm_erqt(jvct_t* pjlc, char *msg) {
	jl_io_offset(pjlc->sg.usrd, "ERQT");
	jl_io_print_lowc(pjlc->sg.usrd, msg);
	_jl_fl_errf(pjlc, msg);
	jlvm_quit(pjlc, -1);
}

//Show Error On Screen
void jlvm_dies(jvct_t* pjlc, strt msg) {
	//TODO: Make Screen With Window Saying Error Message Followed By Prompt.
	//	Also, don't automatically quit, like it does now!  ERQT can be
	//	inline at that point.
	jlvm_erqt(pjlc, (void*) msg->data);
}

//If, you can't use jlvm_dies(), because the screen hasn't been created, use
//This function.  Otherwise, DON'T! It makes it hard to debug.  The error message
//Will Be saved to a file, like in jlvm_dies().
void jlvm_kill(jvct_t* pjlc, char *msg) {
	jlvm_erqt(pjlc, msg);
}

//quit the program
void jl_sg_kill(jl_t* pusr) {
	if(pusr->loop == JL_SG_WM_EXIT)
		jlvm_quit(pusr->pjlc, 0);
	else
		pusr->loop = JL_SG_WM_EXIT;
}

//Do Nothing
void dont(jl_t* pusr) { }

//void sgrp_

static void _sg_time_reset(jvct_t* pjlc) {
	pjlc->sg.prev_tick = pjlc->sg.this_tick;
}

//return how many frames passed since last call
static inline float _jal5_sgrp_istm(jvct_t* pjlc) {
	pjlc->sg.processingTimeMillis=
		(pjlc->sg.this_tick=SDL_GetTicks())-pjlc->sg.prev_tick;
	if(pjlc->sg.processingTimeMillis <= JAL5_SGRP_MAIN_SAPT) {
		SDL_Delay(JAL5_SGRP_MAIN_SAPT-pjlc->sg.processingTimeMillis);
//		printf("[TIMING] fast: 1\n");
		_sg_time_reset(pjlc);
		return 1.f;
	}else{
		float frames =
			(float)pjlc->sg.processingTimeMillis/
			(float)JAL5_SGRP_MAIN_SAPT;
//		printf("[TIMING] slow: %d, %d, %f\n", processingTimeMillis,
//			JAL5_SGRP_MAIN_SAPT, frames);
		_sg_time_reset(pjlc);
		return frames;
	}
}

/*
 * Add images From program "pprg" in file "pfile"
*/
void jl_sg_add_image(jl_t* pusr, strt pprg, strt pfile) {
	//Load Graphics
	uint8_t *img;
	img = jl_fl_pk_load(
		pusr,
		(void*)(jl_fl_get_resloc(pusr, pprg, pfile)->data),
		"jlex/2/_img");
	#if JLVM_DEBUG >= JLVM_DEBUG_PROGRESS
	jl_io_print_lowc(pusr, "Loading Images...\n");
	#endif
	if(img != NULL) {
		jlvm_ini_images(pusr->pjlc, img);
	}
}

void _jal5_sgrp_init(jvct_t * pjlc) {
	jl_io_offset(pjlc->sg.usrd, "SGRP");
	jl_io_offset(pjlc->sg.usrd, "INIT");
	//Set Up Variables
	pjlc->gl.textures = NULL;
	pjlc->gl.uniforms.textures = NULL;
	pjlc->sg.usrd->errf = JL_ERRF_NERR; //no error
	pjlc->sg.usrd->psec = 0.f;
	pjlc->sg.usrd->mode = 0;
	pjlc->sg.usrd->mdec = 0;
	pjlc->sg.usrd->loop = JL_SG_WM_TERM; //Set Default Window To Terminal
	pjlc->sg.image_id= 0; //Reset Image Id
	pjlc->sg.igid = 0; //Reset Image Group Id
	SDL_GetMouseState(&pjlc->sg.xmse, &pjlc->sg.ymse);
	//Load Graphics
	jl_sg_add_image(pjlc->sg.usrd, Strt("JLVM"), Strt("jlvm.zip"));
}

//Run Loop X
void _jal5_sgrp_loop(jl_t* pusr) {
	jvct_t *pjlc = pusr->pjlc;
	_jl_ct_loop(pjlc);//Update events
	pusr->psec = _jal5_sgrp_istm(pjlc);//Check time
	//Run mode specific loop
	pjlc->sg.mdes[pusr->mode].tclp[pusr->loop](pusr);
	_jal5_jl_gr_loop(pusr);
	_jal5_lsdl_loop();
	_jal5_audi_loop(pjlc);
//	jgr_upd(255, 0, 255); //Update Screen
}

/*
	x 0-255
	y 0-191(upper screen), 192-383(lower screen)
*/

//NON_USER FUNCTION

static inline void jlvm_ini_finish(void) {
	jl_gr_draw_msge("LOADING JLLIB....");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "started up display.\n");
	#endif
}

//The Libraries Needed At Very Beginning: The Base Of It All
static inline jvct_t* _jlvm_init_blib(void) {
	//MEMORY
	jvct_t* jcpt = _jl_me_init();
	//OTHER
	_jl_io_init(jcpt);
	return jcpt;
}

static inline void _jlvm_init_libs(jvct_t *jcpt) {
	_jal5_lsdl_init(jcpt); //create window, load sdl
	_jl_ct_init(jcpt); //Prepare input structures
	_jal5_file_init(jcpt); //Load needed packages
	_jal5_audi_init(jcpt); //Load audiostuffs from packages
	_jal5_sgrp_init(jcpt); //Load graphics from packages
	_jal5_eogl_init(jcpt);
	_jal5_jl_gr_init(jcpt); //
}

static inline void jlvm_ini(jvct_t *jcpt) {
	#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
	jl_io_print_lowc(jcpt->sg.usrd, "Initializing...\n");
	#endif
	_jlvm_init_libs(jcpt);
	jlvm_ini_finish();
	hack_user_init(jcpt->sg.usrd);
	jl_io_offset(jcpt->sg.usrd, "JLVM");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	jl_io_print_lowc(jcpt->sg.usrd, "Init5...\n");
	#endif
//	jlvm_ini_memory_allocate();
	#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
	jl_io_print_lowc(jcpt->sg.usrd, "Initialized!\n");
	#endif
}

int32_t main(int argc, char *argv[]) {
	jvct_t* jcpt = _jlvm_init_blib(); //Set Up Memory And Logging
	#if JLVM_DEBUG >= JLVM_DEBUG_MINIMAL
	jl_io_print_lowc(jcpt->sg.usrd, "STARTING JLVM V-3.2.0-e0....\n");
	#endif
	jlvm_ini(jcpt);//initialize
	_jl_fl_errf(jcpt, "going into loop....");
	if(jcpt->sg.usrd->mdec) {
		while(1) _jal5_sgrp_loop(jcpt->sg.usrd); //main loop
	}
	jl_sg_kill(jcpt->sg.usrd);//kill the program
	return 126;
}
