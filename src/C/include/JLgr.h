#ifndef JLGR
#define JLGR

#include "jl.h"

// Enum:
typedef enum {
	JL_SCR_DN,
	JL_SCR_UP,
	JL_SCR_SS,
}jl_gr_which_screen_t;

// Types:

typedef void(*jl_gr_sp_fnt)(void* ctx, jl_sprd_t* spr);

typedef struct{
	jl_sprd_t data;		// Sprite Data
	jl_gr_sp_fnt loop;	// Loop function
	jl_gr_sp_fnt draw;	// Draw function
	jl_pr_t *pr;		// Pre-renderer.
}jl_sprite_t;

typedef struct{
	char *opt;
	jl_fnct run;
}jl_popup_button_t;

typedef struct{
	jl_t* jlc;

	// Window Info
	struct {
		uint32_t taskbar[5];
		uint32_t init_image_location;

		// If Matching FPS
		uint8_t on_time;
		uint8_t changed;
		
		//For loading images
		uint16_t image_id;
		uint16_t igid;
		strt image_data;
		
		// Offset x and y
		float offsx, offsy;
		
		// 1 Background for each screen
		struct {
			jl_vo_t* up;
			jl_vo_t* dn;
		}bg;

		float screen_height;

		void* loop; // ( jl_gr_fnct )
		u8_t cscreen; // The current screen "jl_gr_which_screen_t"
	}sg;

	//Input Information
	struct {
		void* getEvents[JL_CT_MAXX];

		float msx, msy;
		int msxi, msyi;

		SDL_Event event;
		
		#if JL_PLAT == JL_PLAT_PHONE
		uint8_t menu;

		#elif JL_PLAT == JL_PLAT_COMPUTER
		const Uint8 *keys;
		#endif
		
		struct {
			#if JL_PLAT == JL_PLAT_PHONE
			uint8_t finger;
			uint8_t menu;
			uint8_t back;
			#elif JL_PLAT == JL_PLAT_COMPUTER
			uint8_t click_left; // Or Click
			uint8_t click_right; // Or Ctrl-Click
			uint8_t click_middle; // Or Shift-Click
			#endif
			//Multi-Platform
			uint8_t scroll_right;
			uint8_t scroll_left;
			uint8_t scroll_up;
			uint8_t scroll_down;
		}input;

		uint8_t back; //Back Key, Escape Key, Start Button
		uint8_t heldDown;
		uint8_t keyDown[255];
		uint32_t sd; //NYI: stylus delete
		
		uint8_t sc;
		uint8_t text_input[32];
		uint8_t read_cursor;
	}ct;
	
	//Opengl Data
	struct {
		uint32_t **textures;
		uint16_t allocatedg;
		uint16_t allocatedi;
		uint8_t whichprg;
		m_u32_t prgs[JL_GL_SLPR_MAX];
		//PRG: TEX
		struct {
			struct {
				m_i32_t position;
				m_i32_t texpos;
			} attr;
			struct {
				m_i32_t **textures;
				m_i32_t multiply_alpha;
				m_i32_t translate;
				m_i32_t transform;
			} uniforms;
		} tex;
		//PRG: PRM
		struct {
			struct {
				m_i32_t position;
				m_i32_t texpos;
			} attr;
			struct {
				m_i32_t textures;
				m_i32_t translate;
				m_i32_t transform;
			} uniforms;
		} prm;
		//PRG: CHR
		struct {
			struct {
				m_i32_t position;
				m_i32_t texpos;
			} attr;
			struct {
				m_i32_t textures;
				m_i32_t multiply_alpha;
				m_i32_t new_color;
				m_i32_t translate;
				m_i32_t transform;
			} uniforms;
		} chr;
		//PRG: CLR
		struct {
			struct {
				m_i32_t position;
				m_i32_t acolor;
			} attr;
			struct {
				m_i32_t translate;
				m_i32_t transform;
			} uniforms;
		} clr;
		jl_vo_t *temp_vo;
		// Default texture coordinates.
		uint32_t default_tc;
		
		jl_pr_t* cp; // Renderer currently being drawn on.
		jl_pr_t* bg; // Screen currently being drawn on.
	}gl;

	//Graphics
	struct {
		jl_gr_sp_fnt menuoverlay;
		jl_sprite_t *taskbar;
		struct {
			char* window_name;
			char* message;
			jl_popup_button_t* btns;
		}popup;
		struct {
			jl_vo_t *whole_screen;
		}vos;
		struct {
			m_str_t message;
			m_u16_t g;
			m_u16_t i;
			m_u8_t c;
		}msge;
		strt textbox_string;
	}gr;

	// SDL
	struct {
		uint8_t fullscreen;

		char windowTitle[2][16];

		SDL_DisplayMode current;
		jl_window_t* displayWindow;
	#if JL_GLRTEX == JL_GLRTEX_SDL
		jl_window_t* whichwindow;
	#endif

		float shifty;
		float shiftx;
		
		jl_rect_t window;
		
		// The complete width and height of the window.
		uint16_t full_w, full_h;
		double aspect;
		uint16_t inner_y;
	}dl;
}jl_gr_t;

typedef void(*jl_gr_fnct)(jl_gr_t* jl_gr);
typedef void(*jl_ct_event_fnct)(jl_t* jlc, jl_gr_fnct prun, jl_gr_fnct pno);

// Prototypes:
jl_gr_t* jl_gr_init(jl_t* jlc, str_t window_name, u8_t fullscreen);
void jl_gr_loop_set(jl_gr_t* jl_gr, jl_fnct onescreen, jl_fnct upscreen,
	jl_fnct downscreen);
void jl_gr_loop(jl_gr_t* jl_gr, void* data, u32_t dataSize);
void jl_gr_kill(jl_gr_t* jl_gr);

// JLGRgraphics:
void jl_gr_dont(jl_gr_t* jl_gr);
void jl_gr_sp_dont(void* jl_gr, jl_sprd_t* spr);
void jl_gr_fill_image_set(jl_gr_t* jl_gr, u16_t g, u16_t i, u8_t c, u8_t a);
void jl_gr_fill_image_draw(jl_gr_t* jl_gr);
void jl_gr_pr_old(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gr_pr_new(jl_gr_t* jl_gr, jl_vo_t* pv, u16_t xres);
void jl_gr_pr(jl_gr_t* jl_gr, jl_vo_t* vo, jl_fnct par__redraw);
void jl_gr_pr_draw(jl_gr_t* jl_gr, jl_vo_t* pv, jl_vec3_t* vec);
jl_ccolor_t* jl_gr_convert_color(jl_gr_t* jl_gr, uint8_t *rgba, uint32_t vc,
	uint8_t gradient);
void jl_gr_vo_color(jl_gr_t* jl_gr, jl_vo_t* pv, jl_ccolor_t* cc);
void jl_gr_draw_vo(jl_gr_t* jl_gr, jl_vo_t* pv, jl_vec3_t* vec);
void jl_gr_vos_vec(jl_gr_t* jl_gr, jl_vo_t *pv, uint16_t tricount,
	float* triangles, uint8_t* colors, uint8_t multicolor);
void jl_gr_vos_rec(jl_gr_t* jl_gr, jl_vo_t *pv, jl_rect_t rc, u8_t* colors,
	uint8_t multicolor);
void jl_gr_vos_image(jl_gr_t* jl_gr, jl_vo_t *pv, jl_rect_t rc,
	u16_t g, u16_t i, u8_t c, u8_t a);
void jl_gr_vos_texture(jl_gr_t* jl_gr, jl_vo_t *pv, jl_rect_t rc,
	jl_tex_t* tex, u8_t c, u8_t a);
void jl_gr_vo_old(jl_gr_t* jl_gr, jl_vo_t* pv);
void jl_gr_sp_rsz(jl_gr_t* jl_gr, jl_sprite_t *spr);
void jl_gr_sp_rdr(jl_gr_t* jl_gr, jl_sprite_t *spr);
void jl_gr_sp_rnl(jl_gr_t* jl_gr, jl_sprite_t *spr);
void jl_gr_sp_drw(jl_gr_t* jl_gr, jl_sprite_t *spr);
jl_sprite_t * jl_gr_sp_new(jl_gr_t* jl_gr, jl_rect_t rc,
	jl_gr_sp_fnt draw, jl_gr_sp_fnt loop, u32_t ctxs);
u8_t jl_gr_sprite_collide(jl_gr_t* jl_gr, jl_sprite_t *sprite1,
	jl_sprite_t *sprite2);
void jl_gr_draw_text(jl_gr_t* jl_gr, str_t str, jl_vec3_t xyz, jl_font_t f);
void jl_gr_draw_int(jl_gr_t* jl_gr, i64_t num, jl_vec3_t loc, jl_font_t f);
void jl_gr_draw_float(jl_gr_t* jl_gr, f64_t num, u8_t dec, jl_vec3_t loc,
	jl_font_t f);
void jl_gr_draw_text_area(jl_gr_t* jl_gr, jl_sprite_t * spr, str_t txt);
void jl_gr_draw_text_sprite(jl_gr_t* jl_gr,jl_sprite_t * spr, str_t txt);
void jl_gr_draw_ctxt(jl_gr_t* jl_gr, char *str, float yy, uint8_t* color);
void jl_gr_draw_msge_(jl_gr_t* jl_gr,u16_t g,u16_t i,u8_t c, m_str_t message);
#define jl_gr_draw_msge(jl_gr,g,i,c,...);\
	jl_gr_draw_msge_(jl_gr,g,i,c,jl_me_format(jl_gr->jlc, __VA_ARGS__));
void jl_gr_term_msge(jl_gr_t* jl_gr, char* message);
void jl_gr_slidebtn_rsz(jl_gr_t* jl_gr, jl_sprite_t * spr, str_t txt);
void jl_gr_slidebtn_rnl(jl_gr_t* jl_gr, jl_sprite_t * spr,  float defaultx,
	float slidex, jl_gr_fnct prun);
void jl_gr_draw_glow_button(jl_gr_t* jl_gr, jl_sprite_t * spr,
	char *txt, jl_gr_fnct prun);
uint8_t jl_gr_draw_textbox(jl_gr_t* jl_gr, float x, float y, float w,
	float h, strt *string);
void jl_gr_togglemenubar(jl_gr_t* jl_gr);
void jl_gr_addicon(jl_gr_t* jl_gr, jl_gr_fnct fno, jl_gr_fnct fnc,
	jl_gr_fnct rdr);
void jl_gr_addicon_flip(jl_gr_t* jl_gr);
void jl_gr_addicon_slow(jl_gr_t* jl_gr);
void jl_gr_addicon_name(jl_gr_t* jl_gr);
// OpenGL
void jl_gl_pbo_new(jl_gr_t* jl_gr, jl_tex_t* texture, u8_t* pixels,
	u16_t w, u16_t h, u8_t bpp);
void jl_gl_pbo_set(jl_gr_t* jl_gr, jl_tex_t* texture, u8_t* pixels,
	u16_t w, u16_t h, u8_t bpp);
jl_vo_t *jl_gl_vo_make(jl_gr_t* jl_gr, u32_t count);
void jl_gl_vo_txmap(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t map);
void jl_gl_maketexture(jl_gr_t* jl_gr, uint16_t gid, uint16_t id,
	void* pixels, int width, int height, u8_t bytepp);
double jl_gl_ar(jl_gr_t* jl_gr);
void jl_gl_clear(jl_gr_t* jl_gr, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void jl_gl_pr_rsz(jl_gr_t* jl_gr, jl_pr_t *pr, f32_t w, f32_t h, u16_t w_px);
jl_pr_t * jl_gl_pr_new(jl_gr_t* jl_gr, f32_t w, f32_t h, u16_t w_px);
void jl_gl_pr_draw(jl_gr_t* jl_gr, jl_pr_t* pr, jl_vec3_t* vec, jl_vec3_t* scl);
void jl_gl_pr(jl_gr_t* jl_gr, jl_pr_t * pr, jl_fnct par__redraw);
// video
strt jl_vi_make_jpeg(jl_t* jlc,i32_t quality,m_u8_t* pxdata,u16_t w,u16_t h);
// SG
void jl_sg_kill(jl_t* jlc);
void jl_sg_exit(jl_t* jlc);
void jl_sg_add_image(jl_t* jlc, str_t pzipfile, u16_t pigid);
// Controls
void jl_ct_run_event(jl_gr_t *jl_gr, uint8_t pevent, jl_gr_fnct prun,
	jl_gr_fnct pno);
void jl_ct_quickloop_(jl_gr_t* jl_gr);
float jl_ct_gmousex(jl_gr_t* jl_gr);
float jl_ct_gmousey(jl_gr_t* jl_gr);
uint8_t jl_ct_typing_get(jl_gr_t* pusr);
void jl_ct_typing_disable(void);

// JLGRfiles.c
uint8_t jl_fl_user_select_init(jl_gr_t* jlc, const char *program_name,
	void *newfiledata, uint64_t newfilesize);
void jl_fl_user_select_loop(jl_gr_t* jlc);
char *jl_fl_user_select_get(jl_gr_t* jlc);

#endif
