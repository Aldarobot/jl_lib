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
// "me.c"
	void jl_me_clr(void *pmem, uint64_t size);
	void * jl_me_copy(jl_t* pusr, const void *src, size_t size);
	void jl_me_alloc(jl_t* pusr, void **a, uint32_t size, uint32_t oldsize);
	#define List(x) jl_me_list_allc(sizeof(void*)*x)
	void jl_me_strt_clear(strt pa);
	strt jl_me_strt_make(u32t size, u08t type);
	void jl_me_strt_free(strt pstr);
	strt jl_me_strt_c8ts(const char *string);
	#define Strt(x) jl_me_strt_c8ts((const void*)x)
	strt jl_me_strt_mkfrom_data(jl_t* pusr, uint32_t size, void *data);
	void jl_me_strt_strt(jl_t*pusr,strt a,strt b,uint64_t p,uint64_t bytes);
	void jl_me_strt_merg(jl_t *pusr, strt a, strt b);
	void jl_me_strt_trunc(jl_t *pusr, strt a, uint32_t size);
	strt jl_me_strt_fnum(s32t a);
	char* jl_me_string_fnum(jl_t* pusr, int32_t a);
	char* jl_me_string_fstrt(jl_t* pusr, strt a);
	u08t jl_me_strt_byte(strt pstr);
	void jl_me_strt_add_byte(strt pstr, u08t pvalue);
	void jl_me_strt_delete_byte(jl_t *pusr, strt pstr);
	void jl_me_strt_insert_byte(jl_t *pusr, strt pstr, uint8_t pvalue);
	u32t jl_me_random_int(u32t a);
	u08t jl_me_test_next(strt script, strt particle);
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
	void jl_sg_smode_fncs(jl_t* pusr, uint8_t mode, jl_simple_fnt exit,
		jl_simple_fnt wups, jl_simple_fnt wdns, jl_simple_fnt term);
	void jl_sg_setlm(jl_t* pusr, uint8_t mode, jl_sg_wm_t loop);
	void jl_sg_kill(jl_t* pusr, char * msg);
	void jl_sg_exit(jl_t* pusr);
	void jl_sg_add_image(jl_t* pusr, char *pzipfile, uint16_t pigid);
// "dl.c"
	void jl_dl_progname(jl_t* pusr, strt name);
// "gr.c"
	void jl_gr_set_clippane(jl_t* pusr, float xmin, float xmax,
		float ymin, float ymax);
	void jl_gr_default_clippane(jl_t* pusr);
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
		float slidex, jl_simple_fnt prun);
	void jl_gr_draw_glow_button(jl_t* pusr, jl_sprite_t * psprite,
		char *txt, jl_simple_fnt prun);
	uint8_t jl_gr_draw_textbox(jl_t*pusr, float x, float y, float w,
		float h, strt *string);
	void jl_gr_togglemenubar(jl_t* pusr);
	void jl_gr_addicon(jl_t* pusr, uint16_t grp, uint8_t iid,
		uint8_t chr, jl_simple_fnt fno, jl_simple_fnt fnc);
	void jl_gr_addicon_flip(jl_t* pusr);
	void jl_gr_addicon_slow(jl_t* pusr);
	void jl_gr_addicon_name(jl_t* pusr);
// "ct.c"
// "io.c"
	void jl_io_offset(jl_t* pusr, char * this, int16_t tag);
	void jl_io_close_block(jl_t* pusr);
	void jl_io_tag_set(jl_t* pusr,
		int16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn);
	void jl_io_printc(jl_t* pusr, const char * print);
	void jl_io_printt(jl_t *pusr, uint8_t a, const char *print);
	void jl_io_printi(jl_t *pusr, int print);
	void jl_io_printd(jl_t *pusr, double print);
// "fl.c"
	void jl_fl_save(jl_t* pusr, void *file, const char *name,
		uint32_t bytes);
	strt jl_fl_load(jl_t* pusr, char *file_name);
	char jl_fl_pk_save(jl_t* pusr, char *packageFileName,
		char *fileName,	void *data, uint64_t dataSize);
	uint8_t *jl_fl_pk_load(jl_t* pusr, const char *packageFileName,
		const char *filename);
	uint8_t *jl_fl_media(jl_t* pusr, char *Fname, char *pzipfile,
		void *pdata, uint64_t psize);
	void jl_fl_mkdir(jl_t* pusr, strt pfilebase);
	char * jl_fl_get_resloc(jl_t* pusr, char* pprg_name, char* pfilename);
	void jl_fl_user_select_init(jl_t* pusr, char *program_name, void *newfiledata,
		uint64_t newfilesize);
	void jl_fl_user_select_loop(jl_t* pusr);
	char *jl_fl_user_select_get(jl_t* pusr);
// "cm.c"
// "vi.c"
// "au.c"
	void jl_au_mus_play(jl_t* pusr, uint32_t p_IDinStack,
		uint8_t p_secondsOfFadeOut, uint8_t p_secondsOfFadeIn);
	void jl_au_mus_halt(u08t p_secondsOfFadeOut);
	uint8_t jl_au_mus_playing(void);
	void jl_au_panning(uint8_t left, uint8_t toCenter);
	void jl_au_panning_default(void);
	void jl_au_add_audio(jl_t* pusr, char *pzipfile, uint16_t pigid);
/*
 *	This a Jeron Lau project. JL_lib (c) 2014 
*/
