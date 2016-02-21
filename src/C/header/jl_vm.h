#include <stdint.h>
#include <stdlib.h>
#include "clump/clump.h"
#include "JLgr.h"

//IO:

	typedef struct {
		m_u32_t var_set_count;
		void **vars;
	}_sg_sprt_t;

	//Standard Mode Class
	typedef struct {
		//Standard Functions
		jl_fnct tclp[JL_SG_WM_MAX];
	}__sg_mode_t;

//OTHER:
typedef struct {
	void (*duu)(void *param);
}jlvm_t;

//JLVM Context Structure
typedef struct {
	jl_t * jlc; // JL_Lib context

	struct {
		__sg_mode_t *mdes; // Array Sizof Number Of Modes
		__sg_mode_t mode; // Current Mode Data
	}mode;

	// Memory
	struct {
		char buffer[256];
		void * tmp_ptr[16];
		m_u8_t status;
		m_u64_t usedmem;
	}me;

	// Terminal Data
	struct {
		int8_t ofs2;
		char head[16][5];
		jl_io_print_fnt printfn;
		int16_t tag[16];
		char stack[50][30];
		uint8_t level;
		char buffer[256];
	}io;
	
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
		strt separator;

		struct{
			char* root; // The root directory "-- JL_Lib/"
			char* cprg; // The current program "-- JL_Lib/program"
			char* errf; // The error file "-- JL_Lib/errf.txt"
		}paths; // Paths to different files.
	}fl; //File Manager

	//in: What's Available
	struct {
		uint8_t graphics; //graphics are enabled
		uint8_t fileviewer; //Fileviewer is enabled
		uint8_t filesys; // Filesystem is enabled.
		uint8_t input; // Input is enabled.
		uint8_t quickloop; // Quickloop is enabled
	}has;

	uint64_t cprg; //current program ID
	_sg_sprt_t fncs; //Functions that change
}jvct_t;
