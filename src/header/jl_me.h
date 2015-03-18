//Fixed point numbers go from -8.f to 8.f
#define JFXD(x) ((int16_t)(x*4000)) //create fixed number from float
#define JFLT(x) (((float)x)/4000.f) //create float from fixed number

//JLGR (Prev: grph)
	/* pre defined screen locations */
	#define JAL5_GRPH_YDEP (JFXD(9./16.))
	/* screens */
	#define JL_SCREEN_UP 0 //upper screen
	#define JL_SCREEN_DN 1 //lower screen
	/* functions */
	void jal5_jlgr_draw_msge(char * str);

//Image ID's
#define IMGID_TEXT_IMAGE 1
#define IMGID_TERMINAL_TEXT 2
#define IMGID_LANDSCAPE 3
#define IMGID_TASK_BUTTON 4

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

/**
 * fnct(r,n,a) & fncl(r,n,a)
 *
 * @r - return type
 * @n - name of function variable
 * @a - parameters
 */
#define fnct(r,n,a) r (* n)(a)
#define fnc_simple_t(n) fnct(void, n, jl_t* pusr)
#define fnc_event_t(n) void (* n)(jl_t* pusr, fnc_onevent_t(prun))
#define fnc_onevent_t(n) void ( *n )(jl_t *pusr, float x, float y)

typedef struct{
	u16t g; //Image group ID
	u16t i; //Image Id in group
	dect x, y, w, h; //Where & how big to draw sprite
	dect cx, cy, cw, ch; //Collision Box
	u08t c, a; //How to texture the sprite
	fnct(void, loop, void * pusr);
	void* ctx; //The sprite's context [ for variables]
}jl_sprite_t;

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
