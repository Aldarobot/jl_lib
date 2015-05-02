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
	"uniform vec4 transform;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec4 acolor;\n"
	"\n"
	"varying vec4 vcolor;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position, 1.0) * transform;\n"
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
	"uniform vec4 transform;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec2 texpos;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"varying vec3 poscoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position, 1.0) * transform;\n"
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
	jl_sg_die(pjlc, "\n");
}

void __jl_gl_buff_bind(jvct_t *pjlc, uint32_t buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	_jl_gl_cerr(pjlc, 0,"bind buffer");
}

void _jl_gl_buff_data(jvct_t *pjlc, uint32_t buffer, void *buffer_data,
	u08t buffer_size)
{
	if(buffer == 0) jl_sg_die(pjlc,	"buffer got deleted!");
	__jl_gl_buff_bind(pjlc, buffer);
	glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(float), buffer_data,
		GL_STATIC_DRAW);
	_jl_gl_cerr(pjlc, buffer,"buffer data");
}

void __jl_gl_buff_make(jvct_t *pjlc, uint32_t *buffer)
{
	glGenBuffers(1, buffer);
	_jl_gl_cerr(pjlc, 0,"buffer gen");
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
					jl_sg_die(pjlc, "\n");
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
		jl_sg_die(pjlc, ":couldn't load vertex shader\n");
	}

	GLuint pixelShader = loadShader(pjlc, GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		jl_sg_die(pjlc, ":couldn't load fragment shader\n");
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
					jl_sg_die(pjlc, "\n");
				}else{
					jl_sg_die(pjlc, ":failed malloc\n");
				}
			}else{
				glDeleteProgram(program);
				jl_sg_die(pjlc, ":no info log\n");
			}
		}
	}
	if (program == 0) jl_sg_die(pjlc, "Failed to load program");
	return program;
}

void jl_gl_maketexture(jl_t* pusr, uint16_t gid, uint16_t id,
	void *pixels, int width, int height)
{
	jvct_t *pjlc = pusr->pjlc;
	char *hstr;
	char *wstr;
	
	jl_io_offset(pjlc->sg.usrd, "MKTX", JL_IO_TAG_SIMPLE-JL_IO_TAG_MAX);
	if (!pixels)
		jl_sg_die(pjlc, "null pixels");
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
		jl_sg_die(pjlc, ":Bad Texture, but no gl error? WHY!?\n");
	}
	jl_io_close_block(pjlc->sg.usrd); //Close Block "MKTX"
}

//Lower Level Stuff
void _jl_gl_colr(float r, float g, float b, float a) {
//	glColor4f(r,g,b,a);
//	_jl_gl_cerr(pjlc, 0,"glColor4f");
}

void _jl_gl_usep(jvct_t *pjlc, GLuint prg) {
	if(!prg) { jl_sg_die(pjlc, ":program ain't a prg!\n"); } 
	glUseProgram(prg);
	_jl_gl_cerr(pjlc, 0,"glUseProgram");
}

void _jl_gl_bind(jvct_t *pjlc, u16t g, u16t i) {
	glActiveTexture(GL_TEXTURE0);
	_jl_gl_cerr(pjlc, 0,"glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[g][i]);
	_jl_gl_cerr(pjlc, 0,"glBindTexture");
	glUniform1i(pjlc->gl.uniforms.textures[g][i], i);
	_jl_gl_cerr(pjlc, 0,"glUniform1i");
}

void _jl_gl_setalpha(jvct_t *pjlc, float a) {
	glUniform1f(pjlc->gl.uniforms.multiply_alpha, a);
	_jl_gl_cerr(pjlc, 0,"glUniform1i");
}

//This sets vertex attribute "vertexAttrib" to "pointer".
//Set xyzw to 2 if 2 dimensional coordinates 3 if 3D. etc.
void _jl_gl_setv(jvct_t *pjlc, uint32_t vertexAttrib, uint8_t xyzw) {
//	__jl_gl_buff_bind(pjlc, 0);
	_jl_gl_cerr(pjlc, 0,"glBindBuffer");
	glEnableVertexAttribArray(vertexAttrib);
	_jl_gl_cerr(pjlc, vertexAttrib,"glEnableVertexAttribArray");
	glVertexAttribPointer(
		vertexAttrib,  	//attribute
		xyzw,			// x+y+z = 3
		GL_FLOAT,		// type
		GL_FALSE,		// normalized?
		0,				// stride
		0				// array buffer offset
	);
	_jl_gl_cerr(pjlc, 0,"glVertexAttribPointer");
}

void _jl_gl_drra(jvct_t *pjlc, uint8_t count) {
	glDrawArrays(GL_TRIANGLE_FAN, 0, count);
	_jl_gl_cerr(pjlc, 0,"glDrawArrays");
}

static void _jl_gl_vrtx_updatevectors(jvct_t *pjlc, uint8_t *update,
	GLint* transform, GLint* cliprange, GLint* position)
{
	_jl_gl_setv(pjlc, *position, 3);
	if(*update == 1) {
		glUniform4f(*transform,
			pjlc->gl.transform.x, pjlc->gl.transform.y,
			pjlc->gl.transform.w, pjlc->gl.transform.h);
		glUniform4f(*cliprange,
			pjlc->gl.cliprange.x, pjlc->gl.cliprange.y,
			pjlc->gl.cliprange.w, pjlc->gl.cliprange.h);
		*update = 0;
	}
}

static void _jl_gl_updatetex(jvct_t *pjlc) {
//	jl_gl_setp(pjlc, JL_GL_SLPR_TEX);
	_jl_gl_vrtx_updatevectors(pjlc, &pjlc->gl.update,
		&pjlc->gl.uniforms.transform,
		&pjlc->gl.uniforms.cliprange,
		&pjlc->gl.attr.tex.position);
}

static void _jl_gl_updateclr(jvct_t *pjlc) {
//	jl_gl_setp(pjlc, JL_GL_SLPR_CLR);
	_jl_gl_vrtx_updatevectors(pjlc, &pjlc->gl.update2,
		&pjlc->gl.uniforms.transformclr,
		&pjlc->gl.uniforms.cliprangeclr,
		&pjlc->gl.attr.clr.position);	
}

static void _jl_gl_updatevectors(jvct_t *pjlc) {
	if(pjlc->gl.whichprg == JL_GL_SLPR_TEX) {
		_jl_gl_updatetex(pjlc);
	}else if(pjlc->gl.whichprg == JL_GL_SLPR_CLR){
		_jl_gl_updateclr(pjlc);
	}
}

void jl_gl_setp(jvct_t *pjlc, jl_gl_slpr id) {
	if(pjlc->gl.whichprg != id) {
		pjlc->gl.whichprg = id;
		_jl_gl_usep(pjlc, pjlc->gl.prgs[id]);
	}
	_jl_gl_updatevectors(pjlc);
}

void _jl_gl_setmatrix(jvct_t *pjlc) {
	pjlc->gl.update = 1;
	pjlc->gl.update2 = 1;
	pjlc->gl.cliprange.x = 0.+(pjlc->dl.shiftx/pjlc->dl.multiplyx);
	pjlc->gl.cliprange.y = 1.+(pjlc->dl.shiftx/pjlc->dl.multiplyx);
	pjlc->gl.cliprange.w = 0.+(pjlc->dl.shifty/pjlc->dl.multiplyy);
	pjlc->gl.cliprange.h = (-(jl_dl_p(pjlc->sg.usrd) *
			(pjlc->sg.usrd->smde + 1)))+
		(pjlc->dl.shifty/pjlc->dl.multiplyy);
	pjlc->gl.transform.x = pjlc->dl.multiplyx;
	pjlc->gl.transform.y = pjlc->dl.multiplyy;
	pjlc->gl.transform.w = 2.;
	pjlc->gl.transform.h = 1.;
}

//HIGHER LEVEL

void jl_gl_vrtx(jvct_t *pjlc, u08t vertices, dect *xyzw) {
	glEnable( GL_BLEND );
	_jl_gl_cerr(pjlc, 0,"glEnable( GL_BLEND )");
	glBlendColor(1.f,1.f,1.f,0.f);
	_jl_gl_cerr(pjlc, 0,"glBlendColor");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_jl_gl_cerr(pjlc, 0,"glBlendFunc");
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
	u16t i;
	for(i = 0; i < vertices*3; i+=3) {
		pjlc->gl.buff_vert[i] = xyzw[i]+(pjlc->dl.shiftx/pjlc->dl.multiplyx);
		pjlc->gl.buff_vert[i+1] =
			(-((xyzw[i+1]+ pjlc->gl.ytrans))
			+(pjlc->dl.shifty/pjlc->dl.multiplyy));
		pjlc->gl.buff_vert[i+2] = xyzw[i+2];
	}
	_jl_gl_buff_data(pjlc, pjlc->gl.temp_buff_vrtx,
		pjlc->gl.buff_vert, vertices * 3);
	//
	_jl_gl_updatevectors(pjlc);
}

void jl_gl_clrs(jvct_t *pjlc, float *rgba) {
	jl_gl_setp(pjlc, JL_GL_SLPR_CLR);
	_jl_gl_buff_data(pjlc, pjlc->gl.temp_buff_txtr, rgba, 16);
	_jl_gl_setv(pjlc, pjlc->gl.attr.clr.acolor, 4);
}

void jl_gl_colr(jvct_t *pjlc, float *rgba) {
	jl_gl_setp(pjlc, JL_GL_SLPR_CLR);
	float Rgba[] = {
		rgba[0], rgba[1], rgba[2], rgba[3],
		rgba[0], rgba[1], rgba[2], rgba[3],
		rgba[0], rgba[1], rgba[2], rgba[3],
		rgba[0], rgba[1], rgba[2], rgba[3]
	};
	_jl_gl_buff_data(pjlc, pjlc->gl.temp_buff_txtr, Rgba, 16);
	_jl_gl_setv(pjlc, pjlc->gl.attr.clr.acolor, 4);
}

void jl_gl_txtr(jvct_t *pjlc, u08t map, u08t a, u16t pgid, u16t pi) {
	jl_gl_setp(pjlc, JL_GL_SLPR_TEX);
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
		_jl_gl_buff_data(pjlc, pjlc->gl.temp_buff_txtr, tex1, 8);
	}else{
		float tex2[] =
		{	
			0. + (2./1024.), 0.,
			0. + (2./1024.), 1.,
			1., 1.,
			1., 0.
		};
		_jl_gl_buff_data(pjlc, pjlc->gl.temp_buff_txtr, tex2, 8);
	}
	_jl_gl_setv(pjlc, pjlc->gl.attr.tex.texpos, 2);
	glBindTexture(GL_TEXTURE_2D, pjlc->gl.textures[pgid][pi]);
	_jl_gl_setalpha(pjlc, ((float)a) / 255.f);
}

//Draw object with "vertices" vertices.  The vertex data is in "x","y" and "z".
//"map" refers to the charecter map.  0 means don't zoom in to one charecter.
//Otherwise it will zoom in x16 to a single charecter
void jl_gl_draw(jvct_t *pjlc) {
	_jl_gl_drra(pjlc, 4);
}

int32_t _jl_gl_getu(jvct_t *pjlc, GLuint prg, char *var) {
	return glGetUniformLocation(prg, var);
	int32_t a;
	if((a = glGetUniformLocation(pjlc->gl.prgs[JL_GL_SLPR_TEX], var)) == -1) {
		_jl_fl_errf(pjlc, ":opengl: bad name; is:\n:");
		_jl_fl_errf(pjlc, var);
		jl_sg_die(pjlc, "\n");
	}
	_jl_gl_cerr(pjlc, 0,"glGetUniformLocation");
	return a;
}

void _jl_gl_geta(jvct_t *pjlc, GLuint prg, s32t *attrib, const char *title) {
	if((*attrib
		= glGetAttribLocation(prg, title)) == -1)
	{
		 jl_sg_die(pjlc,
		 	"attribute name is either reserved or non-existant");
	}
}

//Load and create all resources
static inline void _jl_gl_make_res(jvct_t *pjlc) {
	jl_io_offset(pjlc->sg.usrd, "GLIN", JL_IO_TAG_SIMPLE-JL_IO_TAG_MAX);
	//set up opengl
	//TODO:Later, Add Implementation with this enabled
	jl_io_printc(pjlc->sg.usrd, "setting properties...\n");
	glDisable( GL_DEPTH_TEST);
	
	_jl_gl_cerr(pjlc, 0, "glDisable(GL_DEPTH_TEST)");
	glDisable( GL_DITHER );
	_jl_gl_cerr(pjlc, 0, "glDisable(GL_DITHER)");
	glEnable( GL_BLEND );
	_jl_gl_cerr(pjlc, 0, "glEnable(GL_BLEND)");
	jl_io_printc(pjlc->sg.usrd, "set glproperties.\n");
	//Create the temporary buffers.
	jl_io_printc(pjlc->sg.usrd, "creating buffers....\n");
	__jl_gl_buff_make(pjlc, &pjlc->gl.temp_buff_vrtx); 
	__jl_gl_buff_make(pjlc, &pjlc->gl.temp_buff_txtr);
	if(pjlc->gl.temp_buff_vrtx == 0 || pjlc->gl.temp_buff_txtr == 0) {
		jl_sg_die(pjlc,	"buffer is made wrongly.");
	}

	jl_io_printc(pjlc->sg.usrd, "created buffers.\n");

	jl_io_printc(pjlc->sg.usrd, "making GLSL programs....\n");
	pjlc->gl.prgs[JL_GL_SLPR_TEX] = createProgram(pjlc, source_vert_tex, source_frag_tex);
	pjlc->gl.prgs[JL_GL_SLPR_CLR] = createProgram(pjlc, source_vert_clr, source_frag_clr);
	jl_io_printc(pjlc->sg.usrd, "made programs.\n");

	jl_io_printc(pjlc->sg.usrd, "setting up shaders....\n");
	if(pjlc->gl.uniforms.textures == NULL)
		jl_sg_die(pjlc, "Couldn't create uniforms");
	pjlc->gl.uniforms.textures[0][0] =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX], "texture");
	pjlc->gl.uniforms.multiply_alpha =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX], "multiply_alpha");
	pjlc->gl.uniforms.transform =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX], "transform");
	pjlc->gl.uniforms.transformclr =
		_jl_gl_getu(pjlc, pjlc->gl.prgs[JL_GL_SLPR_CLR], "transform");
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
//	_jl_gl_setmatrix(pjlc);
	jl_io_close_block(pjlc->sg.usrd); //Close Block "GLIN"
}

void _jl_gl_init(jvct_t *pjlc) {
#ifndef JLVM_USEL_GLES
	printf(" [JLVM/GLEW] init\n");
	if(glewInit()!=GLEW_OK) jl_sg_die(pjlc, "glew fail!(no sticky)");
#endif
	_jl_gl_make_res(pjlc);
	//Textures on by default
	pjlc->gl.whichprg = JL_GL_SLPR_TEX;
	_jl_gl_usep(pjlc, pjlc->gl.prgs[JL_GL_SLPR_TEX]);
}
