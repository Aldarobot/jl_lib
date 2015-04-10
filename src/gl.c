#include "header/jl_pr.h"

char *source_frag_tex = 
#ifdef GL_ES_VERSION_2_0
	"#version 100\n"
	"precision mediump float;\n"
#else
	"#version 120\n"
#endif
	"\n"
	"uniform sampler2D texture;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
//	"	gl_FragColor = mix(\n"
//	"		texture2D(textures[0], texcoord),\n"
//	"		texture2D(textures[1], texcoord),\n"
//	"		fade_factor\n"
//	"	);\n"
//	"	gl_FragColor = texture2D(texture, vec2(texcoord.x, texcoord.y));"
	"	gl_FragColor = texture2D(texture,"
	"		vec2(texcoord.x - .0039, texcoord.y + .001));"
//	"	gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);"
//	"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
	"}\n\0";

char *source_vert_tex = 
#ifdef GL_ES_VERSION_2_0
	"#version 100\n"
	"precision mediump float;\n"
#else
	"#version 120\n"
#endif
	"\n"
	"attribute vec3 position;\n"
	"attribute vec2 texpos;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position, 1.0);\n"
	"	texcoord = texpos;\n"
	"}\n\0";

/*static const char source_vert_tex[] = 
	"#version 100\n"
	"attribute vec4 position;\n"
	"void main() {\n"
	"  gl_Position = position;\n"
	"}\n";

static const char source_frag_tex[] = 
	"#version 100\n"
	"precision mediump float;\n"
	"void main() {\n"
	"  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n";*/

void jal5_eogl_cerr(jvct_t *pjlc, int width, char *fname) {
	GLenum err= glGetError();
	if(err==0) return;
	char *fstrerr;
	if(err == GL_INVALID_ENUM) {
		fstrerr = "opengl: invalid enum";
	}else if(err == GL_INVALID_VALUE) {
		fstrerr = "opengl: invalid value!!\n";
	}else if(err == GL_INVALID_OPERATION) {
		fstrerr = "opengl: invalid operation!!\n";
	}else if(err == GL_OUT_OF_MEMORY) {
		fstrerr = "opengl: out of memory ): !! (Texture too big?)\n";
	}else{
		fstrerr = "opengl: unknown error!\n";
	}
	jlvm_dies(pjlc, jl_me_strt_merg(
		jl_me_strt_merg(Strt(fname), Strt(fstrerr), STRT_TEMP),
		jl_me_strt_merg(Strt(":"), jl_me_strt_fnum(width), STRT_TEMP),
		STRT_TEMP));
}

void _jal5_eogl_buff_bind(jvct_t *pjlc, uint32_t buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	jal5_eogl_cerr(pjlc, 0,"bind buffer");
}

void jal5_eogl_buff_data(jvct_t *pjlc, uint32_t buffer, void *buffer_data,
	u08t buffer_size)
{
	_jal5_eogl_buff_bind(pjlc, buffer);
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(double), buffer_data,
		GL_STATIC_DRAW);
	jal5_eogl_cerr(pjlc, 0,"buffer data");
}

void _jal5_eogl_buff_make(jvct_t *pjlc, uint32_t *buffer)
{
	glGenBuffers(1, buffer);
	jal5_eogl_cerr(pjlc, 0,"buffer gen");
}

GLuint loadShader(jvct_t *pjlc, GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	jal5_eogl_cerr(pjlc, 0,"couldn't create shader");
	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pSource, NULL);
		jal5_eogl_cerr(pjlc, 0,"glShaderSource");
		glCompileShader(shader);
		jal5_eogl_cerr(pjlc, 0,"glCompileShader");
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		jal5_eogl_cerr(pjlc, 0,"glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			char* buf;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			jal5_eogl_cerr(pjlc, 1,"glGetShaderiv");
			if (infoLen) {
				buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					jlvm_dies(pjlc, jl_me_strt_merg(
						Strt("Could not compile shader:\n"),
						Strt(buf), STRT_TEMP));
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint createProgram(jvct_t *pjlc, const char* pVertexSource,
	const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(pjlc, GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		jlvm_dies(pjlc, Strt("couldn't load vertex shader"));
	}

	GLuint pixelShader = loadShader(pjlc, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		jlvm_dies(pjlc, Strt("couldn't load fragment shader"));
	}

	GLuint program = glCreateProgram();
	jal5_eogl_cerr(pjlc, 0,"glCreateProgram");
	if (program) {
		GLint linkStatus = GL_FALSE;

		glAttachShader(program, vertexShader);
		jal5_eogl_cerr(pjlc, 0,"glAttachShader (vertex)");
		glAttachShader(program, pixelShader);
		jal5_eogl_cerr(pjlc, 0,"glAttachShader (fragment)");
		glLinkProgram(program);
		jal5_eogl_cerr(pjlc, 0,"glLinkProgram");
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		jal5_eogl_cerr(pjlc, 0,"glGetProgramiv");
		glValidateProgram(program);
		jal5_eogl_cerr(pjlc, 1,"glValidateProgram");
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			char* buf;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			jal5_eogl_cerr(pjlc, 1,"glGetProgramiv");
			if (bufLength) {
				buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					jlvm_dies(pjlc, jl_me_strt_merg(
						Strt("Could not link program:\n"),
						Strt(buf), STRT_TEMP));
					free(buf);
				}else{
					jlvm_dies(pjlc, Strt("failed malloc"));
				}
			}else{
				glDeleteProgram(program);
				jlvm_dies(pjlc, Strt("no info log"));
			}
		}
	}
	return program;
}

void jl_gl_maketexture(jl_t* pusr, uint16_t gid, uint16_t id,
	void *pixels, int width, int height)
{
	jvct_t *pjlc = pusr->pjlc;
	if (!pixels)
		jlvm_dies(pjlc, Strt("null pixels"));
	if (pjlc->gl.allocatedg < gid + 1) {
		pjlc->gl.textures =
			realloc(pjlc->gl.textures, sizeof(GLuint *) * (gid+1));
		pjlc->gl.uniforms.textures =
			realloc(pjlc->gl.uniforms.textures,
				sizeof(GLint *) * (gid+1));
		pjlc->gl.allocatedg = gid + 1;
		pjlc->gl.allocatedi = 0;
		pjlc->gl.textures[gid] = NULL;
		pjlc->gl.uniforms.textures[gid] = NULL;
	}
	if (pjlc->gl.allocatedi < id + 1) {
		pjlc->gl.textures[gid] =
			realloc(pjlc->gl.textures[gid],
				sizeof(GLuint) * (id+1));
		pjlc->gl.uniforms.textures[gid] =
			realloc(pjlc->gl.uniforms.textures[gid],
				sizeof(GLint) * (id+1));
		pjlc->gl.allocatedi = id + 1;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("generating texture...");
	#endif
	glGenTextures(1, &pjlc->gl.textures[gid][id]);
	jal5_eogl_cerr(pjlc, 0,"gen textures");
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[gid][id]);
	jal5_eogl_cerr(pjlc, 0,"bind textures");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("settings pars...");
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	jal5_eogl_cerr(pjlc, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	jal5_eogl_cerr(pjlc, 1,"glTexParameteri");
	//GL_CLAMP_TO_BORDER GL_REPEAT GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	jal5_eogl_cerr(pjlc, 2,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	jal5_eogl_cerr(pjlc, 3,"glTexParameteri");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("wh:%d,%d\n", width, height);
	#endif
	glTexImage2D(
		GL_TEXTURE_2D, 0,		/* target, level */
		GL_RGBA,			/* internal format */
		width, height, 0,		/* width, height, border */
		GL_RGBA, GL_UNSIGNED_BYTE,	/* external format, type */
		pixels				/* pixels */
	);
	jal5_eogl_cerr(pjlc, 0,"texture image 2D");
//	free(pixels);
	if(pjlc->gl.textures[gid][id] == 0) {
		printf("bad texture:\n");
		jal5_eogl_cerr(pjlc, 0,"BADT");
		jlvm_dies(pjlc, Strt("Bad Texture, but no gl error? WHY!?"));
	}
}

//Lower Level Stuff
void _opn_eogl_colr(float r, float g, float b, float a) {
//	glColor4f(r,g,b,a);
//	jal5_eogl_cerr(pjlc, 0,"glColor4f");
}

void _opn_eogl_usep(jvct_t *pjlc, GLuint prg) {
	if(!prg) { jlvm_dies(pjlc, Strt("program ain't a prg!")); } 
	glUseProgram(prg);
	jal5_eogl_cerr(pjlc, 0,"glUseProgram");
}

void _opn_eogl_bind(jvct_t *pjlc, u16t g, u16t i) {
	glActiveTexture(GL_TEXTURE0);
	jal5_eogl_cerr(pjlc, 0,"glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[g][i]);
	jal5_eogl_cerr(pjlc, 0,"glBindTexture");
	glUniform1i(pjlc->gl.uniforms.textures[g][i], i);
	jal5_eogl_cerr(pjlc, 0,"glUniform1i");
}

//This sets vertex attribute "vertexAttrib" to "pointer".
//Set xyzw to 2 if 2 dimensional coordinates 3 if 3D. etc.
void jal5_eogl_setv(jvct_t *pjlc, uint32_t vertexAttrib, uint8_t xyzw) {
//	_jal5_eogl_buff_bind(0);
	jal5_eogl_cerr(pjlc, 0,"glBindBuffer");
	glEnableVertexAttribArray(vertexAttrib);
	jal5_eogl_cerr(pjlc, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,  	//attribute
		xyzw,			// x+y+z = 3
		GL_DOUBLE,		// type
		GL_FALSE,		// normalized?
		0,				// stride
		0				// array buffer offset
	);
	jal5_eogl_cerr(pjlc, 0,"glVertexAttribPointer");
}

void _opn_eogl_drra(jvct_t *pjlc, uint8_t count) {
	glDrawArrays(GL_TRIANGLE_FAN, 0, count);
	jal5_eogl_cerr(pjlc, 0,"glDrawArrays");
}

//HIGHER LEVEL

void eogl_vrtx(jvct_t *pjlc, u08t vertices, dect *xyzw) {
	glEnable( GL_BLEND );
	jal5_eogl_cerr(pjlc, 0,"glEnable( GL_BLEND )");
	glBlendColor(1.f,1.f,1.f,0.f);
	jal5_eogl_cerr(pjlc, 0,"glBlendColor");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	jal5_eogl_cerr(pjlc, 0,"glBlendFunc");

//	_opn_eogl_usep(pjlc, pjlc->gl.program);
	if(xyzw) {
		u16t i;
		for(i = 0; i < vertices*3; i+=3) {
			pjlc->gl.buff_vert[i] = (xyzw[i]*2.)-1.;
			pjlc->gl.buff_vert[i+1] = -(xyzw[i+1]*2.)+1.;
			pjlc->gl.buff_vert[i+2] = xyzw[i+2]*2.;
		}
		jal5_eogl_buff_data(pjlc, pjlc->gl.temp_buff_vrtx,
			pjlc->gl.buff_vert, vertices * 3);
	}else{
		GLdouble coord_default[] = {
			-1.,1.,0.,
			-1.,-1.,0.,
			1.,-1.,0.,
			1.,1.,0.
		};
		jal5_eogl_buff_data(pjlc, pjlc->gl.temp_buff_vrtx,
			coord_default, 12);
	}
	jal5_eogl_setv(pjlc, pjlc->gl.attributes.position, 3);
}

void eogl_txtr(jvct_t *pjlc, u08t map, u08t a, u16t pgid, u16t pi) {
	if(map) {
		int32_t cX = map%16;
		int32_t cY = map/16;
		double CX = (((double)cX)/16.);
		double CY = 1.-((double)cY)/16.;
		GLdouble tex1[] =
		{
			CX, CY - (1./16.),
			CX, CY,
			(1./16.)+CX, CY,
			(1./16.)+CX, CY - (1./16.)
		};
		jal5_eogl_buff_data(pjlc, pjlc->gl.temp_buff_txtr, tex1, 8);
	}else{
		GLdouble tex2[] =
		{	
			0. + (2./1024.), 0.,
			0. + (2./1024.), 1.,
			1., 1.,
			1., 0.
		};
		jal5_eogl_buff_data(pjlc, pjlc->gl.temp_buff_txtr, tex2, 8);
	}
	jal5_eogl_setv(pjlc, pjlc->gl.attributes.texpos, 2);
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[pgid][pi]);
}

//Draw object with "vertices" vertices.  The vertex data is in "x","y" and "z".
//"map" refers to the charecter map.  0 means don't zoom in to one charecter.
//Otherwise it will zoom in x16 to a single charecter
void eogl_draw(jvct_t *pjlc) {
	_opn_eogl_drra(pjlc, 4);
}

int32_t _opn_eogl_getu(jvct_t *pjlc, char *var) {
	return glGetUniformLocation(pjlc->gl.program, var);
	int32_t a;
	if((a = glGetUniformLocation(pjlc->gl.program, var)) == -1)
		jlvm_dies(pjlc,
			jl_me_strt_merg(
				Strt("opengl: bad name; is:"),
				Strt(var), STRT_TEMP)
		);
	jal5_eogl_cerr(pjlc, 0,"glGetUniformLocation");
	return a;
}

void jal5_eogl_geta(jvct_t *pjlc, s32t *attrib, const char *title) {
	if((*attrib
		= glGetAttribLocation(pjlc->gl.program, title)) == -1)
	{
		jlvm_dies(pjlc,
			Strt("attribute name is either reserved or non-existant"
			));
	}
}

//Load and create all resources
static inline void _jl_gl_make_res(jvct_t *pjlc) {
	//set up opengl
	//TODO:Later, Add Implementation with this enabled
	printf("setting properties...\n");
	glDisable( GL_DEPTH_TEST);
	jal5_eogl_cerr(pjlc, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable( GL_DITHER );
	jal5_eogl_cerr(pjlc, 0, "glDisable(GL_DITHER)");
	glEnable( GL_BLEND );
	jal5_eogl_cerr(pjlc, 0, "glEnable(GL_BLEND)");
	printf("set glproperties.\n");
	//Create the temporary buffers.
	printf("creating buffers....\n");
	_jal5_eogl_buff_make(pjlc, &pjlc->gl.temp_buff_vrtx); 
	_jal5_eogl_buff_make(pjlc, &pjlc->gl.temp_buff_txtr);
	printf("created buffers.\n");

	printf("making program....\n");
	pjlc->gl.program = createProgram(pjlc, source_vert_tex, source_frag_tex);
	if (pjlc->gl.program == 0)
		jlvm_dies(pjlc, Strt("Failed to load program"));
	printf("made program.\n");

	printf("setting up shaders....\n");
	if(pjlc->gl.uniforms.textures == NULL)
		jlvm_dies(pjlc, Strt("Couldn't create uniforms"));
	pjlc->gl.uniforms.textures[0][0] = _opn_eogl_getu(pjlc, "texture");
	printf("setting up shaders more....\n");
	jal5_eogl_geta(pjlc, &pjlc->gl.attributes.position, "position");
	jal5_eogl_geta(pjlc, &pjlc->gl.attributes.texpos, "texpos");
	printf("set up shaders.\n");
}

void _jl_gl_init(jvct_t *pjlc) {
#ifndef JLVM_USEL_GLES
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf(" [JLVM/GLEW] init\n");
	#endif
	if(glewInit()!=GLEW_OK) jlvm_dies(pjlc, Strt("glew fail!(no sticky)"));
#endif
	_jl_gl_make_res(pjlc);
	_opn_eogl_usep(pjlc, pjlc->gl.program);
}
