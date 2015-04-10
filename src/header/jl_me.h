//Image ID's
typedef enum{
	JL_IMGI_TEXT_IMAGE,
	JL_IMGI_TASK_BUTTON,
	JL_IMGI_TERMINAL_TEXT,
	JL_IMGI_LANDSCAPE,
}jl_imgi_t;

typedef enum{
	JL_IO_TAG_MINIMAL,	//JL-lib prints starting/started/stopping etc.
	JL_IO_TAG_PROGRESS,	//JL-lib prints image/audio loading
	JL_IO_TAG_SIMPLE,	//JL-lib prints all landmarks
	JL_IO_TAG_INTENSE,	//JL-lib prints all debug info
	JL_IO_TAG_MAX,
}jl_io_tag_t;

typedef void(*jl_simple_fnt)(jl_t*pusr);
typedef void(*jl_ct_onevent_fnt)(jl_t *pusr, float x, float y);
typedef void(*jl_ct_event_fnt)(jl_t* pusr, jl_ct_onevent_fnt prun,
	jl_ct_onevent_fnt pno);
typedef void(*jl_io_print_fnt)(jl_t* pusr, const char * print);

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
	u08t* data; //Actual String
	u32t size; //Allocated Space In String
	u32t curs; //Cursor In String
	u08t type;
}strt_t;

//strt_t.Type:
	//TEMPORARY - Free after used
	#define STRT_TEMP 0
	//NOT TEMPORARY
	#define STRT_KEEP 1
