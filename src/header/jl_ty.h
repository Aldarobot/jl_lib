/** \file
 * jl_ty.h
 * 	Variable Types.
**/

typedef float jl_ccolor_t;

//Rust-like Variable Types
typedef const uint8_t u8_t;	//8-bit Unsigned Constant
typedef const int8_t i8_t;	//8-bit Signed Constant
typedef uint8_t m_u8_t;	//8-bit Unsigned Modifiable
typedef int8_t m_i8_t;	//8-bit Signed Modifiable

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

//4 bytes of information about the string are included
typedef struct{
	uint8_t* data; //Actual String
	uint32_t size; //Allocated Space In String
	uint32_t curs; //Cursor In String
}strt_t;

typedef strt_t* strt;
typedef strt* strl;

typedef struct{
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
	jl_pr_t *cp;	// Container Pre-Renderer.
}jl_vo;

// Coordinate Structures
typedef struct{
	float x, y, w, h;
}jl_rect_t;

typedef struct{
	float x, y, z;
}jl_vec3_t;

typedef struct {
	jl_vec3_t tr;				// The translate vector.
	jl_rect_t cb;				// Collision Box
	float rh, rw;				// Real Height & Width
	void* ctx;				// The sprite's context.
}jl_sprd_t;

typedef struct{
	uint8_t smde; //Whether 2 or 1 Screens are showing.
	uint32_t info; //@startup:# images loaded from media.zip.Set by others.
	jl_err_t errf; //Set if error
	float psec; //seconds since last loop
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
}jl_t;

typedef void(*jl_simple_fnt)(jl_t* jlc);
typedef void(*jl_ct_event_fnt)(jl_t* jlc, jl_simple_fnt prun,jl_simple_fnt pno);
typedef void(*jl_io_print_fnt)(jl_t* jlc, const char * print);
typedef void(*jl_gr_sprdr_fnt)(jl_t* jlc, jl_sprd_t* spr);

typedef struct{
	jl_sprd_t data;		// Sprite Data
	jl_simple_fnt loop;	// Loop function
	jl_gr_sprdr_fnt draw;	// Draw function
	jl_pr_t *pr;		// Pre-renderer.
}jl_sprite_t;

typedef struct{
	char *opt;
	jl_simple_fnt run;
}jl_popup_button_t;
