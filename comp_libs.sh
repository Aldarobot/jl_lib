#!/bin/sh

printf "[COMP] compiling libraries\n"
JLVM_OUTD=../../obj/jlvm
JLVM_LIBO=../../bin/jlvm/jlib

################################################################################
printf "[COMP] compiling clump....\n"

JLVM_TEMP=src/lib/clump
JLVM_OBJD=$JLVM_OUTD/clmp

gcc $JLVM_TEMP/clump.c -c -o $JLVM_OBJD/clump.o
gcc $JLVM_TEMP/pool.c -c -o $JLVM_OBJD/pool.o
gcc $JLVM_TEMP/list.c -c -o $JLVM_OBJD/list.o
gcc $JLVM_TEMP/hash.c -c -o $JLVM_OBJD/hash.o
gcc $JLVM_TEMP/tree.c -c -o $JLVM_OBJD/tree.o
gcc $JLVM_TEMP/bitarray.c -c -o $JLVM_OBJD/bitarray.o
gcc $JLVM_TEMP/hcodec.c -c -o $JLVM_OBJD/hcodec.o

ar csr $JLVM_LIBO/lib_clump.o $JLVM_OBJD/*

################################################################################
printf "[COMP] compiling SDL_image...\n"

JLVM_TEMP=../../bin/install/SDL2/jlvm_SDL_image
JLVM_OBJD=$JLVM_OUTD/img

gcc $JLVM_TEMP/IMG.c -c -o $JLVM_OBJD/IMG.o
gcc $JLVM_TEMP/IMG_bmp.c -c -o $JLVM_OBJD/IMG_bmp.o
gcc $JLVM_TEMP/IMG_gif.c -c -o $JLVM_OBJD/IMG_gif.o
gcc $JLVM_TEMP/IMG_jpg.c -c -o $JLVM_OBJD/IMG_jpg.o
gcc $JLVM_TEMP/IMG_lbm.c -c -o $JLVM_OBJD/IMG_lbm.o
gcc $JLVM_TEMP/IMG_pcx.c -c -o $JLVM_OBJD/IMG_pcx.o
gcc $JLVM_TEMP/IMG_png.c -c -o $JLVM_OBJD/IMG_png.o
gcc $JLVM_TEMP/IMG_pnm.c -c -o $JLVM_OBJD/IMG_pnm.o
gcc $JLVM_TEMP/IMG_tga.c -c -o $JLVM_OBJD/IMG_tga.o
gcc $JLVM_TEMP/IMG_tif.c -c -o $JLVM_OBJD/IMG_tif.o
gcc $JLVM_TEMP/IMG_webp.c -c -o $JLVM_OBJD/IMG_webp.o
gcc $JLVM_TEMP/IMG_xcf.c -c -o $JLVM_OBJD/IMG_xcf.o
gcc $JLVM_TEMP/IMG_xpm.c -c -o $JLVM_OBJD/IMG_xpm.o
gcc $JLVM_TEMP/IMG_xv.c -c -o $JLVM_OBJD/IMG_xv.o
gcc $JLVM_TEMP/IMG_xxx.c -c -o $JLVM_OBJD/IMG_xxx.o
gcc $JLVM_TEMP/showimage.c -c -o $JLVM_OBJD/showimage.o
ar csr $JLVM_LIBO/lib_sdl_image.o $JLVM_OBJD/*

################################################################################
printf "[COMP] compiling glew...\n"
gcc src/lib/glew/glew.c -o $JLVM_LIBO/lib_glew.o -c

################################################################################
printf "[COMP] compiling libzip ... \n"

JLVM_OBJD=$JLVM_OUTD/libzip
JLVM_TEMP=src/lib/libzip/jni/

gcc $JLVM_TEMP/zip_add.c -o $JLVM_OBJD/zip_add.o -c
gcc $JLVM_TEMP/zip_add_dir.c -o $JLVM_OBJD/zip_add_dir.o -c
gcc $JLVM_TEMP/zip_close.c -o $JLVM_OBJD/zip_close.o -c
gcc $JLVM_TEMP/zip_delete.c -o $JLVM_OBJD/zip_del.o -c
gcc $JLVM_TEMP/zip_dirent.c -o $JLVM_OBJD/zip_dirent.o -c
gcc $JLVM_TEMP/zip_entry_free.c -o $JLVM_OBJD/zip_entry_free.o -c
gcc $JLVM_TEMP/zip_entry_new.c -o $JLVM_OBJD/zip_entry_new.o -c
gcc $JLVM_TEMP/zip_err_str.c -o $JLVM_OBJD/zip_str.o -c
gcc $JLVM_TEMP/zip_error.c -o $JLVM_OBJD/zip_err.o -c
gcc $JLVM_TEMP/zip_error_clear.c -o $JLVM_OBJD/zip_errclr.o -c
gcc $JLVM_TEMP/zip_error_get.c -o $JLVM_OBJD/zip_errget.o -c
gcc $JLVM_TEMP/zip_error_get_sys_type.c -o $JLVM_OBJD/zip_errgetsys.o -c
gcc $JLVM_TEMP/zip_error_strerror.c -o $JLVM_OBJD/zip_strerr.o -c
gcc $JLVM_TEMP/zip_error_to_str.c -o $JLVM_OBJD/zip_errstr.o -c
gcc $JLVM_TEMP/zip_fclose.c -o $JLVM_OBJD/zip_fclose.o -c
gcc $JLVM_TEMP/zip_file_error_clear.c -o $JLVM_OBJD/zip_filerrclr.o -c
gcc $JLVM_TEMP/zip_file_error_get.c -o $JLVM_OBJD/zip_filerrget.o -c
gcc $JLVM_TEMP/zip_file_get_offset.c -o $JLVM_OBJD/zip_filerroffs.o -c
gcc $JLVM_TEMP/zip_file_strerror.c -o $JLVM_OBJD/zip_filestrerr.o -c
gcc $JLVM_TEMP/zip_filerange_crc.c -o $JLVM_OBJD/zip_fr.o -c
gcc $JLVM_TEMP/zip_fopen.c -o $JLVM_OBJD/zip_fopen.o -c
gcc $JLVM_TEMP/zip_fopen_index.c -o $JLVM_OBJD/zip_fopeni.o -c
gcc $JLVM_TEMP/zip_fread.c -o $JLVM_OBJD/zip_fread.o -c
gcc $JLVM_TEMP/zip_free.c -o $JLVM_OBJD/zip_free.o -c
gcc $JLVM_TEMP/zip_get_archive_comment.c -o $JLVM_OBJD/zip_archcmt.o -c
gcc $JLVM_TEMP/zip_get_archive_flag.c -o $JLVM_OBJD/zip_getarchflag.o -c
gcc $JLVM_TEMP/zip_get_file_comment.c -o $JLVM_OBJD/zip_filecomment.o -c
gcc $JLVM_TEMP/zip_get_num_files.c -o $JLVM_OBJD/zip_numfile.o -c
gcc $JLVM_TEMP/zip_get_name.c -o $JLVM_OBJD/zip_getname.o -c
gcc $JLVM_TEMP/zip_memdup.c -o $JLVM_OBJD/zip_memdup.o -c
gcc $JLVM_TEMP/zip_name_locate.c -o $JLVM_OBJD/zip_namloc.o -c
gcc $JLVM_TEMP/zip_new.c -o $JLVM_OBJD/zip_new.o -c
gcc $JLVM_TEMP/zip_open.c -o $JLVM_OBJD/zip_open.o -c
gcc $JLVM_TEMP/zip_rename.c -o $JLVM_OBJD/zip_rename.o -c
gcc $JLVM_TEMP/zip_replace.c -o $JLVM_OBJD/zip_replace.o -c
gcc $JLVM_TEMP/zip_set_archive_comment.c -o $JLVM_OBJD/zip_sac.o -c
gcc $JLVM_TEMP/zip_set_archive_flag.c -o $JLVM_OBJD/zip_saf.o -c
gcc $JLVM_TEMP/zip_set_file_comment.c -o $JLVM_OBJD/zip_sfc.o -c
gcc $JLVM_TEMP/zip_source_buffer.c -o $JLVM_OBJD/zip_srcbuff.o -c
gcc $JLVM_TEMP/zip_source_file.c -o $JLVM_OBJD/zip_srcfile.o -c
gcc $JLVM_TEMP/zip_source_filep.c -o $JLVM_OBJD/zip_filep.o -c
gcc $JLVM_TEMP/zip_source_free.c -o $JLVM_OBJD/zip_srcfree.o -c
gcc $JLVM_TEMP/zip_source_function.c -o $JLVM_OBJD/zip_srcfunc.o -c
gcc $JLVM_TEMP/zip_source_zip.c -o $JLVM_OBJD/zip_srczip.o -c
gcc $JLVM_TEMP/zip_set_name.c -o $JLVM_OBJD/zip_setnae.o -c
gcc $JLVM_TEMP/zip_stat.c -o $JLVM_OBJD/zip_stat.o -c
gcc $JLVM_TEMP/zip_stat_index.c -o $JLVM_OBJD/zip_statindx.o -c
gcc $JLVM_TEMP/zip_stat_init.c -o $JLVM_OBJD/zip_statinit.o -c
gcc $JLVM_TEMP/zip_strerror.c -o $JLVM_OBJD/zip_strerror.o -c
gcc $JLVM_TEMP/zip_unchange.c -o $JLVM_OBJD/zip_unchange.o -c
gcc $JLVM_TEMP/zip_unchange_all.c -o $JLVM_OBJD/zip_unchangeall.o -c
gcc $JLVM_TEMP/zip_unchange_archive.c -o $JLVM_OBJD/zip_unchangearc.o -c
gcc $JLVM_TEMP/zip_unchange_data.c -o $JLVM_OBJD/zip_unchagedata.o -c

ar csr $JLVM_LIBO/lib_zip.o $JLVM_OBJD/*.o

################################################################################
printf "[COMP] compiling SDL_mixer....\n"
JLVM_TEMP=../../bin/install/SDL2/SDL_mixer
JLVM_OBJD=$JLVM_OUTD/mix
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

ar csr $JLVM_LIBO/lib_sdl_mixer.o $JLVM_OBJD/*.o

################################################################################
printf "[COMP] done!\n"
