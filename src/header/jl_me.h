typedef enum{
	JL_GL_SLPR_TEX, //Texture Shader
	JL_GL_SLPR_CLR, //Color Shader
	JL_GL_SLPR_MAX,
}jl_gl_slpr;

//Image ID's
typedef enum{
	JL_IMGI_TEXT_IMAGE,
	JL_IMGI_TASK_BUTTON,
	JL_IMGI_TERMINAL_TEXT,
	JL_IMGI_LANDSCAPE,
}jl_imgi_t;

typedef enum{
	_JL_IO_MINIMAL,	//JL-lib prints starting/started/stopping etc.
	_JL_IO_PROGRESS,//JL-lib prints image/audio loading
	_JL_IO_SIMPLE,	//JL-lib prints all landmarks
	_JL_IO_INTENSE,	//JL-lib prints all debug info
	_JL_IO_MAX,
}jl_io_tag_t;

//IO TAGS
#define JL_IO_MINIMAL _JL_IO_MINIMAL - _JL_IO_MAX
#define JL_IO_PROGRESS _JL_IO_PROGRESS - _JL_IO_MAX
#define JL_IO_SIMPLE _JL_IO_SIMPLE - _JL_IO_MAX
#define JL_IO_INTENSE _JL_IO_INTENSE - _JL_IO_MAX

#define JLVM_DEBUG error deprecated

typedef void(*jl_simple_fnt)(jl_t*pusr);
typedef void(*jl_ct_event_fnt)(jl_t* pusr, jl_simple_fnt prun,
	jl_simple_fnt pno);
typedef void(*jl_io_print_fnt)(jl_t* pusr, const char * print);

typedef float jl_ccolor_t;

//Variable Types
#define u08t uint8_t	//Small int
#define u16t uint16_t	//Short int
#define u32t uint32_t	//Normal int
#define u64t uint64_t	//Large int
#define s08t int8_t	//Small int
#define s16t int16_t	//Short int
#define s32t int32_t	//Normal int
#define s64t int64_t	//Large int
#define dect float	//floating point decimal
#define f08t s08t	//8-bit fixed point decimal
#define f16t s16t	//16-bit fixed point decimal
#define f32t s32t	//32-bit fixed point decimal
#define f64t s64t	//64-bit fixed point decimal
#define strt strt_t *
#define strl strt *

//5 bytes of information about the string are included
typedef struct{
	uint8_t* data; //Actual String
	uint32_t size; //Allocated Space In String
	uint32_t curs; //Cursor In String
	uint8_t type;
}strt_t;

//Vertex Object
typedef struct{
	uint8_t rs; // Rendering Style 0=GL_TRIANGLE_FAN 1=GL_TRIANGLES
	uint32_t vc; // # of Vertices
	uint32_t gl; // GL Vertex Buffer Object [ 0 = Not Enabled ]
	uint32_t bt; // Buffer for Texture coordinates or Color Vertices.
	float* cv; // Converted Vertices
	jl_ccolor_t* cc; // Converted Colors [ NULL = Texturing Instead ]
	uint16_t g; // Texturing: Image Group
	uint16_t i; // Texturing: Image ID
	float a; // Texturing: Converted Alpha
}jl_vo;

typedef struct{
	char *opt;
	jl_simple_fnt run;
}jl_popup_button_t;

//strt_t.Type:
	//TEMPORARY - Free after used
	#define STRT_TEMP 0
	//NOT TEMPORARY
	#define STRT_KEEP 1
