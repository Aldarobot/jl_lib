#include <stdint.h>
#include <stdlib.h>
#include "../lib/clump/clump.h"

//IO:

	typedef struct{
		uint8_t offs;
		int8_t ofs2;
		uint8_t newline;
		char head[16][5];
		jl_io_print_fnt *printfn;
		uint16_t maxtag;
		int16_t tag[16];
	}_io_t;

	typedef struct{
		u32t var_set_count;
		void **vars;
	}_sg_sprt_t;

	//Standard Mode Class
	typedef struct{
		//Sprites
		u32t sprite_count;
		_sg_sprt_t *sprites;

		//Standard Functions
		jl_simple_fnt tclp[JL_SG_WM_MAX];
	}__sg_mode_t;

//AU:
	typedef struct{
		Mix_Music *_MUS;
		char _VOL;
	}__mixr_jmus;

	typedef struct{
		uint32_t idis; //Which music to play next
		uint8_t sofi; //Seconds Of Fade In
		uint8_t sofo; //Seconds Of Fade Out

		int smax; //Music Stack Maximum Music pieces
		__mixr_jmus *jmus; //Pointer Of "smax" Music Pieces
		double pofr; //Point Of Return (Where Music Should Start)
	}_au_t;
	
	typedef struct {
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
		char *errf_filename;
	}_fl_t;

//OTHER:
typedef struct{
	void (*duu)(void *param);
}jlvm_t;

//JLVM Context Structure
typedef struct{
	jl_t * jlc; // JL_Lib context
	
	// Memory
	struct{
		uint8_t temp_buff[30];
	}me;

	_io_t io; //Terminal Data
	//Window Info
	struct{
		__sg_mode_t *mdes; // Array Sizof Number Of Modes
		__sg_mode_t mode; // Current Mode Data
		
		uint32_t taskbar[5];
		uint32_t init_image_location;
		uint32_t prev_tick;
		uint32_t this_tick;
		uint32_t processingTimeMillis;
		
		//For loading images
		uint16_t image_id;
		uint16_t igid;
		void *image_data;
		
		// 1 Background for each screen
		struct{
			jl_vo* up;
			jl_vo* dn;
		}bg;
		
		jl_vo* cbg;
		float screen_height;
		
		jl_simple_fnt loop;
	}sg;
	
	//Input Information
	struct{
		jl_ct_event_fnt getEvents[JL_CT_MAXX];

		float msx, msy;
		int msxi, msyi;

		SDL_Event event;
		
		#if JL_PLAT == JL_PLAT_PHONE
		uint8_t menu;

		#elif JL_PLAT == JL_PLAT_COMPUTER
		const Uint8 *keys;
		#endif
		
		struct{
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
	struct{
		GLuint **textures;
		uint16_t allocatedg;
		uint16_t allocatedi;
		uint8_t whichprg;
		GLuint prgs[JL_GL_SLPR_MAX];
		//PRG: TEX
		struct{
			struct {
				GLint position;
				GLint texpos;
			} attr;
			struct {
				//PRG: TEX
				GLint **textures;
				GLint multiply_alpha;
				GLint translate;
			} uniforms;
		} tex;
		//PRG: CLR
		struct{
			struct {
				GLint position;
				GLint acolor;
			} attr;
			struct {
				GLint translate;
			} uniforms;
		} clr;
		jl_vo *temp_vo;
		// Default texture coordinates.
		uint32_t default_tc;
	}gl;
	
	_fl_t fl; //File Manager
	
	//Graphics
	struct{
		jl_simple_fnt menuoverlay;
		struct{
			uint16_t grp[10];
			uint16_t iid[10];
			uint8_t chr[10];
			jl_simple_fnt func[2][10];
			uint8_t cursor;
			float iconx;
		}menubar;
		struct{
			char* window_name;
			char* message;
			jl_popup_button_t* btns;
		}popup;
		strt textbox_string;
	}gr;

	// SDL
	struct {
		uint8_t fullscreen;

		char windowTitle[2][16];

		//where everything is blitted to
		SDL_GLContext *glcontext;
		SDL_DisplayMode current;
		SDL_Window *displayWindow;

		uint16_t currenty;
		float multiplyy;
		float multiplyx;
		float shifty;
		float shiftx;
		
		jl_rect_t window;
		
		// The complete width and height of the window.
		uint16_t full_w, full_h;
	}dl;


	//in: What's Available
	struct{
		uint8_t graphics; //graphics are enabled
		uint8_t fileviewer; //Fileviewer is enabled
	}has;

	uint64_t cprg; //current program ID
	_sg_sprt_t fncs; //Functions that change
}jvct_t;
