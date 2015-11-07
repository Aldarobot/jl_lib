#!/bin/sh
#TODO: DELETE this file

# Remove header files.
rm ../../bin/jl_lib/jlib/jl.o
rm ../../bin/jl_lib/jlib/jl.h
rm ../../bin/jl_lib/jlib/jl_ty.h
rm ../../bin/jl_lib/jlib/jl_en.h
rm ../../bin/jl_lib/jlib/jl_ct.h
#



# Add include files.
printf "[COMP] updating jl_lib's bin folder....\n"
cp --recursive -u -t ../../bin/jl_lib/jlib/ src/include/*
# Copy Object Files
cp ../../obj/lb/lib_zip.o ../../bin/jl_lib/jlib/lib_zip.o
cp ../../obj/lb/lib_SDL_image.o ../../bin/jl_lib/jlib/lib_SDL_image.o
# Update the android directory
printf "[COMP] updating andr...\n"


