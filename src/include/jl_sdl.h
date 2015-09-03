#if JL_PLAT == JL_PLAT_COMPUTER
//	#include "../lib/SDL/header/SDL.h"
	#include <SDL2/SDL.h>
#elif JL_PLAT == JL_PLAT_PHONE
	#include "../../lib/sdl/header/SDL_test_common.h"
	#include "../../lib/sdl/header/SDL_main.h"
#endif
