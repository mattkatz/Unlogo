#!/bin/bash

set -e
set -o pipefail

SDK=/Developer/SDKs/MacOSX10.6.sdk
CFLAGS="-msse -arch x86_64 -arch i686 -nostdinc -g -pipe  \
    -B$SDK/usr/include/gcc -B$SDK/usr/lib/gcc \
    -isystem$SDK/usr/include -F$SDK/System/Library/Frameworks"
LDFLAGS="-arch x86_64 -arch i686 -Wl,-syslibroot,$SDK "
CXXFLAGS="$CFLAGS"
export SDK CFLAGS CXXFLAGS LDFLAGS


WGET=wget
type -P wget &>/dev/null || { 
WGET=curl -L -O
}

clear
echo -------------------------
echo "Welcome to the FFMPEG installer"
echo "CFLAGS $CFLAGS"
echo "CPPFLAGS $CFLAGS"
echo "LDFLAGS $LDFLAGS"
echo -------------------------


read -p "Press any key to start install"
echo "Making the distribution directory..."




set -e
set -o pipefail
SCRIPT=$(cd ${0%/*} && echo $PWD/${0##*/})
PREFIX=`dirname $SCRIPT`
mkdir -p $PREFIX/dist


# Use this method to see if each library is installed
# http://serverfault.com/questions/54736/how-to-check-if-a-library-is-installed




if [ -e "$PREFIX/lib/libmp3lame.a" ]
then
	echo "LAME is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building LAME"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.sourceforge.net/project/lame/lame/3.98.4/lame-3.98.4.tar.gz
	tar -xvf lame-3.98.4.tar.gz
	cd lame-3.98.4
	./configure --prefix=$PREFIX -enable-shared=no
	make && make install
fi



if [ -e "$PREFIX/lib/libyasm.a" ]
then
	echo "YASM is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building YASM"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://www.tortall.net/projects/yasm/releases/yasm-1.1.0.tar.gz
	tar -xvf yasm-1.1.0.tar.gz
	cd yasm-1.1.0
	./configure --prefix=$PREFIX
	make && make install
fi



if [ -e "$PREFIX/lib/libfaac.a" ]
then
	echo "FAAC is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building FAAC"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.sourceforge.net/faac/faac-1.28.tar.gz
	tar -xvf faac-1.28.tar.gz
	cd faac-1.28
	./configure --prefix=$PREFIX --enable-shared=no
	make && make install
fi



if [ -e "$PREFIX/lib/libopencore-amrnb.a" ]
then
	echo "opencore-arm is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building opencore-arm"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.sourceforge.net/project/opencore-amr/opencore-amr/0.1.2/opencore-amr-0.1.2.tar.gz
	tar -xvf opencore-amr-0.1.2.tar.gz
	cd opencore-amr-0.1.2
	./configure --enable-shared=no --prefix=$PREFIX
	make && make install clean
fi




if [ -e "$PREFIX/lib/libogg.a" ]
then
	echo "libogg is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building libogg"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.xiph.org/releases/ogg/libogg-1.2.0.tar.gz
	tar -xvf libogg-1.2.0.tar.gz
	cd libogg-1.2.0
	./configure --enable-shared=no --prefix=$PREFIX
	make && make install clean
	cd $PREFIX/dist
fi



if [ -e "$PREFIX/lib/libvorbis.a" ]
then
	echo "libvorbis is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building libvorbis"
	echo -------------------------
	${WGET} http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.1.tar.gz
	tar -xvf libvorbis-1.3.1.tar.gz
	cd libvorbis-1.3.1
	./configure --enable-shared=no --prefix=$PREFIX
	make && make install clean
fi



if [ -e "$PREFIX/lib/libtheora.a" ]
then
	echo "libvorbis is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building libtheora"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.bz2
	tar -xvf libtheora-1.1.1.tar.bz2
	cd libtheora-1.1.1
	./configure --enable-shared=no --prefix=$PREFIX --with-ogg=$PREFIX --with-vorbis=$PREFIX
	make && make install clean
	cd $PREFIX/dist
fi



if [ -e "$PREFIX/lib/libx264.a" ]
then
	echo "x264 is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building x264"
	echo -------------------------
	${WGET} ftp://ftp.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-20100922-2245.tar.bz2
	tar -xvf x264-snapshot-20100922-2245.tar.bz2
	cd x264-snapshot-20100922-2245
	./configure --prefix=$PREFIX
	make && make install clean
fi


if [ -e "$PREFIX/lib/libxvidcore.a" ]
then
	echo "xvidcore is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building xvidcore"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.xvid.org/downloads/xvidcore-1.2.2.tar.gz
	tar -xvf xvidcore-1.2.2.tar.gz
	cd xvidcore/build/generic
	./configure --prefix=$PREFIX
	make && make install clean
fi




#test if cmake is installed.
type -P cmake &>/dev/null || { 
	clear
	echo -------------------------
	echo "Downloading and building CMake"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://www.cmake.org/files/v2.8/cmake-2.8.2.tar.gz
	tar -xvf cmake-2.8.2.tar.gz
	cd cmake-2.8.2
	./configure --prefix=$PREFIX
	make && make install clean
}


# HACK ALERT!
# OpenCV ships with a bunch of libraries (Jasper, Jpeg, PNG, TIFF) in a folder called "3rdParty"
# I'd like to be able to compile my own versions of these libraries and have OpenCV use them
# instead of the who-knows-how-outdated versions that are in there. 
# The FFMPEG Makefile needs CFLAGS, CPPFLAGS, LDFLAGS set so that it knows where to find 
# stuff like x264, lame, vorbis, etc. 
# But if we install Jasper, Jpeg, PNG, TIFF separately and then try to install OpenCV, we get an 
# error.  Sometimes with JPEG, sometimes with TIFF, depending on the version of OpenCV.
# I guess it confuses the headers that have already been installed with the ones that it is trying to compile?
# I can't seem to stop OpenCV from making them, even with OPENCV_BUILD_3RDPARTY_LIBS=FALSE
# So, there are 2 options: 
# 1. We can compile Jasper, JPEG, TIFF, and then unset these variables before compiling OpenCV so that
# 	it doesn't get confused.  Problem:  OpenCV is going to link against its own "3rdParty" versions of 
#	the libraries no matter what, and it *might* cause problems later on to use a different version
# 2. We can just suck it up and use the versions that come with OpenCV.  To do this, we need to
#	not have these varialbes (which are required for FFMPEG to find x264, faac, etc.) set.

if [ -e "$PREFIX/lib/libopencv_core.a" ]
then
	echo "OpenCV is already installed"
else
	export CFLAGS=""
	export CPPFLAGS=""
	export LDFLAGS=""
	clear
	echo -------------------------
	echo "Checking out and building OpenCV"
	echo -------------------------
	cd $PREFIX/dist;
	svn -r 4374 co https://code.ros.org/svn/opencv/trunk/opencv opencv2.2
	cd opencv2.2
	cmake -G "Unix Makefiles" -D CMAKE_OSX_ARCHITECTURES="i386;x86_64" -D BUILD_EXAMPLES=OFF -D BUILD_NEW_PYTHON_SUPPORT=OFF -D BUILD_TESTS=OFF -D BUILD_SHARED_LIBS=OFF -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=$PREFIX .
	make && make install
	export CFLAGS="-I$PREFIX/include"
	export CPPFLAGS="-I$PREFIX/include"
	export LDFLAGS="-L$PREFIX/lib"
fi


if [ -e "$PREFIX/include/gavl/gavl.h" ]
then
	echo "GAVL is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building gavl-1.1.1"
	echo -------------------------
	cd $PREFIX/dist
	${WGET} http://downloads.sourceforge.net/project/gmerlin/gavl/1.1.1/gavl-1.1.1.tar.gz
	tar -xvf gavl-1.1.1.tar.gz
	cd $PREFIX/dist/gavl-1.1.1
	./configure --prefix=$PREFIX --without-doxygen --disable-libpng
	make
	make install
fi



if [ -e "$PREFIX/include/frei0r.h" ]
then
	echo "Frei0r is already installed"
else
	clear
	echo -------------------------
	echo "Downloading and building frei0r-plugins-1.2"
	echo -------------------------
	export LDFLAGS="-L$PREFIX/lib -framework vecLib"
	cd $PREFIX/dist
	${WGET} http://www.piksel.no/frei0r/releases/frei0r-plugins-1.2.tar.gz
	tar -xvf frei0r-plugins-1.2.tar.gz
	cd $PREFIX/dist/frei0r-plugins-1.2
	./configure --prefix=$PREFIX
	# the 'make' will fail because of a problem with the documentation.  But the build did succeed
	make --keep-going --ignore-errors
	make install
fi



clear
echo -------------------------
echo "Checking out and building FFMPEG"
echo -------------------------
cd $PREFIX/dist
svn co -r 26206 svn://svn.ffmpeg.org/ffmpeg/trunk ffmpeg 
cd $PREFIX/dist/ffmpeg
./configure --prefix=$PREFIX --enable-frei0r --disable-asm --disable-stripping --enable-debug --enable-gpl --disable-doc --enable-libmp3lame --enable-libx264 --enable-nonfree --enable-libvorbis --enable-libxvid --enable-version3 --enable-pthreads --enable-libfaac --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libtheora --enable-libxvid --enable-x11grab
make && make install


#ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.4.tar.gz
#ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng-1.4.4.tar.gz
#http://www.ijg.org/files/jpegsrc.v8b.tar.gz
#http://www.ece.uvic.ca/~mdadams/jasper/software/jasper-1.900.1.zip



clear
echo -------------------------
echo "Done!"
echo -------------------------
exit 0
