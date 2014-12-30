/*
 * simple example program that starts up and immediately quits.  Good for making
 * script-like programs.
*/
#include "bin/jlib/jlvm.h"
#include <stdint.h>
#include <SDL2/SDL_image.h>

#define FORMAT_PIC 3
#define FORMAT_HQB 2
#define FORMAT_IMG 1

#define HEADER "JLVM0:JYMJ"
#define STN_IMG_SIZE (1+strlen(HEADER)+(256*4)+(1024*1024)+1)
#define IMAGE_DESKTOP "umed/img.text"

void jl_pr_get_cols(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a)
{
	uint8_t *rgba = (void*)&color;
	r[0] = rgba[0];
	g[0] = rgba[1];
	b[0] = rgba[2];
	a[0] = rgba[3];
}

//Get a pixels RGBA values from a surface and xy
void _jlvm_gpix(
	/*in */ SDL_Surface *surface, int32_t x, int32_t y,
	/*out*/ uint8_t *or, uint8_t *og, uint8_t *ob, uint8_t *oa)
{
	uint8_t *p = (uint8_t *)surface->pixels + (y * surface->pitch) + (x * 1);
	SDL_Color *color= &surface->format->palette->colors[*p];
//	uint8_t r, g, b, a;
//	jl_pr_get_cols(*(uint32_t *)p, &r, &g, &b, &a);
	or[0] = color->r;
	og[0] = color->g;
	ob[0] = color->b;
	oa[0] = color->a;
}

void add_byte(strt basestr, uint8_t byte) {
	if(basestr->curs>=basestr->size) { printf("out v'room\n"); exit(-1); }
	basestr->data[basestr->curs] = byte;
	basestr->curs++;
}

//Add "str" at "cursor" in "basestr"
void add_string(strt basestr, char *str) {
	int i;
	for(i = 0; i < strlen(str); i++) {
		add_byte(basestr, str[i]);
	}
}

void add_u32t(strt basestr, uint32_t pu32t) {
	uint8_t *fu32t = (void *)&pu32t;
	int i;
	for(i = 0; i < 4; i++) {
		add_byte(basestr, fu32t[i]);
	}
}

//Returns A String Size "size"
strt strt_(uint32_t size) {
	strt a = malloc(sizeof(strt));
	a->data = malloc(size);
	a->size = size;
	a->curs = 0;
	int i;
	for( i = 0; i < size; i++) {
		a->data[i] = 0;
	}
	return a;
}

uint8_t *jgr_load_stn_img(char *fileName) {
	siop_prnt_lwst(0,Strt("MEDI/CONV"),amem_strt_merg(
		Strt("Lōdēŋ ymydj..."), Strt(fileName), STRT_TEMP));
	//1=format,256*4=key,1024*1024=pixelmap
	strt data_out = strt_(STN_IMG_SIZE);
	SDL_Surface *image = IMG_Load((void *)
		amem_strt_merg(Strt("umed/FIMG/"), Strt(fileName), STRT_TEMP)
			->data);
	if ( image == NULL ) {
		fprintf(stderr, "[JLL/JGR/LIM] Cûd'nt lōd ymyj: %s\n",IMG_GetError());
//		jgr_termination_message("IMAGE LOAD FAILED.");
		exit(-1);
	}
	int ii, jj, kk;
	uint32_t keyCount = 0;
	uint32_t key[256];
	key[0] = 1;
	uint8_t isUnuse;
	uint32_t p;
	uint8_t *keyp = (void *)&p;
	uint8_t kp[1024][1024];
	uint8_t pi;
	for(ii = 0; ii < 256; ii++) {
		key[ii] = 0x01010101;
	}

	uint8_t test_r;
	uint8_t test_g;
	uint8_t test_b;
	uint8_t test_a;
	//Test
	_jlvm_gpix(image, 0, 0, &test_r, &test_g, &test_b, &test_a);
	printf(" [MEDI/CONV] pix(0,0) %d,%d,%d,%d\n",test_r,test_g,test_b,test_a);

	//Getting Information
	for(ii = 0; ii < 1024; ii++) {
		for(jj = 0; jj < 1024; jj++) {
			isUnuse = 1;
			_jlvm_gpix(image,ii,jj,&keyp[0],&keyp[1],&keyp[2],&keyp[3]);
			for(kk = 0; kk < keyCount; kk++) {
				if(key[kk] == p) {
					isUnuse = 0;
					pi = kk;
					break;
				}
			}
			if(isUnuse) {
				key[keyCount] = p;
				kp[ii][jj] = keyCount;
				//uint8_t *fu32t = (void *)&key[keyCount];
				//printf("(%d,%d)%d,%d,%d,%d\n", ii, jj, fu32t[0], fu32t[1], fu32t[2], fu32t[3]);
				keyCount++;
			}else{
				kp[ii][jj] = pi;
			}
		}
	}
	printf(" [MEDI/CONV] CULÛR CAŪNT: %d\n", keyCount);

	//FORMAT
	add_string(data_out, HEADER); //JLVM FILE TYPE HEADER VERSION0: JYMJ FILE
	add_byte(data_out, FORMAT_IMG); //SUBTYPE:Standard Image Type (Like GIF)
	//KEY
	for(ii = 0; ii < 255; ii++) {
		add_u32t(data_out, key[ii]);
	}
	printf(" [MEDI/CONV] Set'ēŋ pyksŷl's...\n");
	//PIXELMAP
	for(ii = 0; ii < 1024; ii++) {
		for(jj = 0; jj < 1024; jj++) {
			//Add Pixel: Unrotate (Not sure why rotated)
			add_byte(data_out, kp[jj][1024-ii]);
		}
	}
	printf(" [MEDI/CONV] Set'yd pyksŷls! Lōd'yd ymyj!\n");
	return data_out->data;
}

void hack_user_init(sgrp_user_t* pusr) {
	siop_prnt_cplo(0,"MEDI/CONV","File MEDI/CONVerter: Version 2.0");
//	jlvmpi_prg_name("PRG_FILE_FORMAT_MEDI/CONVERTER");
	uint8_t *list = file_file_load(pusr, IMAGE_DESKTOP);
	uint8_t file_name[100];
	uint32_t file_name_cursor = 0;
	uint8_t i;
	for(i = 0; i < 100; i++)
		file_name[i] = '\0';
	
	siop_prnt_cplo(0,"MEDI/CONV",(void*)list);
	if((list[0] == '\n') || (list[0] == '\0')) {
		printf(" [MEDI/CONV] NO CONTENTS! TO %s\n", IMAGE_DESKTOP);
		return;
	}
	list--;
	while(1) {
		list++;
		if(list[0] == '\0') {
			break;
		}
		if(list[0] == '\n') {
			printf(" [MEDI/CONV] COMPILING %s...\n", file_name);
			char *fdat = (void*)jgr_load_stn_img((void *)
				amem_strt_merg(Strt(file_name), Strt(".gif"),
					STRT_KEEP)->data);
			file_file_save(pusr,fdat,
				(void*)amem_strt_merg(
					amem_strt_merg(Strt("umed/IMG/"),
						Strt(file_name), STRT_TEMP),
					Strt(".jymj"), STRT_TEMP)->data,
				STN_IMG_SIZE);
			printf(" [MEDI/CONV] COMPILED %s\n", file_name);
			for(i = 0; i < 100; i++) {
				file_name[i] = '\0';
			}
			file_name_cursor = 0;
			list++;
		}
		file_name[file_name_cursor] = list[0];
		file_name_cursor++;
	}

	printf(" [MEDI/CONV] DONE/DUN!\n");
}
