#include "header/jl_pr.h"
#include "header/jl_opengl.h"

// Shader Code

#ifdef GL_ES_VERSION_2_0
	#define GLSL_HEAD "#version 100\nprecision highp float;\n"
#else
	#define GLSL_HEAD "#version 100\n"
#endif

static char *source_frag_clr = 
	GLSL_HEAD
	"varying vec4 vcolor;\n"
	"\n"
	"void main() {\n"
	"	gl_FragColor = vcolor;\n"
	"}\n\0";
	
static char *source_vert_clr = 
	GLSL_HEAD
	"uniform vec3 translate;\n"
	"uniform vec4 transform;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec4 acolor;\n"
	"\n"
	"varying vec4 vcolor;\n"
	"\n"
	"void main() {\n"
	"	gl_Position = transform * vec4(position + translate, 1.0);\n"
	"	vcolor = acolor;\n"
	"}\n\0";

static char *source_frag_tex = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"uniform float multiply_alpha;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = "
	"		texture2D(texture, texcoord)"
	"		* vec4(1., 1., 1., multiply_alpha);"
//	"	gl_FragColor = mix(\n"
//	"		texture2D(textures[0], texcoord),\n"
//	"		texture2D(textures[1], texcoord),\n"
//	"		fade_factor\n"
//	"	);\n"
//	"	gl_FragColor = texture2D(texture, vec2(texcoord.x, texcoord.y));"

//	"	gl_FragColor = vec4(texcoord.x, texcoord.y, 1.0, .50);"
//	"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
	"}\n\0";

static char *source_vert_tex = 
	GLSL_HEAD
	"uniform vec3 translate;\n"
	"uniform vec4 transform;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec2 texpos;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	texcoord = texpos;\n"
	"	gl_Position = transform * vec4(position + translate, 1.0);\n"
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
	
// Full texture
static const float DEFAULT_TC[] = {
	0., 0.,
	0., 1.,
	1., 1.,
	1., 0.
};

#if JL_GLRTEX == JL_GLRTEX_EGL
	static EGLConfig eglConf;
	static EGLContext eglCtx;
	static EGLDisplay eglDisp;
#endif

// Prototypes:
#if JL_GLRTEX == JL_GLRTEX_EGL
#elif JL_GLRTEX == JL_GLRTEX_SDL
void jl_dl_screen_(jvct_t* _jlc, jl_window_t* which);
jl_window_t* jl_dl_screen_new_(jvct_t* _jlc, u16_t w, u16_t h);
#else
static void jl_gl_depthbuffer_set__(jvct_t *_jlc, u16_t w, u16_t h);
static void _jl_gl_depthbuffer_bind(jvct_t *_jlc, uint32_t db);
static void _jl_gl_depthbuffer_make(jvct_t *_jlc, uint32_t *db);
static void jl_gl_depthbuffer_off__(jvct_t *_jlc);
static void jl_gl_framebuffer_addtx__(jvct_t *_jlc, u32_t tx);
static void jl_gl_framebuffer_adddb__(jvct_t *_jlc, u32_t db);
static void jl_gl_framebuffer_status__(jvct_t* _jlc);
#endif
static void _jl_gl_viewport(jvct_t *_jlc, uint16_t w, uint16_t h);

// Definitions:
#ifdef JL_DEBUG_LIB
	#define JL_GL_ERROR(_jlc, x, fname) jl_gl_get_error___(_jlc, x, fname)
	#define JL_EGL_ERROR(_jlc, x, fname) jl_gl_egl_geterror__(_jlc, x, fname)
#else
	#define JL_GL_ERROR(_jlc, x, fname)
	#define JL_EGL_ERROR(_jlc, x, fname)
#endif

// Functions:

#ifdef JL_DEBUG_LIB
	static void jl_gl_get_error___(jvct_t *_jlc, int width, char *fname) {
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
			fstrerr = "opengl: out of memory ): "
				"!! (Texture too big?)\n";
		}else{
			fstrerr = "opengl: unknown error!\n";
		}
		_jl_fl_errf(_jlc, ":");
		_jl_fl_errf(_jlc, fname);
		_jl_fl_errf(_jlc, "\n:");
		_jl_fl_errf(_jlc, fstrerr);
		_jl_fl_errf(_jlc, ":\n:");
		_jl_fl_errf(_jlc, (void*)jl_me_strt_fnum(width)->data);
		_jl_fl_errf(_jlc, "\n");
		jl_sg_kill(_jlc->jlc);
	}
#endif

static void jl_gl_buffer_use__(jvct_t *_jlc, uint32_t buffer) {
	// Check For Deleted Buffer
	if(buffer == 0) {
		_jl_fl_errf(_jlc, ":buffer got deleted!\n");
		jl_sg_kill(_jlc->jlc);
	}
	// bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	JL_GL_ERROR(_jlc, 0,"bind buffer");
}

// Set the Data for VBO "buffer" to "buffer_data" with "buffer_size"
static void jl_gl_buffer_set__(jvct_t *_jlc, uint32_t buffer,
	const void *buffer_data, u16_t buffer_size)
{
	//Bind Buffer "buffer"
	jl_gl_buffer_use__(_jlc, buffer);
	//Set the data
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(float), buffer_data,
		GL_DYNAMIC_DRAW);
	JL_GL_ERROR(_jlc, buffer_size, "buffer data");
}

static void jl_gl_buffer_new__(jvct_t *_jlc, uint32_t *buffer) {
	glGenBuffers(1, buffer);
	JL_GL_ERROR(_jlc, 0,"buffer gen");
	if(*buffer == 0) {
		_jl_fl_errf(_jlc, ":buffer is made wrongly!\n");
		jl_sg_kill(_jlc->jlc);
	}
}

static void jl_gl_buffer_old__(jvct_t *_jlc, uint32_t *buffer) {
	glDeleteBuffers(1, buffer);
	JL_GL_ERROR(_jlc, 0,"buffer free");
	*buffer = 0;
}

GLuint jl_gl_load_shader(jvct_t *_jlc, GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	JL_GL_ERROR(_jlc, 0,"couldn't create shader");
	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pSource, NULL);
		JL_GL_ERROR(_jlc, 0,"glShaderSource");
		glCompileShader(shader);
		JL_GL_ERROR(_jlc, 0,"glCompileShader");
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		JL_GL_ERROR(_jlc, 0,"glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			char* buf;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			JL_GL_ERROR(_jlc, 1,"glGetShaderiv");
			if (infoLen) {
				buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					_jl_fl_errf(_jlc, ":Could not compile shader:\n:");
					_jl_fl_errf(_jlc, buf);
					free(buf);
					_jl_fl_errf(_jlc, "\n");
					jl_sg_kill(_jlc->jlc);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint jl_gl_glsl_prg_create(jvct_t *_jlc, const char* pVertexSource,
	const char* pFragmentSource)
{
	GLuint vertexShader = jl_gl_load_shader(_jlc, GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		_jl_fl_errf(_jlc, ":couldn't load vertex shader\n");
		jl_sg_kill(_jlc->jlc);
	}

	GLuint pixelShader = jl_gl_load_shader(_jlc, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		_jl_fl_errf(_jlc, ":couldn't load fragment shader\n");
		jl_sg_kill(_jlc->jlc);
	}

	GLuint program = glCreateProgram();
	JL_GL_ERROR(_jlc, 0,"glCreateProgram");
	if (program) {
		GLint linkStatus = GL_FALSE;

		glAttachShader(program, vertexShader);
		JL_GL_ERROR(_jlc, 0,"glAttachShader (vertex)");
		glAttachShader(program, pixelShader);
		JL_GL_ERROR(_jlc, 0,"glAttachShader (fragment)");
		glLinkProgram(program);
		JL_GL_ERROR(_jlc, 0,"glLinkProgram");
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		JL_GL_ERROR(_jlc, 0,"glGetProgramiv");
		glValidateProgram(program);
		JL_GL_ERROR(_jlc, 1,"glValidateProgram");
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			char* buf;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			JL_GL_ERROR(_jlc, 1,"glGetProgramiv");
			if (bufLength) {
				buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					_jl_fl_errf(_jlc, "Could not link program:\n");
					_jl_fl_errf(_jlc, buf);
					_jl_fl_errf(_jlc, "\n");
					free(buf);
					jl_sg_kill(_jlc->jlc);
				}else{
					_jl_fl_errf(_jlc, ":failed malloc\n");
					jl_sg_kill(_jlc->jlc);
				}
			}else{
				glDeleteProgram(program);
				_jl_fl_errf(_jlc, ":no info log\n");
				jl_sg_kill(_jlc->jlc);
			}
		}
	}
	if (program == 0) {
		_jl_fl_errf(_jlc, "Failed to load program\n");
		jl_sg_kill(_jlc->jlc);
	}
	return program;
}

static void jl_gl_texture_make__(jvct_t* _jlc, uint32_t *tex) {
	glGenTextures(1, tex);
	if(!(*tex)) {
		JL_GL_ERROR(_jlc, 0, "jl_gl_texture_make__: glGenTextures");
		_jl_fl_errf(_jlc, "jl_gl_texture_make__: GL tex = 0");
		jl_sg_kill(_jlc->jlc);
	}
	JL_GL_ERROR(_jlc, 0, "jl_gl_texture_make__: glGenTextures");
}

// Set the bound texture.  pm is the pixels 0 - blank texture.
static void jl_gl_texture_set__(jvct_t* _jlc, u8_t* pm, u16_t w, u16_t h) {
	glTexImage2D(
		GL_TEXTURE_2D, 0,		/* target, level */
		GL_RGBA,			/* internal format */
		w, h, 0,			/* width, height, border */
		GL_RGBA, GL_UNSIGNED_BYTE,	/* external format, type */
		pm				/* pixels */
	);
	JL_GL_ERROR(_jlc, 0,"texture image 2D");
}

static void jl_gl_texpar_set__(jvct_t* _jlc) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	JL_GL_ERROR(_jlc, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	JL_GL_ERROR(_jlc, 1,"glTexParameteri");
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	JL_GL_ERROR(_jlc, 2,"glTexParameteri");
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	JL_GL_ERROR(_jlc, 3,"glTexParameteri");
}

static void jl_gl_texture__bind__(jvct_t *_jlc, uint32_t tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
	JL_GL_ERROR(_jlc, tex,"jl_gl_texture_bind__: glBindTexture");
}

// Bind a texture.
static void jl_gl_texture_bind__(jvct_t *_jlc, uint32_t tex) {
	if(tex == 0) {
		_jl_fl_errf(_jlc, "jl_gl_texture_bind__: GL tex = 0");
		jl_sg_kill(_jlc->jlc);
	}
	jl_gl_texture__bind__(_jlc, tex);
}

// Unbind a texture
static void jl_gl_texture_off__(jvct_t *_jlc) {
	jl_gl_texture__bind__(_jlc, 0);
}

// Make & Bind a new texture.
static void jl_gl_texture_new__(jvct_t *_jlc, m_u32_t *tex, u8_t* px,
	u16_t w, u16_t h)
{
	// Make the texture
	jl_gl_texture_make__(_jlc, tex);
	// Bind the texture
	jl_gl_texture_bind__(_jlc, *tex);
	// Set texture
	jl_gl_texture_set__(_jlc, px, w, h);
	// Set the texture parametrs.
	jl_gl_texpar_set__(_jlc);
}

#if JL_GLRTEX == JL_GLRTEX_EGL
#elif JL_GLRTEX == JL_GLRTEX_SDL
#else
// Make & Bind a new depth buffer.
static void jl_gl_depthbuffer_new__(jvct_t* _jlc,m_u32_t*db ,u16_t w,u16_t h) {
	// Make the depth buffer.
	_jl_gl_depthbuffer_make(_jlc, db);
	// Bind the depth buffer
	_jl_gl_depthbuffer_bind(_jlc, *db);
	// Set the depth buffer
	jl_gl_depthbuffer_set__(_jlc, w, h);
}
#endif

// Make a texture - doesn't free "pixels"
void jl_gl_maketexture(jl_t* jlc, uint16_t gid, uint16_t id,
	void* pixels, int width, int height)
{
	jvct_t *_jlc = jlc->_jlc;

	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "MKTX");
	if (!pixels) {
		_jl_fl_errf(_jlc, "null pixels\n");
		jl_sg_kill(jlc);
	}
	if (_jlc->gl.allocatedg < gid + 1) {
		_jlc->gl.textures =
			realloc(_jlc->gl.textures,
				sizeof(uint32_t *) * (gid+1));
		_jlc->gl.tex.uniforms.textures =
			realloc(_jlc->gl.tex.uniforms.textures,
				sizeof(GLint *) * (gid+1));
		_jlc->gl.allocatedg = gid + 1;
		_jlc->gl.allocatedi = 0;
		_jlc->gl.textures[gid] = NULL;
		_jlc->gl.tex.uniforms.textures[gid] = NULL;
	}
	if (_jlc->gl.allocatedi < id + 1) {
		_jlc->gl.textures[gid] =
			realloc(_jlc->gl.textures[gid],
				sizeof(uint32_t) * (id+1));
		_jlc->gl.tex.uniforms.textures[gid] =
			realloc(_jlc->gl.tex.uniforms.textures[gid],
				sizeof(GLint) * (id+1));
		_jlc->gl.allocatedi = id + 1;
	}
	jl_io_printc(_jlc->jlc, "generating texture (");
	jl_io_printi(_jlc->jlc, width);
	jl_io_printc(_jlc->jlc, ",");
	jl_io_printi(_jlc->jlc, height);
	jl_io_printc(_jlc->jlc, ")\n");
	// Make the texture.
	jl_gl_texture_new__(_jlc, &_jlc->gl.textures[gid][id], pixels, width,
		height);
	jl_io_close_block(_jlc->jlc); //Close Block "MKTX"
}

//Lower Level Stuff
static void _jl_gl_usep(jvct_t *_jlc, GLuint prg) {
	if(!prg) {
		_jl_fl_errf(_jlc, ":program ain't a prg!\n");
		jl_sg_kill(_jlc->jlc);
	}
	glUseProgram(prg);
	JL_GL_ERROR(_jlc, prg, "glUseProgram");
}

static void _jl_gl_setalpha(jvct_t *_jlc, float a) {
	glUniform1f(_jlc->gl.tex.uniforms.multiply_alpha, a);
	JL_GL_ERROR(_jlc, 0,"glUniform1f");
}

static void jl_gl_uniform3f__(jvct_t *_jlc, GLint uv, float x, float y, float z)
{
	glUniform3f(uv, x, y, z);
	JL_GL_ERROR(_jlc, 0,"glUniform3f");
}

static void jl_gl_uniform4f__(jvct_t *_jlc, GLint uv, float x, float y, float z,
	float w)
{
	glUniform4f(uv, x, y, z, w);
	JL_GL_ERROR(_jlc, 0,"glUniform4f");
}

//This pushes VBO "buff" up to the shader's vertex attribute "vertexAttrib"
//Set xyzw to 2 if 2D coordinates 3 if 3D. etc.
void _jl_gl_setv(jvct_t *_jlc, uint32_t buff, uint32_t vertexAttrib, uint8_t xyzw) {
	// Bind Buffer
	jl_gl_buffer_use__(_jlc, buff);
	// Set Vertex Attrib Pointer
	glEnableVertexAttribArray(vertexAttrib);
	JL_GL_ERROR(_jlc, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,	// attribute
		xyzw,		// x+y+z = 3
		GL_FLOAT,	// type
		GL_FALSE,	// normalized?
		0,		// stride
		0		// array buffer offset
	);
	JL_GL_ERROR(_jlc, 0,"glVertexAttribPointer");
}

static void _jl_gl_draw_arrays(jvct_t *_jlc, GLenum mode, uint8_t count) {
	glDrawArrays(mode, 0, count);
	JL_GL_ERROR(_jlc, 0,"glDrawArrays");
}

static inline void _jl_gl_init_disable_extras(jvct_t *_jlc) {
	glDisable(GL_DEPTH_TEST);
	JL_GL_ERROR(_jlc, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable(GL_DITHER);
	JL_GL_ERROR(_jlc, 0, "glDisable(GL_DITHER)");
}

static inline void _jl_gl_init_enable_alpha(jvct_t *_jlc) {
	glEnable(GL_BLEND);
	JL_GL_ERROR(_jlc, 0,"glEnable( GL_BLEND )");
//	glEnable(GL_CULL_FACE);
//	JL_GL_ERROR(_jlc, 0,"glEnable( GL_CULL_FACE )");
	glBlendColor(1.f,1.f,1.f,0.f);
	JL_GL_ERROR(_jlc, 0,"glBlendColor");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	JL_GL_ERROR(_jlc, 0,"glBlendFunc");
}

// Copy & Push vertices to a VBO.
static void jl_gl_vertices__(jvct_t *_jlc, const float *xyzw, uint8_t vertices,
	float* cv, u32_t gl)
{
	u16_t items = (vertices*3);

	// Copy Vertices
	jl_me_copyto(xyzw, cv, items * sizeof(float));
	// Copy Buffer Data "cv" to Buffer "gl"
	jl_gl_buffer_set__(_jlc, gl, cv, items);
}

void jl_gl_vo_vertices(jvct_t *_jlc, jl_vo* pv, const float *xyzw,
	uint8_t vertices)
{
	pv->vc = vertices;
	if(vertices) {
		// Free pv->cv if non-null
		if(pv->cv) jl_me_alloc(_jlc->jlc, (void**)&pv->cv, 0, 0);
		// Allocate pv->cv
		jl_me_alloc(_jlc->jlc,
			(void**)&pv->cv, vertices * sizeof(float) * 3, 0);
		// Set pv->cv & pv->gl
		jl_gl_vertices__(_jlc, xyzw, vertices, pv->cv, pv->gl);
	}
}

void jl_gl_vo_free(jvct_t *_jlc, jl_vo *pv) {
	// Free GL VBO
	jl_gl_buffer_old__(_jlc, &pv->gl);
	// Free GL Texture Buffer
	jl_gl_buffer_old__(_jlc, &pv->bt);
	// Free Converted Vertices & Colors
	if(pv->cv) jl_me_alloc(_jlc->jlc, (void**)&pv->cv, 0,0);
	if(pv->cc) jl_me_alloc(_jlc->jlc, (void**)&pv->cc, 0,0);
	// Free main structure
	jl_me_alloc(_jlc->jlc, (void**)&pv, 0,0);
}

static void _jl_gl_updatevectors(jvct_t *_jlc, uint32_t gl){
	_jl_gl_setv(_jlc, gl,
		(_jlc->gl.whichprg == JL_GL_SLPR_TEX) ?
		_jlc->gl.tex.attr.position : _jlc->gl.clr.attr.position,
		3);
}

static void _jl_gl_setp(jvct_t *_jlc, jl_gl_slpr id) {
	if(_jlc->gl.whichprg != id) {
		_jlc->gl.whichprg = id;
		_jl_gl_usep(_jlc, _jlc->gl.prgs[id]);
	}
}

static void _jl_gl_col_begin(jvct_t *_jlc, jl_vo* pv) {
	//Free anything old
	if(pv->cc != NULL) jl_me_alloc(_jlc->jlc, (void**)&pv->cc, 0, 0);
}

#if JL_GLRTEX == JL_GLRTEX_EGL

#elif JL_GLRTEX == JL_GLRTEX_SDL

static void jl_gl_pbo_tex__(jvct_t *_jlc) {
	if(_jlc->gl.cp) {
		// Bind Texture &
		jl_gl_texture_bind__(_jlc, _jlc->gl.cp->tx);
		// Copy to texture.
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
			_jlc->gl.cp->w, _jlc->gl.cp->h,
			GL_RGBA, GL_UNSIGNED_BYTE, _jlc->gl.cp->px);
		JL_GL_ERROR(_jlc, 0, "jl_gl_pbo_tex__: glTexSubImage2D");
//		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0,
//			_jlc->gl.cp->w, _jlc->gl.cp->h, 0);
//		JL_GL_ERROR(_jlc, 0, "jl_gl_pbo_tex__: glCopyTexImage2D");
	}
}

static void jl_gl_pbo_pix__(jvct_t *_jlc) {
	if(_jlc->gl.cp) {
		glReadPixels(0, 0, _jlc->gl.cp->w, _jlc->gl.cp->h, GL_RGBA,
			GL_UNSIGNED_BYTE, _jlc->gl.cp->px);
	}
}

static void jl_gl_pbo_off__(jvct_t *_jlc) {
	// Read pixels from previous pre-renderer if enabled.
	jl_gl_pbo_pix__(_jlc);
	// Select shown screen & context.
	jl_dl_screen_(_jlc, _jlc->dl.displayWindow);
	// Write to texture if There is a current pre-renderer enabled.
	jl_gl_pbo_tex__(_jlc);
	// Set the viewport.
	jl_gl_viewport_screen(_jlc);
}

static void jl_gl_pbo_use__(jvct_t *_jlc, jl_pr_t *pr) {
	// Read pixels from previous pre-renderer if enabled.
	jl_gl_pbo_pix__(_jlc);
	// Select shown screen & context.
	jl_dl_screen_(_jlc, _jlc->dl.displayWindow);
	// Write to texture if There is a current pre-renderer enabled.
	jl_gl_pbo_tex__(_jlc);
	// Select pr's hidden screen.
	jl_dl_screen_(_jlc, pr->sw);
	// Set the viewport.
	_jl_gl_viewport(_jlc, pr->w, pr->h);
}

#else
static void _jl_gl_framebuffer_free(jvct_t *_jlc, uint32_t *fb) {
	glDeleteFramebuffers(1, fb);
	JL_GL_ERROR(_jlc, *fb, "glDeleteFramebuffers");
	*fb = 0;
}

static void jl_gl_framebuffer_make__(jvct_t *_jlc, m_u32_t *fb) {
	glGenFramebuffers(1, fb);
	if(!(*fb)) {
		_jl_fl_errf(_jlc, "jl_gl_framebuffer_make__: GL FB = 0\n");
		jl_sg_kill(_jlc->jlc);
	}
	JL_GL_ERROR(_jlc, *fb,"glGenFramebuffers");
}

static void jl_gl_framebuffer_use__(jvct_t *_jlc, u32_t fb, u32_t db, u32_t tx,
	u16_t w, u16_t h)
{
	if(fb == 0) {
		_jl_fl_errf(_jlc, "jl_gl_framebuffer_use__: GL FB = 0");
		jl_sg_kill(_jlc->jlc);
	}
	// Bind the texture.
	jl_gl_texture_bind__(_jlc, tx);
	// Bind the depthbuffer.
	_jl_gl_depthbuffer_bind(_jlc, db);
	// Bind the framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	JL_GL_ERROR(_jlc, fb,"glBindFramebuffer");
	// Render on the whole framebuffer [ lower left -> upper right ]
	_jl_gl_viewport(_jlc, w, h);
}

static void jl_gl_framebuffer_off__(jvct_t *_jlc) {
	// Unbind the texture.
	jl_gl_texture_off__(_jlc);
	// Unbind the depthbuffer.
	jl_gl_depthbuffer_off__(_jlc);
	// Unbind the framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	JL_GL_ERROR(_jlc, 0,"jl_gl_framebuffer_off__: glBindFramebuffer");
	// Render to the whole screen.
	jl_gl_viewport_screen(_jlc);
}

// add a texture to a framebuffer object.
static void jl_gl_framebuffer_addtx__(jvct_t *_jlc, u32_t tx) {
	// Set "*tex" as color attachment #0.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, tx, 0);
	JL_GL_ERROR(_jlc, tx,"jl_gl_framebuffer_addtx: glFramebufferTexture2D");
}

// add a depthbuffer to a framebuffer object.
static void jl_gl_framebuffer_adddb__(jvct_t *_jlc, u32_t db) {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, db);
	JL_GL_ERROR(_jlc, db,"make pr: glFramebufferRenderbuffer");
}

static void jl_gl_framebuffer_status__(jvct_t* _jlc) {
	// Check to see if framebuffer was made properly.
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		_jl_fl_errf(_jlc, "Frame buffer not complete!\n");
		jl_sg_kill(_jlc->jlc);
	}
}

// Create a new framebuffer with "tx" texture and "db" depthbuffer.
static inline void jl_gl_framebuffer_new__(jvct_t* _jlc, m_u32_t* fb, u32_t tx,
	u32_t db, u16_t w, u16_t h)
{
	jl_gl_framebuffer_make__(_jlc, fb);
	jl_gl_framebuffer_use__(_jlc, *fb, db, tx, w, h);
	// Attach depth and texture buffer.
	jl_gl_framebuffer_addtx__(_jlc, tx);
	jl_gl_framebuffer_adddb__(_jlc, db);
	jl_gl_framebuffer_status__(_jlc);
}
#endif

static void _jl_gl_texture_free(jvct_t *_jlc, uint32_t *tex) {
	glDeleteTextures(1, tex);
	JL_GL_ERROR(_jlc, 0, "_jl_gl_texture_free: glDeleteTextures");
	*tex = 0;
}

#if JL_GLRTEX == JL_GLRTEX_EGL
#ifdef JL_DEBUG_LIB
static void jl_gl_egl_geterror__(jvct_t *_jlc, i32_t x, str_t fname) {
	EGLint err = eglGetError();
	
	if(err == EGL_SUCCESS) return;
	
	_jl_fl_errf(_jlc, ":source: paraphrased from www.khronos.org: \n:\"");
	_jl_fl_errf(_jlc, fname);
	_jl_fl_errf(_jlc, "\"\n:");

	if(err == EGL_NOT_INITIALIZED)
		_jl_fl_errf(_jlc, "EGL couldn't initialize or isn't init'd");
	else if(err == EGL_BAD_ACCESS)
		_jl_fl_errf(_jlc, "EGL can't access a resource. (Threads?)");
	else if(err == EGL_BAD_ALLOC)
		_jl_fl_errf(_jlc, "EGL failed to allocate resources.");
	else if(err == EGL_BAD_ATTRIBUTE)
		_jl_fl_errf(_jlc, "Attribute not recognized (value or/ list).");
	else if(err == EGL_BAD_CONFIG)
		_jl_fl_errf(_jlc, "EGLConfig argument is invalid.");
	else if(err == EGL_BAD_CONTEXT)
		_jl_fl_errf(_jlc, "EGLContext argument is invalid.");
	else if(err == EGL_BAD_CURRENT_SURFACE)
		_jl_fl_errf(_jlc, "Surface passed is invalid.");
	else if(err == EGL_BAD_DISPLAY)
		_jl_fl_errf(_jlc, "EGLDisplay argument is invalid.");
	else if(err == EGL_BAD_MATCH)
		_jl_fl_errf(_jlc, "EGLContext requires buffers not alloc'd.");
	else if(err == EGL_BAD_NATIVE_PIXMAP)
		_jl_fl_errf(_jlc, "EGLNativePixmapType argument is invalid.");
	else if(err == EGL_BAD_NATIVE_WINDOW)
		_jl_fl_errf(_jlc, "EGLNativeWindowType argument is invalid.");
	else if(err == EGL_BAD_PARAMETER)
		_jl_fl_errf(_jlc, "1 or more argument values are invalid.");
	else if(err == EGL_BAD_SURFACE)
		_jl_fl_errf(_jlc, "EGLSurface argument is an invalid surface.");
	else if(err == EGL_CONTEXT_LOST)
		_jl_fl_errf(_jlc, "A power management event has occurred.");
	else
		_jl_fl_errf(_jlc, "Unknown error.");

	_jl_fl_errf(_jlc, ":\n:");
	_jl_fl_errf(_jlc, (void*)jl_me_strt_fnum(x)->data);
	_jl_fl_errf(_jlc, "\n");
	jl_sg_kill(_jlc->jlc);
}

#endif // JL_DEBUG_LIB

static void jl_gl_pixelbuffer_old__(jvct_t *_jlc, EGLSurface pb) {
	eglDestroySurface(eglDisp, pb);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_pixelbuffer_old__()");
}

static EGLSurface jl_gl_pixelbuffer_new__(jvct_t *_jlc,u16_t w,u16_t h){
	const EGLint surfaceAttr[] = {
		EGL_WIDTH, w,
		EGL_HEIGHT, h,
		EGL_NONE
	};
	
	// Create & return a pixelbuffer surface
	return eglCreatePbufferSurface(eglDisp, eglConf, surfaceAttr);
	JL_EGL_ERROR(_jlc, w, "jl_gl_pixelbuffer_new__()");
}

static void jl_gl_pixelbuffer_use__(jvct_t *_jlc, EGLSurface pb) {
	eglMakeCurrent(eglDisp, pb, pb, eglCtx);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_pixelbuffer_use__()");
}

static void jl_gl_pixelbuffer_off__(jvct_t *_jlc) {
	eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE,
		EGL_NO_CONTEXT);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_pixelbuffer_off__()");
}

#elif JL_GLRTEX == JL_GLRTEX_SDL
#else // !JL_GLRTEX

static void _jl_gl_depthbuffer_free(jvct_t *_jlc, uint32_t *db) {
	glDeleteRenderbuffers(1, db);
	JL_GL_ERROR(_jlc,*db,"_jl_gl_depthbuffer_free: glDeleteRenderbuffers");
	*db = 0;
}

static void _jl_gl_depthbuffer_make(jvct_t *_jlc, uint32_t *db) {
	glGenRenderbuffers(1, db);
	if(!(*db)) {
		_jl_fl_errf(_jlc, "_jl_gl_depthbuffer_make: GL buff=0\n");
		jl_sg_kill(_jlc->jlc);
	}
	JL_GL_ERROR(_jlc,*db,"make pr: glGenRenderbuffers");
}

static void jl_gl_depthbuffer_set__(jvct_t *_jlc, u16_t w, u16_t h) {
	if(!w) {
		_jl_fl_errf(_jlc, "jl_gl_depthbuffer_set: w = 0\n");
		jl_sg_kill(_jlc->jlc);
	}
	if(!h) {
		_jl_fl_errf(_jlc, "jl_gl_depthbuffer_set: h = 0\n");
		jl_sg_kill(_jlc->jlc);
	}
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	JL_GL_ERROR(_jlc, w, "make pr: glRenderbufferStorage");
}

static void _jl_gl_depthbuffer_bind(jvct_t *_jlc, uint32_t db) {
	if(db == 0) {
		_jl_fl_errf(_jlc, "_jl_gl_depthbuffer_bind: GL db = 0\n");
		jl_sg_kill(_jlc->jlc);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, db);
	JL_GL_ERROR(_jlc, db,"_jl_gl_depthbuffer_bind: glBindRenderbuffer");
}

static void jl_gl_depthbuffer_off__(jvct_t *_jlc) {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	JL_GL_ERROR(_jlc, 0,"jl_gl_depthbuffer_off__: glBindRenderbuffer");
}

#endif // JL_GLRTEX

static void _jl_gl_viewport(jvct_t *_jlc, uint16_t w, uint16_t h) {
	glViewport(0, 0, w, h);
	JL_GL_ERROR(_jlc, w * h, "glViewport");
}

static void _jl_gl_pr_unuse(jvct_t *_jlc) {
	// Render to the screen
	#if JL_GLRTEX == JL_GLRTEX_EGL
		jl_gl_pixelbuffer_off__(_jlc);
	#elif JL_GLRTEX == JL_GLRTEX_SDL
		jl_gl_pbo_off__(_jlc);
	#else
		jl_gl_framebuffer_off__(_jlc);
	#endif
	// Reset the aspect ratio.
	_jlc->gl.cp = NULL;
}

static void _jl_gl_pr_obj_free(jvct_t *_jlc, jl_pr_t *pr) {
	_jl_gl_texture_free(_jlc, &(pr->tx));
#if JL_GLRTEX == JL_GLRTEX_EGL
	jl_gl_pixelbuffer_old__(_jlc, pr->pb);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	jl_me_alloc(_jlc->jlc, (void**)&pr->px, 0, 4 * pr->w * pr->h);
#else
	_jl_gl_framebuffer_free(_jlc, &(pr->fb));
	_jl_gl_depthbuffer_free(_jlc, &(pr->db));
#endif
}

static void jl_gl_pr_obj_make_tx__(jvct_t *_jlc, jl_pr_t *pr) {
	// Make a new texture for pre-renderering.  The "NULL" sets it blank.
	jl_gl_texture_new__(_jlc, &(pr->tx), NULL, pr->w, pr->h);
	jl_gl_texture_off__(_jlc);
}

// Initialize an already allocated pr object with width and hieght of "pr->w" &
// "pr->h".
static void _jl_gl_pr_obj_make(jvct_t *_jlc, jl_pr_t *pr) {
	_jlc->gl.cp = NULL;
	if(pr->w < 1) {
		_jl_fl_errf(_jlc, ":_jl_gl_pr_obj_make() failed:\n:");
		_jl_fl_errf(_jlc, ": 'w' must be more than 1\n");
		jl_sg_kill(_jlc->jlc);
	}
	if(pr->h < 1) {
		_jl_fl_errf(_jlc, ":_jl_gl_pr_obj_make() failed:\n:");
		_jl_fl_errf(_jlc, ": 'h' must be more than 1\n");
		jl_sg_kill(_jlc->jlc);
	}
	if((pr->w > GL_MAX_TEXTURE_SIZE) || (pr->h > GL_MAX_TEXTURE_SIZE)) {
		char *wmsg = jl_me_string_fnum(_jlc->jlc, pr->w);
		char *hmsg = jl_me_string_fnum(_jlc->jlc, pr->h);
		_jl_fl_errf(_jlc, ":_jl_gl_pr_obj_make() failed:\n");
		_jl_fl_errf(_jlc, ":w = ");
		_jl_fl_errf(_jlc, wmsg);
		_jl_fl_errf(_jlc, "\n:h = ");
		_jl_fl_errf(_jlc, hmsg);
		_jl_fl_errf(_jlc, "\n");
		_jl_fl_errf(_jlc, ": texture is too big for graphics card.\n");
		jl_sg_kill(_jlc->jlc);
	}
#if JL_GLRTEX == JL_GLRTEX_EGL
	// Make the texture.
	jl_gl_pr_obj_make_tx__(_jlc, pr);
	// Make a new Pixelbuffer.
	pr->pb = jl_gl_pixelbuffer_new__(_jlc, pr->w, pr->h);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	// Make the texture.
	jl_gl_pr_obj_make_tx__(_jlc, pr);
	// Make a new Hidden Window.
	pr->sw = jl_dl_screen_new_(_jlc, pr->w, pr->h);
	// Allocate pixel space
	jl_me_alloc(_jlc->jlc, (void**)&pr->px, 4 * pr->w * pr->h, 0);
	// Use the Pre-renderer.
	printf("jl_gl_pbo_use__: %dx%d\n", pr->w, pr->h);
	jl_gl_pbo_use__(_jlc, pr);
#else
	// Make the texture.
	jl_gl_pr_obj_make_tx__(_jlc, pr);
	// Make a new Depthbuffer.
	jl_gl_depthbuffer_new__(_jlc, &(pr->db), pr->w, pr->h);
	jl_gl_depthbuffer_off__(_jlc);
	// Make & Bind a new Framebuffer.
	jl_gl_framebuffer_new__(_jlc, &(pr->fb), pr->tx, pr->db, pr->w, pr->h);
	// Set Viewport to image and clear.
	_jl_gl_viewport(_jlc, pr->w, pr->h);
#endif
	// Clear the pre-renderer.
	jl_gl_clear(_jlc->jlc, 255, 0, 0, 255);
	// Unbind Pixelbuffer or Framebuffer.
#if JL_GLRTEX == JL_GLRTEX_EGL
	jl_gl_pixelbuffer_off__(_jlc);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	jl_gl_pbo_off__(_jlc);
#else
	jl_gl_framebuffer_off__(_jlc);
#endif
	// De-activate pre-renderer.
	jl_gl_pr_scr(_jlc);
}

static void _jl_gl_txtr(jvct_t *_jlc, jl_vo** pv, uint8_t a, uint8_t is_rt) {
	if((*pv) == NULL) (*pv) = _jlc->gl.temp_vo;
	// Set Simple Variabes
	(*pv)->a = ((float)a) / 255.f;
	// Make sure non-textured colors aren't attempted
	if((!is_rt) && ((*pv)->cc != NULL))
		jl_me_alloc(_jlc->jlc, (void**)&((*pv)->cc), 0,0);
}

static inline void _jl_gl_set_shader(jvct_t *_jlc, jl_vo* pv) {
	_jl_gl_setp(_jlc, pv->cc == NULL ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
}

// Prepare to draw a solid color
static inline void _jl_gl_draw_colr(jvct_t *_jlc, jl_vo* pv) {
	// Bind Colors to shader
	_jl_gl_setv(_jlc, pv->bt, _jlc->gl.clr.attr.acolor, 4);
}

// Prepare to draw a texture with texture coordinates "tc". 
static void _jl_gl_draw_txtr(jvct_t *_jlc, f32_t a, u32_t tx, u32_t tc) {
	// Bind Texture Coordinates to shader
	_jl_gl_setv(_jlc, tc, _jlc->gl.tex.attr.texpos, 2);
	// Set Alpha Value In Shader
	_jl_gl_setalpha(_jlc, a);
	// Bind the texture
	jl_gl_texture_bind__(_jlc, tx);
}

static void _jl_gl_draw_onts(jvct_t *_jlc, u32_t gl, u8_t rs, u32_t vc) {
	// Update the clipping pane & position in shader.
	_jl_gl_updatevectors(_jlc, gl);
	// Finally, draw the image!
	_jl_gl_draw_arrays(_jlc, rs ? GL_TRIANGLES : GL_TRIANGLE_FAN, vc);
}

/************************/
/***  ETOM Functions  ***/
/************************/

// Set the viewport to the screen size.
void jl_gl_viewport_screen(jvct_t *_jlc) {
//SDL_GL_GetDrawableSize(SDL_Window* window,
//                          int*        w,
//                        int*        h)

	_jl_gl_viewport(_jlc, _jlc->dl.full_w, _jlc->dl.full_h);
}

void jl_gl_pr_old_(jvct_t *_jlc, jl_pr_t** pr) {
	// 
	_jl_gl_pr_obj_free(_jlc, *pr);
	// Free old pr object.
	jl_me_alloc(_jlc->jlc, (void**)pr, 0, 0);
}

// Free a pr for a vertex object
void jl_gl_pr_old(jvct_t *_jlc, jl_vo* pv) {
	if(pv->pr == NULL) {
		_jl_fl_errf(_jlc, "pr(): double free!\n");
		jl_sg_kill(_jlc->jlc);
	}
	jl_gl_pr_old_(_jlc, &(pv->pr));
}

uint8_t jl_gl_pr_isi_(jvct_t *_jlc, jl_pr_t* pr) {
	if(pr) {
#if JL_GLRTEX == JL_GLRTEX_EGL
		return !!pr->tx; // Return 0 if 0, 1 if nonzero
#elif JL_GLRTEX == JL_GLRTEX_SDL
		return !!pr->tx;
#else
		if(pr->tx && pr->db && pr->fb) {
			return 1;
		}else if((!pr->db) && (!pr->fb)) {
			return 0;
		}else{
			if(pr->tx)
				jl_io_printc(_jlc->jlc, "[OK] pr->tx\n");
			else jl_io_printc(_jlc->jlc, "[FAIL] pr->tx\n");
			if(pr->db)
				jl_io_printc(_jlc->jlc, "[OK] pr->db\n");
			else jl_io_printc(_jlc->jlc, "[FAIL] pr->db\n");
			if(pr->fb)
				jl_io_printc(_jlc->jlc, "[OK] pr->fb\n");
			else jl_io_printc(_jlc->jlc, "[FAIL] pr->fb\n");
			_jl_fl_errf(_jlc, "\n");
			jl_sg_kill(_jlc->jlc);
		}
#endif
	}else{
		return 0;
	}
	// Never reached
	return 2;
}

// Test if pre-renderer is initialized.
uint8_t jl_gl_pr_isi(jvct_t *_jlc, jl_vo* pv) {
	if(pv)
		return jl_gl_pr_isi_(_jlc, pv->pr);
	else
		return 2;
}

void jl_gl_pr_use_(jvct_t *_jlc, jl_pr_t* pr) {
// Render to the framebuffer.
#if JL_GLRTEX == JL_GLRTEX_EGL
	jl_gl_pixelbuffer_use__(_jlc, pr->pb);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	jl_gl_pbo_use__(_jlc, pr);
#else
	jl_gl_framebuffer_use__(_jlc, pr->fb, pr->db, pr->tx, pr->w, pr->h);
#endif
	// Reset the aspect ratio.
	_jlc->gl.cp = pr;
}

// Use a vertex object's pre-renderer for rendering.
void jl_gl_pr_use(jvct_t *_jlc, jl_vo* pv) {
	pv->a = 1.f;
	if(!pv->pr) {
		_jl_fl_errf(_jlc, "jl_gl_pr_use: pre-renderer not created\n");
		jl_sg_kill(_jlc->jlc);
	}
	jl_gl_pr_use_(_jlc, pv->pr);
}

// Use the screens prerenderer.
void jl_gl_pr_scr(jvct_t *_jlc) {
	if(_jlc->gl.bg) jl_gl_pr_use_(_jlc, _jlc->gl.bg);
	else _jl_gl_pr_unuse(_jlc);
}

// Set the screens prerenderer.
void jl_gl_pr_scr_set(jvct_t *_jlc, jl_vo* vo) {
	if(vo) _jlc->gl.bg = vo->pr;
	else _jlc->gl.bg = NULL;
}

// Turn a pre-renderer off in order to draw to the screen.
void jl_gl_pr_off(jvct_t *_jlc) {
	_jl_gl_pr_unuse(_jlc);
}

/**
 * Render an image onto a vertex object's pr.
**/
void jl_gl_pr(jl_t *jlc, jl_vo* vo, jl_simple_fnt par__redraw) {
	jvct_t *_jlc = jlc->_jlc;

	jl_gl_pr_(_jlc, vo->pr, par__redraw);
}

// Set vertices for a polygon.
void jl_gl_poly(jvct_t *_jlc, jl_vo* pv, uint8_t vertices, const float *xyzw) {
	const float FS_RECT[] = {
		0.,jl_gl_ar(_jlc->jlc),0.,
		0.,0.,0.,
		1.,0.,0.,
		1.,jl_gl_ar(_jlc->jlc),0.
	};

	if(pv == NULL) pv = _jlc->gl.temp_vo;
	if(xyzw == NULL) xyzw = FS_RECT;
	// Rendering Style = polygon
	pv->rs = 0;
	// Set the vertices of vertex object "pv"
	jl_gl_vo_vertices(_jlc, pv, xyzw, vertices);
}

// Set vertices for vector triangles.
void jl_gl_vect(jvct_t *_jlc, jl_vo* pv, uint8_t vertices, const float *xyzw) {
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	// Rendering Style = triangles
	pv->rs = 1;
	// Set the vertices of vertex object "pv"
	jl_gl_vo_vertices(_jlc, pv, xyzw, vertices);
}

// Set colors to "cc" in vertex oject "pv" - cc will be freed when done
void jl_gl_clrc(jvct_t *_jlc, jl_vo* pv, jl_ccolor_t* cc) {
	_jl_gl_col_begin(_jlc, pv); // Free "pv->cc" if non-null
	pv->cc = cc;
	// Set Color Buffer "pv->bt" to "pv->cc"
	jl_gl_buffer_set__(_jlc, pv->bt, pv->cc, pv->vc * 4);
}

//Convert color to solid
jl_ccolor_t* jl_gl_clrcs(jvct_t *_jlc, u8_t *rgba, uint32_t vc) {
	int i;
	jl_ccolor_t* cc = NULL;

	//Allocate memory
	jl_me_alloc(_jlc->jlc,
		(void**)&cc, vc * sizeof(float) * 4, 0);
	//Set RGBA for each vertex
	for(i = 0; i < vc; i++) { 
		cc[(i * 4) + 0] = ((double) rgba[0]) / 255.;
		cc[(i * 4) + 1] = ((double) rgba[1]) / 255.;
		cc[(i * 4) + 2] = ((double) rgba[2]) / 255.;
		cc[(i * 4) + 3] = ((double) rgba[3]) / 255.;
	}
	return cc;
}

//Convert Color To Gradient
jl_ccolor_t* jl_gl_clrcg(jvct_t *_jlc, u8_t *rgba, uint32_t vc) {
	int i;
	jl_ccolor_t* cc = NULL;

	//Allocate memory
	jl_me_alloc(_jlc->jlc, (void**)&cc, vc * sizeof(float) * 4, 0);
	//Set RGBA for each vertex
	for(i = 0; i < vc; i++) { 
		cc[(i * 4) + 0] = ((double) rgba[(i * 4) + 0]) / 255.;
		cc[(i * 4) + 1] = ((double) rgba[(i * 4) + 1]) / 255.;
		cc[(i * 4) + 2] = ((double) rgba[(i * 4) + 2]) / 255.;
		cc[(i * 4) + 3] = ((double) rgba[(i * 4) + 3]) / 255.;
	}
	return cc;
}

// Set Texturing to Gradient Color "rgba" { (4 * vertex count) values }
void jl_gl_clrg(jvct_t *_jlc, jl_vo* pv, u8_t *rgba) {
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	_jl_gl_col_begin(_jlc, pv);
	jl_gl_clrc(_jlc, pv, jl_gl_clrcg(_jlc, rgba, pv->vc));
}

// Set Texturing to Solid Color "rgba" { 4 values }
void jl_gl_clrs(jvct_t *_jlc, jl_vo* pv, u8_t *rgba) {
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	_jl_gl_col_begin(_jlc, pv);
	jl_gl_clrc(_jlc, pv, jl_gl_clrcs(_jlc, rgba, pv->vc));
}

// Set texturing to a bitmap
void jl_gl_txtr(jvct_t *_jlc, jl_vo* pv, u8_t map, u8_t a, u16_t pgid, u16_t pi)
{
	_jl_gl_txtr(_jlc, &pv, a, 0);
	pv->tx = _jlc->gl.textures[pgid][pi];
	if(map) {
		int32_t cX = map%16;
		int32_t cY = map/16;
		double CX = -.0039 + (((double)cX)/16.);
		double CY = .001 + (((double)(15-cY))/16.);
		float tex1[] =
		{
			(DEFAULT_TC[0]/16.) + CX, (DEFAULT_TC[1]/16.) + CY,
			(DEFAULT_TC[2]/16.) + CX, (DEFAULT_TC[3]/16.) + CY,
			(DEFAULT_TC[4]/16.) + CX, (DEFAULT_TC[5]/16.) + CY,
			(DEFAULT_TC[6]/16.) + CX, (DEFAULT_TC[7]/16.) + CY
		};
		jl_gl_buffer_set__(_jlc, pv->bt, tex1, 8);
	}else{
		jl_gl_buffer_set__(_jlc, pv->bt, DEFAULT_TC, 8);
	}
}

//TODO:MOVE
// Shader true if texturing, false if coloring
// X,Y,Z are all [0. -> 1.]
// X,Y are turned into [-.5 -> .5] - center around zero.
static void jl_gl_translate__(jvct_t *_jlc, const uint8_t shader, float x,
	float y, float z, f64_t ar)
{
	// Determine which shader to use: texturing or coloring?
	_jl_gl_setp(_jlc, shader ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set the uniforms
	glUniform3f(shader ? _jlc->gl.tex.uniforms.translate :
		_jlc->gl.clr.uniforms.translate,
		x - (1./2.), y - (ar/2.), z);
	JL_GL_ERROR(_jlc, 0,"glUniform3f - translate");	
}

//TODO:MOVE
static void jl_gl_transform__(jvct_t *_jlc, const uint8_t shader, float x,
	float y, float z, f64_t ar)
{
	// Determine which shader to use: texturing or coloring?
	_jl_gl_setp(_jlc, shader ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set the uniforms
	glUniform4f(shader ? _jlc->gl.tex.uniforms.transform :
		_jlc->gl.clr.uniforms.transform,
		2. * x, -2. * (y / ar), 2. * z, 1.f);
	JL_GL_ERROR(_jlc, 0,"glUniform3f - transform");
}

void jl_gl_transform_pr_(jvct_t *_jlc, jl_pr_t* pr, float x, float y, float z,
	float xm, float ym, float zm)
{
	f64_t ar = jl_gl_ar(_jlc->jlc);

	if(!jl_gl_pr_isi_(_jlc, pr)) {
		_jl_fl_errf(_jlc, "jl_gl_translate_pr: "
			"Pre-renderer Not Initialized!\n");
		jl_sg_kill(_jlc->jlc);
	}
	jl_gl_translate__(_jlc, 1, x, y, z, ar);
	jl_gl_transform__(_jlc, 1, xm, ym, zm, ar);
}

void jl_gl_transform_vo_(jvct_t *_jlc, jl_vo* vo, float x, float y, float z,
	float xm, float ym, float zm)
{
	f64_t ar = jl_gl_ar(_jlc->jlc);

	if(vo == NULL) vo = _jlc->gl.temp_vo;
	jl_gl_translate__(_jlc, (vo->cc == NULL), x, y, z, ar);
	jl_gl_transform__(_jlc, (vo->cc == NULL), xm, ym, zm, ar);
	// If it has a pre-renderer; transform it with.
	if(vo->pr) jl_gl_transform_pr_(_jlc, vo->pr, x, y, z, xm, ym, zm);
}

//Draw object with "vertices" vertices.  The vertex data is in "x","y" and "z".
//"map" refers to the charecter map.  0 means don't zoom in to one character.
//Otherwise it will zoom in x16 to a single charecter
/**
 * If "pv" is NULL then draw what's on the temporary buffer
 * Else render vertex object "pv" on the screen.
*/
void jl_gl_draw(jvct_t *_jlc, jl_vo* pv) {
	// Fail if no vertex object.
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	// Determine which shader to use: texturing or coloring?
	_jl_gl_set_shader(_jlc, pv);
	// Set texture and transparency if texturing.  If colors: bind colors
	if(pv->cc) _jl_gl_draw_colr(_jlc, pv);
	else _jl_gl_draw_txtr(_jlc, pv->a, pv->tx, pv->bt);
	// Draw onto the screen.
	_jl_gl_draw_onts(_jlc, pv->gl, pv->rs, pv->vc);
}

void jl_gl_draw_pr_(jl_t* jlc, jl_pr_t* pr) {
	jvct_t *_jlc = jlc->_jlc;

	// Fail if no pre-rendered texture.
	if(!jl_gl_pr_isi_(_jlc, pr)) {
		_jl_fl_errf(_jlc, "Pre-renderer Not Initialized!\n");
		jl_sg_kill(jlc);
	}
	// Use texturing shader.
	_jl_gl_setp(_jlc, JL_GL_SLPR_TEX);
	// Set texture to pr.
	_jl_gl_draw_txtr(_jlc, 255, pr->tx, _jlc->gl.default_tc);
	// Draw onto the screen.
	_jl_gl_draw_onts(_jlc, pr->gl, 0, 4);
}

void jl_gl_pr_draw(jl_t* jlc, jl_vo* pv) {
	jvct_t *_jlc = jlc->_jlc;

	if(pv == NULL) pv = _jlc->gl.temp_vo;
	jl_gl_draw_pr_(jlc, pv->pr);
}

int32_t _jl_gl_getu(jvct_t *_jlc, GLuint prg, char *var) {
	int32_t a = 0;
	if((a = glGetUniformLocation(prg, var)) == -1) {
		_jl_fl_errf(_jlc, ":opengl: bad name; is:\n:");
		_jl_fl_errf(_jlc, var);
		_jl_fl_errf(_jlc, "\n");
		jl_sg_kill(_jlc->jlc);
	}
	JL_GL_ERROR(_jlc, a,"glGetUniformLocation");
	return a;
}

void _jl_gl_geta(jvct_t *_jlc, GLuint prg, m_i32_t *attrib, const char *title) {
	if((*attrib = glGetAttribLocation(prg, title)) == -1) {
		_jl_fl_errf(_jlc,
			"attribute name is either reserved or non-existant");
		jl_sg_kill(_jlc->jlc);
	}
}

/***	  @cond	   ***/
/************************/
/*** Static Functions ***/
/************************/

static inline void _jl_gl_init_setup_gl(jvct_t *_jlc) {
	jl_io_printc(_jlc->jlc, "setting properties...\n");
	//Disallow Dither & Depth Test
	_jl_gl_init_disable_extras(_jlc);
	//Set alpha=0 to transparent
	_jl_gl_init_enable_alpha(_jlc);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	jl_io_printc(_jlc->jlc, "set glproperties.\n");
}

static inline void _jl_gl_init_shaders(jvct_t *_jlc) {
	jl_io_printc(_jlc->jlc, "making GLSL programs....\n");
	_jlc->gl.prgs[JL_GL_SLPR_TEX] = 
		jl_gl_glsl_prg_create(_jlc, source_vert_tex, source_frag_tex);
	_jlc->gl.prgs[JL_GL_SLPR_CLR] =
		jl_gl_glsl_prg_create(_jlc, source_vert_clr, source_frag_clr);
	jl_io_printc(_jlc->jlc, "made programs.\n");

	jl_io_printc(_jlc->jlc, "setting up shaders....\n");
	if(_jlc->gl.tex.uniforms.textures == NULL) {
		_jl_fl_errf(_jlc, "Couldn't create uniforms");
		jl_sg_kill(_jlc->jlc);
	}
	_jlc->gl.tex.uniforms.textures[0][0] =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX], "texture");
	_jlc->gl.tex.uniforms.multiply_alpha =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX],
			"multiply_alpha");
	// Translate Vector
	_jlc->gl.tex.uniforms.translate =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX], "translate");
	_jlc->gl.clr.uniforms.translate =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR], "translate");
	// Transform Vector
	_jlc->gl.tex.uniforms.transform =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX], "transform");
	_jlc->gl.clr.uniforms.transform =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR], "transform");
	//
	jl_io_printc(_jlc->jlc, "setting up tex shader attrib's....\n");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX],
		&_jlc->gl.tex.attr.position, "position");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX],
		&_jlc->gl.tex.attr.texpos, "texpos");
	jl_io_printc(_jlc->jlc, "setting up clr shader attrib's....\n");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR],
		&_jlc->gl.clr.attr.position, "position");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR],
		&_jlc->gl.clr.attr.acolor, "acolor");
	jl_io_printc(_jlc->jlc, "set up shaders.\n");
}

#if JL_GLRTEX == JL_GLRTEX_EGL
static inline void jl_gl_init_egl(jvct_t *_jlc) {
	// EGL config attributes
	const EGLint confAttr[] = {
		// very important!
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		// we will create a pixelbuffer surface
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_RED_SIZE,   8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE,  8,
		EGL_ALPHA_SIZE, 8,	// if you need the alpha channel
		EGL_DEPTH_SIZE, 16,	// if you need the depth buffer
		EGL_NONE
	};

	// EGL context attributes
	const EGLint ctxAttr[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,			  // very important!
		EGL_NONE
	};

	EGLint eglMajVers, eglMinVers;
	EGLint numConfigs;

	eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_init_egl:eglGetDisplay");
	eglInitialize(eglDisp, &eglMajVers, &eglMinVers);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_init_egl:eglInitialize");

	printf("EGL init with version %d.%d", eglMajVers, eglMinVers);

	// choose the first config, i.e. best config
	eglChooseConfig(eglDisp, confAttr, &eglConf, 1, &numConfigs);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_init_egl:eglChooseConfig");

	eglCtx = eglCreateContext(eglDisp, eglConf, EGL_NO_CONTEXT, ctxAttr);
	JL_EGL_ERROR(_jlc, 0, "jl_gl_init_egl:eglCreateContext");
}
#endif

//Load and create all resources
static inline void _jl_gl_make_res(jvct_t *_jlc) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "GLIN");
#if JL_GLRTEX == JL_GLRTEX_EGL
	// Initialize EGL.
	jl_gl_init_egl(_jlc);
#endif
	// Create SDL_GL context to link with EGL

	// Get GL Extensions.
	SDL_Log("GL_EXTENSIONS = \"%s\"\n", glGetString(GL_EXTENSIONS));
	JL_GL_ERROR(_jlc, 0, "glGetString()");
	// Setup opengl properties
	_jl_gl_init_setup_gl(_jlc);
	// Create shaders and set up attribute/uniform variable communication
	_jl_gl_init_shaders(_jlc);
	//
	jl_io_printc(_jlc->jlc, "making temporary vertex object....\n");
	_jlc->gl.temp_vo = jl_gl_vo_make(_jlc->jlc, 1);
	jl_io_printc(_jlc->jlc, "making default texc buff!\n");
	// Default GL Texture Coordinate Buffer
	jl_gl_buffer_new__(_jlc, &(_jlc->gl.default_tc));
	jl_gl_buffer_set__(_jlc, _jlc->gl.default_tc, DEFAULT_TC, 8);
	jl_io_printc(_jlc->jlc, "made temp vo & default tex. c. buff!\n");
	jl_io_close_block(_jlc->jlc); //Close Block "GLIN"
}

static inline void _jl_gl_vo_make(jvct_t *_jlc, jl_vo* vo, u32_t nc) {
	// How many more vo's will be made.
	vo->nc = nc;
	// GL VBO
	jl_gl_buffer_new__(_jlc, &vo->gl);
	// GL Texture Coordinate Buffer
	jl_gl_buffer_new__(_jlc, &vo->bt);
	// Converted Vertices
	vo->cv = NULL;
	// Vertex Count
	vo->vc = 0;
	// Converted Colors
	vo->cc = NULL;
	// Rendering Style = Polygon
	vo->rs = 0;
	// Texture
	vo->tx = 0;
	// No pre-renderer has been created.
	vo->pr = NULL;
	// Set the container pre-renderer.
	vo->cb.x = 0.f, vo->cb.y = 0.f, vo->cb.z = 0.f;
	vo->cb.w = 1., vo->cb.h = 1., vo->cb.d = 1.;
}

static void jl_gl_pr_set__(jl_t* jlc,jl_pr_t *pr,f32_t w,f32_t h,u16_t w_px) {
	// Get Aspect Ratio.
	f32_t ar = h / w;
	// Get height in pixels.
	f32_t h_px = w_px * ar;

	// Set width and height.
	pr->w = w_px;
	pr->h = h_px;
	// Set aspect ratio.
	pr->ar = ar;
}

/**	  @endcond	  **/
/************************/
/*** Global Functions ***/
/************************/

/**
 * Create 1 or more empty vertex object/s & return it/them.
 * @param jlc: The library context.
 * @param count: How many vertex objects to create - default = 1.
 * @returns: A new vertex object with 0 vertices.
**/
jl_vo *jl_gl_vo_make(jl_t* jlc, u32_t count) {
	jvct_t *_jlc = jlc->_jlc;
	jl_vo *rtn = NULL;
	m_u32_t i;

	// Allocate space in "rtn"
	jl_me_alloc(_jlc->jlc, (void**)&rtn, sizeof(jl_vo) * count,0);
	// Make each vertex object.
	for(i = 0; i < count; i++) _jl_gl_vo_make(_jlc, &rtn[i], (count-1) - i);
	// Return the vertex object[s].
	return rtn;
}

/**
 * Get the Aspect Ratio for the pre-renderer in use.
 * @param jlc: The library context.
**/
double jl_gl_ar(jl_t* jlc) {
	jvct_t *_jlc = jlc->_jlc;

	return _jlc->gl.cp ? _jlc->gl.cp->ar : _jlc->dl.aspect;
}

/**
 * Clear the screen with a color
 * @param jlc: The library context.
 * @param r: The amount of red [ 0 - 255 ]
 * @param g: The amount of green [ 0 - 255 ]
 * @param b: The amount of blue [ 0 - 255 ]
 * @param a: The translucency [ 0 - 255 ]
**/
void jl_gl_clear(jl_t* jlc, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	glClearColor(((double)r)/255., ((double)g)/255.,
		((double)b)/255., ((double)a)/255.);
	JL_GL_ERROR(jlc->_jlc, a, "jl_gl_clear(): glClearColor");
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	JL_GL_ERROR(jlc->_jlc, a, "jl_gl_clear(): glClear");
}

/**
 * Resize a prerenderer.
 * @param jlc: The library context.
 * @param pr: The pre-renderer to resize.
 * @param w: The display width. [ 0. - 1. ]
 * @param h: The display height. [ 0. - 1. ]
 * @param w_px: The resolution in pixels along the x axis [ 1- ]
**/
void jl_gl_pr_rsz(jl_t* jlc, jl_pr_t *pr, f32_t w, f32_t h, u16_t w_px) {
	SDL_Log("%f, %f, %d", w, h, w_px);

	f32_t xyzw[] = {
		0.f,	h,	0.f,
		0.f,	0.f,	0.f,
		w,	0.f,	0.f,
		w,	h,	0.f
	};

	// Use the screen's pre-renderer if it exists.
	jl_gl_pr_scr(jlc->_jlc);
	// Create the VBO.
	jl_gl_vertices__(jlc->_jlc, xyzw, 4, pr->cv, pr->gl);
	// Set width, height and aspect ratio.
	jl_gl_pr_set__(jlc, pr, w, h, w_px);
	// Resize the actual texture.
	_jl_gl_pr_obj_free(jlc->_jlc, pr);
	_jl_gl_pr_obj_make(jlc->_jlc, pr);
}

/**
 * Make a new pre-renderer.
 * @param jlc: The library context.
 * @param w: The display width. [ 0. - 1. ]
 * @param h: The display height. [ 0. - 1. ]
 * @param w_px: The resolution in pixels along the x axis [ 1- ]
**/
jl_pr_t * jl_gl_pr_new(jl_t* jlc, f32_t w, f32_t h, u16_t w_px) {
	jvct_t *_jlc = jlc->_jlc;
	jl_pr_t *pr = NULL;

	// Make the pr structure.
	jl_me_alloc(jlc, (void**)(&pr), sizeof(jl_pr_t), 0);
	// Set the initial pr structure values - Nothings made yet.
	pr->tx = 0;
	#if JL_GLRTEX == JL_GLRTEX_EGL
	pr->pb = (EGLSurface)0;
	#elif JL_GLRTEX == JL_GLRTEX_SDL
	pr->sw = 0;
	pr->px = NULL;
	#else
	pr->db = 0;
	pr->fb = 0;
	#endif
	pr->gl = 0;
	pr->cv = NULL;
	// Set width, height and aspect ratio.
	jl_gl_pr_set__(jlc, pr, w, h, w_px);
	// Allocate pr->cv 
	jl_me_alloc(jlc, (void**)&pr->cv, 4*sizeof(float)*3, 0);
	// Make OpenGL Objects
	jl_gl_buffer_new__(_jlc, &(pr->gl));
	_jl_gl_pr_obj_make(_jlc, pr);
	// Resize the new pre-renderer.
	jl_gl_pr_rsz(jlc, pr, w, h, w_px);
	// Return the new pre-renderer.
	return pr;
}

/***	  @cond	   ***/
/************************/
/***  ETOM Functions  ***/
/************************/

void jl_gl_pr_(jvct_t *_jlc, jl_pr_t * pr, jl_simple_fnt par__redraw) {
	if(!pr) {
		_jl_fl_errf(_jlc, "Drawing on lost pre-renderer.\n");
		jl_sg_kill(_jlc->jlc);
	}
	// Use the vo's pr
	jl_gl_pr_use_(_jlc, pr);
	// Render to the pr.
	par__redraw(_jlc->jlc);
	// Go back to the screen without clearing it.
	jl_gl_pr_scr(_jlc);
}

void jl_gl_resz_(jvct_t* _jlc) {
	// Deselect any pre-renderer.
	_jlc->gl.cp = NULL;
	// Deselect the screen.
	jl_gl_pr_scr_set(_jlc, NULL);
}

void _jl_gl_init(jvct_t *_jlc) {
#ifdef JL_GLTYPE_HAS_GLEW
	printf(" [JLVM/GLEW] init\n");
	if(glewInit()!=GLEW_OK) {
		_jl_fl_errf(_jlc, "glew fail!(no sticky)\n");
		jl_sg_kill(_jlc->jlc);
	}
#endif
	SDL_Log("GL_MAX_TEXTURE_SIZE: %d\n", GL_MAX_TEXTURE_SIZE);
	_jlc->gl.cp = NULL;
	_jlc->gl.bg = NULL;
	_jl_gl_make_res(_jlc);
	//Set uniform values
	_jl_gl_usep(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR]);
	jl_gl_uniform3f__(_jlc, _jlc->gl.clr.uniforms.translate, 0.f, 0.f, 0.f);
	jl_gl_uniform4f__(_jlc, _jlc->gl.clr.uniforms.transform, 1.f, 1.f, 1.f,
		1.f);
	_jl_gl_usep(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX]);
	jl_gl_uniform3f__(_jlc, _jlc->gl.tex.uniforms.translate, 0.f, 0.f, 0.f);
	jl_gl_uniform4f__(_jlc, _jlc->gl.tex.uniforms.transform, 1.f, 1.f, 1.f,
		1.f);
	//Textures on by default
	_jlc->gl.whichprg = JL_GL_SLPR_TEX;
	// Make sure no pre-renderer is activated.
	_jl_gl_pr_unuse(_jlc);
}

/**	  @endcond	  **/
/***   #End of File   ***/

//		glVertexPointer(3, GL_FLOAT, 0, head);
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Draw the cube 
	glColor4f(1.0,1.0,1.0,1.0);
//		glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, cube);
	glEnableClientState(GL_VERTEX_ARRAY);
//		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
	glctx->drawarrays(GL_TRIANGLES, 0, 6);

	glMatrixMode(GL_MODELVIEW);*/
