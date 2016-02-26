#include "JLgr.h"

typedef struct{
	// Used for all icons on the taskbar.
	jl_vo_t* icon;
	// Not Pressed & Pressed & Redraw Functions for 10 icons.
	jl_gr_fnct func[3][10];
	// Redraw? - 0 = no, 1 = yes
	m_u8_t redraw;
	// Cursor
	m_i8_t cursor;
}jl_taskbar_t;

void jl_sg_add_some_imgs_(jl_gr_t* jl_gr, u16_t x);
uint32_t _jl_sg_gpix(/*in */ SDL_Surface* surface, int32_t x, int32_t y);
void jl_gl_viewport_screen(jl_gr_t* jl_gr);
void jl_gl_poly(jl_gr_t* jl_gr, jl_vo_t* pv, uint8_t vertices, const float *xyzw);
void jl_gl_vect(jl_gr_t* jl_gr, jl_vo_t* pv, uint8_t vertices, const float *xyzw);
void jl_gl_clrc(jl_gr_t* jl_gr, jl_vo_t* pv, jl_ccolor_t* cc);
jl_ccolor_t* jl_gl_clrcs(jl_gr_t* jl_gr, u8_t *rgba, uint32_t vc);
jl_ccolor_t* jl_gl_clrcg(jl_gr_t* jl_gr, u8_t *rgba, uint32_t vc);
void jl_gl_clrg(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t *rgba);
void jl_gl_clrs(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t *rgba);
void jl_gl_txtr(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t map, u8_t a, u16_t pgid, u16_t pi);
void jl_gl_txtr_(jl_gr_t* _jlc, jl_vo_t* pv, u8_t map, u8_t a, u32_t tx);
void jl_gl_transform_pr_(jl_gr_t* jl_gr, jl_pr_t* pr, float x, float y, float z,
	float xm, float ym, float zm);
void jl_gl_transform_vo_(jl_gr_t* jl_gr, jl_vo_t* vo, float x, float y, float z,
	float xm, float ym, float zm);
void jl_gl_transform_chr_(jl_gr_t* jl_gr, jl_vo_t* vo, float x, float y, float z,
	float xm, float ym, float zm);
void jl_gl_draw(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gl_draw_chr(jl_gr_t* jl_gr, jl_vo_t* pv,
	m_f32_t r, m_f32_t g, m_f32_t b, m_f32_t a);
void jl_gl_draw_pr_(jl_t* jlc, jl_pr_t* pr);
uint8_t jl_gl_pr_isi_(jl_gr_t* jl_gr, jl_pr_t* pr);
void jl_gl_pr_use(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gl_pr_off(jl_gr_t* jl_gr);
void jl_gl_pr_scr(jl_gr_t* jl_gr);

//DL
void _jl_dl_loop(jvct_t* _jlc);
void _jl_sg_loop(jl_gr_t* jl_gr);
float jl_sg_seconds_past_(jl_t* jlc);
strt jl_vi_make_jpeg_(jl_t* jlc,i32_t quality,m_u8_t* pxdata,u16_t w,u16_t h);
m_u8_t* jl_vi_load_(jl_t* jlc, strt data, m_u16_t* w, m_u16_t* h);

// Resize function
void jl_dl_resz__(jl_gr_t* jl_gr, uint16_t x, uint16_t y);
void jl_gl_resz__(jl_gr_t* jl_gr);
void jl_sg_resz__(jl_t* jlc);
// init functions.
void jl_dl_init__(jl_gr_t* jl_gr);
void jl_sg_inita__(jl_gr_t* jl_gr);
void jl_sg_initb__(jl_gr_t* jl_gr);
void jl_gl_init__(jl_gr_t* jl_gr);
void jl_gr_init__(jl_gr_t* jl_gr);
void jl_ct_init__(jl_gr_t* jl_gr);
void jl_gr_fl_init(jl_gr_t* jl_gr);
// loop
void jl_ct_loop__(jl_gr_t* jl_gr);
void jl_dl_loop__(jl_gr_t* jl_gr);
// kill
void jl_dl_kill__(jl_gr_t* jl_gr);
