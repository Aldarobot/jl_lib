#!/bin/sh

printf "[COMP]compiling separate SDL_mixer object files....\n"
gcc src/lib/SDL_mixer/external/libogg-1.3.1/src/bitwise.c -c -o obj/mix/bitwise.o
gcc src/lib/SDL_mixer/external/libogg-1.3.1/src/framing.c -c -o obj/mix/framing.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/analysis.c -c -o obj/mix/analysis.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/bitrate.c -c -o obj/mix/bitrate.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/block.c -c -o obj/mix/block.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/codebook.c -c -o obj/mix/codebook.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/envelope.c -c -o obj/mix/envelope.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/floor0.c -c -o obj/mix/floor0.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/floor1.c -c -o obj/mix/floor1.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/info.c -c -o obj/mix/info.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/lookup.c -c -o obj/mix/lookup.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/lpc.c -c -o obj/mix/lpc.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/lsp.c -c -o obj/mix/lsp.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/mapping0.c -c -o obj/mix/mapping0.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/mdct.c -c -o obj/mix/mdct.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/psy.c -c -o obj/mix/psy.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/registry.c -c -o obj/mix/registry.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/res0.c -c -o obj/mix/res0.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/sharedbook.c -c -o obj/mix/sharedbook.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/smallft.c -c -o obj/mix/smallft.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/synthesis.c -c -o obj/mix/synthesis.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/vorbisenc.c -c -o obj/mix/vorbisenc.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/vorbisfile.c -c -o obj/mix/vorbisfile.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/window.c -c -o obj/mix/window.o
gcc src/lib/SDL_mixer/music_ogg.c -c -o obj/mix/music_ogg.o
gcc src/lib/SDL_mixer/dynamic_ogg.c -c -o obj/mix/dynamic_ogg.o
gcc src/lib/SDL_mixer/load_ogg.c -c -o obj/mix/load_ogg.o
gcc src/lib/SDL_mixer/load_aiff.c -c -o obj/mix/load_aiff.o
gcc src/lib/SDL_mixer/load_voc.c -c -o obj/mix/load_voc.o
gcc src/lib/SDL_mixer/effects_internal.c -c -o obj/mix/effects_internal.o
gcc src/lib/SDL_mixer/music.c -c -o obj/mix/music.o
gcc src/lib/SDL_mixer/effect_position.c -c -o obj/mix/effect_position.o
gcc src/lib/SDL_mixer/mixer.c -c -o obj/mix/mixer.o

cd obj/mix/

printf "[COMP]compiling singular SDL_mixer object file...\n"
ar csr ../jlvm_sdl_mixer.o bitwise.o framing.o analysis.o bitrate.o block.o codebook.o envelope.o floor0.o floor1.o info.o  lookup.o lpc.o lsp.o mapping0.o mdct.o psy.o registry.o res0.o sharedbook.o smallft.o synthesis.o vorbisenc.o vorbisfile.o window.o music_ogg.o dynamic_ogg.o load_ogg.o load_aiff.o load_voc.o effects_internal.o music.o effect_position.o mixer.o

printf "[COMP]done!\n"
