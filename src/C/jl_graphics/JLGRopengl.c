#include "jl_pr.h"
#include "jl_opengl.h"
#include "JLGRinternal.h"

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
	"	gl_FragColor = vec4(vcolor.rgb * vcolor.a, vcolor.a);\n"
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

static char *source_frag_tex_premult = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"uniform float multiply_alpha;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec4 vcolor = texture2D(texture, texcoord);\n"
	"	vcolor.a *= multiply_alpha;\n"
	"	gl_FragColor = vec4(vcolor.rgb * vcolor.a, vcolor.a);\n"
	"}\n\0";

static char *source_frag_tex_charmap = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"uniform float multiply_alpha;\n"
	"uniform vec4 new_color;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec4 vcolor = texture2D(texture, texcoord);\n"
	"	if((vcolor.r < 0.1) && (vcolor.g < 0.1) &&"
	"	   (vcolor.b < 0.1) && (vcolor.a > .9))\n"
	"		vcolor = new_color;\n"
	"	vcolor.a *= multiply_alpha;\n"
	"	gl_FragColor = vec4(vcolor.rgb * vcolor.a, vcolor.a);\n"
	"}\n\0";

static char *source_frag_tex = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = texture2D(texture, texcoord);\n"
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
	
// Full texture
static const float DEFAULT_TC[] = {
	0., 1.,
	0., 0.,
	1., 0.,
	1., 1.
};

#if JL_GLRTEX == JL_GLRTEX_EGL
	static EGLConfig eglConf;
	static EGLContext eglCtx;
	static EGLDisplay eglDisp;
#endif

// Prototypes:
#if JL_GLRTEX == JL_GLRTEX_EGL
#elif JL_GLRTEX == JL_GLRTEX_SDL
void jl_dl_screen_(jl_gr_t* jl_gr, jl_window_t* which);
jl_window_t* jl_dl_screen_new_(jl_gr_t* jl_gr, u16_t w, u16_t h);
#else
//static void jl_gl_depthbuffer_set__(jl_gr_t* jl_gr, u16_t w, u16_t h);
//static void _jl_gl_depthbuffer_bind(jl_gr_t* jl_gr, uint32_t db);
//static void _jl_gl_depthbuffer_make(jl_gr_t* jl_gr, uint32_t *db);
//static void jl_gl_depthbuffer_off__(jl_gr_t* jl_gr);
static void jl_gl_framebuffer_addtx__(jl_gr_t* jl_gr, u32_t tx);
static void jl_gl_framebuffer_adddb__(jl_gr_t* jl_gr, u32_t db);
static void jl_gl_framebuffer_status__(jl_gr_t* jl_gr);
#endif
static void _jl_gl_viewport(jl_gr_t* jl_gr, uint16_t w, uint16_t h);
void jl_gl_pr_scr(jl_gr_t* jl_gr);

// Definitions:
#ifdef JL_DEBUG_LIB
	#define JL_GL_ERROR(jl_gr, x, fname) jl_gl_get_error___(jl_gr, x, fname)
	#define JL_EGL_ERROR(jl_gr, x, fname) jl_gl_egl_geterror__(jl_gr, x, fname)
#else
	#define JL_GL_ERROR(jl_gr, x, fname)
	#define JL_EGL_ERROR(jl_gr, x, fname)
#endif

// Functions:

#ifdef JL_DEBUG_LIB
	static void jl_gl_get_error___(jl_gr_t* jl_gr, int width, char *fname) {
		GLenum err= glGetError();
		if(err == GL_NO_ERROR) return;
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
			GLint a;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &a);
			printf("Max texture size: %d/%d\n", width, a);
		}else{
			fstrerr = "opengl: unknown error!\n";
		}
		jl_io_print(jl_gr->jlc, "error: %s:%s (%d)",fname,fstrerr,width);
		jl_sg_kill(jl_gr->jlc);
	}
#endif

static void jl_gl_buffer_use__(jl_gr_t* jl_gr, uint32_t buffer) {
	// Check For Deleted Buffer
	if(buffer == 0) {
		jl_io_print(jl_gr->jlc, "buffer got deleted!");
		jl_sg_kill(jl_gr->jlc);
	}
	// bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	JL_GL_ERROR(jl_gr, 0,"bind buffer");
}

// Set the Data for VBO "buffer" to "buffer_data" with "buffer_size"
static void jl_gl_buffer_set__(jl_gr_t* jl_gr, uint32_t buffer,
	const void *buffer_data, u16_t buffer_size)
{
	//Bind Buffer "buffer"
	jl_gl_buffer_use__(jl_gr, buffer);
	//Set the data
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(float), buffer_data,
		GL_DYNAMIC_DRAW);
	JL_GL_ERROR(jl_gr, buffer_size, "buffer data");
}

static void jl_gl_buffer_new__(jl_gr_t* jl_gr, GLuint *buffer) {
	glGenBuffers(1, buffer);
	JL_GL_ERROR(jl_gr, 0,"buffer gen");
	if(*buffer == 0) {
		jl_io_print(jl_gr->jlc, "buffer is made wrongly!");
		jl_sg_kill(jl_gr->jlc);
	}
}

static void jl_gl_buffer_old__(jl_gr_t* jl_gr, uint32_t *buffer) {
	glDeleteBuffers(1, buffer);
	JL_GL_ERROR(jl_gr, 0,"buffer free");
	*buffer = 0;
}

GLuint jl_gl_load_shader(jl_gr_t* jl_gr, GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	JL_GL_ERROR(jl_gr, 0,"couldn't create shader");
	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pSource, NULL);
		JL_GL_ERROR(jl_gr, 0,"glShaderSource");
		glCompileShader(shader);
		JL_GL_ERROR(jl_gr, 0,"glCompileShader");
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		JL_GL_ERROR(jl_gr, 0,"glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			char* buf;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			JL_GL_ERROR(jl_gr, 1,"glGetShaderiv");
			if (infoLen) {
				buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					jl_io_print(jl_gr->jlc,
						"Could not compile shader:%s",buf);
					free(buf);
					jl_sg_kill(jl_gr->jlc);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint jl_gl_glsl_prg_create(jl_gr_t* jl_gr, const char* pVertexSource,
	const char* pFragmentSource)
{
	GLuint vertexShader = jl_gl_load_shader(jl_gr, GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		jl_io_print(jl_gr->jlc, "couldn't load vertex shader");
		jl_sg_kill(jl_gr->jlc);
	}

	GLuint pixelShader = jl_gl_load_shader(jl_gr, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		jl_io_print(jl_gr->jlc, "couldn't load fragment shader");
		jl_sg_kill(jl_gr->jlc);
	}

	GLuint program = glCreateProgram();
	JL_GL_ERROR(jl_gr, 0,"glCreateProgram");
	if (program) {
		GLint linkStatus = GL_FALSE;

		glAttachShader(program, vertexShader);
		JL_GL_ERROR(jl_gr, 0,"glAttachShader (vertex)");
		glAttachShader(program, pixelShader);
		JL_GL_ERROR(jl_gr, 0,"glAttachShader (fragment)");
		glLinkProgram(program);
		JL_GL_ERROR(jl_gr, 0,"glLinkProgram");
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		JL_GL_ERROR(jl_gr, 0,"glGetProgramiv");
		glValidateProgram(program);
		JL_GL_ERROR(jl_gr, 1,"glValidateProgram");
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			char* buf;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			JL_GL_ERROR(jl_gr, 1,"glGetProgramiv");
			if (bufLength) {
				buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					jl_io_print(jl_gr->jlc,
						"Could not link program:%s",buf);
					free(buf);
					jl_sg_kill(jl_gr->jlc);
				}else{
					jl_io_print(jl_gr->jlc, "failed malloc");
					jl_sg_kill(jl_gr->jlc);
				}
			}else{
				glDeleteProgram(program);
				jl_io_print(jl_gr->jlc, "no info log");
				jl_sg_kill(jl_gr->jlc);
			}
		}
	}
	if (program == 0) {
		jl_io_print(jl_gr->jlc, "Failed to load program");
		jl_sg_kill(jl_gr->jlc);
	}
	return program;
}

static void jl_gl_texture_make__(jl_gr_t* jl_gr, uint32_t *tex) {
	glGenTextures(1, tex);
	if(!(*tex)) {
		JL_GL_ERROR(jl_gr, 0, "jl_gl_texture_make__: glGenTextures");
		jl_io_print(jl_gr->jlc, "jl_gl_texture_make__: GL tex = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	JL_GL_ERROR(jl_gr, 0, "jl_gl_texture_make__: glGenTextures");
}

// Set the bound texture.  pm is the pixels 0 - blank texture.
static void jl_gl_texture_set__(jl_gr_t* jl_gr, u8_t* pm, u16_t w, u16_t h,
	u8_t bytepp)
{
	GLenum format = GL_RGBA;
	if(bytepp == 3)	format = GL_RGB;
	JL_GL_ERROR(jl_gr, w, "before texture image 2D");
	glTexImage2D(
		GL_TEXTURE_2D, 0,		/* target, level */
		format,				/* internal format */
		w, h, 0,			/* width, height, border */
		format, GL_UNSIGNED_BYTE,	/* external format, type */
		pm				/* pixels */
	);
	JL_GL_ERROR(jl_gr, w, "texture image 2D");
}

static void jl_gl_texpar_set__(jl_gr_t* jl_gr) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	JL_GL_ERROR(jl_gr, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	JL_GL_ERROR(jl_gr, 1,"glTexParameteri");
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	JL_GL_ERROR(jl_gr, 2,"glTexParameteri");
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	JL_GL_ERROR(jl_gr, 3,"glTexParameteri");
}

static void jl_gl_texture__bind__(jl_gr_t* jl_gr, uint32_t tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
	JL_GL_ERROR(jl_gr, tex,"jl_gl_texture_bind__: glBindTexture");
}

// Bind a texture.
static void jl_gl_texture_bind__(jl_gr_t* jl_gr, uint32_t tex) {
	if(tex == 0) {
		jl_io_print(jl_gr->jlc, "jl_gl_texture_bind__: GL tex = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	jl_gl_texture__bind__(jl_gr, tex);
}

// Unbind a texture
static void jl_gl_texture_off__(jl_gr_t* jl_gr) {
	jl_gl_texture__bind__(jl_gr, 0);
}

// Make & Bind a new texture.
static void jl_gl_texture_new__(jl_gr_t* jl_gr, m_u32_t *tex, u8_t* px,
	u16_t w, u16_t h, u8_t bytepp)
{
	jl_io_function(jl_gr->jlc, "jl_gl_texture_new__");
	jl_io_print(jl_gr->jlc, "make tx");
	// Make the texture
	jl_gl_texture_make__(jl_gr, tex);
	// Bind the texture
	jl_gl_texture_bind__(jl_gr, *tex);
	jl_io_print(jl_gr->jlc, "set tx");
	// Set texture
	jl_gl_texture_set__(jl_gr, px, w, h, bytepp);
	jl_io_print(jl_gr->jlc, "set txpar");
	// Set the texture parametrs.
	jl_gl_texpar_set__(jl_gr);
	jl_io_print(jl_gr->jlc, "setd tx");
	jl_io_return(jl_gr->jlc, "jl_gl_texture_new__");
}

/*
#if JL_GLRTEX == JL_GLRTEX_EGL
#elif JL_GLRTEX == JL_GLRTEX_SDL
#else
// Make & Bind a new depth buffer.
static void jl_gl_depthbuffer_new__(jl_gr_t* jl_gr,m_u32_t*db ,u16_t w,u16_t h) {
	// Make the depth buffer.
	_jl_gl_depthbuffer_make(jl_gr, db);
	// Bind the depth buffer
	_jl_gl_depthbuffer_bind(jl_gr, *db);
	// Set the depth buffer
	jl_gl_depthbuffer_set__(jl_gr, w, h);
}
#endif
*/
// Make a texture - doesn't free "pixels"
void jl_gl_maketexture(jl_gr_t* jl_gr, uint16_t gid, uint16_t id,
	void* pixels, int width, int height, u8_t bytepp)
{
	jl_io_function(jl_gr->jlc, "GL_MkTex");
	if (!pixels) {
		jl_io_print(jl_gr->jlc, "null pixels");
		jl_sg_kill(jl_gr->jlc);
	}
	if (jl_gr->gl.allocatedg < gid + 1) {
		jl_gr->gl.textures =
			realloc(jl_gr->gl.textures,
				sizeof(uint32_t *) * (gid+1));
		jl_gr->gl.tex.uniforms.textures =
			realloc(jl_gr->gl.tex.uniforms.textures,
				sizeof(GLint *) * (gid+1));
		jl_gr->gl.allocatedg = gid + 1;
		jl_gr->gl.allocatedi = 0;
		jl_gr->gl.textures[gid] = NULL;
		jl_gr->gl.tex.uniforms.textures[gid] = NULL;
	}
	if (jl_gr->gl.allocatedi < id + 1) {
		jl_gr->gl.textures[gid] =
			realloc(jl_gr->gl.textures[gid],
				sizeof(uint32_t) * (id+1));
		jl_gr->gl.tex.uniforms.textures[gid] =
			realloc(jl_gr->gl.tex.uniforms.textures[gid],
				sizeof(GLint) * (id+1));
		jl_gr->gl.allocatedi = id + 1;
	}
	jl_io_print(jl_gr->jlc, "generating texture (%d,%d)",width,height);
	// Make the texture.
	jl_gl_texture_new__(jl_gr, &jl_gr->gl.textures[gid][id], pixels, width,
		height, bytepp);
	jl_io_return(jl_gr->jlc, "GL_MkTex");
}

//Lower Level Stuff
static void _jl_gl_usep(jl_gr_t* jl_gr, GLuint prg) {
	if(!prg) {
		jl_io_print(jl_gr->jlc, ":program ain't a prg!");
		jl_sg_kill(jl_gr->jlc);
	}
	glUseProgram(prg);
	JL_GL_ERROR(jl_gr, prg, "glUseProgram");
}

static void _jl_gl_setalpha(jl_gr_t* jl_gr, float a) {
	glUniform1f(jl_gr->gl.tex.uniforms.multiply_alpha, a);
	JL_GL_ERROR(jl_gr, 0,"glUniform1f");
}

static void jl_gl_uniform3f__(jl_gr_t* jl_gr, GLint uv, float x, float y, float z)
{
	glUniform3f(uv, x, y, z);
	JL_GL_ERROR(jl_gr, 0,"glUniform3f");
}

static void jl_gl_uniform4f__(jl_gr_t* jl_gr, GLint uv, float x, float y, float z,
	float w)
{
	glUniform4f(uv, x, y, z, w);
	JL_GL_ERROR(jl_gr, 0,"glUniform4f");
}

//This pushes VBO "buff" up to the shader's vertex attribute "vertexAttrib"
//Set xyzw to 2 if 2D coordinates 3 if 3D. etc.
void _jl_gl_setv(jl_gr_t* jl_gr, uint32_t buff, uint32_t vertexAttrib, uint8_t xyzw) {
	// Bind Buffer
	jl_gl_buffer_use__(jl_gr, buff);
	// Set Vertex Attrib Pointer
	glEnableVertexAttribArray(vertexAttrib);
	JL_GL_ERROR(jl_gr, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,	// attribute
		xyzw,		// x+y+z = 3
		GL_FLOAT,	// type
		GL_FALSE,	// normalized?
		0,		// stride
		0		// array buffer offset
	);
	JL_GL_ERROR(jl_gr, 0,"glVertexAttribPointer");
}

static void _jl_gl_draw_arrays(jl_gr_t* jl_gr, GLenum mode, uint8_t count) {
	glDrawArrays(mode, 0, count);
	JL_GL_ERROR(jl_gr, 0,"glDrawArrays");
}

static inline void _jl_gl_init_disable_extras(jl_gr_t* jl_gr) {
	glDisable(GL_DEPTH_TEST);
	JL_GL_ERROR(jl_gr, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable(GL_DITHER);
	JL_GL_ERROR(jl_gr, 0, "glDisable(GL_DITHER)");
}

static inline void _jl_gl_init_enable_alpha(jl_gr_t* jl_gr) {
	glEnable(GL_BLEND);
	JL_GL_ERROR(jl_gr, 0,"glEnable( GL_BLEND )");
//	glEnable(GL_CULL_FACE);
//	JL_GL_ERROR(jl_gr, 0,"glEnable( GL_CULL_FACE )");
	glBlendColor(0.f,0.f,0.f,0.f);
	JL_GL_ERROR(jl_gr, 0,"glBlendColor");
	glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
//	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
//		GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	JL_GL_ERROR(jl_gr, 0,"glBlendFunc");
}

// Copy & Push vertices to a VBO.
static void jl_gl_vertices__(jl_gr_t* jl_gr, const float *xyzw, uint8_t vertices,
	float* cv, u32_t gl)
{
	u16_t items = (vertices*3);

	// Copy Vertices
	jl_me_copyto(xyzw, cv, items * sizeof(float));
	// Copy Buffer Data "cv" to Buffer "gl"
	jl_gl_buffer_set__(jl_gr, gl, cv, items);
}

void jl_gl_vo_vertices(jl_gr_t* jl_gr, jl_vo_t* pv, const float *xyzw,
	uint8_t vertices)
{
	pv->vc = vertices;
	if(vertices) {
		// Free pv->cv if non-null
		if(pv->cv) jl_me_alloc(jl_gr->jlc, (void**)&pv->cv, 0, 0);
		// Allocate pv->cv
		jl_me_alloc(jl_gr->jlc,
			(void**)&pv->cv, vertices * sizeof(float) * 3, 0);
		// Set pv->cv & pv->gl
		jl_gl_vertices__(jl_gr, xyzw, vertices, pv->cv, pv->gl);
	}
}

void jl_gl_vo_free(jl_gr_t* jl_gr, jl_vo_t *pv) {
	// Free GL VBO
	jl_gl_buffer_old__(jl_gr, &pv->gl);
	// Free GL Texture Buffer
	jl_gl_buffer_old__(jl_gr, &pv->bt);
	// Free Converted Vertices & Colors
	if(pv->cv) jl_me_alloc(jl_gr->jlc, (void**)&pv->cv, 0,0);
	if(pv->cc) jl_me_alloc(jl_gr->jlc, (void**)&pv->cc, 0,0);
	// Free main structure
	jl_me_alloc(jl_gr->jlc, (void**)&pv, 0,0);
}

static void _jl_gl_setp(jl_gr_t* jl_gr, jl_gl_slpr id) {
	if(jl_gr->gl.whichprg != id) {
		jl_gr->gl.whichprg = id;
		_jl_gl_usep(jl_gr, jl_gr->gl.prgs[id]);
	}
}

static void _jl_gl_col_begin(jl_gr_t* jl_gr, jl_vo_t* pv) {
	//Free anything old
	if(pv->cc != NULL) jl_me_alloc(jl_gr->jlc, (void**)&pv->cc, 0, 0);
}

// TODO: MOVE
void jl_gl_pbo_new(jl_gr_t* jl_gr, jl_tex_t* texture, u8_t* pixels,
	u16_t w, u16_t h, u8_t bpp)
{
	jl_gl_buffer_new__(jl_gr, &(texture->gl_buffer));
	jl_gl_texture_make__(jl_gr, &(texture->gl_texture));
	jl_gl_texture__bind__(jl_gr, texture->gl_texture);
	jl_gl_texpar_set__(jl_gr);
	jl_gl_texture_set__(jl_gr, pixels, w, h, bpp);
	jl_gl_buffer_use__(jl_gr, texture->gl_buffer);
	glBufferData(GL_ARRAY_BUFFER, w * h * 4, pixels, GL_DYNAMIC_DRAW);
	JL_GL_ERROR(jl_gr, 0, "jl_gl_pbo_set__: glBufferData");
}
// TODO: MOVE
void jl_gl_pbo_set(jl_gr_t* jl_gr, jl_tex_t* texture, u8_t* pixels,
	u16_t w, u16_t h, u8_t bpp)
{
	GLenum format = GL_RGBA;

	if(bpp == 3) format = GL_RGB;

	// Bind Texture &
	//jl_gl_texture_bind__(jl_gr, jl_gr->gl.cp->tx);
	// Copy to texture.
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
	//	jl_gr->gl.cp->w, jl_gr->gl.cp->h,
	//	format, GL_UNSIGNED_BYTE, pixels);
	//JL_GL_ERROR(jl_gr, 0, "jl_gl_pbo_set__: glTexSubImage2D");

	jl_gl_texture__bind__(jl_gr, texture->gl_texture);
	jl_gl_buffer_use__(jl_gr, texture->gl_buffer);

	glBufferSubData(GL_ARRAY_BUFFER, 0, w * h * 4, pixels);
	JL_GL_ERROR(jl_gr, 0, "jl_gl_pbo_set__: glBufferData");

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		w, h, format, GL_UNSIGNED_BYTE, pixels);
	JL_GL_ERROR(jl_gr, 0, "jl_gl_pbo_set__: glTexSubImage2D");
}

static void jl_gl_framebuffer_free__(jl_gr_t* jl_gr, m_u32_t *fb) {
	glDeleteFramebuffers(1, fb);
	JL_GL_ERROR(jl_gr, *fb, "glDeleteFramebuffers");
	*fb = 0;
}

static void jl_gl_framebuffer_make__(jl_gr_t* jl_gr, m_u32_t *fb) {
	glGenFramebuffers(1, fb);
	if(!(*fb)) {
		jl_io_print(jl_gr->jlc, "jl_gl_framebuffer_make__: GL FB = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	JL_GL_ERROR(jl_gr, *fb,"glGenFramebuffers");
}

static void jl_gl_framebuffer_use__(jl_gr_t* jl_gr, u32_t fb, u32_t db, u32_t tx,
	u16_t w, u16_t h)
{
	jl_io_function(jl_gr->jlc, "jl_gl_framebuffer_use__");
	if(fb == 0) {
		jl_io_print(jl_gr->jlc, "jl_gl_framebuffer_use__: GL FB = 0");
		jl_sg_kill(jl_gr->jlc);
//	}else if(db == 0) {
//		jl_io_print(jl_gr->jlc, "jl_gl_framebuffer_use__: GL DB = 0");
//		jl_sg_kill(jl_gr->jlc);
	}else if(tx == 0) {
		jl_io_print(jl_gr->jlc, "jl_gl_framebuffer_use__: GL TX = 0");
		jl_sg_kill(jl_gr->jlc);
	}else if(w == 0) {
		jl_io_print(jl_gr->jlc, "jl_gl_framebuffer_use__: GL W = 0");
		jl_sg_kill(jl_gr->jlc);
	}else if(h == 0) {
		jl_io_print(jl_gr->jlc, "jl_gl_framebuffer_use__: GL H = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	// Bind the texture.
	jl_gl_texture_bind__(jl_gr, tx);
	// Bind the depthbuffer.
	//_jl_gl_depthbuffer_bind(jl_gr, db);
	// Bind the framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	JL_GL_ERROR(jl_gr, fb,"glBindFramebuffer");
	// Render on the whole framebuffer [ lower left -> upper right ]
	_jl_gl_viewport(jl_gr, w, h);
	jl_io_return(jl_gr->jlc, "jl_gl_framebuffer_use__");
}

static void jl_gl_framebuffer_off__(jl_gr_t* jl_gr) {
	// Unbind the texture.
	jl_gl_texture_off__(jl_gr);
	// Unbind the depthbuffer.
	// jl_gl_depthbuffer_off__(jl_gr);
	// Unbind the framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	JL_GL_ERROR(jl_gr, 0,"jl_gl_framebuffer_off__: glBindFramebuffer");
	// Render to the whole screen.
	jl_gl_viewport_screen(jl_gr);
}

// add a texture to a framebuffer object.
static void jl_gl_framebuffer_addtx__(jl_gr_t* jl_gr, u32_t tx) {
	// Set "*tex" as color attachment #0.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, tx, 0);
	JL_GL_ERROR(jl_gr, tx,"jl_gl_framebuffer_addtx: glFramebufferTexture2D");
}

// add a depthbuffer to a framebuffer object.
static void jl_gl_framebuffer_adddb__(jl_gr_t* jl_gr, u32_t db) {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, db);
	JL_GL_ERROR(jl_gr, db,"make pr: glFramebufferRenderbuffer");
}

static void jl_gl_framebuffer_status__(jl_gr_t* jl_gr) {
	// Check to see if framebuffer was made properly.
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		jl_io_print(jl_gr->jlc, "Frame buffer not complete!");
		jl_sg_kill(jl_gr->jlc);
	}
}

// Create a new framebuffer with "tx" texture and "db" depthbuffer.
static inline void jl_gl_framebuffer_new__(jl_gr_t* jl_gr, m_u32_t* fb, u32_t tx,
	u32_t db, u16_t w, u16_t h)
{
	jl_gl_framebuffer_make__(jl_gr, fb);
	jl_gl_framebuffer_use__(jl_gr, *fb, db, tx, w, h);
	// Attach depth and texture buffer.
	jl_gl_framebuffer_addtx__(jl_gr, tx);
	jl_gl_framebuffer_adddb__(jl_gr, db);
	jl_gl_framebuffer_status__(jl_gr);
}

static void _jl_gl_texture_free(jl_gr_t* jl_gr, uint32_t *tex) {
	glDeleteTextures(1, tex);
	JL_GL_ERROR(jl_gr, 0, "_jl_gl_texture_free: glDeleteTextures");
	*tex = 0;
}

#if JL_GLRTEX == JL_GLRTEX_EGL
#ifdef JL_DEBUG_LIB
static void jl_gl_egl_geterror__(jl_gr_t* jl_gr, i32_t x, str_t fname) {
	EGLint err = eglGetError();
	
	if(err == EGL_SUCCESS) return;
	
	jl_io_print(jl_gr->jlc, ":source: paraphrased from www.khronos.org:");
	jl_io_print(jl_gr->jlc, "\"%s\"", fname);

	if(err == EGL_NOT_INITIALIZED)
		jl_io_print(jl_gr->jlc,"EGL couldn't initialize or isn't init'd");
	else if(err == EGL_BAD_ACCESS)
		jl_io_print(jl_gr->jlc,"EGL can't access a resource. (Threads?)");
	else if(err == EGL_BAD_ALLOC)
		jl_io_print(jl_gr->jlc,"EGL failed to allocate resources.");
	else if(err == EGL_BAD_ATTRIBUTE)
		jl_io_print(jl_gr->jlc,
			"Attribute not recognized (value or/ list).");
	else if(err == EGL_BAD_CONFIG)
		jl_io_print(jl_gr->jlc, "EGLConfig argument is invalid.");
	else if(err == EGL_BAD_CONTEXT)
		jl_io_print(jl_gr->jlc, "EGLContext argument is invalid.");
	else if(err == EGL_BAD_CURRENT_SURFACE)
		jl_io_print(jl_gr->jlc, "Surface passed is invalid.");
	else if(err == EGL_BAD_DISPLAY)
		jl_io_print(jl_gr->jlc, "EGLDisplay argument is invalid.");
	else if(err == EGL_BAD_MATCH)
		jl_io_print(jl_gr->jlc, "EGLContext requires buffers not alloc'd.");
	else if(err == EGL_BAD_NATIVE_PIXMAP)
		jl_io_print(jl_gr->jlc, "EGLNativePixmapType argument is invalid.");
	else if(err == EGL_BAD_NATIVE_WINDOW)
		jl_io_print(jl_gr->jlc, "EGLNativeWindowType argument is invalid.");
	else if(err == EGL_BAD_PARAMETER)
		jl_io_print(jl_gr->jlc, "1 or more argument values are invalid.");
	else if(err == EGL_BAD_SURFACE)
		jl_io_print(jl_gr->jlc, "EGLSurface argument is an invalid surface.");
	else if(err == EGL_CONTEXT_LOST)
		jl_io_print(jl_gr->jlc, "A power management event has occurred.");
	else
		jl_io_print(jl_gr->jlc, "Unknown error.");
	jl_io_print(jl_gr->jlc, ":%d", (void*)jl_me_strt_fnum(x)->data);
	jl_sg_kill(jl_gr->jlc);
}

#endif // JL_DEBUG_LIB

static void jl_gl_pixelbuffer_old__(jl_gr_t* jl_gr, EGLSurface pb) {
	eglDestroySurface(eglDisp, pb);
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_pixelbuffer_old__()");
}

static EGLSurface jl_gl_pixelbuffer_new__(jl_gr_t* jl_gr,u16_t w,u16_t h){
	const EGLint surfaceAttr[] = {
		EGL_WIDTH, w,
		EGL_HEIGHT, h,
		EGL_NONE
	};
	
	// Create & return a pixelbuffer surface
	return eglCreatePbufferSurface(eglDisp, eglConf, surfaceAttr);
	JL_EGL_ERROR(jl_gr, w, "jl_gl_pixelbuffer_new__()");
}

static void jl_gl_pixelbuffer_use__(jl_gr_t* jl_gr, EGLSurface pb) {
	eglMakeCurrent(eglDisp, pb, pb, eglCtx);
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_pixelbuffer_use__()");
}

static void jl_gl_pixelbuffer_off__(jl_gr_t* jl_gr) {
	eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE,
		EGL_NO_CONTEXT);
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_pixelbuffer_off__()");
}

#elif JL_GLRTEX == JL_GLRTEX_SDL
#else // !JL_GLRTEX
/*
static void _jl_gl_depthbuffer_free(jl_gr_t* jl_gr, uint32_t *db) {
	glDeleteRenderbuffers(1, db);
	JL_GL_ERROR(jl_gr,*db,"_jl_gl_depthbuffer_free: glDeleteRenderbuffers");
	*db = 0;
}

static void _jl_gl_depthbuffer_make(jl_gr_t* jl_gr, uint32_t *db) {
	glGenRenderbuffers(1, db);
	if(!(*db)) {
		jl_io_print(jl_gr->jlc, "_jl_gl_depthbuffer_make: GL buff=0");
		jl_sg_kill(jl_gr->jlc);
	}
	JL_GL_ERROR(jl_gr,*db,"make pr: glGenRenderbuffers");
}

static void jl_gl_depthbuffer_set__(jl_gr_t* jl_gr, u16_t w, u16_t h) {
	if(!w) {
		jl_io_print(jl_gr->jlc, "jl_gl_depthbuffer_set: w = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	if(!h) {
		jl_io_print(jl_gr->jlc, "jl_gl_depthbuffer_set: h = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	JL_GL_ERROR(jl_gr, w, "make pr: glRenderbufferStorage");
}

static void _jl_gl_depthbuffer_bind(jl_gr_t* jl_gr, uint32_t db) {
	if(db == 0) {
		jl_io_print(jl_gr->jlc, "_jl_gl_depthbuffer_bind: GL db = 0");
		jl_sg_kill(jl_gr->jlc);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, db);
	JL_GL_ERROR(jl_gr, db,"_jl_gl_depthbuffer_bind: glBindRenderbuffer");
}

static void jl_gl_depthbuffer_off__(jl_gr_t* jl_gr) {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	JL_GL_ERROR(jl_gr, 0,"jl_gl_depthbuffer_off__: glBindRenderbuffer");
}
*/
#endif // JL_GLRTEX

static void _jl_gl_viewport(jl_gr_t* jl_gr, uint16_t w, uint16_t h) {
	glViewport(0, 0, w, h);
	JL_GL_ERROR(jl_gr, w * h, "glViewport");
}

static void _jl_gl_pr_unuse(jl_gr_t* jl_gr) {
	// Render to the screen
	#if JL_GLRTEX == JL_GLRTEX_EGL
		jl_gl_pixelbuffer_off__(jl_gr);
	#elif JL_GLRTEX == JL_GLRTEX_SDL
		jl_gl_pbo_off__(jl_gr);
	#else
		jl_gl_framebuffer_off__(jl_gr);
	#endif
	// Reset the aspect ratio.
	jl_gr->gl.cp = NULL;
}

static void _jl_gl_pr_obj_free(jl_gr_t* jl_gr, jl_pr_t *pr) {
	_jl_gl_texture_free(jl_gr, &(pr->tx));
#if JL_GLRTEX == JL_GLRTEX_EGL
	jl_gl_pixelbuffer_old__(jl_gr, pr->pb);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	jl_me_alloc(jl_gr->jlc, (void**)&pr->px, 0, 4 * pr->w * pr->h);
#else
	jl_gl_framebuffer_free__(jl_gr, &(pr->fb));
//	_jl_gl_depthbuffer_free(jl_gr, &(pr->db));
#endif
}

static void jl_gl_pr_obj_make_tx__(jl_gr_t* jl_gr, jl_pr_t *pr) {
	// Make a new texture for pre-renderering.  The "NULL" sets it blank.
	jl_gl_texture_new__(jl_gr, &(pr->tx), NULL, pr->w, pr->h, 0);
	jl_gl_texture_off__(jl_gr);
}

// Initialize an already allocated pr object with width and hieght of "pr->w" &
// "pr->h".
static void _jl_gl_pr_obj_make(jl_gr_t* jl_gr, jl_pr_t *pr) {
	jl_gr->gl.cp = NULL;
	if(pr->w < 1) {
		jl_io_print(jl_gr->jlc,
			"_jl_gl_pr_obj_make() failed: 'w' must be more than 1");
		jl_sg_kill(jl_gr->jlc);
	}
	if(pr->h < 1) {
		jl_io_print(jl_gr->jlc,
			"_jl_gl_pr_obj_make() failed: 'h' must be more than 1");
		jl_sg_kill(jl_gr->jlc);
	}
	if((pr->w > GL_MAX_TEXTURE_SIZE) || (pr->h > GL_MAX_TEXTURE_SIZE)) {
		jl_io_print(jl_gr->jlc, "_jl_gl_pr_obj_make() failed:");
		jl_io_print(jl_gr->jlc, "w = %d,h = %d", pr->w, pr->h);
		jl_io_print(jl_gr->jlc, "texture is too big for graphics card.");
		jl_sg_kill(jl_gr->jlc);
	}
#if JL_GLRTEX == JL_GLRTEX_EGL
	// Make the texture.
	jl_gl_pr_obj_make_tx__(jl_gr, pr);
	// Make a new Pixelbuffer.
	pr->pb = jl_gl_pixelbuffer_new__(jl_gr, pr->w, pr->h);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	// Make the texture.
	jl_gl_pr_obj_make_tx__(jl_gr, pr);
	// Make a new Hidden Window.
	pr->sw = jl_dl_screen_new_(jl_gr, pr->w, pr->h);
	// Allocate pixel space
	jl_me_alloc(jl_gr->jlc, (void**)&pr->px, 4 * pr->w * pr->h, 0);
	// Use the Pre-renderer.
	printf("jl_gl_pbo_use__: %dx%d\n", pr->w, pr->h);
	jl_gl_pbo_use__(jl_gr, pr);
#else
	// Make the texture.
	jl_gl_pr_obj_make_tx__(jl_gr, pr);
	// Make a new Depthbuffer.
	//jl_gl_depthbuffer_new__(jl_gr, &(pr->db), pr->w, pr->h);
	//jl_gl_depthbuffer_off__(jl_gr);
	// Make & Bind a new Framebuffer.
	jl_gl_framebuffer_new__(jl_gr, &(pr->fb), pr->tx, pr->db, pr->w, pr->h);
	// Set Viewport to image and clear.
	_jl_gl_viewport(jl_gr, pr->w, pr->h);
#endif
	// Clear the pre-renderer.
	jl_gl_clear(jl_gr, 0, 0, 0, 0);
	// Unbind Pixelbuffer or Framebuffer.
#if JL_GLRTEX == JL_GLRTEX_EGL
	jl_gl_pixelbuffer_off__(jl_gr);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	jl_gl_pbo_off__(jl_gr);
#else
	jl_gl_framebuffer_off__(jl_gr);
#endif
	// De-activate pre-renderer.
	jl_gl_pr_scr(jl_gr);
}

static void _jl_gl_txtr(jl_gr_t* jl_gr, jl_vo_t** pv, uint8_t a, uint8_t is_rt) {
	if((*pv) == NULL) (*pv) = jl_gr->gl.temp_vo;
	// Set Simple Variabes
	(*pv)->a = ((float)a) / 255.f;
	// Make sure non-textured colors aren't attempted
	if((!is_rt) && ((*pv)->cc != NULL))
		jl_me_alloc(jl_gr->jlc, (void**)&((*pv)->cc), 0,0);
}

static inline void _jl_gl_set_shader(jl_gr_t* jl_gr, jl_vo_t* pv) {
	_jl_gl_setp(jl_gr, pv->cc == NULL ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
}

// Prepare to draw a solid color
static inline void _jl_gl_draw_colr(jl_gr_t* jl_gr, jl_vo_t* pv) {
	// Bind Colors to shader
	_jl_gl_setv(jl_gr, pv->bt, jl_gr->gl.clr.attr.acolor, 4);
}

// Prepare to draw a texture with texture coordinates "tc". 
static void _jl_gl_draw_txtr(jl_gr_t* jl_gr, f32_t a, u32_t tx, u32_t tc) {
	jl_io_function(jl_gr->jlc, "OPENGL/Draw Texture");
	// Bind Texture Coordinates to shader
	_jl_gl_setv(jl_gr, tc, jl_gr->gl.tex.attr.texpos, 2);
	// Set Alpha Value In Shader
	_jl_gl_setalpha(jl_gr, a);
	// Bind the texture
	jl_gl_texture_bind__(jl_gr, tx);
	jl_io_return(jl_gr->jlc, "OPENGL/Draw Texture");
}

static void jl_gl_draw_vertices(jl_gr_t* jl_gr, u32_t gl, i32_t attr) {
	_jl_gl_setv(jl_gr, gl, attr, 3);
}

static void jl_gl_draw_final__(jl_gr_t* jl_gr, u8_t rs, u32_t vc) {
	_jl_gl_draw_arrays(jl_gr, rs ? GL_TRIANGLES : GL_TRIANGLE_FAN, vc);
}

static void _jl_gl_draw_onts(jl_gr_t* jl_gr, u32_t gl, u8_t rs, u32_t vc) {
	// Update the position variable in shader.
	jl_gl_draw_vertices(jl_gr, gl, (jl_gr->gl.whichprg == JL_GL_SLPR_TEX) ?
		jl_gr->gl.tex.attr.position : jl_gr->gl.clr.attr.position);
	// Draw the image on the screen!
	jl_gl_draw_final__(jl_gr, rs, vc);
}

/************************/
/***  ETOM Functions  ***/
/************************/

// Set the viewport to the screen size.
void jl_gl_viewport_screen(jl_gr_t* jl_gr) {
	_jl_gl_viewport(jl_gr, jl_gr->dl.full_w, jl_gr->dl.full_h);
}

void jl_gl_pr_old_(jl_gr_t* jl_gr, jl_pr_t** pr) {
	// 
	_jl_gl_pr_obj_free(jl_gr, *pr);
	// Free old pr object.
	jl_me_alloc(jl_gr->jlc, (void**)pr, 0, 0);
}

// Free a pr for a vertex object
void jl_gl_pr_old(jl_gr_t* jl_gr, jl_vo_t* pv) {
	if(pv->pr == NULL) {
		jl_io_print(jl_gr->jlc, "pr(): double free!");
		jl_sg_kill(jl_gr->jlc);
	}
	jl_gl_pr_old_(jl_gr, &(pv->pr));
}

uint8_t jl_gl_pr_isi_(jl_gr_t* jl_gr, jl_pr_t* pr) {
	if(pr) {
#if JL_GLRTEX == JL_GLRTEX_EGL
		return !!pr->tx; // Return 0 if 0, 1 if nonzero
#elif JL_GLRTEX == JL_GLRTEX_SDL
		return !!pr->tx;
#else
		if(pr->tx && /*pr->db &&*/ pr->fb) {
			return 1;
		}else if(/*(!pr->db) && */(!pr->fb)) {
			return 0;
		}else{
			if(pr->tx)
				jl_io_printc(jl_gr->jlc, "[OK] pr->tx\n");
			else jl_io_printc(jl_gr->jlc, "[FAIL] pr->tx\n");
//			if(pr->db)
//				jl_io_printc(jl_gr->jlc, "[OK] pr->db\n");
//			else jl_io_printc(jl_gr->jlc, "[FAIL] pr->db\n");
			if(pr->fb)
				jl_io_print(jl_gr->jlc, "[OK] pr->fb");
			else jl_io_print(jl_gr->jlc, "[FAIL] pr->fb");
			jl_sg_kill(jl_gr->jlc);
		}
#endif
	}else{
		return 0;
	}
	// Never reached
	return 2;
}

// Test if pre-renderer is initialized.
uint8_t jl_gl_pr_isi(jl_gr_t* jl_gr, jl_vo_t* pv) {
	if(pv)
		return jl_gl_pr_isi_(jl_gr, pv->pr);
	else
		return 2;
}

void jl_gl_pr_use_(jl_gr_t* jl_gr, jl_pr_t* pr) {
// Render to the framebuffer.
#if JL_GLRTEX == JL_GLRTEX_EGL
	jl_gl_pixelbuffer_use__(jl_gr, pr->pb);
#elif JL_GLRTEX == JL_GLRTEX_SDL
	jl_gl_pbo_use__(jl_gr, pr);
#else
	jl_gl_framebuffer_use__(jl_gr, pr->fb, pr->db, pr->tx, pr->w, pr->h);
#endif
	// Reset the aspect ratio.
	jl_gr->gl.cp = pr;
}

// Use a vertex object's pre-renderer for rendering.
void jl_gl_pr_use(jl_gr_t* jl_gr, jl_vo_t* pv) {
	pv->a = 1.f;
	if(!pv->pr) {
		jl_io_print(jl_gr->jlc,"jl_gl_pr_use: pre-renderer not created");
		jl_sg_kill(jl_gr->jlc);
	}
	jl_gl_pr_use_(jl_gr, pv->pr);
}

// Use the screens prerenderer.
void jl_gl_pr_scr(jl_gr_t* jl_gr) {
	if(jl_gr->gl.bg) jl_gl_pr_use_(jl_gr, jl_gr->gl.bg);
	else _jl_gl_pr_unuse(jl_gr);
}

// Set the screens prerenderer.
void jl_gl_pr_scr_set(jl_gr_t* jl_gr, jl_vo_t* vo) {
	if(vo) jl_gr->gl.bg = vo->pr;
	else jl_gr->gl.bg = NULL;
}

// Turn a pre-renderer off in order to draw to the screen.
void jl_gl_pr_off(jl_gr_t* jl_gr) {
	_jl_gl_pr_unuse(jl_gr);
}

// Set vertices for a polygon.
void jl_gl_poly(jl_gr_t* jl_gr, jl_vo_t* pv, uint8_t vertices, const float *xyzw) {
	const float FS_RECT[] = {
		0.,jl_gl_ar(jl_gr),0.,
		0.,0.,0.,
		1.,0.,0.,
		1.,jl_gl_ar(jl_gr),0.
	};

	if(pv == NULL) pv = jl_gr->gl.temp_vo;
	if(xyzw == NULL) xyzw = FS_RECT;
	// Rendering Style = polygon
	pv->rs = 0;
	// Set the vertices of vertex object "pv"
	jl_gl_vo_vertices(jl_gr, pv, xyzw, vertices);
}

// Set vertices for vector triangles.
void jl_gl_vect(jl_gr_t* jl_gr, jl_vo_t* pv, uint8_t vertices, const float *xyzw) {
	if(pv == NULL) pv = jl_gr->gl.temp_vo;
	// Rendering Style = triangles
	pv->rs = 1;
	// Set the vertices of vertex object "pv"
	jl_gl_vo_vertices(jl_gr, pv, xyzw, vertices);
}

// Set colors to "cc" in vertex oject "pv" - cc will be freed when done
void jl_gl_clrc(jl_gr_t* jl_gr, jl_vo_t* pv, jl_ccolor_t* cc) {
	_jl_gl_col_begin(jl_gr, pv); // Free "pv->cc" if non-null
	pv->cc = cc;
	// Set Color Buffer "pv->bt" to "pv->cc"
	jl_gl_buffer_set__(jl_gr, pv->bt, pv->cc, pv->vc * 4);
}

//Convert color to solid
jl_ccolor_t* jl_gl_clrcs(jl_gr_t* jl_gr, u8_t *rgba, uint32_t vc) {
	int i;
	jl_ccolor_t* cc = NULL;

	//Allocate memory
	jl_me_alloc(jl_gr->jlc,
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
jl_ccolor_t* jl_gl_clrcg(jl_gr_t* jl_gr, u8_t *rgba, uint32_t vc) {
	int i;
	jl_ccolor_t* cc = NULL;

	//Allocate memory
	jl_me_alloc(jl_gr->jlc, (void**)&cc, vc * sizeof(float) * 4, 0);
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
void jl_gl_clrg(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t *rgba) {
	if(pv == NULL) pv = jl_gr->gl.temp_vo;
	_jl_gl_col_begin(jl_gr, pv);
	jl_gl_clrc(jl_gr, pv, jl_gl_clrcg(jl_gr, rgba, pv->vc));
}

// Set Texturing to Solid Color "rgba" { 4 values }
void jl_gl_clrs(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t *rgba) {
	if(pv == NULL) pv = jl_gr->gl.temp_vo;
	_jl_gl_col_begin(jl_gr, pv);
	jl_gl_clrc(jl_gr, pv, jl_gl_clrcs(jl_gr, rgba, pv->vc));
}

// Set texturing to a bitmap
void jl_gl_txtr(jl_gr_t* jl_gr,jl_vo_t* pv,u8_t map,u8_t a,u16_t pgid,u16_t pi){
	_jl_gl_txtr(jl_gr, &pv, a, 0);
	pv->tx = jl_gr->gl.textures[pgid][pi];
	if(!pv->tx) {
		jl_io_print(jl_gr->jlc, "Error: No Texture @%d/%d", pgid, pi);
		exit(-1);
	}
	jl_gl_vo_txmap(jl_gr, pv, map);
}

// Set texturing to a bitmap
void jl_gl_txtr_(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t map, u8_t a, u32_t tx) {
	_jl_gl_txtr(jl_gr, &pv, a, 0);
	pv->tx = tx;
	jl_gl_vo_txmap(jl_gr, pv, map);
}

//TODO:MOVE
// Shader true if texturing, false if coloring
// X,Y,Z are all [0. -> 1.]
// X,Y are turned into [-.5 -> .5] - center around zero.
static void jl_gl_translate__(jl_gr_t* jl_gr, i32_t shader, i8_t which, float x,
	float y, float z, f64_t ar)
{
	// Determine which shader to use
	_jl_gl_setp(jl_gr, which);
	// Set the uniforms
	glUniform3f(shader, x - (1./2.), y - (ar/2.), z);
	JL_GL_ERROR(jl_gr, 0,"glUniform3f - translate");	
}

//TODO:MOVE
static void jl_gl_transform__(jl_gr_t* jl_gr, i32_t shader, i8_t which, float x,
	float y, float z, f64_t ar)
{
	_jl_gl_setp(jl_gr, which);
	glUniform4f(shader, 2. * x, 2. * (y / ar), 2. * z, 1.f);
	JL_GL_ERROR(jl_gr, 0,"glUniform3f - transform");
}

void jl_gl_transform_pr_(jl_gr_t* jl_gr, jl_pr_t* pr, float x, float y, float z,
	float xm, float ym, float zm)
{
	f64_t ar = jl_gl_ar(jl_gr);

	if(!jl_gl_pr_isi_(jl_gr, pr)) {
		jl_io_print(jl_gr->jlc, "jl_gl_translate_pr: "
			"Pre-renderer Not Initialized!");
		jl_sg_kill(jl_gr->jlc);
	}
	jl_gl_translate__(jl_gr, jl_gr->gl.prm.uniforms.translate, JL_GL_SLPR_PRM,
		x, y, z, ar);
	jl_gl_transform__(jl_gr, jl_gr->gl.prm.uniforms.transform, JL_GL_SLPR_PRM,
		xm, ym, zm, ar);
}

void jl_gl_transform_vo_(jl_gr_t* jl_gr, jl_vo_t* vo, float x, float y, float z,
	float xm, float ym, float zm)
{
	f64_t ar = jl_gl_ar(jl_gr);
	if(vo == NULL) vo = jl_gr->gl.temp_vo;

	jl_gl_translate__(jl_gr, (vo->cc == NULL) ?
		jl_gr->gl.tex.uniforms.translate:jl_gr->gl.clr.uniforms.translate,
		(vo->cc == NULL) ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR,
		x, y, z, ar);
	jl_gl_transform__(jl_gr, (vo->cc == NULL) ?
		jl_gr->gl.tex.uniforms.transform:jl_gr->gl.clr.uniforms.transform,
		(vo->cc == NULL) ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR,
		xm, ym, zm, ar);
	// If it has a pre-renderer; transform it with.
	if(vo->pr) jl_gl_transform_pr_(jl_gr, vo->pr, x, y, z, xm, ym, zm);
}

void jl_gl_transform_chr_(jl_gr_t* jl_gr, jl_vo_t* vo, float x, float y, float z,
	float xm, float ym, float zm)
{
	f64_t ar = jl_gl_ar(jl_gr);
	if(vo == NULL) vo = jl_gr->gl.temp_vo;

	jl_gl_translate__(jl_gr, jl_gr->gl.chr.uniforms.translate, JL_GL_SLPR_CHR,
		x, y, z, ar);
	jl_gl_transform__(jl_gr, jl_gr->gl.chr.uniforms.transform, JL_GL_SLPR_CHR,
		xm, ym, zm, ar);
}

//Draw object with "vertices" vertices.  The vertex data is in "x","y" and "z".
//"map" refers to the charecter map.  0 means don't zoom in to one character.
//Otherwise it will zoom in x16 to a single charecter
/**
 * If "pv" is NULL then draw what's on the temporary buffer
 * Else render vertex object "pv" on the screen.
*/
void jl_gl_draw(jl_gr_t* jl_gr, jl_vo_t* pv) {
	jl_io_function(jl_gr->jlc, "OPENGL/Draw");
	// Use Temporary Vertex Object If no vertex object.
	if(pv == NULL) pv = jl_gr->gl.temp_vo;
	// Determine which shader to use: texturing or coloring?
	_jl_gl_set_shader(jl_gr, pv);
	// Set texture and transparency if texturing.  If colors: bind colors
	if(pv->cc) _jl_gl_draw_colr(jl_gr, pv);
	else _jl_gl_draw_txtr(jl_gr, pv->a, pv->tx, pv->bt);
	// Draw onto the screen.
	_jl_gl_draw_onts(jl_gr, pv->gl, pv->rs, pv->vc);
	jl_io_return(jl_gr->jlc, "OPENGL/Draw");
}

/**
 * If "pv" is NULL then draw what's on the temporary buffer
 * Else render vertex object "pv" on the screen.
*/
void jl_gl_draw_chr(jl_gr_t* jl_gr, jl_vo_t* pv,
	m_f32_t r, m_f32_t g, m_f32_t b, m_f32_t a)
{
	// Use Temporary Vertex Object If no vertex object.
	if(pv == NULL) pv = jl_gr->gl.temp_vo;
	// Set Shader
	_jl_gl_setp(jl_gr, JL_GL_SLPR_CHR);
	// Bind Texture Coordinates to shader
	_jl_gl_setv(jl_gr, pv->bt, jl_gr->gl.chr.attr.texpos, 2);
	// Set Alpha Value In Shader
	glUniform1f(jl_gr->gl.chr.uniforms.multiply_alpha, pv->a);
	JL_GL_ERROR(jl_gr, 0,"jl_gl_draw_chr: glUniform1f");
	// Set New Color In Shader
	jl_gl_uniform4f__(jl_gr, jl_gr->gl.chr.uniforms.new_color, r, g, b, a);
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, pv->tx);
	JL_GL_ERROR(jl_gr, pv->tx,"jl_gl_draw_chr: glBindTexture");
	// Update the position variable in shader.
	jl_gl_draw_vertices(jl_gr, pv->gl, jl_gr->gl.chr.attr.position);
	// Draw the image on the screen!
	jl_gl_draw_final__(jl_gr, pv->rs, pv->vc);
}

// Draw the pre-rendered texture.
void jl_gl_draw_pr_(jl_t* jlc, jl_pr_t* pr) {
	jl_gr_t* jl_gr = jlc->jl_gr;

	// Fail if no pre-rendered texture.
	if(!jl_gl_pr_isi_(jl_gr, pr)) {
		jl_io_print(jl_gr->jlc, "Pre-renderer Not Initialized!");
		jl_sg_kill(jlc);
	}
	// Use pre-mixed texturing shader.
	_jl_gl_setp(jl_gr, JL_GL_SLPR_PRM);
	// Bind Texture Coordinates to shader
	_jl_gl_setv(jl_gr, jl_gr->gl.default_tc, jl_gr->gl.prm.attr.texpos, 2);
	// Bind the texture
	jl_gl_texture_bind__(jl_gr, pr->tx);
	// Update the position variable in shader.
	_jl_gl_setv(jl_gr, pr->gl, jl_gr->gl.prm.attr.position, 3);
	// Draw the image on the screen!
	_jl_gl_draw_arrays(jl_gr, GL_TRIANGLE_FAN, 4);
}

int32_t _jl_gl_getu(jl_gr_t* jl_gr, GLuint prg, char *var) {
	int32_t a = 0;
	if((a = glGetUniformLocation(prg, var)) == -1) {
		jl_io_print(jl_gr->jlc, ":opengl: bad name; is: %s", var);
		jl_sg_kill(jl_gr->jlc);
	}
	JL_GL_ERROR(jl_gr, a,"glGetUniformLocation");
	return a;
}

void _jl_gl_geta(jl_gr_t* jl_gr, GLuint prg, m_i32_t *attrib, const char *title) {
	if((*attrib = glGetAttribLocation(prg, title)) == -1) {
		jl_io_print(jl_gr->jlc, 
			"attribute name is either reserved or non-existant");
		jl_sg_kill(jl_gr->jlc);
	}
}

/***	  @cond	   ***/
/************************/
/*** Static Functions ***/
/************************/

static inline void _jl_gl_init_setup_gl(jl_gr_t* jl_gr) {
	jl_io_print(jl_gr->jlc, "setting properties....");
	//Disallow Dither & Depth Test
	_jl_gl_init_disable_extras(jl_gr);
	//Set alpha=0 to transparent
	_jl_gl_init_enable_alpha(jl_gr);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	jl_io_print(jl_gr->jlc, "set glproperties.");
}

static inline void _jl_gl_init_shaders(jl_gr_t* jl_gr) {
	jl_io_print(jl_gr->jlc, "making GLSL programs....");
	jl_gr->gl.prgs[JL_GL_SLPR_PRM] = jl_gl_glsl_prg_create(jl_gr,
		source_vert_tex, source_frag_tex);
	jl_gr->gl.prgs[JL_GL_SLPR_TEX] = jl_gl_glsl_prg_create(jl_gr,
		source_vert_tex, source_frag_tex_premult);
	jl_gr->gl.prgs[JL_GL_SLPR_CHR] = jl_gl_glsl_prg_create(jl_gr,
		source_vert_tex, source_frag_tex_charmap);
	jl_gr->gl.prgs[JL_GL_SLPR_CLR] = jl_gl_glsl_prg_create(jl_gr,
		source_vert_clr, source_frag_clr);
	jl_io_print(jl_gr->jlc, "made programs.");

	jl_io_print(jl_gr->jlc, "setting up shaders....");
	if(jl_gr->gl.tex.uniforms.textures == NULL) {
		jl_io_print(jl_gr->jlc, "Couldn't create uniforms");
		jl_sg_kill(jl_gr->jlc);
	}
	// Texture
	jl_gr->gl.prm.uniforms.textures =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_PRM], "texture");
	jl_gr->gl.tex.uniforms.textures[0][0] =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX], "texture");
	jl_gr->gl.chr.uniforms.textures =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR], "texture");
	// Multipy alpha
	jl_gr->gl.tex.uniforms.multiply_alpha =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX],
			"multiply_alpha");
	jl_gr->gl.chr.uniforms.multiply_alpha =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR],
			"multiply_alpha");
	// Translate Vector
	jl_gr->gl.prm.uniforms.translate =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_PRM], "translate");
	jl_gr->gl.tex.uniforms.translate =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX], "translate");
	jl_gr->gl.chr.uniforms.translate =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR], "translate");
	jl_gr->gl.clr.uniforms.translate =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CLR], "translate");
	// Transform Vector
	jl_gr->gl.prm.uniforms.transform =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_PRM], "transform");
	jl_gr->gl.tex.uniforms.transform =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX], "transform");
	jl_gr->gl.chr.uniforms.transform =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR], "transform");
	jl_gr->gl.clr.uniforms.transform =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CLR], "transform");
	// New Color
	jl_gr->gl.chr.uniforms.new_color =
		_jl_gl_getu(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR], "new_color");
	//
	jl_io_print(jl_gr->jlc, "setting up prm shader attrib's....");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_PRM],
		&jl_gr->gl.prm.attr.position, "position");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_PRM],
		&jl_gr->gl.prm.attr.texpos, "texpos");
	jl_io_print(jl_gr->jlc, "setting up tex shader attrib's....");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX],
		&jl_gr->gl.tex.attr.position, "position");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX],
		&jl_gr->gl.tex.attr.texpos, "texpos");
	jl_io_print(jl_gr->jlc, "setting up chr shader attrib's....");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR],
		&jl_gr->gl.chr.attr.position, "position");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CHR],
		&jl_gr->gl.chr.attr.texpos, "texpos");
	jl_io_print(jl_gr->jlc, "setting up clr shader attrib's....");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CLR],
		&jl_gr->gl.clr.attr.position, "position");
	_jl_gl_geta(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CLR],
		&jl_gr->gl.clr.attr.acolor, "acolor");
	jl_io_print(jl_gr->jlc, "set up shaders.");
}

#if JL_GLRTEX == JL_GLRTEX_EGL
static inline void jl_gl_init_egl(jl_gr_t* jl_gr) {
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
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_init_egl:eglGetDisplay");
	eglInitialize(eglDisp, &eglMajVers, &eglMinVers);
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_init_egl:eglInitialize");

	printf("EGL init with version %d.%d", eglMajVers, eglMinVers);

	// choose the first config, i.e. best config
	eglChooseConfig(eglDisp, confAttr, &eglConf, 1, &numConfigs);
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_init_egl:eglChooseConfig");

	eglCtx = eglCreateContext(eglDisp, eglConf, EGL_NO_CONTEXT, ctxAttr);
	JL_EGL_ERROR(jl_gr, 0, "jl_gl_init_egl:eglCreateContext");
}
#endif

//Load and create all resources
static inline void _jl_gl_make_res(jl_gr_t* jl_gr) {
	jl_io_function(jl_gr->jlc, "GL_Init");
#if JL_GLRTEX == JL_GLRTEX_EGL
	// Initialize EGL.
	jl_gl_init_egl(jl_gr);
#endif
	// Get GL Extensions.
	// SDL_Log("GL_EXTENSIONS = \"%s\"\n", glGetString(GL_EXTENSIONS));
	// JL_GL_ERROR(jl_gr, 0, "glGetString()");
	// Setup opengl properties
	_jl_gl_init_setup_gl(jl_gr);
	// Create shaders and set up attribute/uniform variable communication
	_jl_gl_init_shaders(jl_gr);
	//
	jl_io_print(jl_gr->jlc, "making temporary vertex object....");
	jl_gr->gl.temp_vo = jl_gl_vo_make(jl_gr, 1);
	jl_io_print(jl_gr->jlc, "making default texc buff!");
	// Default GL Texture Coordinate Buffer
	jl_gl_buffer_new__(jl_gr, &(jl_gr->gl.default_tc));
	jl_gl_buffer_set__(jl_gr, jl_gr->gl.default_tc, DEFAULT_TC, 8);
	jl_io_print(jl_gr->jlc, "made temp vo & default tex. c. buff!");
	jl_io_return(jl_gr->jlc, "GL_Init");
}

static inline void _jl_gl_vo_make(jl_gr_t* jl_gr, jl_vo_t* vo, u32_t nc) {
	// How many more vo's will be made.
	vo->nc = nc;
	// GL VBO
	jl_gl_buffer_new__(jl_gr, &vo->gl);
	// GL Texture Coordinate Buffer
	jl_gl_buffer_new__(jl_gr, &vo->bt);
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

static void jl_gl_pr_set__(jl_pr_t *pr,f32_t w,f32_t h,u16_t w_px) {
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
jl_vo_t *jl_gl_vo_make(jl_gr_t* jl_gr, u32_t count) {
	jl_vo_t *rtn = NULL;
	m_u32_t i;

	// Allocate space in "rtn"
	jl_me_alloc(jl_gr->jlc, (void**)&rtn, sizeof(jl_vo_t) * count,0);
	// Make each vertex object.
	for(i = 0; i < count; i++) _jl_gl_vo_make(jl_gr, &rtn[i], (count-1) - i);
	// Return the vertex object[s].
	return rtn;
}

/**
 * Change the character map for a texture.
 * @param jlc: The library context.
 * @param pv: The vertext object to change.
 * @param map: The character value to map.
**/
void jl_gl_vo_txmap(jl_gr_t* jl_gr, jl_vo_t* pv, u8_t map) {
	if(map) {
		int32_t cX = map%16;
		int32_t cY = map/16;
		double CX = ((double)cX)/16.;
		double CY = ((double)cY)/16.;
		float tex1[] = {
			(DEFAULT_TC[0]/16.) + CX, (DEFAULT_TC[1]/16.) + CY,
			(DEFAULT_TC[2]/16.) + CX, (DEFAULT_TC[3]/16.) + CY,
			(DEFAULT_TC[4]/16.) + CX, (DEFAULT_TC[5]/16.) + CY,
			(DEFAULT_TC[6]/16.) + CX, (DEFAULT_TC[7]/16.) + CY
		};
		jl_gl_buffer_set__(jl_gr, pv->bt, tex1, 8);
	}else{
		jl_gl_buffer_set__(jl_gr, pv->bt, DEFAULT_TC, 8);
	}
}

/**
 * Get the Aspect Ratio for the pre-renderer in use.
 * @param jlc: The library context.
**/
double jl_gl_ar(jl_gr_t* jl_gr) {
	return jl_gr->gl.cp ? jl_gr->gl.cp->ar : jl_gr->dl.aspect;
}

/**
 * Clear the screen with a color
 * @param jlc: The library context.
 * @param r: The amount of red [ 0 - 255 ]
 * @param g: The amount of green [ 0 - 255 ]
 * @param b: The amount of blue [ 0 - 255 ]
 * @param a: The translucency [ 0 - 255 ]
**/
void jl_gl_clear(jl_gr_t* jl_gr, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	glClearColor(((double)r)/255., ((double)g)/255.,
		((double)b)/255., ((double)a)/255.);
	JL_GL_ERROR(jl_gr, a, "jl_gl_clear(): glClearColor");
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	JL_GL_ERROR(jl_gr, a, "jl_gl_clear(): glClear");
}

/**
 * Resize a prerenderer.
 * @param jlc: The library context.
 * @param pr: The pre-renderer to resize.
 * @param w: The display width. [ 0. - 1. ]
 * @param h: The display height. [ 0. - 1. ]
 * @param w_px: The resolution in pixels along the x axis [ 1- ]
**/
void jl_gl_pr_rsz(jl_gr_t* jl_gr, jl_pr_t *pr, f32_t w, f32_t h, u16_t w_px) {

	f32_t xyzw[] = {
		0.f,	h,	0.f,
		0.f,	0.f,	0.f,
		w,	0.f,	0.f,
		w,	h,	0.f
	};

	// Use the screen's pre-renderer if it exists.
	jl_gl_pr_scr(jl_gr);
	// Create the VBO.
	jl_gl_vertices__(jl_gr, xyzw, 4, pr->cv, pr->gl);
	// Set width, height and aspect ratio.
	jl_gl_pr_set__(pr, w, h, w_px);
	// Resize the actual texture.
	_jl_gl_pr_obj_free(jl_gr, pr);
	_jl_gl_pr_obj_make(jl_gr, pr);
}

/**
 * Make a new pre-renderer.
 * @param jlc: The library context.
 * @param w: The display width. [ 0. - 1. ]
 * @param h: The display height. [ 0. - 1. ]
 * @param w_px: The resolution in pixels along the x axis [ 1- ]
**/
jl_pr_t * jl_gl_pr_new(jl_gr_t* jl_gr, f32_t w, f32_t h, u16_t w_px) {
	jl_pr_t *pr = NULL;

	// Make the pr structure.
	jl_me_alloc(jl_gr->jlc, (void**)(&pr), sizeof(jl_pr_t), 0);
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
	jl_gl_pr_set__(pr, w, h, w_px);
	// Allocate pr->cv 
	jl_me_alloc(jl_gr->jlc, (void**)&pr->cv, 4*sizeof(float)*3, 0);
	// Make OpenGL Objects
	jl_gl_buffer_new__(jl_gr, &(pr->gl));
	_jl_gl_pr_obj_make(jl_gr, pr);
	// Resize the new pre-renderer.
	jl_gl_pr_rsz(jl_gr, pr, w, h, w_px);
	// Return the new pre-renderer.
	return pr;
}

/**
 * Draw a pre-rendered texture.
 * @param jlc: The library context.
 * @param pr: The pre-rendered texture.
 * @param vec: The vector of offset/translation.
 * @param scl: The scale factor.
**/
void jl_gl_pr_draw(jl_gr_t* jl_gr, jl_pr_t* pr, jl_vec3_t* vec, jl_vec3_t* scl) {
	if(pr == NULL) pr = jl_gr->gl.temp_vo->pr;
	if(vec == NULL) {
		jl_gl_transform_pr_(jl_gr, pr,
			0.f, 0.f, 0.f, 1., 1., 1.);
	}else if(scl == NULL) {
		jl_gl_transform_pr_(jl_gr, pr,
			vec->x, vec->y, vec->z, 1., 1., 1.);
	}else{
		jl_gl_transform_pr_(jl_gr, pr,
			vec->x, vec->y, vec->z, scl->x, scl->y, scl->z);	
	}
	jl_gl_draw_pr_(jl_gr->jlc, pr);
}

void jl_gl_pr(jl_gr_t* jl_gr, jl_pr_t * pr, jl_fnct par__redraw) {
	if(!pr) {
		jl_io_print(jl_gr->jlc, "Drawing on lost pre-renderer.");
		jl_sg_kill(jl_gr->jlc);
	}
	// Use the vo's pr
	jl_gl_pr_use_(jl_gr, pr);
	// Render to the pr.
	par__redraw(jl_gr->jlc);
	// Go back to the screen without clearing it.
	jl_gl_pr_scr(jl_gr);
}

/***	  @cond	   ***/
/************************/
/***  ETOM Functions  ***/
/************************/

void jl_gl_resz__(jl_gr_t* jl_gr) {
	// Deselect any pre-renderer.
	jl_gr->gl.cp = NULL;
	// Deselect the screen.
	jl_gl_pr_scr_set(jl_gr, NULL);
}

void jl_gl_init__(jl_gr_t* jl_gr) {
#ifdef JL_GLTYPE_HAS_GLEW
	printf(" [JLVM/GLEW] init\n");
	if(glewInit()!=GLEW_OK) {
		jl_io_print(jl_gr->jlc, "glew fail!(no sticky)");
		jl_sg_kill(jl_gr->jlc);
	}
#endif
	jl_gr->gl.cp = NULL;
	jl_gr->gl.bg = NULL;
	_jl_gl_make_res(jl_gr);
	//Set uniform values
	_jl_gl_usep(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_CLR]);
	jl_gl_uniform3f__(jl_gr, jl_gr->gl.clr.uniforms.translate, 0.f, 0.f, 0.f);
	jl_gl_uniform4f__(jl_gr, jl_gr->gl.clr.uniforms.transform, 1.f, 1.f, 1.f,
		1.f);
	_jl_gl_usep(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_PRM]);
	jl_gl_uniform3f__(jl_gr, jl_gr->gl.prm.uniforms.translate, 0.f, 0.f, 0.f);
	jl_gl_uniform4f__(jl_gr, jl_gr->gl.prm.uniforms.transform, 1.f, 1.f, 1.f,
		1.f);
	_jl_gl_usep(jl_gr, jl_gr->gl.prgs[JL_GL_SLPR_TEX]);
	jl_gl_uniform3f__(jl_gr, jl_gr->gl.tex.uniforms.translate, 0.f, 0.f, 0.f);
	jl_gl_uniform4f__(jl_gr, jl_gr->gl.tex.uniforms.transform, 1.f, 1.f, 1.f,
		1.f);
	//Textures on by default
	jl_gr->gl.whichprg = JL_GL_SLPR_TEX;
	// Make sure no pre-renderer is activated.
	_jl_gl_pr_unuse(jl_gr);
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
