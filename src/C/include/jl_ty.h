/** \file
 * jl_ty.h
 * 	Variable Types.
**/

#if JL_GLRTEX == JL_GLRTEX_EGL
	#include <EGL/egl.h>
#endif
#include "jl_sdl.h"

typedef float jl_ccolor_t;

//Rust-like Variable Types
typedef const uint8_t u8_t;	//8-bit Unsigned Constant
typedef const int8_t i8_t;	//8-bit Signed Constant
typedef uint8_t m_u8_t;		//8-bit Unsigned Modifiable
typedef int8_t m_i8_t;		//8-bit Signed Modifiable

typedef const uint16_t u16_t;	//16-bit Short int
typedef const int16_t i16_t;	//16-bit Short int
typedef uint16_t m_u16_t;	//16-bit Short int
typedef int16_t m_i16_t;	//16-bit Short int

typedef const uint32_t u32_t;	//32-bit Normal int
typedef const int32_t i32_t;	//32-bit Normal int
typedef uint32_t m_u32_t;	//32-bit Normal int
typedef int32_t m_i32_t;	//32-bit Normal int

typedef const uint64_t u64_t;	//64-bit Large int
typedef const int64_t i64_t;	//64-bit Large int
typedef uint64_t m_u64_t;	//64-bit Large int
typedef int64_t m_i64_t;	//64-bit Large int

typedef const double f64_t;	//floating point decimal
typedef const float f32_t;	//floating point decimal
typedef double m_f64_t;		//floating point decimal
typedef float m_f32_t;		//floating point decimal

typedef const char chr_t;	// Character Constant
typedef const char* str_t;	// String Constant
typedef char m_chr_t;		// Character Modifiable
typedef char* m_str_t;		// String Modifiable

//4 bytes of information about the string are included
typedef struct{
	uint8_t* data; //Actual String
	uint32_t size; //Allocated Space In String
	uint32_t curs; //Cursor In String
}strt_t;

typedef strt_t* strt;
typedef strt* strl;

typedef struct {
	SDL_Window* w;		// Window
	SDL_GLContext* c;	// GL Context
}jl_window_t;

typedef struct {
	m_u32_t gl_texture;
	m_u32_t gl_buffer;
	m_u16_t w, h;
	void* pixels;
}jl_tex_t;

typedef struct {
	// What to render
	uint32_t tx;	// ID to texture.
	uint32_t db;	// ID to Depth Buffer
	uint32_t fb;	// ID to Frame Buffer
	uint16_t w, h;	// Width and hieght of texture
	// Render Area
	uint32_t gl;	// GL Vertex Buffer Object [ 0 = Not Enabled ]
	float ar;	// Aspect Ratio: h:w
	float* cv;	// Converted Vertices
}jl_pr_t;

// Collision Box.
typedef struct{
	m_f32_t x, y, z;
	m_f32_t w, h, d; // Width, Height, Depth.
}jl_cb_t;

//Vertex Object
typedef struct{
	// Other vo's
	m_u32_t nc;	// How many more vo's are following this vo in the array
	// Basic:
	uint8_t rs;	// Rendering Style 0=GL_TRIANGLE_FAN 1=GL_TRIANGLES
	uint32_t gl;	// GL Vertex Buffer Object [ 0 = Not Enabled ]
	uint32_t vc;	// # of Vertices
	float* cv;	// Converted Vertices
	uint32_t bt;	// Buffer for Texture coordinates or Color Vertices.
	// Coloring:
	jl_ccolor_t* cc;// Converted Colors [ NULL = Texturing Instead ]
	// Texturing:
	uint32_t tx;	// ID to texture.
	float a;	// Converted Alpha.
	jl_pr_t *pr;	// Pre-renderer.
	// Collision Box
	jl_cb_t cb;	// 2D/3D collision box.
}jl_vo_t;

// Coordinate Structures
typedef struct{
	float x, y, w, h;
}jl_rect_t;

typedef struct{
	float x, y, z;
}jl_vec3_t;

typedef struct{
	jl_vec3_t pt1;
	jl_vec3_t pt2;
}jl_line_t;

typedef struct {
	m_i32_t g, i; // Group ID, Image ID
	m_u8_t multicolor; // Allow Multiple Colors
	m_u8_t* colors; // The Colors
	m_f32_t size; // The Size
}jl_font_t;

typedef struct {
	jl_vec3_t tr;				// The translate vector.
	jl_rect_t cb;				// Collision Box
	float rh, rw;				// Real Height & Width
	void* ctx;				// The sprite's context.
	void* spr;				// Pointer back to sprite.
}jl_sprd_t;

typedef struct{
	uint8_t smde; //Whether 2 or 1 Screens are showing.
	uint32_t info; //@startup:# images loaded from media.zip.Set by others.
	jl_err_t errf; //Set if error
	struct{
		float psec; // Seconds since last frame.
		uint32_t prev_tick; // Time 1 frame ago started
		uint32_t this_tick; // Time this frame started
		uint16_t fps; // Frames per second.
	}time;
	uint8_t mdec; //Number of Modes
	uint8_t mode; //Current Mode
	jl_sg_wm_t loop; //[WINDOW MODE] ( terminal,up,down,exit screen)
	void* mouse; //jl_sprite_t: Sprite to represent mouse pointer
	void* _jlc; //The library's context
	void* uctx; //The program's context
	struct{
		float x; //X position 0-1
		float y; //Y position 0-1
		float r; //Rotational Value in "pi radians" 2=full circle
		float p; //Pressure 0-1
		uint8_t h; //How long held down.
		uint8_t k; //Which key [ a-z , left/right click ]
	}ctrl;
	uint8_t fontcolor[4];
	jl_font_t font;
	char temp[256];
	// Built-in library pointers.
	void* jl_gr;
	void* jl_au;
}jl_t;

typedef void(*jl_fnct)(jl_t* jlc);
typedef void(*jl_io_print_fnt)(jl_t* jlc, const char * print);
//
