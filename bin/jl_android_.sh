#!/bin/sh

#printf "[JL/ANDR] exporting your program to android....\n"
#sh androidbuild.sh com.company.app src/*.c
#printf "[J/ANDR] exported your program to android!\n"

args=("$@")
DEBUG=${args[0]}
JLL_PATH=${args[1]}
PROGNAME=${args[2]}
PACKNAME=${args[3]}
USERNAME=${args[4]}
USERPROG=${args[5]}
NDK_PATH=${args[6]}
SDK_PATH=${args[7]}
ANDROID_PROJECT=${args[8]}
ANDROID_MEDIA=${args[9]}
ANDROID_HOME=${args[10]}
IS_DEBUG=${args[11]}

export ANDROID_HOME

printf "JLL_PATH = $JLL_PATH\n"
printf "DEBUG = $DEBUG\n"
printf "PROGNAME = $PROGNAME\n"
printf "PACKNAME = $PACKNAME\n"
printf "USERNAME = $USERNAME\n"
printf "USERPROG = $USERPROG\n"
printf "NDK_PATH = $NDK_PATH\n"
printf "SDK_PATH = $SDK_PATH\n"
printf "ANDROID_PROJECT = $ANDROID_PROJECT\n"
printf "ANDROID_MEDIA = $ANDROID_MEDIA\n"
printf "ANDROID_HOME = $ANDROID_HOME\n"

cp $JLL_PATH/android-src/AndroidManifest.xml \
$ANDROID_PROJECT/AndroidManifest.xml
rm $ANDROID_PROJECT/res/values/strings.xml
cp $JLL_PATH/android-src/strings.xml \
$ANDROID_PROJECT/res/values/strings.xml
cp $JLL_PATH/android-src/build.xml \
$ANDROID_PROJECT/build.xml
sed -i "s|JLR_USERNAME|$USERNAME|g" \
$ANDROID_PROJECT/build.xml
sed -i "s|JLR_APPNAME|$PACKNAME|g" \
$ANDROID_PROJECT/build.xml
sed -i "s|JLR_APPNAME|$PROGNAME|g" \
$ANDROID_PROJECT/res/values/strings.xml
sed -i "s|JLR_USERNAME|$USERNAME|g" \
$ANDROID_PROJECT/AndroidManifest.xml
sed -i "s|JLR_APPNAME|$PACKNAME|g" \
$ANDROID_PROJECT/AndroidManifest.xml
sed -i "s|JLR_IS_DEBUG|$IS_DEBUG|g" \
$ANDROID_PROJECT/AndroidManifest.xml
rm -r $ANDROID_PROJECT/src/jlw/
mkdir -p $ANDROID_PROJECT/src/jlw/$USERNAME/$PACKNAME/
cp \
$JLL_PATH/android-src/jl_Activity.java \
$ANDROID_PROJECT/src/jlw/$USERNAME/\
$PACKNAME/jl_Activity.java
sed -i "s|JLR_USERNAME|$USERNAME|g" \
$ANDROID_PROJECT/src/jlw/$USERNAME/\
$PACKNAME/jl_Activity.java
sed -i "s|JLR_APPNAME|$PACKNAME|g" \
$ANDROID_PROJECT/src/jlw/$USERNAME/\
$PACKNAME/jl_Activity.java

printf "[JL/ANDR] setting up files....\n"
if [ ! -e bin/andr/my-release-key.keystore ];then
	printf "[JL/ANDR] Jarsigner key not found.  For android you must create\n"
	printf "[JL/ANDR] a key.  Create your key for jarsigner:\n"
	keytool -sigalg SHA1withRSA -keyalg RSA -keysize 1024 -genkey -keystore bin/andr/my-release-key.keystore -alias daliasle -validity 3650
fi
cp --recursive -t $ANDROID_PROJECT/jni/src/gen/src/ src/*
rm -r $ANDROID_MEDIA/
mkdir $ANDROID_MEDIA/
cp -l media/genr/* $ANDROID_MEDIA/
cp bin/andr/my-release-key.keystore $ANDROID_PROJECT/
rm $ANDROID_PROJECT/res/drawable/prgm_icon.png
cp media/icon.png $ANDROID_PROJECT/res/drawable/prgm_icon.png

sudo $SDK_PATH/platform-tools/adb kill-server

printf "[JL/ANDR] compiling....\n"
cd $ANDROID_PROJECT
export NDK_MODULE_PATH="$PWD"/jni
echo $NDK_MODULE_PATH
$NDK_PATH/ndk-build $DEBUG && ant clean release

printf "[JL/ANDR] signing jar with jarsigner....\n"
jarsigner -verbose -tsa http://timestamp.digicert.com -sigalg SHA1withRSA -digestalg SHA1 -keystore my-release-key.keystore bin/jlw.$USERNAME.$PACKNAME-release-unsigned.apk daliasle
