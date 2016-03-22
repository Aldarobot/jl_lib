/*
 * JL_lib(c) Jeron A. Lau
 * The header to be included within your programs you make with JL_lib.
*/

#ifndef JLL
#define JLL

#include <time.h>
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

	int jl_init(jl_fnct _fnc_init_, jl_fnct _fnc_kill_);
	void jl_dont(jl_t* jl);
// "JLmem.c"
	void *jl_mem(jl_t* jl, void *a, uint32_t size);
	void *jl_memi(jl_t* jl, uint32_t size);
	void * jl_mem_copy(jl_t* jl, const void *src, size_t size);
	u64_t jl_mem_tbiu(void);
	void jl_mem_leak_init(jl_t* jl);
	void jl_mem_leak_fail(jl_t* jl, str_t fn_name);
	/**
	 * Clear memory pointed to by "mem" of size "size"
	 * @param pmem: memory to clear
	 * @param size: size of "mem"
	**/
	#define jl_mem_clr(pmem, size) memset(pmem, 0, size);
	#define jl_mem_copyto(src, dest, size) memcpy(dest, src, size)
	m_str_t jl_mem_format(jl_t* jl, str_t format, ... );
	u32_t jl_mem_random_int(u32_t a);
	void *jl_mem_tmp_ptr(jl_t* jl, uint8_t which, void *tmp_ptr);
// "JLdata_t.c"
	void jl_data_clear(jl_t* jl, data_t* pa);
	data_t* jl_data_make(u32_t size);
	void jl_data_free(data_t* pstr);
	data_t* jl_data_mkfrom_str(str_t string);
	data_t* jl_data_mkfrom_data(jl_t* jl, u32_t size, const void *data);
	void jl_data_strt(jl_t *jl, data_t* a, const data_t* b, uint64_t bytes);
	void jl_data_merg(jl_t *jl, data_t* a, const data_t* b);
	void jl_data_trunc(jl_t *jl, data_t* a, uint32_t size);
	u8_t jl_data_byte(data_t* pstr);
	void jl_data_loadto(data_t* pstr, u32_t varsize, void* var);
	void jl_data_saveto(data_t* pstr, u32_t varsize, const void* var);
	void jl_data_add_byte(data_t* pstr, u8_t pvalue);
	void jl_data_delete_byte(jl_t *jl, data_t* pstr);
	void jl_data_resize(jl_t *jl, data_t* pstr, u32_t newsize);
	void jl_data_insert_byte(jl_t *jl, data_t* pstr, uint8_t pvalue);
	void jl_data_insert_data(jl_t *jl, data_t* pstr, void* data, u32_t size);
	char* jl_data_tostring(jl_t* jl, data_t* a);
	u8_t jl_data_test_next(data_t* script, str_t particle);
	data_t* jl_data_read_upto(jl_t* jl, data_t* script, u8_t end, u32_t psize);
// "cl.c"
	void jl_cl_list_alphabetize(struct cl_list *list);
// "sg.c"
	void jl_mode_set(jl_t* jl, u8_t mode, u8_t wm, jl_fnct loop);
	void jl_mode_override(jl_t* jl, uint8_t wm, jl_fnct loop);
	void jl_mode_reset(jl_t* jl);
	void jl_mode_switch(jl_t* jl, u8_t mode);
// "JLprint.c"
	void jl_print_tag_set(jl_t* jl, jl_print_fnt tagfn);
	void jl_printc(jl_t* jl, const char * print);
	#define jl_print(jl, ...)\
		jl_printc(jl, jl_mem_format(jl, __VA_ARGS__))
	#ifdef DEBUG
		#define JL_PRINT_DEBUG(jl, ...)\
			jl_printc(jl, jl_mem_format(jl, __VA_ARGS__))
	#else
		#define JL_PRINT_DEBUG(jl, ...)
	#endif
	void jl_print_function(jl_t* jl, str_t fn_name);
	void jl_print_return(jl_t* jl, str_t fn_name);
	void jl_print_stacktrace(jl_t* jl);
// "JLfile.c"
	void jl_file_print(jl_t* jl, str_t fname, str_t msg);
	u8_t jl_file_exist(jl_t* jl, str_t path);
	void jl_file_save(jl_t* jl, const void *file, const char *name,
		uint32_t bytes);
	data_t* jl_file_load(jl_t* jl, str_t file_name);
	char jl_file_pk_save(jl_t* jl, str_t packageFileName, str_t fileName,
		void *data, uint64_t dataSize);
	data_t* jl_file_pk_load(jl_t* jl, const char *packageFileName,
		const char *filename);
	data_t* jl_file_media(jl_t* jl, str_t Fname, str_t pzipfile,
		void *pdata, uint64_t psize);
	u8_t jl_file_mkdir(jl_t* jl, str_t path);
	str_t jl_file_get_resloc(jl_t* jl, str_t prg_folder, str_t fname);
// "JLthread.c"
	uint8_t jl_thread_new(jl_t *jl, str_t name, SDL_ThreadFunction fn);
	uint8_t jl_thread_current(jl_t *jl);
	int32_t jl_thread_old(jl_t *jl, u8_t threadnum);
	SDL_mutex* jl_thread_mutex_new(jl_t *jl);
	void jl_thread_mutex_use(jl_t *jl, SDL_mutex* mutex, jl_fnct fn_);
	void jl_thread_mutex_cpy(jl_t *jl, SDL_mutex* mutex, void* src,
		void* dst, u32_t size);
	void jl_thread_mutex_old(jl_t *jl, SDL_mutex* mutex);
	jl_comm_t* jl_thread_comm_make(jl_t* jl, u32_t size);
	void jl_thread_comm_send(jl_t* jl, jl_comm_t* comm, const void* src);
	void jl_thread_comm_recv(jl_t* jl, jl_comm_t* comm, jl_data_fnct fn);
	void jl_thread_comm_kill(jl_t* jl, jl_comm_t* comm);
#endif
/*
 *	This a Jeron Lau project. JL_lib (c) 2014 
*/
