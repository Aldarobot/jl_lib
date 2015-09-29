#include <stdint.h>
#include <stdlib.h>
#include "../lib/clump/clump.h"

//IO:

	typedef struct {
		uint8_t offs;
		int8_t ofs2;
		char head[16][5];
		jl_io_print_fnt *printfn;
		uint16_t maxtag;
		int16_t tag[16];
		char buffer[80];
//		#if JL_IO_DEBUG == 1
		char stack[50][30];
		uint8_t level;
//		#endif
	}_io_t;

	typedef struct {
		m_u32_t var_set_count;
		void **vars;
	}_sg_sprt_t;

	//Standard Mode Class
	typedef struct {
		//Standard Functions
		jl_simple_fnt tclp[JL_SG_WM_MAX];
	}__sg_mode_t;

//AU:
	typedef struct {
		Mix_Music *_MUS;
		char _VOL;
	}__mixr_jmus;

	typedef struct {
		uint32_t idis; //Which music to play next
		uint8_t sofi; //Seconds Of Fade In
		uint8_t sofo; //Seconds Of Fade Out

		int smax; //Music Stack Maximum Music pieces
		__mixr_jmus *jmus; //Pointer Of "smax" Music Pieces
		double pofr; //Point Of Return (Where Music Should Start)
	}_au_t;

//OTHER:
typedef struct {
	void (*duu)(void *param);
}jlvm_t;

//JLVM Context Structure
typedef struct {
	jl_t * jlc; // JL_Lib context
	
	// Memory
	struct {
		uint8_t temp_buff[30];
		void * tmp_ptr[16];
		m_u8_t status;
	}me;

	_io_t io; //Terminal Data
	//Window Info
	struct {
		__sg_mode_t *mdes; // Array Sizof Number Of Modes
		__sg_mode_t mode; // Current Mode Data
		
		uint32_t taskbar[5];
		uint32_t init_image_location;
		uint32_t prev_tick;
		uint32_t processingTimeMillis;
		uint8_t on_time;
		uint8_t changed;
		
		//For loading images
		uint16_t image_id;
		uint16_t igid;
		void *image_data;
		
		// Offset x and y
		float offsx, offsy;
		
		// 1 Background for each screen
		struct {
			jl_vo_t* up;
			jl_vo_t* dn;
		}bg;

		float screen_height;

		jl_simple_fnt loop;
	}sg;
	
	//Input Information
	struct {
		jl_ct_event_fnt getEvents[JL_CT_MAXX];

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
	
	_au_t au; //Audio Info
	
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
				//PRG: TEX
				m_i32_t **textures;
				m_i32_t multiply_alpha;
				m_i32_t translate;
				m_i32_t transform;
			} uniforms;
		} tex;
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
	
	struct {
		struct cl_list *filelist; //List of all files in working dir.
		int8_t cursor;
		uint8_t cpage;
		char *dirname;
		char *selecteditem;
		uint8_t returnit;
		uint8_t drawupto;
		uint8_t inloop;
		jl_sprite_t * btns[2];
		void *newfiledata;
		uint64_t newfilesize;
		uint8_t prompt;
		strt promptstring;

		struct{
			char* root; // The root directory "-- JL_Lib/"
			char* cprg; // The current program "-- JL_Lib/program"
			char* errf; // The error file "-- JL_Lib/errf.txt"
		}paths; // Paths to different files.
	}fl; //File Manager
	
	//Graphics
	struct {
		jl_simple_fnt menuoverlay;
		jl_sprite_t *taskbar;
		struct {
			char* window_name;
			char* message;
			jl_popup_button_t* btns;
		}popup;
		struct {
			jl_vo_t *whole_screen;
		}vos;
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

		float multiplyy;
		float multiplyx;
		float shifty;
		float shiftx;
		
		jl_rect_t window;
		
		// The complete width and height of the window.
		uint16_t full_w, full_h;
		double aspect;
		uint16_t inner_y;
	}dl;

	//in: What's Available
	struct {
		uint8_t graphics; //graphics are enabled
		uint8_t fileviewer; //Fileviewer is enabled
		uint8_t filesys; // Filesystem is enabled.
	}has;

	uint64_t cprg; //current program ID
	_sg_sprt_t fncs; //Functions that change
}jvct_t;
