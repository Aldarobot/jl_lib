// Version Definitions
#define JL_VERSION "4.0.1e3"
/*
 * Version System:
 * 	major version "." minor version "." debug version "."
 *
 *	A new major version is made every time the current API changes.
 *	A new minor version is made every time new API is added.
 *	A new debug version is made for every release without API changes.
 *	A new "e#" version is made every commit that's not released.
 */

// Uncommented for debugging ( GL/SDL error check && verbose printing. )
#define JL_DEBUG_LIB

//Platform Declarations
#define JL_PLAT_COMPUTER 0 //PC/MAC
#define JL_PLAT_PHONE 1 //ANDROID/IPHONE
#if defined(__IPHONEOS__) || defined(__ANDROID__)
        #define JL_PLAT JL_PLAT_PHONE
#else
        #define JL_PLAT JL_PLAT_COMPUTER
#endif

//Determine Which OpenGL to use.

#define JL_GLTYPE_NO_SPRT 0 // No Support for OpenGL
// GLES version 2
#define JL_GLTYPE_SDL_GL2 1 // Include OpenGL with SDL
#define JL_GLTYPE_OPENGL2 2 // Include OpenGL with glut.
#define JL_GLTYPE_SDL_ES2 3 // Include OpenGLES 2 with SDL
#define JL_GLTYPE_OPENES2 4 // Include OpenGLES 2 standardly.
// Newer versions...

#define JL_GLTYPE JL_GLTYPE_NO_SPRT

// Determine How OpenGL renders to texture.
#define JL_GLRTEX_FBO 1
#define JL_GLRTEX_EGL 2 // NYI
#define JL_GLRTEX_SDL 3 // NYI

// Platform Capabilities.
#if JL_PLAT == JL_PLAT_COMPUTER
	// All Linux Platforms
	#undef JL_GLTYPE
	#define JL_GLTYPE JL_GLTYPE_SDL_ES2
	#define JL_GLRTEX JL_GLRTEX_FBO
	// Windows
	// #define JL_GLTYPE JL_GLTYPE_SDL_GL2
#elif JL_PLAT == JL_PLAT_PHONE
	#undef JL_GLTYPE
	#define JL_GLTYPE JL_GLTYPE_SDL_ES2
	#define JL_GLRTEX JL_GLRTEX_FBO
#else
	#error "NO OpenGL support for this platform!"
#endif
