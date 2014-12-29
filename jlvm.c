#include "jlvmpu.h"

//#include "NEWPROJECT/GEN/img.h"

	/*gl_FragColor = texture2D(texture,\
		vec2(v_Position.x,v_Position.y));*/\
	/*gl_FragColor = texture2D(vtexture,vec2(v_Position.x,v_Position.y))*/;\
	/*gl_FragColor.r = v_mouse[0];\
	gl_FragColor.g = v_mouse[1];\
	gl_FragColor.b = v_mouse[2];\
	gl_FragColor.a = v_mouse[3];\
	if((v_Position.x > u_mouse_p.x) && (v_Position.y < u_mouse_p.y)) {\
		discard;\
		gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\
	}else{\
		gl_FragColor = v_color;\
	}*/

//Functions

jgr_vec_t *jgr_load_jlvg(uint8_t *data) {
	int ii, kk, ll, mm;
	jgr_vec_t *image = malloc(sizeof(jgr_img_t));
	if(data==NULL) { printf("NULL DATA!!\n"); exit(-1); } //check for null data
	printf(" [JLVM/JGR/LIM] loading image...\n");
	image->frameCount = data[0];
	image->colorCount = data[1];
	kk = 2;
	for(ii = 0; ii < image->colorCount; ii++) {
		image->colors[ii].r = data[kk];
		kk++;
		image->colors[ii].g = data[kk];
		kk++;
		image->colors[ii].b = data[kk];
		kk++;
		image->colors[ii].a = data[kk];
		kk++;
	}
	image->triangleCount = data[kk];
	kk++;
	for(ii = 0; ii < image->triangleCount; ii++) {
		for(ll = 0; ll < image->frameCount; ll++) {
			for(mm = 0; mm < 3; mm++) {
				image->triangles[ll][ii].x[mm] = data[kk];
				kk++;
			}
			for(mm = 0; mm < 3; mm++) {
				image->triangles[ll][ii].y[mm] = data[kk];
				kk++;
			}
			for(mm = 0; mm < 3; mm++) {
				image->triangles[ll][ii].c[mm] = data[kk];
				kk++;
			}
		}
	}
	printf(" [JLVM/JGR/LIM] loaded image.\n");
	return image;
}

uint8_t jgr_collide(jgr_spr_t *sprite1, jgr_spr_t *sprite2)
{
	if (  (sprite1->y >= (sprite2->y+sprite2->h)) ||
			(sprite1->x >= (sprite2->x+sprite2->w)) ||
			(sprite2->y >= (sprite1->y+sprite1->h)) ||
			(sprite2->x >= (sprite1->x+sprite1->w)) )
	{
		return 0;
	}else{
		return 1;
	}
}

/*	GLfloat Otwo[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};*/
/*	//BIND&SET TEXTURE_COORD ARRAY BUFFER
	jlvm_bind_buffer(BUFFER_TEXTURE);
	jlvm_check_gl_error(0, "SPR/BIND_BUFF");
	jlvm_set_buffer(Otwo, 12);
	jlvm_check_gl_error(0, "SPR/SET_BUFF");
	//USE IT
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	jlvm_check_gl_error(0, "SPR/TAA");
	glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
	jlvm_check_gl_error(0, "SPR/TAP");*/

uint32_t jgr_loc(uint32_t x, uint32_t y, uint32_t w, uint8_t which_rgba) {
	return (x+(y*w)*4)+which_rgba;
}
