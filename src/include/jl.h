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

#define JL_IMG_HEADER "JLVM0:JYMJ\0" // File format for images
//1=format,4=size,x=data
#define JL_IMG_SIZE_FLS 5 // How many bytes start for images.

	void jl_kill(jl_t* jlc, int rc);
	void jl_init(jl_simple_fnt _fnc_init_);
	void jl_dont(jl_t* jlc);
// "me.c"
	u64_t jl_me_tbiu(void);
	void jl_me_leak_init(jl_t* jlc);
	void jl_me_leak_fail(jl_t* jlc, str_t fn_name);
	void jl_me_clr(void *pmem, uint64_t size);
	#define jl_me_copyto(src, dest, size) memcpy(dest, src, size)
	void * jl_me_copy(jl_t* jlc, const void *src, size_t size);
	void jl_me_alloc(jl_t* jlc, void **a, uint32_t size, uint32_t oldsize);
	#define List(x) jl_me_list_allc(sizeof(void*)*x)
	void jl_me_strt_clear(strt pa);
	strt jl_me_strt_make(u32_t size);
	void jl_me_strt_free(strt pstr);
	strt jl_me_strt_mkfrom_str(str_t string);
	#define Strt(x) jl_me_strt_mkfrom_str((const void*)x)
	strt jl_me_strt_mkfrom_data(jl_t* jlc, uint32_t size, void *data);
	void jl_me_strt_strt(jl_t*jlc, strt a, strt b, uint64_t p);
	void jl_me_strt_merg(jl_t *jlc, strt a, strt b);
	void jl_me_strt_trunc(jl_t *jlc, strt a, uint32_t size);
	char* jl_me_string_fstrt(jl_t* jlc, strt a);
	m_str_t jl_me_format(jl_t* jlc, str_t format, ... );
	uint8_t jl_me_string_print(jl_t* jlc, char *string, const char* format,
		const char *var, u64_t n);
	u8_t jl_me_strt_byte(strt pstr);
	void jl_me_strt_loadto(strt pstr, u32_t varsize, void* var);
	void jl_me_strt_saveto(strt pstr, u32_t varsize, const void* var);
	void jl_me_strt_add_byte(strt pstr, u8_t pvalue);
	void jl_me_strt_delete_byte(jl_t *jlc, strt pstr);
	void jl_me_strt_insert_byte(jl_t *jlc, strt pstr, uint8_t pvalue);
	u32_t jl_me_random_int(u32_t a);
	u8_t jl_me_test_next(strt script, str_t particle);
	strt jl_me_read_upto(jl_t* jlc, strt script, u8_t end, u32_t psize);
	void *jl_me_tmp_ptr(jl_t* jlc, uint8_t which, void *tmp_ptr);
// "cl.c"
	void jl_cl_list_alphabetize(struct cl_list *list);
// "gl.c"
	void jl_gl_pbo_new(jl_t* jlc, jl_tex_t* texture, u8_t* pixels,
		u16_t w, u16_t h, u8_t bpp);
	void jl_gl_pbo_set(jl_t* jlc, jl_tex_t* texture, u8_t* pixels,
		u16_t w, u16_t h, u8_t bpp);
	jl_vo_t *jl_gl_vo_make(jl_t* jlc, u32_t count);
	void jl_gl_vo_txmap(jl_t* jlc, jl_vo_t* pv, u8_t map);
	void jl_gl_maketexture(jl_t* jlc, uint16_t gid, uint16_t id,
		void* pixels, int width, int height, u8_t bytepp);
	double jl_gl_ar(jl_t* jlc);
	void jl_gl_clear(jl_t* jlc, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void jl_gl_pr_rsz(jl_t* jlc, jl_pr_t *pr, f32_t w, f32_t h, u16_t w_px);
	jl_pr_t * jl_gl_pr_new(jl_t* jlc, f32_t w, f32_t h, u16_t w_px);
	void jl_gl_pr_draw(jl_t* jlc, jl_pr_t* pr, jl_vec3_t* vec, jl_vec3_t* scl);
	void jl_gl_pr(jl_t* jlc, jl_pr_t * pr, jl_simple_fnt par__redraw);
// "sg.c"
	void jl_sg_mode_init(jl_t* jlc, u8_t mdec);
	void jl_sg_mode_set(jl_t* jlc, u8_t mode, u8_t wm, jl_simple_fnt loop);
	void jl_sg_mode_override(jl_t* jlc, uint8_t wm, jl_simple_fnt loop);
	void jl_sg_mode_reset(jl_t* jlc);
	void jl_sg_mode_switch(jl_t* jlc, uint8_t mode, jl_sg_wm_t loop);
	void jl_sg_kill(jl_t* jlc);
	void jl_sg_exit(jl_t* jlc);
	void jl_sg_add_image(jl_t* jlc, str_t pzipfile, u16_t pigid);
// "dl.c"
	void jl_dl_setfullscreen(jl_t *jlc, uint8_t is);
	void jl_dl_togglefullscreen(jl_t *jlc);
	uint16_t jl_dl_getw(jl_t *jlc);
	uint16_t jl_dl_geth(jl_t *jlc);
	void jl_dl_progname(jl_t* jlc, strt name);
// "gr.c"
	void jl_gr_sp_dont(jl_t* jlc, jl_sprd_t* spr);
	void jl_gr_fill_image_set(jl_t *jlc, u16_t g, u16_t i, u8_t c, u8_t a);
	void jl_gr_fill_image_draw(jl_t *jlc);
	void jl_gr_pr_old(jl_t* jlc, jl_vo_t* pv);
	void jl_gr_pr_new(jl_t* jlc, jl_vo_t* pv, u16_t xres);
	void jl_gr_pr(jl_t *jlc, jl_vo_t* vo, jl_simple_fnt par__redraw);
	void jl_gr_pr_draw(jl_t* jlc, jl_vo_t* pv, jl_vec3_t* vec);
	jl_ccolor_t* jl_gr_convert_color(jl_t* jlc, uint8_t *rgba, uint32_t vc,
		uint8_t gradient);
	void jl_gr_vo_color(jl_t* jlc, jl_vo_t* pv, jl_ccolor_t* cc);
	void jl_gr_draw_vo(jl_t* jlc, jl_vo_t* pv, jl_vec3_t* vec);
	void jl_gr_vos_vec(jl_t* jlc, jl_vo_t *pv, uint16_t tricount,
		float* triangles, uint8_t* colors, uint8_t multicolor);
	void jl_gr_vos_rec(jl_t* jlc, jl_vo_t *pv, jl_rect_t rc, u8_t* colors,
		uint8_t multicolor);
	void jl_gr_vos_image(jl_t* jlc, jl_vo_t *pv, jl_rect_t rc,
		u16_t g, u16_t i, u8_t c, u8_t a);
	void jl_gr_vos_texture(jl_t* jlc, jl_vo_t *pv, jl_rect_t rc,
		jl_tex_t* tex, u8_t c, u8_t a);
	void jl_gr_vo_old(jl_t* jlc, jl_vo_t* pv);
	void jl_gr_sp_rsz(jl_t* jlc, jl_sprite_t *spr);
	void jl_gr_sp_rdr(jl_t* jlc, jl_sprite_t *spr);
	void jl_gr_sp_rnl(jl_t* jlc, jl_sprite_t *spr);
	void jl_gr_sp_drw(jl_t* jlc, jl_sprite_t *spr);
	jl_sprite_t * jl_gr_sp_new(jl_t* jlc, jl_rect_t rc,
		jl_gr_sp_fnt draw, jl_gr_sp_fnt loop, u32_t ctxs);
	u8_t jl_gr_sprite_collide(jl_t* jlc, jl_sprite_t *sprite1,
		jl_sprite_t *sprite2);
	void jl_gr_draw_text(jl_t* jlc, str_t str, jl_vec3_t xyz, jl_font_t f);
	void jl_gr_draw_int(jl_t* jlc, i64_t num, jl_vec3_t loc, jl_font_t f);
	void jl_gr_draw_float(jl_t* jlc, f64_t num, u8_t dec, jl_vec3_t loc,
		jl_font_t f);
	void jl_gr_draw_text_area(jl_t* jlc, jl_sprite_t * spr, str_t txt);
	void jl_gr_draw_text_sprite(jl_t* jlc,jl_sprite_t * spr, str_t txt);
	void jl_gr_draw_ctxt(jl_t* jlc, char *str, float yy, uint8_t* color);
	void jl_gr_draw_msge_(jl_t* jlc,u16_t g,u16_t i,u8_t c,m_str_t message);
	#define jl_gr_draw_msge(jlc,g,i,c,...);\
		jl_gr_draw_msge_(jlc,g,i,c,jl_me_format(jlc, __VA_ARGS__));
	void jl_gr_term_msge(jl_t* jlc, char* message);
	void jl_gr_slidebtn_rsz(jl_t* jlc, jl_sprite_t * spr, str_t txt);
	void jl_gr_slidebtn_rnl(jl_t* jlc, jl_sprite_t * spr,  float defaultx,
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
	void jl_io_tag_set(jl_t* jlc, jl_io_print_fnt tagfn);
	void jl_io_printc(jl_t* jlc, const char * print);
	void jl_io_printt(jl_t *jlc, uint8_t a, const char *print);
	void jl_io_printi(jl_t *jlc, int print);
	void jl_io_printd(jl_t *jlc, double print);
	#define jl_io_print(jlc, ...)\
		jl_io_printc(jlc, jl_me_format(jlc, __VA_ARGS__))
	void jl_io_function(jl_t* jlc, str_t fn_name);
	void jl_io_return(jl_t* jlc, str_t fn_name);
	void jl_io_stacktrace(jl_t* jlc);
// "fl.c"
	void jl_fl_print(jl_t* jlc, str_t fname, str_t msg);
	u8_t jl_fl_exist(jl_t* jlc, str_t path);
	void jl_fl_save(jl_t* jlc, const void *file, const char *name,
		uint32_t bytes);
	strt jl_fl_load(jl_t* jlc, str_t file_name);
	char jl_fl_pk_save(jl_t* jlc, str_t packageFileName, str_t fileName,
		void *data, uint64_t dataSize);
	strt jl_fl_pk_load(jl_t* jlc, const char *packageFileName,
		const char *filename);
	strt jl_fl_media(jl_t* jlc, str_t Fname, str_t pzipfile,
		void *pdata, uint64_t psize);
	u8_t jl_fl_mkdir(jl_t* jlc, str_t path);
	str_t jl_fl_get_resloc(jl_t* jlc, str_t prg_folder, str_t fname);
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
	void jl_au_add_audio(jl_t* jlc, str_t pzipfile, uint16_t pigid);
/*
 *	This a Jeron Lau project. JL_lib (c) 2014 
*/
