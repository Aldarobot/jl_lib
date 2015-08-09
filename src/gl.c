#include "header/jl_pr.h"

// Shader Code

#ifdef GL_ES_VERSION_2_0
	#define GLSL_HEAD "#version 100\nprecision highp float;\n"
#else
	#define GLSL_HEAD "#version 100\n"
#endif

char *source_frag_clr = 
	GLSL_HEAD
	"varying vec4 vcolor;\n"
	"\n"
	"void main() {\n"
	"	gl_FragColor = vcolor;\n"
	"}\n\0";
	
char *source_vert_clr = 
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

char *source_frag_tex = 
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

char *source_vert_tex = 
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
	"	gl_Position = transform * vec4(position + translate, 1.0);\n"
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
	
// Full texture - small shrink to prevent bleeding textures.
static const float DEFAULT_TC[] = {
	0. + (1. / 1024.), 0. + (1. / 1024.),
	0. + (1. / 1024.), 1. - (1. / 1024.),
	1. - (1. / 1024.), 1. - (1. / 1024.),
	1. - (1. / 1024.), 0. + (1. / 1024.)
};

// Functions:

void _jl_gl_cerr(jvct_t *_jlc, int width, char *fname) {
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
	_jl_fl_errf(_jlc, ":");
	_jl_fl_errf(_jlc, fname);
	_jl_fl_errf(_jlc, "\n:");
	_jl_fl_errf(_jlc, fstrerr);
	_jl_fl_errf(_jlc, ":\n:");
	_jl_fl_errf(_jlc, (void*)jl_me_strt_fnum(width)->data);
	jl_sg_kill(_jlc->jlc, "\n");
}

static void jl_gl_buffer_use__(jvct_t *_jlc, uint32_t buffer) {
	// Check For Deleted Buffer
	if(buffer == 0) jl_sg_kill(_jlc->jlc, "buffer got deleted!");
	// bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	_jl_gl_cerr(_jlc, 0,"bind buffer");
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
	_jl_gl_cerr(_jlc, buffer,"buffer data");
}

static void jl_gl_buffer_new__(jvct_t *_jlc, uint32_t *buffer) {
	glGenBuffers(1, buffer);
	_jl_gl_cerr(_jlc, 0,"buffer gen");
	if(*buffer == 0) jl_sg_kill(_jlc->jlc, "buffer is made wrongly.");
}

static void jl_gl_buffer_old__(jvct_t *_jlc, uint32_t *buffer) {
	glDeleteBuffers(1, buffer);
	_jl_gl_cerr(_jlc, 0,"buffer free");
}

GLuint jl_gl_load_shader(jvct_t *_jlc, GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	_jl_gl_cerr(_jlc, 0,"couldn't create shader");
	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pSource, NULL);
		_jl_gl_cerr(_jlc, 0,"glShaderSource");
		glCompileShader(shader);
		_jl_gl_cerr(_jlc, 0,"glCompileShader");
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		_jl_gl_cerr(_jlc, 0,"glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			char* buf;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			_jl_gl_cerr(_jlc, 1,"glGetShaderiv");
			if (infoLen) {
				buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					_jl_fl_errf(_jlc, ":Could not compile shader:\n:");
					_jl_fl_errf(_jlc, buf);
					free(buf);
					jl_sg_kill(_jlc->jlc, "\n");
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
		jl_sg_kill(_jlc->jlc, ":couldn't load vertex shader\n");
	}

	GLuint pixelShader = jl_gl_load_shader(_jlc, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		jl_sg_kill(_jlc->jlc, ":couldn't load fragment shader\n");
	}

	GLuint program = glCreateProgram();
	_jl_gl_cerr(_jlc, 0,"glCreateProgram");
	if (program) {
		GLint linkStatus = GL_FALSE;

		glAttachShader(program, vertexShader);
		_jl_gl_cerr(_jlc, 0,"glAttachShader (vertex)");
		glAttachShader(program, pixelShader);
		_jl_gl_cerr(_jlc, 0,"glAttachShader (fragment)");
		glLinkProgram(program);
		_jl_gl_cerr(_jlc, 0,"glLinkProgram");
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		_jl_gl_cerr(_jlc, 0,"glGetProgramiv");
		glValidateProgram(program);
		_jl_gl_cerr(_jlc, 1,"glValidateProgram");
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			char* buf;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			_jl_gl_cerr(_jlc, 1,"glGetProgramiv");
			if (bufLength) {
				buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					_jl_fl_errf(_jlc, "Could not link program:\n");
					_jl_fl_errf(_jlc, buf);
					free(buf);
					jl_sg_kill(_jlc->jlc, "\n");
				}else{
					jl_sg_kill(_jlc->jlc,
						":failed malloc\n");
				}
			}else{
				glDeleteProgram(program);
				jl_sg_kill(_jlc->jlc, ":no info log\n");
			}
		}
	}
	if (program == 0) jl_sg_kill(_jlc->jlc, "Failed to load program");
	return program;
}

static void jl_gl_texture_make__(jvct_t* _jlc, uint32_t *tex) {
	glGenTextures(1, tex);
	if(!(*tex)) jl_sg_kill(_jlc->jlc, "jl_gl_texture_make__: GL tex = 0");
	_jl_gl_cerr(_jlc, 0, "jl_gl_texture_make__: glGenTextures");
}

// Set the bound texture.  pm is the pixels 0 - blank texture.
static void jl_gl_texture_set__(jvct_t* _jlc, u8_t* pm, u16_t w, u16_t h) {
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pm);
//	glTexImage2D(
//		GL_TEXTURE_2D, 0,		/* target, level */
//		GL_RGBA,			/* internal format */
//		width, height, 0,		/* width, height, border */
//		GL_RGBA, GL_UNSIGNED_BYTE,	/* external format, type */
//		pixels				/* pixels */
//	);
	_jl_gl_cerr(_jlc, 0,"texture image 2D");
}

static void jl_gl_texpar_set__(jvct_t* _jlc) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_jl_gl_cerr(_jlc, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	_jl_gl_cerr(_jlc, 1,"glTexParameteri");
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	_jl_gl_cerr(_jlc, 2,"glTexParameteri");
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	_jl_gl_cerr(_jlc, 3,"glTexParameteri");
}

// Bind a texture.
static void jl_gl_texture_bind__(jvct_t *_jlc, uint32_t tex) {
	if(tex == 0) jl_sg_kill(_jlc->jlc, "jl_gl_texture_bind__: GL tex = 0");
	glBindTexture(GL_TEXTURE_2D, tex);
	_jl_gl_cerr(_jlc, tex,"jl_gl_texture_bind__: glBindTexture");
}

// Unbind a texture
static void _jl_gl_texture_unbind(jvct_t *_jlc) {
	glBindTexture(GL_TEXTURE_2D, 0);
	_jl_gl_cerr(_jlc, 0,"_jl_gl_texture_unbind: glBindTexture");
}

// Make a new texture.
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

// Make a texture - doesn't free "pixels"
void jl_gl_maketexture(jl_t* jlc, uint16_t gid, uint16_t id,
	void* pixels, int width, int height)
{
	jvct_t *_jlc = jlc->_jlc;

	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "MKTX");
	if (!pixels)
		jl_sg_kill(jlc, "null pixels");
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
	if(!prg) jl_sg_kill(_jlc->jlc, ":program ain't a prg!\n");
	glUseProgram(prg);
	_jl_gl_cerr(_jlc, prg, "glUseProgram");
}

static void _jl_gl_setalpha(jvct_t *_jlc, float a) {
	glUniform1f(_jlc->gl.tex.uniforms.multiply_alpha, a);
	_jl_gl_cerr(_jlc, 0,"glUniform1f");
}

static void jl_gl_uniform3f__(jvct_t *_jlc, GLint uv, float x, float y, float z)
{
	glUniform3f(uv, x, y, z);
	_jl_gl_cerr(_jlc, 0,"glUniform3f");
}

static void jl_gl_uniform4f__(jvct_t *_jlc, GLint uv, float x, float y, float z,
	float w)
{
	glUniform4f(uv, x, y, z, w);
	_jl_gl_cerr(_jlc, 0,"glUniform4f");
}

//This pushes VBO "buff" up to the shader's vertex attribute "vertexAttrib"
//Set xyzw to 2 if 2D coordinates 3 if 3D. etc.
void _jl_gl_setv(jvct_t *_jlc, uint32_t buff, uint32_t vertexAttrib, uint8_t xyzw) {
	// Bind Buffer
	jl_gl_buffer_use__(_jlc, buff);
	// Set Vertex Attrib Pointer
	glEnableVertexAttribArray(vertexAttrib);
	_jl_gl_cerr(_jlc, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,  	// attribute
		xyzw,		// x+y+z = 3
		GL_FLOAT,	// type
		GL_FALSE,	// normalized?
		0,		// stride
		0		// array buffer offset
	);
	_jl_gl_cerr(_jlc, 0,"glVertexAttribPointer");
}

static void _jl_gl_draw_arrays(jvct_t *_jlc, GLenum mode, uint8_t count) {
	glDrawArrays(mode, 0, count);
	_jl_gl_cerr(_jlc, 0,"glDrawArrays");
}

static inline void _jl_gl_init_disable_extras(jvct_t *_jlc) {
	glDisable( GL_DEPTH_TEST);
	_jl_gl_cerr(_jlc, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable( GL_DITHER );
	_jl_gl_cerr(_jlc, 0, "glDisable(GL_DITHER)");
}

static inline void _jl_gl_init_enable_alpha(jvct_t *_jlc) {
	glEnable( GL_BLEND );
	_jl_gl_cerr(_jlc, 0,"glEnable( GL_BLEND )");
	glBlendColor(1.f,1.f,1.f,0.f);
	_jl_gl_cerr(_jlc, 0,"glBlendColor");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_jl_gl_cerr(_jlc, 0,"glBlendFunc");
}

// Convert & Push vertices to a VBO.
static void jl_gl_vertices__(jvct_t *_jlc, const float *xyzw, uint8_t vertices,
	float* cv, u32_t gl)
{
	m_u16_t i;

	for(i = 0; i < vertices*3; i+=3) {
		cv[i] = xyzw[i];
		cv[i+1] = xyzw[i+1];
		cv[i+2] = xyzw[i+2];
	}
	//Write Buffer Data "cv" to Buffer "gl"
	jl_gl_buffer_set__(_jlc, gl, cv, vertices * 3);
}

void jl_gl_vo_vertices(jvct_t *_jlc, jl_vo* pv, const float *xyzw,
	uint8_t vertices)
{
	pv->vc = vertices;
	if(vertices) {
		//Free pv->cv if non-null
		if(pv->cv) jl_me_alloc(_jlc->jlc, (void**)&pv->cv, 0, 0);
		//Allocate pv->cv
		jl_me_alloc(_jlc->jlc,
			(void**)&pv->cv, vertices * sizeof(float) * 3, 0);
	}
	jl_gl_vertices__(_jlc, xyzw, vertices, pv->cv, pv->gl);
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
		_jlc->gl.clr.attr.position : _jlc->gl.tex.attr.position,
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

static void _jl_gl_depthbuffer_free(jvct_t *_jlc, uint32_t *db) {
	glDeleteRenderbuffers(1, db);
	_jl_gl_cerr(_jlc,*db,"_jl_gl_depthbuffer_free: glDeleteRenderbuffers");
	*db = 0;
}

static void _jl_gl_depthbuffer_make(jvct_t *_jlc, uint32_t *db) {
	glGenRenderbuffers(1, db);
	if(!(*db)) jl_sg_kill(_jlc->jlc, "_jl_gl_depthbuffer_make: GL buff=0");
	_jl_gl_cerr(_jlc,*db,"make pr: glGenRenderbuffers");
}

static void _jl_gl_depthbuffer_bind(jvct_t *_jlc, uint32_t db) {
	if(db == 0) jl_sg_kill(_jlc->jlc, "_jl_gl_depthbuffer_bind: GL db = 0");
	glBindRenderbuffer(GL_RENDERBUFFER, db);
	_jl_gl_cerr(_jlc, db,"_jl_gl_depthbuffer_bind: glBindRenderbuffer");
}

static void _jl_gl_depthbuffer_unbind(jvct_t *_jlc) {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	_jl_gl_cerr(_jlc, 0,"_jl_gl_depthbuffer_unbind: glBindRenderbuffer");
}

static void _jl_gl_framebuffer_free(jvct_t *_jlc, uint32_t *fb) {
	glDeleteFramebuffers(1, fb);
	_jl_gl_cerr(_jlc, *fb, "glDeleteFramebuffers");
	*fb = 0;
}

static void _jl_gl_framebuffer_make(jvct_t *_jlc, uint32_t *fb) {
	glGenFramebuffers(1, fb);
	if(!(*fb)) jl_sg_kill(_jlc->jlc, "_jl_gl_framebuffer_make: GL FB = 0");
	_jl_gl_cerr(_jlc, *fb,"glGenFramebuffers");
}

static void _jl_gl_framebuffer_bind(jvct_t *_jlc, uint32_t fb) {
	if(fb == 0) jl_sg_kill(_jlc->jlc, "_jl_gl_framebuffer_bind: GL FB = 0");
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	_jl_gl_cerr(_jlc, fb,"glBindFramebuffer");
}

static void _jl_gl_framebuffer_unbind(jvct_t *_jlc) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	_jl_gl_cerr(_jlc, 0,"_jl_gl_framebuffer_unbind: glBindFramebuffer");
}

static void _jl_gl_texture_free(jvct_t *_jlc, uint32_t *tex) {
	glDeleteTextures(1, tex);
	_jl_gl_cerr(_jlc, 0, "_jl_gl_texture_free: glDeleteTextures");
	*tex = 0;
}

static void _jl_gl_viewport(jvct_t *_jlc, uint16_t w, uint16_t h) {
	glViewport(0, 0, w, h);
	_jl_gl_cerr(_jlc, w * h, "glViewport");
}

static void _jl_gl_pr_use(jvct_t *_jlc, uint32_t tex, uint32_t db, uint32_t fb,
	uint16_t w, uint16_t h, jl_pr_t* pr)
{
	// Render to our framebuffer
	_jl_gl_framebuffer_bind(_jlc, fb);
	// Render on the texture
	jl_gl_texture_bind__(_jlc, tex);
	// Render on the depth buffer
	_jl_gl_depthbuffer_bind(_jlc, db);
	// Render on the whole framebuffer [ lower left -> upper right ]
	_jl_gl_viewport(_jlc, w, h);
	// Reset the aspect ratio.
/*	if(_jlc->gl.cp) {
		printf("_jl_gl_pr_use: %d, %d\n",_jlc->gl.cp->w,_jlc->gl.cp->h);
	}else{
		printf("_j2l_gl_pr_use: %d, %d\n", _jlc->dl.full_w,
			_jlc->dl.full_h);
	}*/
	_jlc->gl.cp = pr;
}

static void _jl_gl_pr_unuse(jvct_t *_jlc) {
	// Render to the screen
	_jl_gl_framebuffer_unbind(_jlc);
	// Unbind Texture
	_jl_gl_texture_unbind(_jlc);
	// Use the screen's depth buffer.
	_jl_gl_depthbuffer_unbind(_jlc);
	// Render to the whole screen.
	jl_gl_viewport_screen(_jlc);
	// Reset the aspect ratio.
	_jlc->gl.cp = NULL;
}

static void _jl_gl_pr_obj_free(jvct_t *_jlc,
	uint32_t *tex, uint32_t *db, uint32_t *fb)
{
	_jl_gl_texture_free(_jlc, tex);
	_jl_gl_framebuffer_free(_jlc, fb);
	_jl_gl_depthbuffer_free(_jlc, db);
}

// Create a pr object with width and hieght of "w" & "h" and save the
// Texture into variable "tex", the frame buffer into variable "fb", and the
// depth buffer into variable "db"
static void _jl_gl_pr_obj_make(jvct_t *_jlc,
	uint32_t *tex, uint32_t *db, uint32_t *fb,
	uint16_t w, uint16_t h)
{
	if(w < 1) {
		_jl_fl_errf(_jlc, ":_jl_gl_pr_obj_make() failed:\n:");
		jl_sg_kill(_jlc->jlc, ": 'w' must be more than 1\n");
	}
	if(h < 1) {
		_jl_fl_errf(_jlc, ":_jl_gl_pr_obj_make() failed:\n:");
		jl_sg_kill(_jlc->jlc, ": 'h' must be more than 1\n");	
	}
	if((w > GL_MAX_TEXTURE_SIZE) || (h > GL_MAX_TEXTURE_SIZE)) {
		char *wmsg = jl_me_string_fnum(_jlc->jlc, w);
		char *hmsg = jl_me_string_fnum(_jlc->jlc, h);
		_jl_fl_errf(_jlc, ":_jl_gl_pr_obj_make() failed:\n");
		_jl_fl_errf(_jlc, ":w = ");
		_jl_fl_errf(_jlc, wmsg);
		_jl_fl_errf(_jlc, "\n:h = ");
		_jl_fl_errf(_jlc, hmsg);
		_jl_fl_errf(_jlc, "\n");
		jl_sg_kill(_jlc->jlc,
			": texture is too big for graphics card.\n");
	}
	// Create a new texture for pre-renderering.  The "NULL" sets it blank.
	jl_gl_texture_new__(_jlc, tex, NULL, w, h);
// Configure the framebuffer.
	// Make & Bind Frame Buffer
	_jl_gl_framebuffer_make(_jlc, fb);
	_jl_gl_framebuffer_bind(_jlc, *fb);
	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, *tex, 0);
// The depth buffer
	// Make & Bind Depth Buffer
	_jl_gl_depthbuffer_make(_jlc, db);
	_jl_gl_depthbuffer_bind(_jlc, *db);
	// Set the depth buffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	_jl_gl_cerr(_jlc, 0,"make pr: glRenderbufferStorage");
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, *db);
	_jl_gl_cerr(_jlc, 0,"make pr: glFramebufferRenderbuffer");

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		jl_sg_kill(_jlc->jlc, "Frame buffer not complete!\n");
// Set Viewport to image and clear.
	_jl_gl_viewport(_jlc, w, h);
	jl_gl_clear(_jlc->jlc, 255, 0, 0, 255);
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
static void _jl_gl_draw_txtr(jvct_t *_jlc, f32_t a, u32_t tx, uint32_t tc) {
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
	_jl_gl_viewport(_jlc, _jlc->dl.full_w, _jlc->dl.full_h);
}

void jl_gl_pr_old_(jvct_t *_jlc, jl_pr_t** pr) {
	// 
	_jl_gl_pr_obj_free(_jlc, &((*pr)->tx), &((*pr)->db), &((*pr)->fb));
	// Free old pr object.
	jl_me_alloc(_jlc->jlc, (void**)pr, 0, 0);
}

// Free a pr for a vertex object
void jl_gl_pr_old(jvct_t *_jlc, jl_vo* pv) {
	if(pv->pr == NULL) jl_sg_kill(_jlc->jlc, "pr(): double free!");
	jl_gl_pr_old_(_jlc, &(pv->pr));
}

uint8_t jl_gl_pr_isi_(jvct_t *_jlc, jl_pr_t* pr) {
	if(pr) {
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
			jl_sg_kill(_jlc->jlc, "\n");
		}
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
	_jl_gl_pr_use(_jlc, pr->tx, pr->db, pr->fb, pr->w, pr->h, pr);
}

// Use a vertex object's pre-renderer for rendering.
void jl_gl_pr_use(jvct_t *_jlc, jl_vo* pv) {
	pv->a = 1.f;
	if(!pv->pr)
		jl_sg_kill(_jlc->jlc,
			"jl_gl_pr_use: pre-renderer not created\n");
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
jl_ccolor_t* jl_gl_clrcs(jvct_t *_jlc, uint8_t *rgba, uint32_t vc) {
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
jl_ccolor_t* jl_gl_clrcg(jvct_t *_jlc, uint8_t *rgba, uint32_t vc) {
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
void jl_gl_clrg(jvct_t *_jlc, jl_vo* pv, uint8_t *rgba) {
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	_jl_gl_col_begin(_jlc, pv);
	jl_gl_clrc(_jlc, pv, jl_gl_clrcg(_jlc, rgba, pv->vc));
}

// Set Texturing to Solid Color "rgba" { 4 values }
void jl_gl_clrs(jvct_t *_jlc, jl_vo* pv, uint8_t *rgba) {
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
	float y, float z)
{
	// Determine which shader to use: texturing or coloring?
	_jl_gl_setp(_jlc, shader ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set the uniforms
	glUniform3f(shader ? _jlc->gl.tex.uniforms.translate :
		_jlc->gl.clr.uniforms.translate,
		x - (1./2.), y - (jl_gl_ar(_jlc->jlc)/2.), z);
	_jl_gl_cerr(_jlc, 0,"glUniform3f - translate");	
}

//TODO:MOVE
static void jl_gl_transform__(jvct_t *_jlc, const uint8_t shader, float x,
	float y, float z)
{
	// Determine which shader to use: texturing or coloring?
	_jl_gl_setp(_jlc, shader ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set the uniforms
	glUniform4f(shader ? _jlc->gl.tex.uniforms.transform :
		_jlc->gl.clr.uniforms.transform,
		2. * x, -2. * y / jl_gl_ar(_jlc->jlc), 2. * z, 1.f);
	_jl_gl_cerr(_jlc, 0,"glUniform3f - transform");
}

void jl_gl_transform_pr_(jvct_t *_jlc, jl_pr_t* pr, float x, float y, float z,
	float xm, float ym, float zm)
{
	if(!jl_gl_pr_isi_(_jlc, pr))
		jl_sg_kill(_jlc->jlc, "jl_gl_translate_pr: "
			"Pre-renderer Not Initialized!\n");
	jl_gl_translate__(_jlc, 1, x, y, z);
	jl_gl_transform__(_jlc, 1, xm, ym, zm);
}

void jl_gl_transform_vo_(jvct_t *_jlc, jl_vo* vo, float x, float y, float z,
	float xm, float ym, float zm)
{
	if(vo == NULL) vo = _jlc->gl.temp_vo;
	jl_gl_translate__(_jlc, (vo->cc == NULL), x, y, z);
	jl_gl_transform__(_jlc, (vo->cc == NULL), xm, ym, zm);
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

	// Use texturing shader.
	_jl_gl_setp(_jlc, JL_GL_SLPR_TEX);
	// Fail if no pre-rendered texture.
	if(!jl_gl_pr_isi_(_jlc, pr))
		jl_sg_kill(jlc, "Pre-renderer Not Initialized!\n");
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
		jl_sg_kill(_jlc->jlc, "\n");
	}
	_jl_gl_cerr(_jlc, a,"glGetUniformLocation");
	return a;
}

void _jl_gl_geta(jvct_t *_jlc, GLuint prg, m_i32_t *attrib, const char *title) {
	if((*attrib = glGetAttribLocation(prg, title)) == -1) {
		 jl_sg_kill(_jlc->jlc,
		 	"attribute name is either reserved or non-existant");
	}
}

/***      @cond       ***/
/************************/
/*** Static Functions ***/
/************************/

static inline void _jl_gl_init_setup_gl(jvct_t *_jlc) {
	jl_io_printc(_jlc->jlc, "setting properties...\n");
	//Disallow Dither & Depth Test
	_jl_gl_init_disable_extras(_jlc);
	//Set alpha=0 to transparent
	_jl_gl_init_enable_alpha(_jlc);
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
	if(_jlc->gl.tex.uniforms.textures == NULL)
		jl_sg_kill(_jlc->jlc, "Couldn't create uniforms");
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

//Load and create all resources
static inline void _jl_gl_make_res(jvct_t *_jlc) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "GLIN");
	//Setup opengl properties
	_jl_gl_init_setup_gl(_jlc);
	//Create shaders and set up attribute/uniform variable communication
	_jl_gl_init_shaders(_jlc);

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
	// Converted Vertices & Vertex Count
	vo->cv = NULL;
	jl_gl_vo_vertices(_jlc, vo, NULL, 0); // cv & vc
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

/**      @endcond      **/
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
	_jl_gl_cerr(jlc->_jlc, a, "jl_gl_clear(): glClearColor");
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	_jl_gl_cerr(jlc->_jlc, a, "jl_gl_clear(): glClear");
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
	float xyzw[] = {
		0.f,	h,	0.f,
		0.f,	0.f,	0.f,
		w,	0.f,	0.f,
		w,	h,	0.f
	};

	// Use the screen's pre-renderer if it exists.
	jl_gl_pr_scr(jlc->_jlc);
	// Create the VBO.
	jl_gl_vertices__(jlc->_jlc, xyzw, 4, pr->cv, pr->gl);
	// Set variables.
	pr->ar = (h / w);
	pr->w = w_px;
	pr->h = w_px * (h / w);
	// Resize the actual texture.
	_jl_gl_pr_obj_free(jlc->_jlc, &(pr->tx), &(pr->db), &(pr->fb));
	_jl_gl_pr_obj_make(jlc->_jlc, &(pr->tx), &(pr->db), &(pr->fb),
		pr->w, pr->h);
	// TODO: Debug & Remove
	jl_io_printc(jlc, "2: resize: ");
	jl_io_printd(jlc, pr->ar);
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
	const float ar = h / w;
	const float h_px = w_px * ar;

	// Use the screen's pre-renderer if it exists.
	jl_gl_pr_scr(_jlc);
	// Make the pr structure.
	jl_me_alloc(_jlc->jlc, (void**)(&pr), sizeof(jl_pr_t), 0);
	// Set the initial pr structure values - Nothings made yet.
	pr->tx = 0;
	pr->db = 0;
	pr->fb = 0;
	pr->w = w_px;
	pr->h = h_px;
	pr->gl = 0;
	pr->ar = ar;
	pr->cv = NULL;
	// Allocate pr->cv 
	jl_me_alloc(_jlc->jlc, (void**)&pr->cv, 4*sizeof(float)*3, 0);
	// Debugging print
	jl_io_printc(jlc, "w_px=");
	jl_io_printi(jlc, w_px);
	jl_io_printc(jlc, "\n");
	printf("making pre-renderer of size %dx%d\n", pr->w, pr->h);
	// Make OpenGL Objects
	jl_gl_buffer_new__(_jlc, &(pr->gl));
	_jl_gl_pr_obj_make(_jlc, &(pr->tx), &(pr->db), &(pr->fb), pr->w, pr->h);
	// Resize the new pre-renderer.
	// TODO: Debug & Remove
	jl_io_printc(jlc, "1: resize: ");
	jl_io_printd(jlc, pr->ar);
	jl_io_printc(jlc, ", ");
	jl_io_printd(jlc, pr->w);
	jl_io_printc(jlc, ", ");
	jl_io_printd(jlc, pr->h);
	jl_io_printc(jlc, "\n");
	jl_gl_pr_rsz(jlc, pr, w, h, w_px);
	// Return the new pre-renderer.
	return pr;
}

/***      @cond       ***/
/************************/
/***  ETOM Functions  ***/
/************************/

void jl_gl_pr_(jvct_t *_jlc, jl_pr_t * pr, jl_simple_fnt par__redraw) {
	if(!pr) jl_sg_kill(_jlc->jlc, "Drawing on lost pre-renderer.\n");
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
#ifndef JLVM_USEL_GLES
	printf(" [JLVM/GLEW] init\n");
	if(glewInit()!=GLEW_OK)
		jl_sg_kill(_jlc->jlc, "glew fail!(no sticky)");
#endif
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

/**      @endcond      **/
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
