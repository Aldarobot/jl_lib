#include "header/jl_pr.h"

// Shader Code

#ifdef GL_ES_VERSION_2_0
	#define GLSL_HEAD "#version 100\nprecision mediump float;\n"
#else
	#define GLSL_HEAD "#version 120\n"
#endif

char *source_frag_clr = 
	GLSL_HEAD
	"uniform vec4 cliprange;\n"
	"varying vec4 vcolor;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	if((poscoord.x > cliprange.y) || (poscoord.y > cliprange.z) ||"
	"		(poscoord.x < cliprange.x) || (poscoord.y < cliprange.w))"
	"	{"
	"		discard;"
	"	}else{"
	"		gl_FragColor = vcolor;\n"
	"	}"
	"}\n\0";
	
char *source_vert_clr = 
	GLSL_HEAD
	"uniform vec3 translate;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec4 acolor;\n"
	"\n"
	"varying vec4 vcolor;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	poscoord = position + translate;\n"
	"	gl_Position = vec4(poscoord, 1.0);\n"
	"	vcolor = acolor;\n"
	"}\n\0";

char *source_frag_tex = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"uniform float multiply_alpha;\n"
	"uniform vec4 cliprange;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	if((poscoord.x > cliprange.y) || (poscoord.y > cliprange.z) ||"
	"		(poscoord.x < cliprange.x) || (poscoord.y < cliprange.w))"
	"	{"
	"		discard;"
	"	}else{"
	"		gl_FragColor = texture2D(texture,"
	"		vec2(texcoord.x - .0039, texcoord.y + .001));"
	"		gl_FragColor.a = gl_FragColor.a * multiply_alpha;"
	"	}"
//	"	gl_FragColor = mix(\n"
//	"		texture2D(textures[0], texcoord),\n"
//	"		texture2D(textures[1], texcoord),\n"
//	"		fade_factor\n"
//	"	);\n"
//	"	gl_FragColor = texture2D(texture, vec2(texcoord.x, texcoord.y));"

//	"	gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);"
//	"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
	"}\n\0";

char *source_vert_tex = 
	GLSL_HEAD
	"uniform vec3 translate;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec2 texpos;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	poscoord = position + translate;\n"
	"	gl_Position = vec4(poscoord, 1.0);\n"
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

static void _jl_gl_buff_use(jvct_t *_jlc, uint32_t buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	_jl_gl_cerr(_jlc, 0,"bind buffer");
}

// Set the Data for VBO "buffer" to "buffer_data" with "buffer_size"
static void _jl_gl_buff_set(jvct_t *_jlc, uint32_t buffer, void *buffer_data,
	u08t buffer_size)
{
	// Check For Deleted Buffer
	if(buffer == 0) jl_sg_kill(_jlc->jlc, "buffer got deleted!");
	//Bind Buffer "buffer"
	_jl_gl_buff_use(_jlc, buffer);
	//Set the data
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(float), buffer_data,
		GL_STATIC_DRAW);
	_jl_gl_cerr(_jlc, buffer,"buffer data");
}

static void _jl_gl_buffer_make(jvct_t *_jlc, uint32_t *buffer)
{
	glGenBuffers(1, buffer);
	_jl_gl_cerr(_jlc, 0,"buffer gen");
	if(*buffer == 0) jl_sg_kill(_jlc->jlc, "buffer is made wrongly.");
}

static void _jl_gl_buffer_free(jvct_t *_jlc, uint32_t *buffer) {
	glDeleteBuffers(1, buffer);
	_jl_gl_cerr(_jlc, 0,"buffer free");
}

GLuint loadShader(jvct_t *_jlc, GLenum shaderType, const char* pSource) {
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

GLuint createProgram(jvct_t *_jlc, const char* pVertexSource,
	const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(_jlc, GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		jl_sg_kill(_jlc->jlc, ":couldn't load vertex shader\n");
	}

	GLuint pixelShader = loadShader(_jlc, GL_FRAGMENT_SHADER, pFragmentSource);
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

void jl_gl_maketexture(jl_t* jlc, uint16_t gid, uint16_t id,
	void *pixels, int width, int height)
{
	jvct_t *_jlc = jlc->_jlc;
	char *hstr;
	char *wstr;
	
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "MKTX");
	if (!pixels)
		jl_sg_kill(jlc, "null pixels");
	if (_jlc->gl.allocatedg < gid + 1) {
		_jlc->gl.textures =
			realloc(_jlc->gl.textures, sizeof(GLuint *) * (gid+1));
		_jlc->gl.uniforms.textures =
			realloc(_jlc->gl.uniforms.textures,
				sizeof(GLint *) * (gid+1));
		_jlc->gl.allocatedg = gid + 1;
		_jlc->gl.allocatedi = 0;
		_jlc->gl.textures[gid] = NULL;
		_jlc->gl.uniforms.textures[gid] = NULL;
	}
	if (_jlc->gl.allocatedi < id + 1) {
		_jlc->gl.textures[gid] =
			realloc(_jlc->gl.textures[gid],
				sizeof(GLuint) * (id+1));
		_jlc->gl.uniforms.textures[gid] =
			realloc(_jlc->gl.uniforms.textures[gid],
				sizeof(GLint) * (id+1));
		_jlc->gl.allocatedi = id + 1;
	}
	jl_io_printc(_jlc->jlc, "generating texture...");
	glGenTextures(1, &_jlc->gl.textures[gid][id]);
	_jl_gl_cerr(_jlc, 0,"gen textures");
	glBindTexture(GL_TEXTURE_2D, _jlc->gl.textures[gid][id]);
	_jl_gl_cerr(_jlc, 0,"bind textures");
	jl_io_printc(_jlc->jlc, "settings pars...");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_jl_gl_cerr(_jlc, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	_jl_gl_cerr(_jlc, 1,"glTexParameteri");
	//GL_CLAMP_TO_BORDER GL_REPEAT GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	_jl_gl_cerr(_jlc, 2,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	_jl_gl_cerr(_jlc, 3,"glTexParameteri");

	hstr = jl_me_string_fnum(_jlc->jlc, height);
	wstr = jl_me_string_fnum(_jlc->jlc, width);
	jl_io_printc(_jlc->jlc, "wh:");
	jl_io_printc(_jlc->jlc, wstr);
	jl_io_printc(_jlc->jlc, ",");
	jl_io_printc(_jlc->jlc, hstr);
	jl_io_printc(_jlc->jlc, "\n");
	free(hstr);
	free(wstr);

	glTexImage2D(
		GL_TEXTURE_2D, 0,		/* target, level */
		GL_RGBA,			/* internal format */
		width, height, 0,		/* width, height, border */
		GL_RGBA, GL_UNSIGNED_BYTE,	/* external format, type */
		pixels				/* pixels */
	);
	_jl_gl_cerr(_jlc, 0,"texture image 2D");
//	free(pixels);
	if(_jlc->gl.textures[gid][id] == 0) {
		printf("bad texture:\n");
		_jl_gl_cerr(_jlc, 0,"BADT");
		jl_sg_kill(jlc, ":Bad Texture, but no gl error? WHY!?\n");
	}
	jl_io_close_block(_jlc->jlc); //Close Block "MKTX"
}

//Lower Level Stuff
static void _jl_gl_usep(jvct_t *_jlc, GLuint prg) {
	if(!prg) { jl_sg_kill(_jlc->jlc, ":program ain't a prg!\n"); } 
	glUseProgram(prg);
	_jl_gl_cerr(_jlc, prg, "glUseProgram");
}

// Bind a texture.
static void _jl_gl_bind(jvct_t *_jlc, uint16_t g, uint16_t i) {
	glBindTexture(GL_TEXTURE_2D, _jlc->gl.textures[g][i]);
	_jl_gl_cerr(_jlc, 0,"glBindTexture");
}

static void _jl_gl_setalpha(jvct_t *_jlc, float a) {
	glUniform1f(_jlc->gl.uniforms.multiply_alpha, a);
	_jl_gl_cerr(_jlc, 0,"glUniform1f");
}

//This pushes VBO "buff" up to the shader's vertex attribute "vertexAttrib"
//Set xyzw to 2 if 2D coordinates 3 if 3D. etc.
void _jl_gl_setv(jvct_t *_jlc, uint32_t buff, uint32_t vertexAttrib, uint8_t xyzw) {
	// Bind Buffer
	if(buff != 0) _jl_gl_buff_use(_jlc, buff);
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

static float jl_gl_convert_rx(jvct_t *_jlc, float x) {
	return (x*_jlc->dl.multiplyx);
}

static float jl_gl_convert_ry(jvct_t *_jlc, float y) {
	return (y*_jlc->dl.multiplyy);
}

static float jl_gl_convert_rz(jvct_t *_jlc, float z) {
	return z * 2.f;
}

static float jl_gl_convert_x(jvct_t *_jlc, float x) {
	return jl_gl_convert_rx(_jlc, x) + _jlc->dl.shiftx;
}

static float jl_gl_convert_y(jvct_t *_jlc, float y) {
	return jl_gl_convert_ry(_jlc, y) + _jlc->dl.shifty;
}

static float jl_gl_convert_z(jvct_t *_jlc, float z) {
	return jl_gl_convert_rz(_jlc, z);
}

void jl_gl_vo_vertices(jvct_t *_jlc, jl_vo* pv, const float *xyzw,
	uint8_t vertices)
{
	uint16_t i;

	pv->vc = vertices;
	if(vertices) {
		//Free pv->cv if non-null
		if(pv->cv) jl_me_alloc(_jlc->jlc, (void**)&pv->cv, 0, 0);
		//Allocate pv->cv
		jl_me_alloc(_jlc->jlc,
			(void**)&pv->cv, vertices * sizeof(float) * 3, 0);
	}
	for(i = 0; i < vertices*3; i+=3) {
		pv->cv[i] = jl_gl_convert_x(_jlc, xyzw[i]);
		pv->cv[i+1] = jl_gl_convert_y(_jlc, xyzw[i+1]);
		pv->cv[i+2] = jl_gl_convert_z(_jlc, xyzw[i+2]);
	}
	//Write Buffer Data "pv->cv" to Buffer "pv->gl"
	_jl_gl_buff_set(_jlc, pv->gl, pv->cv, vertices * 3);
}

void jl_gl_vo_free(jvct_t *_jlc, jl_vo *pv) {
	// Free GL VBO
	_jl_gl_buffer_free(_jlc, &pv->gl);
	// Free GL Texture Buffer
	_jl_gl_buffer_free(_jlc, &pv->bt);
	// Free Converted Vertices & Colors
	if(pv->cv) jl_me_alloc(_jlc->jlc, (void**)&pv->cv, 0,0);
	if(pv->cc) jl_me_alloc(_jlc->jlc, (void**)&pv->cc, 0,0);
	// Free main structure
	jl_me_alloc(_jlc->jlc, (void**)&pv, 0,0);
}

jl_vo *jl_gl_vo_make(jvct_t *_jlc, float *xyzw, uint8_t vertices) {
	jl_vo *rtn = NULL;

	jl_me_alloc(_jlc->jlc, (void**)&rtn, sizeof(jl_vo),0);
	// GL VBO
	_jl_gl_buffer_make(_jlc, &rtn->gl);
	// GL Texture Buffer
	_jl_gl_buffer_make(_jlc, &rtn->bt);
	// Converted Vertices & Vertex Count
	rtn->cv = NULL;
	jl_gl_vo_vertices(_jlc, rtn, xyzw, vertices); // cv & vc
	// Converted Colors
	rtn->cc = NULL;
	// Rendering Style = Polygon
	rtn->rs = 0;
	return rtn;
}

static void _jl_gl_vrtx_updatevectors(jvct_t *_jlc, uint8_t *update,
	GLint* cliprange, GLint* position, uint32_t gl)
{
	_jl_gl_setv(_jlc, gl, *position, 3);
	if(*update == 1) {
		glUniform4f(*cliprange,
			_jlc->gl.cliprange.x, _jlc->gl.cliprange.y,
			_jlc->gl.cliprange.w, _jlc->gl.cliprange.h);
		*update = 0;
	}
}

static void _jl_gl_updatetex(jvct_t *_jlc, uint32_t gl) {
	_jl_gl_vrtx_updatevectors(_jlc, &_jlc->gl.update,
		&_jlc->gl.uniforms.cliprange,
		&_jlc->gl.attr.tex.position, gl);
}

static void _jl_gl_updateclr(jvct_t *_jlc, uint32_t gl) {
	_jl_gl_vrtx_updatevectors(_jlc, &_jlc->gl.update2,
		&_jlc->gl.uniforms.cliprangeclr,
		&_jlc->gl.attr.clr.position, gl);	
}

static void _jl_gl_update_clip_pane(jvct_t *_jlc, uint32_t gl) {
	if(_jlc->gl.whichprg == JL_GL_SLPR_TEX) {
		_jl_gl_updatetex(_jlc, gl);
	}else if(_jlc->gl.whichprg == JL_GL_SLPR_CLR){
		_jl_gl_updateclr(_jlc, gl);
	}
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

void jl_gl_set_clippane(jvct_t *_jlc, float xmin, float xmax, float ymin, float ymax) {
	_jlc->gl.update = 1;
	_jlc->gl.update2 = 1;
	_jlc->gl.cliprange.x = jl_gl_convert_x(_jlc, xmin);
	_jlc->gl.cliprange.y = jl_gl_convert_x(_jlc, xmax);
	_jlc->gl.cliprange.w = jl_gl_convert_y(_jlc, ymin + _jlc->gl.ytrans);
	_jlc->gl.cliprange.h = jl_gl_convert_y(_jlc, ymax + _jlc->gl.ytrans);
}

void jl_gl_default_clippane(jvct_t *_jlc) {
	jl_gl_set_clippane(_jlc, 0., 1., 0.f, jl_dl_p(_jlc->jlc));
}

void _jl_gl_setmatrix(jvct_t *_jlc) {
	jl_gl_default_clippane(_jlc);
}

//HIGHER LEVEL

// Set vertices for a polygon.
void jl_gl_poly(jvct_t *_jlc, jl_vo* pv, uint8_t vertices, const float *xyzw) {
	const float FS_RECT[] = {
		0.,jl_dl_p(_jlc->jlc),0.,
		0.,0.,0.,
		1.,0.,0.,
		1.,jl_dl_p(_jlc->jlc),0.
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
	_jl_gl_buff_set(_jlc, pv->bt, pv->cc, pv->vc * 4);
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
	jl_me_alloc(_jlc->jlc,
		(void**)&cc, vc * sizeof(float) * 4, 0);
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
void jl_gl_txtr(jvct_t *_jlc, jl_vo* pv, u08t map, u08t a, u16t pgid, u16t pi) {
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	// Set Simple Variabes
	pv->a = ((float)a) / 255.f;
	pv->g = pgid;
	pv->i = pi;
	// Make sure non-textured colors aren't attempted
	if(pv->cc != NULL) jl_me_alloc(_jlc->jlc, (void**)&pv->cc, 0, 0);
	if(map) {
		int32_t cX = map%16;
		int32_t cY = map/16;
		double CX = (((double)cX)/16.);
		double CY = 1.-((double)cY)/16.;
		float tex1[] =
		{
			CX, CY - (1./16.),
			CX, CY,
			(1./16.)+CX, CY,
			(1./16.)+CX, CY - (1./16.)
		};
		_jl_gl_buff_set(_jlc, pv->bt, tex1, 8);
	}else{
		float tex2[] =
		{	
			0. + (2./1024.), 0.,
			0. + (2./1024.), 1.,
			1., 1.,
			1., 0.
		};
		_jl_gl_buff_set(_jlc, pv->bt, tex2, 8);
	}
	_jl_gl_setv(_jlc, pv->bt, _jlc->gl.attr.tex.texpos, 2);
}

void jl_gl_translate(jvct_t *_jlc, jl_vo* pv, float x, float y, float z) {
	if(pv == NULL) pv = _jlc->gl.temp_vo;
	// Use "pv->cc" to determine which shader to use: texturing or coloring?
	_jl_gl_setp(_jlc, pv->cc == NULL ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set the uniforms
	glUniform3f(pv->cc == NULL ? _jlc->gl.uniforms.translate :
		_jlc->gl.uniforms.translateclr,
		jl_gl_convert_rx(_jlc, x),
		jl_gl_convert_ry(_jlc, y + _jlc->gl.ytrans),
		jl_gl_convert_rz(_jlc, z));
	_jl_gl_cerr(_jlc, 0,"glUniform3f - translate");
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
	// Use "pv->cc" to determine which shader to use: texturing or coloring?
	_jl_gl_setp(_jlc, pv->cc == NULL ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set texture and transparency if texturing.  If colors: bind colors
	if(pv->cc) {
		// Bind Colors to shader
		_jl_gl_setv(_jlc, pv->bt, _jlc->gl.attr.clr.acolor, 4);
	}else{
		// Set Alpha Value In Shader
		_jl_gl_setalpha(_jlc, pv->a);
		// Bind the texture
		_jl_gl_bind(_jlc, pv->g, pv->i);
	}
	// Update the clipping pane & position in shader.
	_jl_gl_update_clip_pane(_jlc, pv->gl);
	// Finally, draw the image!
	_jl_gl_draw_arrays(_jlc, pv->rs ? GL_TRIANGLES : GL_TRIANGLE_FAN,
		pv->vc);
}

//void jl_gl_

int32_t _jl_gl_getu(jvct_t *_jlc, GLuint prg, char *var) {
	return glGetUniformLocation(prg, var);
	int32_t a;
	if((a = glGetUniformLocation(_jlc->gl.prgs[JL_GL_SLPR_TEX], var)) == -1) {
		_jl_fl_errf(_jlc, ":opengl: bad name; is:\n:");
		_jl_fl_errf(_jlc, var);
		jl_sg_kill(_jlc->jlc, "\n");
	}
	_jl_gl_cerr(_jlc, 0,"glGetUniformLocation");
	return a;
}

void _jl_gl_geta(jvct_t *_jlc, GLuint prg, s32t *attrib, const char *title) {
	if((*attrib
		= glGetAttribLocation(prg, title)) == -1)
	{
		 jl_sg_kill(_jlc->jlc,
		 	"attribute name is either reserved or non-existant");
	}
}

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
		createProgram(_jlc, source_vert_tex, source_frag_tex);
	_jlc->gl.prgs[JL_GL_SLPR_CLR] =
		createProgram(_jlc, source_vert_clr, source_frag_clr);
	jl_io_printc(_jlc->jlc, "made programs.\n");

	jl_io_printc(_jlc->jlc, "setting up shaders....\n");
	if(_jlc->gl.uniforms.textures == NULL)
		jl_sg_kill(_jlc->jlc, "Couldn't create uniforms");
	_jlc->gl.uniforms.textures[0][0] =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX], "texture");
	_jlc->gl.uniforms.multiply_alpha =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX],
			"multiply_alpha");
	_jlc->gl.uniforms.cliprange =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX], "cliprange");
	_jlc->gl.uniforms.cliprangeclr =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR], "cliprange");
	_jlc->gl.uniforms.translate =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX], "translate");
	_jlc->gl.uniforms.translateclr =
		_jl_gl_getu(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR], "translate");
	jl_io_printc(_jlc->jlc, "setting up tex shader attrib's....\n");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX],
		&_jlc->gl.attr.tex.position, "position");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX],
		&_jlc->gl.attr.tex.texpos, "texpos");
	jl_io_printc(_jlc->jlc, "setting up clr shader attrib's....\n");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR],
		&_jlc->gl.attr.clr.position, "position");
	_jl_gl_geta(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR],
		&_jlc->gl.attr.clr.acolor, "acolor");
	jl_io_printc(_jlc->jlc, "set up shaders.\n");
}

static inline void _jl_gl_init_prerenderer(jvct_t *_jlc) {
	// Make the framebuffer [ groups 0+ textures & 0-1 depth buffers ].
	_jlc->gl.fb = 0;
	glGenFramebuffers(1, &_jlc->gl.fb);
	_jl_gl_cerr(_jlc, _jlc->gl.fb,"glGenFramebuffers");
	glBindFramebuffer(GL_FRAMEBUFFER, _jlc->gl.fb);
	_jl_gl_cerr(_jlc, _jlc->gl.fb,"glBindFramebuffer");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	_jl_gl_cerr(_jlc, _jlc->gl.fb,"glBindFramebuffer2");
}

//Load and create all resources
static inline void _jl_gl_make_res(jvct_t *_jlc) {
	jl_io_offset(_jlc->jlc, JL_IO_SIMPLE, "GLIN");
	//Setup opengl properties
	_jl_gl_init_setup_gl(_jlc);
	//Create shaders and set up attribute/uniform variable communication
	_jl_gl_init_shaders(_jlc);
	//Setup for gl Render To Texture
	_jl_gl_init_prerenderer(_jlc);

	jl_io_printc(_jlc->jlc, "making temporary vertex object....\n");
	_jlc->gl.temp_vo = jl_gl_vo_make(_jlc, NULL, 0);
	jl_io_printc(_jlc->jlc, "made temporary vertex object!\n");
	jl_io_close_block(_jlc->jlc); //Close Block "GLIN"
}

void _jl_gl_init(jvct_t *_jlc) {
#ifndef JLVM_USEL_GLES
	printf(" [JLVM/GLEW] init\n");
	if(glewInit()!=GLEW_OK)
		jl_sg_kill(_jlc->jlc, "glew fail!(no sticky)");
#endif
	_jl_gl_make_res(_jlc);
	//Set uniform values
	_jl_gl_usep(_jlc, _jlc->gl.prgs[JL_GL_SLPR_CLR]);
	glUniform3f(_jlc->gl.uniforms.translateclr, 0.f, 0.f, 0.f);
	_jl_gl_usep(_jlc, _jlc->gl.prgs[JL_GL_SLPR_TEX]);
	glUniform3f(_jlc->gl.uniforms.translate, 0.f, 0.f, 0.f);
	//Textures on by default
	_jlc->gl.whichprg = JL_GL_SLPR_TEX;
}

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
