#!/bin/sh

printf "[COMP]compiling SDL_image...\n"
JLVM_TEMP=../../bin/install/SDL2/SDL_image
JLVM_OBJD=../../obj/jlvm/img
gcc $JLVM_TEMP/IMG.c -c -o $JLVM_OBJD/IMG.o
gcc $JLVM_TEMP/IMG_gif.c -c -o $JLVM_OBJD/gif.o
ar -cvq ../../obj/jlvm/jlvm_sdl_image.a $JLVM_OBJD/IMG.o $JLVM_OBJD/gif.o

printf "[COMP]compiling glew...\n"
gcc src/lib/glew/glew.c -o ../../obj/jlvm/glew.o -c
printf "[COMP]compiling libzip ... \n"
JLVM_TEMP=../../../../../../obj/jlvm/libzip
cd src/lib/libzip/jni/
gcc zip_add.c -o $JLVM_TEMP/zip_add.o -c
gcc zip_add_dir.c -o $JLVM_TEMP/zip_add_dir.o -c
gcc zip_close.c -o $JLVM_TEMP/zip_close.o -c
gcc zip_delete.c -o $JLVM_TEMP/zip_del.o -c
gcc zip_dirent.c -o $JLVM_TEMP/zip_dirent.o -c
gcc zip_entry_free.c -o $JLVM_TEMP/zip_entry_free.o -c
gcc zip_entry_new.c -o $JLVM_TEMP/zip_entry_new.o -c
gcc zip_err_str.c -o $JLVM_TEMP/zip_str.o -c
gcc zip_error.c -o $JLVM_TEMP/zip_err.o -c
gcc zip_error_clear.c -o $JLVM_TEMP/zip_errclr.o -c
gcc zip_error_get.c -o $JLVM_TEMP/zip_errget.o -c
gcc zip_error_get_sys_type.c -o $JLVM_TEMP/zip_errgetsys.o -c
gcc zip_error_strerror.c -o $JLVM_TEMP/zip_strerr.o -c
gcc zip_error_to_str.c -o $JLVM_TEMP/zip_errstr.o -c
gcc zip_fclose.c -o $JLVM_TEMP/zip_fclose.o -c
gcc zip_file_error_clear.c -o $JLVM_TEMP/zip_filerrclr.o -c
gcc zip_file_error_get.c -o $JLVM_TEMP/zip_filerrget.o -c
gcc zip_file_get_offset.c -o $JLVM_TEMP/zip_filerroffs.o -c
gcc zip_file_strerror.c -o $JLVM_TEMP/zip_filestrerr.o -c
gcc zip_filerange_crc.c -o $JLVM_TEMP/zip_fr.o -c
gcc zip_fopen.c -o $JLVM_TEMP/zip_fopen.o -c
gcc zip_fopen_index.c -o $JLVM_TEMP/zip_fopeni.o -c
gcc zip_fread.c -o $JLVM_TEMP/zip_fread.o -c
gcc zip_free.c -o $JLVM_TEMP/zip_free.o -c
gcc zip_get_archive_comment.c -o $JLVM_TEMP/zip_archcmt.o -c
gcc zip_get_archive_flag.c -o $JLVM_TEMP/zip_getarchflag.o -c
gcc zip_get_file_comment.c -o $JLVM_TEMP/zip_filecomment.o -c
gcc zip_get_num_files.c -o $JLVM_TEMP/zip_numfile.o -c
gcc zip_get_name.c -o $JLVM_TEMP/zip_getname.o -c
gcc zip_memdup.c -o $JLVM_TEMP/zip_memdup.o -c
gcc zip_name_locate.c -o $JLVM_TEMP/zip_namloc.o -c
gcc zip_new.c -o $JLVM_TEMP/zip_new.o -c
gcc zip_open.c -o $JLVM_TEMP/zip_open.o -c
gcc zip_rename.c -o $JLVM_TEMP/zip_rename.o -c
gcc zip_replace.c -o $JLVM_TEMP/zip_replace.o -c
gcc zip_set_archive_comment.c -o $JLVM_TEMP/zip_sac.o -c
gcc zip_set_archive_flag.c -o $JLVM_TEMP/zip_saf.o -c
gcc zip_set_file_comment.c -o $JLVM_TEMP/zip_sfc.o -c
gcc zip_source_buffer.c -o $JLVM_TEMP/zip_srcbuff.o -c
gcc zip_source_file.c -o $JLVM_TEMP/zip_srcfile.o -c
gcc zip_source_filep.c -o $JLVM_TEMP/zip_filep.o -c
gcc zip_source_free.c -o $JLVM_TEMP/zip_srcfree.o -c
gcc zip_source_function.c -o $JLVM_TEMP/zip_srcfunc.o -c
gcc zip_source_zip.c -o $JLVM_TEMP/zip_srczip.o -c
gcc zip_set_name.c -o $JLVM_TEMP/zip_setnae.o -c
gcc zip_stat.c -o $JLVM_TEMP/zip_stat.o -c
gcc zip_stat_index.c -o $JLVM_TEMP/zip_statindx.o -c
gcc zip_stat_init.c -o $JLVM_TEMP/zip_statinit.o -c
gcc zip_strerror.c -o $JLVM_TEMP/zip_strerror.o -c
gcc zip_unchange.c -o $JLVM_TEMP/zip_unchange.o -c
gcc zip_unchange_all.c -o $JLVM_TEMP/zip_unchangeall.o -c
gcc zip_unchange_archive.c -o $JLVM_TEMP/zip_unchangearc.o -c
gcc zip_unchange_data.c -o $JLVM_TEMP/zip_unchagedata.o -c
cd ../../../../
ar -cvq ../../bin/jlvm/jlib/libzip.o ../../obj/jlvm/libzip/*.o

printf "[COMP]compiling separate SDL_mixer object files....\n"
JLVM_TEMP=../../bin/install/SDL2/SDL_mixer
JLVM_OBJD=../../obj/jlvm/mix
gcc $JLVM_TEMP/external/libogg-1.3.1/src/bitwise.c -c -o $JLVM_OBJD/bitwise.o
gcc $JLVM_TEMP/external/libogg-1.3.1/src/framing.c -c -o $JLVM_OBJD/framing.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/analysis.c -c -o $JLVM_OBJD/analysis.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/bitrate.c -c -o $JLVM_OBJD/bitrate.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/block.c -c -o $JLVM_OBJD/block.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/codebook.c -c -o $JLVM_OBJD/codebook.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/envelope.c -c -o $JLVM_OBJD/envelope.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/floor0.c -c -o $JLVM_OBJD/floor0.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/floor1.c -c -o $JLVM_OBJD/floor1.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/info.c -c -o $JLVM_OBJD/info.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/lookup.c -c -o $JLVM_OBJD/lookup.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/lpc.c -c -o $JLVM_OBJD/lpc.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/lsp.c -c -o $JLVM_OBJD/lsp.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/mapping0.c -c -o $JLVM_OBJD/mapping0.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/mdct.c -c -o $JLVM_OBJD/mdct.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/psy.c -c -o $JLVM_OBJD/psy.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/registry.c -c -o $JLVM_OBJD/registry.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/res0.c -c -o $JLVM_OBJD/res0.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/sharedbook.c -c -o $JLVM_OBJD/sharedbook.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/smallft.c -c -o $JLVM_OBJD/smallft.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/synthesis.c -c -o $JLVM_OBJD/synthesis.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/vorbisenc.c -c -o $JLVM_OBJD/vorbisenc.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/vorbisfile.c -c -o $JLVM_OBJD/vorbisfile.o
gcc $JLVM_TEMP/external/libvorbis-1.3.3/lib/window.c -c -o $JLVM_OBJD/window.o
gcc $JLVM_TEMP/music_ogg.c -c -o $JLVM_OBJD/music_ogg.o
gcc $JLVM_TEMP/dynamic_ogg.c -c -o $JLVM_OBJD/dynamic_ogg.o
gcc $JLVM_TEMP/load_ogg.c -c -o $JLVM_OBJD/load_ogg.o
gcc $JLVM_TEMP/load_aiff.c -c -o $JLVM_OBJD/load_aiff.o
gcc $JLVM_TEMP/load_voc.c -c -o $JLVM_OBJD/load_voc.o
gcc $JLVM_TEMP/effects_internal.c -c -o $JLVM_OBJD/effects_internal.o
gcc $JLVM_TEMP/music.c -c -o $JLVM_OBJD/music.o
gcc $JLVM_TEMP/effect_position.c -c -o $JLVM_OBJD/effect_position.o
gcc $JLVM_TEMP/mixer.c -c -o $JLVM_OBJD/mixer.o

cd $JLVM_OBJD/

printf "[COMP]compiling singular SDL_mixer object file...\n"
ar -cvq ../jlvm_sdl_mixer.a bitwise.o framing.o analysis.o bitrate.o block.o codebook.o envelope.o floor0.o floor1.o info.o  lookup.o lpc.o lsp.o mapping0.o mdct.o psy.o registry.o res0.o sharedbook.o smallft.o synthesis.o vorbisenc.o vorbisfile.o window.o music_ogg.o dynamic_ogg.o load_ogg.o load_aiff.o load_voc.o effects_internal.o music.o effect_position.o mixer.o

cd ../../../

printf "[COMP]copying stuff...\n"
cp obj/jlvm/jlvm_sdl2.o bin/jlvm/jlib/jlvm_sdl2.o
cp obj/jlvm/jlvm_sdl_mixer.a bin/jlvm/jlib/jlvm_sdl_mixer.a
cp obj/jlvm/jlvm_sdl_image.a bin/jlvm/jlib/jlvm_sdl_image.a

printf "[COMP]done!\n"
