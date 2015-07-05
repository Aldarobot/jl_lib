#include "header/jl_pr.h"

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
	"\n"
	"attribute vec3 position;\n"
	"attribute vec4 acolor;\n"
	"\n"
	"varying vec4 vcolor;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position, 1.0);\n"
	"	vcolor = acolor;\n"
	"	poscoord = position;\n"
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
	"attribute vec3 position;\n"
	"attribute vec2 texpos;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position, 1.0);\n"
	"	texcoord = texpos;\n"
	"	poscoord = position;\n"
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

void _jl_gl_cerr(jvct_t *pjlc, int width, char *fname) {
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
	_jl_fl_errf(pjlc, ":");
	_jl_fl_errf(pjlc, fname);
	_jl_fl_errf(pjlc, "\n:");
	_jl_fl_errf(pjlc, fstrerr);
	_jl_fl_errf(pjlc, ":\n:");
	_jl_fl_errf(pjlc, (void*)jl_me_strt_fnum(width)->data);
	jl_sg_kill(pjlc->sg.usrd, "\n");
}

static void _jl_gl_buff_use(jvct_t *pjlc, uint32_t buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	_jl_gl_cerr(pjlc, 0,"bind buffer");
}

// Set the Data for VBO "buffer" to "buffer_data" with "buffer_size"
static void _jl_gl_buff_set(jvct_t *pjlc, uint32_t buffer, void *buffer_data,
	u08t buffer_size)
{
	// Check For Deleted Buffer
	if(buffer == 0) jl_sg_kill(pjlc->sg.usrd, "buffer got deleted!");
	//Bind Buffer "buffer"
	_jl_gl_buff_use(pjlc, buffer);
	//Set the data
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(float), buffer_data,
		GL_STATIC_DRAW);
	_jl_gl_cerr(pjlc, buffer,"buffer data");
}

static void _jl_gl_buffer_make(jvct_t *pjlc, uint32_t *buffer)
{
	glGenBuffers(1, buffer);
	_jl_gl_cerr(pjlc, 0,"buffer gen");
	if(*buffer == 0) jl_sg_kill(pjlc->sg.usrd, "buffer is made wrongly.");
}

GLuint loadShader(jvct_t *pjlc, GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	_jl_gl_cerr(pjlc, 0,"couldn't create shader");
	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pSource, NULL);
		_jl_gl_cerr(pjlc, 0,"glShaderSource");
		glCompileShader(shader);
		_jl_gl_cerr(pjlc, 0,"glCompileShader");
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		_jl_gl_cerr(pjlc, 0,"glGetShaderiv");
		if (!compiled) {
			GLint infoLen = 0;
			char* buf;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			_jl_gl_cerr(pjlc, 1,"glGetShaderiv");
			if (infoLen) {
				buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					_jl_fl_errf(pjlc, ":Could not compile shader:\n:");
					_jl_fl_errf(pjlc, buf);
					free(buf);
					jl_sg_kill(pjlc->sg.usrd, "\n");
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
		jl_sg_kill(pjlc->sg.usrd, ":couldn't load vertex shader\n");
	}

	GLuint pixelShader = loadShader(pjlc, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		jl_sg_kill(pjlc->sg.usrd, ":couldn't load fragment shader\n");
	}

	GLuint program = glCreateProgram();
	_jl_gl_cerr(pjlc, 0,"glCreateProgram");
	if (program) {
		GLint linkStatus = GL_FALSE;

		glAttachShader(program, vertexShader);
		_jl_gl_cerr(pjlc, 0,"glAttachShader (vertex)");
		glAttachShader(program, pixelShader);
		_jl_gl_cerr(pjlc, 0,"glAttachShader (fragment)");
		glLinkProgram(program);
		_jl_gl_cerr(pjlc, 0,"glLinkProgram");
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		_jl_gl_cerr(pjlc, 0,"glGetProgramiv");
		glValidateProgram(program);
		_jl_gl_cerr(pjlc, 1,"glValidateProgram");
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			char* buf;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			_jl_gl_cerr(pjlc, 1,"glGetProgramiv");
			if (bufLength) {
				buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					_jl_fl_errf(pjlc, "Could not link program:\n");
					_jl_fl_errf(pjlc, buf);
					free(buf);
					jl_sg_kill(pjlc->sg.usrd, "\n");
				}else{
					jl_sg_kill(pjlc->sg.usrd,
						":failed malloc\n");
				}
			}else{
				glDeleteProgram(program);
				jl_sg_kill(pjlc->sg.usrd, ":no info log\n");
			}
		}
	}
	if (program == 0) jl_sg_kill(pjlc->sg.usrd, "Failed to load program");
	return program;
}

void jl_gl_maketexture(jl_t* pusr, uint16_t gid, uint16_t id,
	void *pixels, int width, int height)
{
	jvct_t *pjlc = pusr->pjlc;
	char *hstr;
	char *wstr;
	
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "MKTX");
	if (!pixels)
		jl_sg_kill(pusr, "null pixels");
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
	jl_io_printc(pjlc->sg.usrd, "generating texture...");
	glGenTextures(1, &pjlc->gl.textures[gid][id]);
	_jl_gl_cerr(pjlc, 0,"gen textures");
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[gid][id]);
	_jl_gl_cerr(pjlc, 0,"bind textures");
	jl_io_printc(pjlc->sg.usrd, "settings pars...");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_jl_gl_cerr(pjlc, 0,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	_jl_gl_cerr(pjlc, 1,"glTexParameteri");
	//GL_CLAMP_TO_BORDER GL_REPEAT GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	_jl_gl_cerr(pjlc, 2,"glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	_jl_gl_cerr(pjlc, 3,"glTexParameteri");

	hstr = jl_me_string_fnum(pjlc->sg.usrd, height);
	wstr = jl_me_string_fnum(pjlc->sg.usrd, width);
	jl_io_printc(pjlc->sg.usrd, "wh:");
	jl_io_printc(pjlc->sg.usrd, wstr);
	jl_io_printc(pjlc->sg.usrd, ",");
	jl_io_printc(pjlc->sg.usrd, hstr);
	jl_io_printc(pjlc->sg.usrd, "\n");
	free(hstr);
	free(wstr);

	glTexImage2D(
		GL_TEXTURE_2D, 0,		/* target, level */
		GL_RGBA,			/* internal format */
		width, height, 0,		/* width, height, border */
		GL_RGBA, GL_UNSIGNED_BYTE,	/* external format, type */
		pixels				/* pixels */
	);
	_jl_gl_cerr(pjlc, 0,"texture image 2D");
//	free(pixels);
	if(pjlc->gl.textures[gid][id] == 0) {
		printf("bad texture:\n");
		_jl_gl_cerr(pjlc, 0,"BADT");
		jl_sg_kill(pusr, ":Bad Texture, but no gl error? WHY!?\n");
	}
	jl_io_close_block(pjlc->sg.usrd); //Close Block "MKTX"
}

//Lower Level Stuff
static void _jl_gl_usep(jvct_t *pjlc, GLuint prg) {
	if(!prg) { jl_sg_kill(pjlc->sg.usrd, ":program ain't a prg!\n"); } 
	glUseProgram(prg);
	_jl_gl_cerr(pjlc, 0,"glUseProgram");
}

// Bind a texture.
static void _jl_gl_bind(jvct_t *pjlc, uint16_t g, uint16_t i) {
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[g][i]);
	_jl_gl_cerr(pjlc, 0,"glBindTexture");
}

static void _jl_gl_setalpha(jvct_t *pjlc, float a) {
	glUniform1f(pjlc->gl.uniforms.multiply_alpha, a);
	_jl_gl_cerr(pjlc, 0,"glUniform1f");
}

//This pushes VBO "buff" up to the shader's vertex attribute "vertexAttrib"
//Set xyzw to 2 if 2D coordinates 3 if 3D. etc.
void _jl_gl_setv(jvct_t *pjlc, uint32_t buff, uint32_t vertexAttrib, uint8_t xyzw) {
	// Bind Buffer
	if(buff != 0) _jl_gl_buff_use(pjlc, buff);
	// Set Vertex Attrib Pointer
	glEnableVertexAttribArray(vertexAttrib);
	_jl_gl_cerr(pjlc, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,  	// attribute
		xyzw,		// x+y+z = 3
		GL_FLOAT,	// type
		GL_FALSE,	// normalized?
		0,		// stride
		0		// array buffer offset
	);
	_jl_gl_cerr(pjlc, 0,"glVertexAttribPointer");
}

static void _jl_gl_draw_arrays(jvct_t *pjlc, GLenum mode, uint8_t count) {
	glDrawArrays(mode, 0, count);
	_jl_gl_cerr(pjlc, 0,"glDrawArrays");
}

static inline void _jl_gl_init_disable_extras(jvct_t *pjlc) {
	glDisable( GL_DEPTH_TEST);
	_jl_gl_cerr(pjlc, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable( GL_DITHER );
	_jl_gl_cerr(pjlc, 0, "glDisable(GL_DITHER)");
}

static inline void _jl_gl_init_enable_alpha(jvct_t *pjlc) {
	glEnable( GL_BLEND );
	_jl_gl_cerr(pjlc, 0,"glEnable( GL_BLEND )");
	glBlendColor(1.f,1.f,1.f,0.f);
	_jl_gl_cerr(pjlc, 0,"glBlendColor");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_jl_gl_cerr(pjlc, 0,"glBlendFunc");
}

void jl_gl_vo_vertices(jvct_t *pjlc, jl_vo* pv, float *xyzw, uint8_t vertices) {
	uint16_t i;

	pv->vc = vertices;
	if(vertices) {
		//Free pv->cv if non-null
		if(pv->cv) jl_me_alloc(pjlc->sg.usrd, (void**)&pv->cv, 0, 0);
		//Allocate pv->cv
		jl_me_alloc(pjlc->sg.usrd,
			(void**)&pv->cv, vertices * sizeof(float) * 3, 0);
	}
	for(i = 0; i < vertices*3; i+=3) {
		pv->cv[i] =
			(xyzw[i]*pjlc->dl.multiplyx) + pjlc->dl.shiftx;
		pv->cv[i+1] =
			((-pjlc->dl.multiplyy) * (xyzw[i+1]+ pjlc->gl.ytrans))
			+ pjlc->dl.shifty;
		pv->cv[i+2] = xyzw[i+2] * 2.f;
	}
	//Write Buffer Data "pv->cv" to Buffer "pv->gl"
	_jl_gl_buff_set(pjlc, pv->gl, pv->cv, vertices * 3);
}

jl_vo *jl_gl_vo_make(jvct_t *pjlc, float *xyzw, uint8_t vertices) {
	jl_vo *rtn = NULL;

	jl_me_alloc(pjlc->sg.usrd, (void**)&rtn, sizeof(jl_vo),0);
	// GL VBO
	_jl_gl_buffer_make(pjlc, &rtn->gl);
	// GL Texture Buffer
	_jl_gl_buffer_make(pjlc, &rtn->bt);
	// Converted Vertices & Vertex Count
	rtn->cv = NULL;
	jl_gl_vo_vertices(pjlc, rtn, xyzw, vertices); // cv & vc
	// Converted Colors
	rtn->cc = NULL;
	// Rendering Style = Polygon
	rtn->rs = 0;
	return rtn;
}

static void _jl_gl_vrtx_updatevectors(jvct_t *pjlc, uint8_t *update,
	GLint* cliprange, GLint* position)
{
	_jl_gl_setv(pjlc, 0, *position, 3);
	if(*update == 1) {
		glUniform4f(*cliprange,
			pjlc->gl.cliprange.x, pjlc->gl.cliprange.y,
			pjlc->gl.cliprange.w, pjlc->gl.cliprange.h);
		*update = 0;
	}
}

static void _jl_gl_updatetex(jvct_t *pjlc) {
	_jl_gl_vrtx_updatevectors(pjlc, &pjlc->gl.update,
		&pjlc->gl.uniforms.cliprange,
		&pjlc->gl.attr.tex.position);
}

static void _jl_gl_updateclr(jvct_t *pjlc) {
	_jl_gl_vrtx_updatevectors(pjlc, &pjlc->gl.update2,
		&pjlc->gl.uniforms.cliprangeclr,
		&pjlc->gl.attr.clr.position);	
}

static void _jl_gl_update_clip_pane(jvct_t *pjlc) {
	if(pjlc->gl.whichprg == JL_GL_SLPR_TEX) {
		_jl_gl_updatetex(pjlc);
	}else if(pjlc->gl.whichprg == JL_GL_SLPR_CLR){
		_jl_gl_updateclr(pjlc);
	}
}

static void _jl_gl_setp(jvct_t *pjlc, jl_gl_slpr id) {
	if(pjlc->gl.whichprg != id) {
		pjlc->gl.whichprg = id;
		_jl_gl_usep(pjlc, pjlc->gl.prgs[id]);
	}
}

static void _jl_gl_col_begin(jvct_t *pjlc, jl_vo* pv) {
	//Free anything old
	if(pv->cc != NULL) jl_me_alloc(pjlc->sg.usrd, (void**)&pv->cc, 0, 0);
	//Allocate memory
	jl_me_alloc(pjlc->sg.usrd,
		(void**)&pv->cc, pv->vc * sizeof(float) * 4, 0);
}

static void _jl_gl_col_end(jvct_t *pjlc, jl_vo* pv) {
	_jl_gl_buff_set(pjlc, pv->bt, pv->cc, pv->vc * 4);
	_jl_gl_setv(pjlc, pv->bt, pjlc->gl.attr.clr.acolor, 4);
}

void jl_gl_set_clippane(jvct_t *pjlc, float xmin, float xmax, float ymin, float ymax) {
	pjlc->gl.update = 1;
	pjlc->gl.update2 = 1;
	pjlc->gl.cliprange.x = (xmin*pjlc->dl.multiplyx) + pjlc->dl.shiftx;
	pjlc->gl.cliprange.y = (xmax*pjlc->dl.multiplyx) + pjlc->dl.shiftx;
	pjlc->gl.cliprange.w = (ymin*pjlc->dl.multiplyy) + pjlc->dl.shifty;
	pjlc->gl.cliprange.h = ((-pjlc->dl.multiplyy)*(ymax)) + pjlc->dl.shifty;
}

void jl_gl_default_clippane(jvct_t *pjlc) {
	jl_gl_set_clippane(pjlc, 0., 1., pjlc->gl.ytrans,
		jl_dl_p(pjlc->sg.usrd) + pjlc->gl.ytrans);
}

void _jl_gl_setmatrix(jvct_t *pjlc) {
	jl_gl_default_clippane(pjlc);
}

//HIGHER LEVEL

// Set vertices for a polygon.
void jl_gl_poly(jvct_t *pjlc, jl_vo* pv, uint8_t vertices, float *xyzw) {
	if(pv == NULL) pv = pjlc->gl.temp_vo;
	// Rendering Style = polygon
	pv->rs = 0;
	if(!xyzw) {
		const float DEFAULT_VERTEX[] = {
			0.,jl_dl_p(pjlc->sg.usrd) + pjlc->gl.ytrans,0.,
			0.,0.,0.,
			1.,0.,0.,
			1.,jl_dl_p(pjlc->sg.usrd) + pjlc->gl.ytrans,0.
		};
		xyzw = alloca(12 * sizeof(float));
		memcpy(xyzw, DEFAULT_VERTEX, 12 * sizeof(float));
		vertices = 4;
	}
	// Set the vertices of vertex object "pv"
	jl_gl_vo_vertices(pjlc, pv, xyzw, vertices);
}

// Set vertices for vector triangles.
void jl_gl_vect(jvct_t *pjlc, jl_vo* pv, uint8_t vertices, float *xyzw) {
	if(pv == NULL) pv = pjlc->gl.temp_vo;
	// Rendering Style = triangles
	pv->rs = 1;
	// Set the vertices of vertex object "pv"
	jl_gl_vo_vertices(pjlc, pv, xyzw, vertices);
}

// Set Texturing to Gradient Color "rgba" { (4 * vertex count) values }
void jl_gl_clrg(jvct_t *pjlc, jl_vo* pv, uint8_t *rgba) {
	int i;

	if(pv == NULL) pv = pjlc->gl.temp_vo;
	_jl_gl_col_begin(pjlc, pv);
	//Set RGBA for each vertex
	for(i = 0; i < pv->vc; i++) { 
		pv->cc[(i * 4) + 0] = ((double) rgba[(i * 4) + 0]) / 255.;
		pv->cc[(i * 4) + 1] = ((double) rgba[(i * 4) + 1]) / 255.;
		pv->cc[(i * 4) + 2] = ((double) rgba[(i * 4) + 2]) / 255.;
		pv->cc[(i * 4) + 3] = ((double) rgba[(i * 4) + 3]) / 255.;
	}
	_jl_gl_col_end(pjlc, pv);
}

// Set Texturing to Solid Color "rgba" { 4 values }
void jl_gl_clrs(jvct_t *pjlc, jl_vo* pv, uint8_t *rgba) {
	int i;

	if(pv == NULL) pv = pjlc->gl.temp_vo;
	_jl_gl_col_begin(pjlc, pv);

	//Set RGBA for each vertex
	for(i = 0; i < pv->vc; i++) { 
		pv->cc[(i * 4) + 0] = ((double) rgba[0]) / 255.;
		pv->cc[(i * 4) + 1] = ((double) rgba[1]) / 255.;
		pv->cc[(i * 4) + 2] = ((double) rgba[2]) / 255.;
		pv->cc[(i * 4) + 3] = ((double) rgba[3]) / 255.;
	}
	_jl_gl_col_end(pjlc, pv);
}

// Set texturing to a bitmap
void jl_gl_txtr(jvct_t *pjlc, jl_vo* pv, u08t map, u08t a, u16t pgid, u16t pi) {
	if(pv == NULL) pv = pjlc->gl.temp_vo;
	// Set Simple Variabes
	pv->a = ((float)a) / 255.f;
	pv->g = pgid;
	pv->i = pi;
	// Make sure non-textured colors aren't attempted
	if(pv->cc != NULL) jl_me_alloc(pjlc->sg.usrd, (void**)&pv->cc, 0, 0);
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
		_jl_gl_buff_set(pjlc, pv->bt, tex1, 8);
	}else{
		float tex2[] =
		{	
			0. + (2./1024.), 0.,
			0. + (2./1024.), 1.,
			1., 1.,
			1., 0.
		};
		_jl_gl_buff_set(pjlc, pv->bt, tex2, 8);
	}
	_jl_gl_setv(pjlc, pv->bt, pjlc->gl.attr.tex.texpos, 2);
}

//Draw object with "vertices" vertices.  The vertex data is in "x","y" and "z".
//"map" refers to the charecter map.  0 means don't zoom in to one charecter.
//Otherwise it will zoom in x16 to a single charecter
/**
 * If "pv" is NULL then draw what's on the temporary buffer
 * Else render vertex object "pv" on the screen.
*/
void jl_gl_draw(jvct_t *pjlc, jl_vo* pv) {
	if(pv == NULL) pv = pjlc->gl.temp_vo;
	// Use "pv->cc" to determine which shader to use: texturing or coloring?
	_jl_gl_setp(pjlc, pv->cc == NULL ? JL_GL_SLPR_TEX : JL_GL_SLPR_CLR);
	// Set texture and transparency if texturing.
	if(pv->cc == NULL) {
		_jl_gl_setalpha(pjlc, pv->a);
		_jl_gl_bind(pjlc, pv->g, pv->i); // Bind the texture
	}
	// Bind Position Buffer
	_jl_gl_buff_use(pjlc, pv->gl);
	// Update the clipping pane & position in shader.
	_jl_gl_update_clip_pane(pjlc);
	// Finally, draw the image!
	_jl_gl_draw_arrays(pjlc, pv->rs ? GL_TRIANGLES : GL_TRIANGLE_FAN,
		pv->vc);
}

int32_t _jl_gl_getu(jvct_t *pjlc, GLuint prg, char *var) {
	return glGetUniformLocation(prg, var);
	int32_t a;
	if((a = glGetUniformLocation(pjlc->gl.prgs[JL_GL_SLPR_TEX], var)) == -1) {
		_jl_fl_errf(pjlc, ":opengl: bad name; is:\n:");
		_jl_fl_errf(pjlc, var);
		jl_sg_kill(pjlc->sg.usrd, "\n");
	}
	_jl_gl_cerr(pjlc, 0,"glGetUniformLocation");
	return a;
}

void _jl_gl_geta(jvct_t *pjlc, GLuint prg, s32t *attrib, const char *title) {
	if((*attrib
		= glGetAttribLocation(prg, title)) == -1)
	{
		 jl_sg_kill(pjlc->sg.usrd,
		 	"attribute name is either reserved or non-existant");
	}
}

//Load and create all resources
static inline void _jl_gl_make_res(jvct_t *pjlc) {
	jl_io_offset(pjlc->sg.usrd, JL_IO_SIMPLE, "GLIN");
	//set up opengl

	jl_io_printc(pjlc->sg.usrd, "setting properties...\n");
	//Disallow Dither & Depth Test
	_jl_gl_init_disable_extras(pjlc);
	//Set alpha=0 to transparent
	_jl_gl_init_enable_alpha(pjlc);		
	jl_io_printc(pjlc->sg.usrd, "set glproperties.\n");

	jl_io_printc(pjlc->sg.usrd, "making GLSL programs....\n");
	pjlc->gl.prgs[JL_GL_SLPR_TEX] = createProgram(pjlc, source_vert_tex, source_frag_tex);
	pjlc->gl.prgs[JL_GL_SLPR_CLR] = createProgram(pjlc, source_vert_clr, source_frag_clr);
	jl_io_printc(pjlc->sg.usrd, "made programs.\n");

	jl_io_printc(pjlc->sg.usrd, "setting up shaders....\n");
	if(pjlc->gl.uniforms.textures == NULL)
		jl_sg_kill(pjlc->sg.usrd, "Couldn't create uniforms");
	pjlc->gl.uniforms.textures[0][0] =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX], "texture");
	pjlc->gl.uniforms.multiply_alpha =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX], "multiply_alpha");
	pjlc->gl.uniforms.cliprange =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX], "cliprange");
	pjlc->gl.uniforms.cliprangeclr =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_CLR], "cliprange");
		
	jl_io_printc(pjlc->sg.usrd, "setting up tex shader attrib's....\n");
	_jl_gl_geta(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX],
		&pjlc->gl.attr.tex.position, "position");
	_jl_gl_geta(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX],
		&pjlc->gl.attr.tex.texpos, "texpos");
	jl_io_printc(pjlc->sg.usrd, "setting up clr shader attrib's....\n");
	_jl_gl_geta(pjlc, pjlc->gl.prgs[JL_GL_SLPR_CLR],
		&pjlc->gl.attr.clr.position, "position");
	_jl_gl_geta(pjlc, pjlc->gl.prgs[JL_GL_SLPR_CLR],
		&pjlc->gl.attr.clr.acolor, "acolor");
	jl_io_printc(pjlc->sg.usrd, "set up shaders.\n");
	jl_io_printc(pjlc->sg.usrd, "making temporary vertex object....\n");
	pjlc->gl.temp_vo = jl_gl_vo_make(pjlc, NULL, 0);
	jl_io_printc(pjlc->sg.usrd, "made temporary vertex object!\n");
	jl_io_close_block(pjlc->sg.usrd); //Close Block "GLIN"
}

void _jl_gl_init(jvct_t *pjlc) {
#ifndef JLVM_USEL_GLES
	printf(" [JLVM/GLEW] init\n");
	if(glewInit()!=GLEW_OK)
		jl_sg_kill(pjlc->sg.usrd, "glew fail!(no sticky)");
#endif
	_jl_gl_make_res(pjlc);
	//Textures on by default
	pjlc->gl.whichprg = JL_GL_SLPR_TEX;
	_jl_gl_usep(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX]);
}

//		glVertexPointer(3, GL_FLOAT, 0, head);
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glDrawArrays(GL_TRIANGLES, 0, 6);
