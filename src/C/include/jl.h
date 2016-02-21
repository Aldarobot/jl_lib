/*
 * JL_lib(c) Jeron A. Lau
 * The header to be included within your programs you make with JL_lib.
*/

#ifndef JLL
#define JLL

#include <stdint.h>
#include <stdlib.h>
#include "jl_me.h" // Simple CPU info
#include "jl_en.h" // Enumerations
#include "jl_ty.h" // Variable Types
#include "jl_ct.h" // Input Types
#include "clump.h" // LibClump
// Include libs
#include "JLgr.h"
#include "JLau.h"

#define JL_IMG_HEADER "JLVM0:JYMJ\0" // File format for images
//1=format,4=size,x=data
#define JL_IMG_SIZE_FLS 5 // How many bytes start for images.

	void jl_kill(jl_t* jlc, int rc);
	void jl_init(jl_fnct _fnc_init_);
	void jl_dont(jl_t* jlc);
// "me.c"
	u64_t jl_me_tbiu(void);
	void jl_me_leak_init(jl_t* jlc);
	void jl_me_leak_fail(jl_t* jlc, str_t fn_name);
	void jl_me_clr(void *pmem, uint64_t size);
	#define jl_me_copyto(src, dest, size) memcpy(dest, src, size)
	void * jl_me_copy(jl_t* jlc, const void *src, size_t size);
	void jl_me_alloc(jl_t* jlc, void **a, uint32_t size, uint32_t oldsize);
	#define List(x) jl_me_list_allc(sizeof(void*)*x)
	void jl_me_strt_clear(jl_t* jlc, strt pa);
	strt jl_me_strt_make(u32_t size);
	void jl_me_strt_free(strt pstr);
	strt jl_me_strt_mkfrom_str(str_t string);
	#define Strt(x) jl_me_strt_mkfrom_str((const void*)x)
	strt jl_me_strt_mkfrom_data(jl_t* jlc, uint32_t size, void *data);
	void jl_me_strt_strt(jl_t*jlc, strt a, strt b, uint64_t p);
	void jl_me_strt_merg(jl_t *jlc, strt a, strt b);
	void jl_me_strt_trunc(jl_t *jlc, strt a, uint32_t size);
	char* jl_me_string_fstrt(jl_t* jlc, strt a);
	m_str_t jl_me_format(jl_t* jlc, str_t format, ... );
	uint8_t jl_me_string_print(jl_t* jlc, char *string, const char* format,
		const char *var, u64_t n);
	u8_t jl_me_strt_byte(strt pstr);
	void jl_me_strt_loadto(strt pstr, u32_t varsize, void* var);
	void jl_me_strt_saveto(strt pstr, u32_t varsize, const void* var);
	void jl_me_strt_add_byte(strt pstr, u8_t pvalue);
	void jl_me_strt_delete_byte(jl_t *jlc, strt pstr);
	void jl_me_strt_resize(jl_t *jlc, strt pstr, u32_t newsize);
	void jl_me_strt_insert_byte(jl_t *jlc, strt pstr, uint8_t pvalue);
	void jl_me_strt_insert_data(jl_t *jlc, strt pstr, void* data, u32_t size);
	u32_t jl_me_random_int(u32_t a);
	u8_t jl_me_test_next(strt script, str_t particle);
	strt jl_me_read_upto(jl_t* jlc, strt script, u8_t end, u32_t psize);
	void *jl_me_tmp_ptr(jl_t* jlc, uint8_t which, void *tmp_ptr);
// "cl.c"
	void jl_cl_list_alphabetize(struct cl_list *list);
// "gl.c"

// "sg.c"
	void jl_sg_mode_init(jl_t* jlc, u8_t mdec);
	void jl_sg_mode_set(jl_t* jlc, u8_t mode, u8_t wm, jl_fnct loop);
	void jl_sg_mode_override(jl_t* jlc, uint8_t wm, jl_fnct loop);
	void jl_sg_mode_reset(jl_t* jlc);
	void jl_sg_mode_switch(jl_t* jlc, uint8_t mode, jl_sg_wm_t loop);
// "dl.c"
	void jl_dl_setfullscreen(jl_t *jlc, uint8_t is);
	void jl_dl_togglefullscreen(jl_t *jlc);
	uint16_t jl_dl_getw(jl_t *jlc);
	uint16_t jl_dl_geth(jl_t *jlc);
	void jl_dl_progname(jl_t* jlc, strt name);
// "io.c"
	void jl_io_tag_set(jl_t* jlc, jl_io_print_fnt tagfn);
	void jl_io_printc(jl_t* jlc, const char * print);
	void jl_io_printt(jl_t *jlc, uint8_t a, const char *print);
	void jl_io_printi(jl_t *jlc, int print);
	void jl_io_printd(jl_t *jlc, double print);
	#define jl_io_print(jlc, ...)\
		jl_io_printc(jlc, jl_me_format(jlc, __VA_ARGS__))
	void jl_io_function(jl_t* jlc, str_t fn_name);
	void jl_io_return(jl_t* jlc, str_t fn_name);
	void jl_io_stacktrace(jl_t* jlc);
// "fl.c"
	void jl_fl_print(jl_t* jlc, str_t fname, str_t msg);
	u8_t jl_fl_exist(jl_t* jlc, str_t path);
	void jl_fl_save(jl_t* jlc, const void *file, const char *name,
		uint32_t bytes);
	strt jl_fl_load(jl_t* jlc, str_t file_name);
	char jl_fl_pk_save(jl_t* jlc, str_t packageFileName, str_t fileName,
		void *data, uint64_t dataSize);
	strt jl_fl_pk_load(jl_t* jlc, const char *packageFileName,
		const char *filename);
	strt jl_fl_media(jl_t* jlc, str_t Fname, str_t pzipfile,
		void *pdata, uint64_t psize);
	u8_t jl_fl_mkdir(jl_t* jlc, str_t path);
	str_t jl_fl_get_resloc(jl_t* jlc, str_t prg_folder, str_t fname);
#endif
/*
 *	This a Jeron Lau project. JL_lib (c) 2014 
*/
