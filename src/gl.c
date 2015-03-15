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
	"	gl_FragColor = texture2D(texture, vec2(texcoord.x, texcoord.y));"
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

void jal5_eogl_cerr(jvct_t *pjct, int width, char *fname) {
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
	jlvm_dies(pjct, jl_me_strt_merg(
		jl_me_strt_merg(Strt(fname), Strt(fstrerr), STRT_TEMP),
		jl_me_strt_merg(Strt(":"), jl_me_strt_fnum(width), STRT_TEMP),
		STRT_TEMP));
}

void _jal5_eogl_buff_bind(jvct_t *pjct, uint32_t buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	jal5_eogl_cerr(pjct, 0,"bind buffer");
}

void jal5_eogl_buff_data(jvct_t *pjct, uint32_t buffer, void *buffer_data,
	u08t buffer_size)
{
	_jal5_eogl_buff_bind(pjct, buffer);
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(float), buffer_data,
		GL_STATIC_DRAW);
	jal5_eogl_cerr(pjct, 0,"buffer data");
}

void _jal5_eogl_buff_make(jvct_t *pjct, uint32_t *buffer)
{
	glGenBuffers(1, buffer);
	jal5_eogl_cerr(pjct, 0,"buffer gen");
}

GLuint loadShader(jvct_t *pjct, GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	jal5_eogl_cerr(pjct, 0,"couldn't create shader");
	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pSource, NULL);
		jal5_eogl_cerr(pjct, 0,"glShaderSource");
		glCompileShader(shader);
		jal5_eogl_cerr(pjct, 0,"glCompileShader");
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		jal5_eogl_cerr(pjct, 0,"glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			char* buf;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			jal5_eogl_cerr(pjct, 1,"glGetShaderiv");
			if (infoLen) {
				buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					jlvm_dies(pjct, jl_me_strt_merg(
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

GLuint createProgram(jvct_t *pjct, const char* pVertexSource,
	const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(pjct, GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		jlvm_dies(pjct, Strt("couldn't load vertex shader"));
	}

	GLuint pixelShader = loadShader(pjct, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		jlvm_dies(pjct, Strt("couldn't load fragment shader"));
	}

	GLuint program = glCreateProgram();
	jal5_eogl_cerr(pjct, 0,"glCreateProgram");
	if (program) {
		GLint linkStatus = GL_FALSE;

		glAttachShader(program, vertexShader);
		jal5_eogl_cerr(pjct, 0,"glAttachShader (vertex)");
		glAttachShader(program, pixelShader);
		jal5_eogl_cerr(pjct, 0,"glAttachShader (fragment)");
		glLinkProgram(program);
		jal5_eogl_cerr(pjct, 0,"glLinkProgram");
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		jal5_eogl_cerr(pjct, 0,"glGetProgramiv");
		glValidateProgram(program);
		jal5_eogl_cerr(pjct, 1,"glValidateProgram");
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			char* buf;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			jal5_eogl_cerr(pjct, 1,"glGetProgramiv");
			if (bufLength) {
				buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					jlvm_dies(pjct, jl_me_strt_merg(
						Strt("Could not link program:\n"),
						Strt(buf), STRT_TEMP));
					free(buf);
				}else{
					jlvm_dies(pjct, Strt("failed malloc"));
				}
			}else{
				glDeleteProgram(program);
				jlvm_dies(pjct, Strt("no info log"));
			}
		}
	}
	return program;
}

void eogl_make_txtr(jvct_t *pjct, uint16_t gid, uint16_t id, void *pixels,
	int width, int height)
{
	if (!pixels)
		jlvm_dies(pjct, Strt("null pixels"));
	if (pjct->gl.allocatedg < gid + 1) {
		pjct->gl.textures =
			realloc(pjct->gl.textures, sizeof(GLuint *) * (gid+1));
		pjct->gl.uniforms.textures =
			realloc(pjct->gl.uniforms.textures,
				sizeof(GLint *) * (gid+1));
		pjct->gl.allocatedg = gid + 1;
		pjct->gl.allocatedi = 0;
		pjct->gl.textures[gid] = NULL;
		pjct->gl.uniforms.textures[gid] = NULL;
	}
	if (pjct->gl.allocatedi < id + 1) {
		pjct->gl.textures[gid] =
			realloc(pjct->gl.textures[gid],
				sizeof(GLuint) * (id+1));
		pjct->gl.uniforms.textures[gid] =
			realloc(pjct->gl.uniforms.textures[gid],
				sizeof(GLint) * (id+1));
		pjct->gl.allocatedi = id + 1;
	}
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("generating texture...");
	#endif
	glGenTextures(1, &pjct->gl.textures[gid][id]);
	jal5_eogl_cerr(pjct, 0,"gen textures");
	glBindTexture(GL_TEXTURE_2D, pjct->gl.textures[gid][id]);
	jal5_eogl_cerr(pjct, 0,"bind textures");
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf("settings pars...");
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	jal5_eogl_cerr(pjct, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	jal5_eogl_cerr(pjct, 1,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	jal5_eogl_cerr(pjct, 2,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	jal5_eogl_cerr(pjct, 3,"glTexParameteri");
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
	jal5_eogl_cerr(pjct, 0,"texture image 2D");
//	free(pixels);
	if(pjct->gl.textures[gid][id] == 0) {
		printf("bad texture:\n");
		jal5_eogl_cerr(pjct, 0,"BADT");
		jlvm_dies(pjct, Strt("Bad Texture, but no gl error? WHY!?"));
	}
}

//Lower Level Stuff
void _opn_eogl_colr(float r, float g, float b, float a) {
//	glColor4f(r,g,b,a);
//	jal5_eogl_cerr(pjct, 0,"glColor4f");
}

void _opn_eogl_usep(jvct_t *pjct, GLuint prg) {
	if(!prg) { jlvm_dies(pjct, Strt("program ain't a prg!")); } 
	glUseProgram(prg);
	jal5_eogl_cerr(pjct, 0,"glUseProgram");
}

void _opn_eogl_bind(jvct_t *pjct, u16t g, u16t i) {
	glActiveTexture(GL_TEXTURE0);
	jal5_eogl_cerr(pjct, 0,"glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, pjct->gl.textures[g][i]);
	jal5_eogl_cerr(pjct, 0,"glBindTexture");
	glUniform1i(pjct->gl.uniforms.textures[g][i], i);
	jal5_eogl_cerr(pjct, 0,"glUniform1i");
}

//This sets vertex attribute "vertexAttrib" to "pointer".
//Set xyzw to 2 if 2 dimensional coordinates 3 if 3D. etc.
void jal5_eogl_setv(jvct_t *pjct, uint32_t vertexAttrib, uint8_t xyzw) {
//	_jal5_eogl_buff_bind(0);
	jal5_eogl_cerr(pjct, 0,"glBindBuffer");
	glEnableVertexAttribArray(vertexAttrib);
	jal5_eogl_cerr(pjct, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,  	//attribute
		xyzw,			// x+y+z = 3
		GL_FLOAT,		// type
		GL_FALSE,		// normalized?
		0,				// stride
		0				// array buffer offset
	);
	jal5_eogl_cerr(pjct, 0,"glVertexAttribPointer");
}

void _opn_eogl_drra(jvct_t *pjct, uint8_t count) {
	glDrawArrays(GL_TRIANGLE_FAN, 0, count);
	jal5_eogl_cerr(pjct, 0,"glDrawArrays");
}

//HIGHER LEVEL

void eogl_vrtx(jvct_t *pjct, u08t vertices, dect *xyzw) {
	glEnable( GL_BLEND );
	jal5_eogl_cerr(pjct, 0,"glEnable( GL_BLEND )");
	glBlendColor(1.f,1.f,1.f,0.f);
	jal5_eogl_cerr(pjct, 0,"glBlendColor");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	jal5_eogl_cerr(pjct, 0,"glBlendFunc");

//	_opn_eogl_usep(pjct, pjct->gl.program);
	if(xyzw) {
		u16t i;
		for(i = 0; i < vertices*3; i+=3) {
			pjct->gl.buff_vert[i] = (xyzw[i]*2.f)-1.f;
			pjct->gl.buff_vert[i+1] = -(xyzw[i+1]*2.f)+1.f;
			pjct->gl.buff_vert[i+2] = xyzw[i+2]*2.f;
		}
		jal5_eogl_buff_data(pjct, pjct->gl.temp_buff_vrtx,
			pjct->gl.buff_vert, vertices * 3);
	}else{
		GLfloat coord_default[] = {
			-1.f,1.f,0.f,
			-1.f,-1.f,0.f,
			1.f,-1.f,0.f,
			1.f,1.f,0.f
		};
		jal5_eogl_buff_data(pjct, pjct->gl.temp_buff_vrtx,
			coord_default, 12);
	}
	jal5_eogl_setv(pjct, pjct->gl.attributes.position, 3);
}

void eogl_txtr(jvct_t *pjct, u08t map, u08t a, u16t pgid, u16t pi) {
	if(map) {
		int32_t cX = map%16;
		int32_t cY = map/16;
		float CX = (((double)cX)/16.) - .005;
		float CY = 1.001-((double)cY)/16.;
		GLfloat tex1[] =
		{
			CX, CY - (1./16.),
			CX, CY,
			(1./16.)+CX, CY,
			(1./16.)+CX, CY - (1./16.)
		};
		jal5_eogl_buff_data(pjct, pjct->gl.temp_buff_txtr, tex1, 8);
	}else{
		GLfloat tex2[] =
		{	
			0.f, 0.f,
			0.f, 1.f,
			1.001f, 1.f,
			1.001f, 0.f
		};
		jal5_eogl_buff_data(pjct, pjct->gl.temp_buff_txtr, tex2, 8);
	}
	jal5_eogl_setv(pjct, pjct->gl.attributes.texpos, 2);
	glBindTexture(GL_TEXTURE_2D, pjct->gl.textures[pgid][pi]);
}

//Draw object with "vertices" vertices.  The vertex data is in "x","y" and "z".
//"map" refers to the charecter map.  0 means don't zoom in to one charecter.
//Otherwise it will zoom in x16 to a single charecter
void eogl_draw(jvct_t *pjct) {
	_opn_eogl_drra(pjct, 4);
}

int32_t _opn_eogl_getu(jvct_t *pjct, char *var) {
	return glGetUniformLocation(pjct->gl.program, var);
	int32_t a;
	if((a = glGetUniformLocation(pjct->gl.program, var)) == -1)
		jlvm_dies(pjct,
			jl_me_strt_merg(
				Strt("opengl: bad name; is:"),
				Strt(var), STRT_TEMP)
		);
	jal5_eogl_cerr(pjct, 0,"glGetUniformLocation");
	return a;
}

void jal5_eogl_geta(jvct_t *pjct, s32t *attrib, const char *title) {
	if((*attrib
		= glGetAttribLocation(pjct->gl.program, title)) == -1)
	{
		jlvm_dies(pjct,
			Strt("attribute name is either reserved or non-existant"
			));
	}
}

/*
 * Load and create all of our resources:
 */
static void make_resources(jvct_t *pjct) {
	//set up opengl
	glDisable( GL_DEPTH_TEST); //TODO:Later, Add Implementation with this enabled
	jal5_eogl_cerr(pjct, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable( GL_DITHER );
	jal5_eogl_cerr(pjct, 0, "glDisable(GL_DITHER)");
	glEnable( GL_BLEND );
	jal5_eogl_cerr(pjct, 0, "glEnable(GL_BLEND)");

	//Create the temporary buffers.
	_jal5_eogl_buff_make(pjct, &pjct->gl.temp_buff_vrtx); 
	_jal5_eogl_buff_make(pjct, &pjct->gl.temp_buff_txtr);

	if (pjct->gl.textures[0] == 0/* || pjct->gl.textures[1] == 0*/)
		jlvm_dies(pjct, Strt("Failed to load txtures"));

	pjct->gl.program = createProgram(pjct, source_vert_tex, source_frag_tex);
	if (pjct->gl.program == 0)
		jlvm_dies(pjct, Strt("Failed to load program"));

	pjct->gl.uniforms.textures[0][0] = _opn_eogl_getu(pjct, "texture");
	jal5_eogl_geta(pjct, &pjct->gl.attributes.position, "position");
	jal5_eogl_geta(pjct, &pjct->gl.attributes.texpos, "texpos");
}

void _jal5_eogl_init(jvct_t *pjct) {
#ifndef JLVM_USEL_GLES
	#if JLVM_DEBUG >= JLVM_DEBUG_SIMPLE
	printf(" [JLVM/GLEW] init\n");
	#endif
	if(glewInit()!=GLEW_OK) jlvm_dies(pjct, Strt("glew fail!(no sticky)"));
#endif
	make_resources(pjct);
	_opn_eogl_usep(pjct, pjct->gl.program);
}
