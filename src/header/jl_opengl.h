// SDL OpenGL 2
#if JL_GLTYPE == JL_GLTYPE_SDL_GL2
	#include "lib/SDL/header/SDL_opengl.h"
	#include "lib/glext.h"
#endif

// OpenGL 2
#if JL_GLTYPE == JL_GLTYPE_OPENGL2
	#ifdef APPLE
		#include <OpenGL/glext.h>
	#elif JL_PLAT == JL_PLAT_COMPUTER
//		#include <GL/glext.h>
		#include "../lib/glext.h"
	#else
		#error "cogl ain't supported by non-pc comps, man!"
	#endif
	#include "lib/glew/glew.h"
	#define JL_GLTYPE_HAS_GLEW
#endif

// SDL OpenGLES 2
#if JL_GLTYPE == JL_GLTYPE_SDL_ES2
	#if JL_PLAT == JL_PLAT_COMPUTER
		#include <SDL2/SDL_opengles2.h>
	#else
		#include "lib/sdl/header/SDL_opengles2.h"
	#endif
#endif
	
// OpenGLES 2
#if JL_GLTYPE == JL_GLTYPE_OPENES2
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif