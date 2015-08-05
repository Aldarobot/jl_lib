/*
 * JL_lib(c) Jeron A. Lau
 * The header to be included within your programs you make with JL_lib.
*/

#include <stdint.h>
#include <stdlib.h>
#include "jl_me.h" // Simple CPU info
#include "jl_en.h" // Enumerations
#include "jl_ty.h" // Variable Types
#include "jl_ct.h" // Input Types
#include "clump.h" // LibClump

	void jl_dont(jl_t* jlc);
// "me.c"
	uint32_t jl_me_tbiu(void);
	void jl_me_clr(void *pmem, uint64_t size);
	void jl_me_copyto(const void *src, void* dest, size_t size);
	void * jl_me_copy(jl_t* jlc, const void *src, size_t size);
	void jl_me_alloc(jl_t* jlc, void **a, uint32_t size, uint32_t oldsize);
	#define List(x) jl_me_list_allc(sizeof(void*)*x)
	void jl_me_strt_clear(strt pa);
	strt jl_me_strt_make(u32_t size);
	void jl_me_strt_free(strt pstr);
	strt jl_me_strt_c8ts(const char *string);
	#define Strt(x) jl_me_strt_c8ts((const void*)x)
	strt jl_me_strt_mkfrom_data(jl_t* jlc, uint32_t size, void *data);
	void jl_me_strt_strt(jl_t*jlc, strt a, strt b, uint64_t p);
	void jl_me_strt_merg(jl_t *jlc, strt a, strt b);
	void jl_me_strt_trunc(jl_t *jlc, strt a, uint32_t size);
	strt jl_me_strt_fnum(i32_t a);
	char* jl_me_string_fnum(jl_t* jlc, int32_t a);
	const char* jl_me_string_fnum_tmp(jl_t* jlc, int32_t a);
	char* jl_me_string_fstrt(jl_t* jlc, strt a);
	uint8_t jl_me_string_print(jl_t* jlc, char *string, const char* format,
		const char *var, u64_t n);
	u8_t jl_me_strt_byte(strt pstr);
	void jl_me_strt_add_byte(strt pstr, u8_t pvalue);
	void jl_me_strt_delete_byte(jl_t *jlc, strt pstr);
	void jl_me_strt_insert_byte(jl_t *jlc, strt pstr, uint8_t pvalue);
	u32_t jl_me_random_int(u32_t a);
	u8_t jl_me_test_next(strt script, strt particle);
	strt jl_me_read_upto(strt script, u8_t end, u32_t psize);
	void *jl_me_tmp_ptr(jl_t* jlc, uint8_t which, void *tmp_ptr);
// "cl.c"
	void jl_cl_list_alphabetize(struct cl_list *list);
// "gl.c"
	jl_vo *jl_gl_vo_make(jl_t* jlc, u32_t count);
	void jl_gl_pr_draw(jl_t* jlc, jl_vo* pv);
	void jl_gl_maketexture(jl_t* jlc, uint16_t gid, uint16_t id,
		void *pixels, int width, int height);
	double jl_gl_ar(jl_t* jlc);
	void jl_gl_clear(jl_t* jlc, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	jl_pr_t * jl_gl_pr_new(jl_t* jlc, float w, float h, uint16_t w_px);
// "sg.c"
	void jl_sg_mode_init(jl_t* jlc, u8_t mdec);
	void jl_sg_mode_set(jl_t* jlc, uint8_t mode, uint8_t wm,
		jl_simple_fnt loop);
	void jl_sg_mode_override(jl_t* jlc, uint8_t wm, jl_simple_fnt loop);
	void jl_sg_mode_reset(jl_t* jlc);
	void jl_sg_mode_switch(jl_t* jlc, uint8_t mode, jl_sg_wm_t loop);
	void jl_sg_kill(jl_t* jlc, char * msg);
	void jl_sg_exit(jl_t* jlc);
	void jl_sg_add_image(jl_t* jlc, char *pzipfile, uint16_t pigid);
// "dl.c"
	void jl_dl_setfullscreen(jl_t *jlc, uint8_t is);
	void jl_dl_togglefullscreen(jl_t *jlc);
	uint16_t jl_dl_getw(jl_t *jlc);
	uint16_t jl_dl_geth(jl_t *jlc);
	void jl_dl_progname(jl_t* jlc, strt name);
// "gr.c"
	void jl_gr_sprdr_dont(jl_t* jlc, jl_sprd_t* spr);
	void jl_gr_pr_old(jl_t* jlc, jl_vo* pv);
	void jl_gr_pr_new(jl_t* jlc, jl_vo* pv, u16_t xres);
	jl_ccolor_t* jl_gr_convert_color(jl_t* jlc, uint8_t *rgba, uint32_t vc,
		uint8_t gradient);
	void jl_gr_vo_color(jl_t* jlc, jl_vo* pv, jl_ccolor_t* cc);
	void jl_gr_draw_vo(jl_t* jlc, jl_vo* pv, jl_vec3_t* vec);
	void jl_gr_draw_pr(jl_t* jlc, jl_vo* pv, jl_vec3_t* vec);
	void jl_gr_vos_vec(jl_t* jlc, jl_vo *pv, uint16_t tricount,
		float* triangles, uint8_t* colors, uint8_t multicolor);
	void jl_gr_vos_rec(jl_t* jlc, jl_vo *pv, jl_rect_t rc, uint8_t* colors,
		uint8_t multicolor);
	void jl_gr_vos_image(jl_t* jlc, jl_vo *pv, jl_rect_t rc,
		u16_t g, u16_t i, u8_t c, u8_t a);
	void jl_gr_vo_old(jl_t* jlc, jl_vo* pv);
	void jl_gr_sprite_redraw(jl_t* jlc, jl_sprite_t *spr);
	void jl_gr_sprite_resz(jl_t* jlc, jl_sprite_t *spr);
	void jl_gr_sprite_loop(jl_t* jlc, jl_sprite_t *spr);
	void jl_gr_sprite_draw_pr(jl_t* jlc, jl_sprite_t *spr);
	jl_sprite_t * jl_gr_sprite_make(jl_t* jlc, jl_rect_t rc,
		jl_gr_sprdr_fnt draw, jl_simple_fnt loop, u32_t ctxs);
	u8_t jl_gr_sprite_collide(jl_t* jlc, jl_sprite_t *sprite1,
		jl_sprite_t *sprite2);
	void jl_gr_draw_text(jl_t* jlc, char *str, f32_t x, f32_t y, f32_t size,
		uint8_t a);
	void jl_gr_draw_numi(jl_t* jlc, uint32_t num, f32_t x, f32_t y, f32_t size,
		uint8_t a);
	void jl_gr_draw_text_area(jl_t* jlc, jl_sprite_t * spr, char *txt);
	void jl_gr_draw_text_sprite(jl_t* jlc,jl_sprite_t * spr,char *txt);
	void jl_gr_draw_ctxt(jl_t* jlc, char *str, f32_t p_y, uint8_t p_a);
	void jl_gr_draw_msge(jl_t* jlc, char* message);
	void jl_gr_term_msge(jl_t* jlc, char* message);
	void jl_gr_draw_slide_button(
		jl_t* jlc, jl_sprite_t * spr, char *txt, float defaultx,
		float slidex, jl_simple_fnt prun);
	void jl_gr_draw_glow_button(jl_t* jlc, jl_sprite_t * spr,
		char *txt, jl_simple_fnt prun);
	uint8_t jl_gr_draw_textbox(jl_t*jlc, float x, float y, float w,
		float h, strt *string);
	void jl_gr_togglemenubar(jl_t* jlc);
	void jl_gr_addicon(jl_t* jlc, jl_simple_fnt fno, jl_simple_fnt fnc,
		jl_simple_fnt rdr);
	void jl_gr_addicon_flip(jl_t* jlc);
	void jl_gr_addicon_slow(jl_t* jlc);
	void jl_gr_addicon_name(jl_t* jlc);
// "ct.c"
// "io.c"
	void jl_io_offset(jl_t* jlc, int16_t tag, char * this);
	void jl_io_close_block(jl_t* jlc);
	void jl_io_tag_set(jl_t* jlc,
		int16_t tag, uint8_t shouldprint, jl_io_print_fnt tagfn);
	void jl_io_printc(jl_t* jlc, const char * print);
	void jl_io_printt(jl_t *jlc, uint8_t a, const char *print);
	void jl_io_printi(jl_t *jlc, int print);
	void jl_io_printd(jl_t *jlc, double print);
	void jl_io_function(jl_t* jlc, const char* fn_name);
	void jl_io_return(jl_t* jlc, const char* fn_name);
	void jl_io_stacktrace(jl_t* jlc);
// "fl.c"
	void jl_fl_save(jl_t* jlc, const void *file, const char *name,
		uint32_t bytes);
	strt jl_fl_load(jl_t* jlc, char *file_name);
	char jl_fl_pk_save(jl_t* jlc, char *packageFileName,
		char *fileName,	void *data, uint64_t dataSize);
	uint8_t *jl_fl_pk_load(jl_t* jlc, const char *packageFileName,
		const char *filename);
	uint8_t *jl_fl_media(jl_t* jlc, char *Fname, char *pzipfile,
		void *pdata, uint64_t psize);
	void jl_fl_mkdir(jl_t* jlc, strt pfilebase);
	char * jl_fl_get_resloc(jl_t* jlc, char* pprg_name, char* pfilename);
	uint8_t jl_fl_user_select_init(jl_t* jlc, const char *program_name,
		void *newfiledata, uint64_t newfilesize);
	void jl_fl_user_select_loop(jl_t* jlc);
	char *jl_fl_user_select_get(jl_t* jlc);
// "cm.c"
// "vi.c"
// "au.c"
	void jl_au_mus_play(jl_t* jlc, uint32_t p_IDinStack,
		uint8_t p_secondsOfFadeOut, uint8_t p_secondsOfFadeIn);
	void jl_au_mus_halt(u8_t p_secondsOfFadeOut);
	uint8_t jl_au_mus_playing(void);
	void jl_au_panning(uint8_t left, uint8_t toCenter);
	void jl_au_panning_default(void);
	void jl_au_add_audio(jl_t* jlc, char *pzipfile, uint16_t pigid);
/*
 *	This a Jeron Lau project. JL_lib (c) 2014 
*/
