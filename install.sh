#!/bin/bash

set -e
set -o pipefail
SCRIPT=$(cd ${0%/*} && echo $PWD/${0##*/})
PREFIX=`dirname $SCRIPT`


export PATH=$PREFIX/bin:$PATH
export LD_LIBRARY_PATH=$PREFIX/lib:$LD_LIBRARY_PATH
export CFLAGS="-I$PREFIX/include"
export CPPFLAGS="-I$PREFIX/include"
export LDFLAGS="-L$PREFIX/lib"
export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig

WGET=wget
type -P wget &>/dev/null || { 
WGET=curl -L -O
}

clear
echo -------------------------
echo "Welcome to the Framehack Installer"
echo "Framehack will be installed into $PREFIX"
echo "PATH $PATH"
echo "LD_LIBRARY_PATH $LD_LIBRARY_PATH"
echo "CFLAGS $CFLAGS"
echo "CPPFLAGS $CFLAGS"
echo "LDFLAGS $LDFLAGS"
echo "PKG_CONFIG_PATH $PKG_CONFIG_PATH"
echo -------------------------


read -p "Press any key to start install"
echo "Making the distribution directory..."
mkdir -p $PREFIX/dist
cd $PREFIX/dist



clear
echo -------------------------
echo "Downloading and building LAME"
echo -------------------------
${WGET} http://downloads.sourceforge.net/project/lame/lame/3.98.4/lame-3.98.4.tar.gz
tar -xvf lame-3.98.4.tar.gz
cd lame-3.98.4
./configure --prefix=$PREFIX -enable-shared=no
make && make install
cd $PREFIX/dist


clear
echo -------------------------
echo "Downloading and building YASM"
echo -------------------------
${WGET} http://www.tortall.net/projects/yasm/releases/yasm-1.1.0.tar.gz
tar -xvf yasm-1.1.0.tar.gz
cd yasm-1.1.0
./configure --prefix=$PREFIX
make && make install
cd $PREFIX/dist


clear
echo -------------------------
echo "Downloading and building FAAC"
echo -------------------------
${WGET} http://downloads.sourceforge.net/faac/faac-1.28.tar.gz
tar -xvf faac-1.28.tar.gz
cd faac-1.28
./configure --prefix=$PREFIX --enable-shared=no
make && make install
cd $PREFIX/dist


clear
echo -------------------------
echo "Downloading and building opencore-arm"
echo -------------------------
${WGET} http://downloads.sourceforge.net/project/opencore-amr/opencore-amr/0.1.2/opencore-amr-0.1.2.tar.gz
tar -xvf opencore-amr-0.1.2.tar.gz
cd opencore-amr-0.1.2
./configure --enable-shared=no --prefix=$PREFIX
make && make install clean
cd $PREFIX/dist


clear
echo -------------------------
echo "Downloading and building libogg"
echo -------------------------
${WGET} http://downloads.xiph.org/releases/ogg/libogg-1.2.0.tar.gz
tar -xvf libogg-1.2.0.tar.gz
cd libogg-1.2.0
./configure --enable-shared=no --prefix=$PREFIX
make && make install clean
cd $PREFIX/dist


clear
echo -------------------------
echo "Downloading and building libvorbis"
echo -------------------------
${WGET} http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.1.tar.gz
tar -xvf libvorbis-1.3.1.tar.gz
cd libvorbis-1.3.1
./configure --enable-shared=no --prefix=$PREFIX
make && make install clean
cd $PREFIX/dist


clear
echo -------------------------
echo "Downloading and building libtheora"
echo -------------------------
${WGET} http://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.bz2
tar -xvf libtheora-1.1.1.tar.bz2
cd libtheora-1.1.1
./configure --enable-shared=no --prefix=$PREFIX --with-ogg=$PREFIX --with-vorbis=$PREFIX
make && make install clean
cd $PREFIX/dist


clear
echo -------------------------
echo "Checking out and building x264"
echo -------------------------
${WGET} ftp://ftp.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-20100922-2245.tar.bz2
tar -xvf x264-snapshot-20100922-2245.tar.bz2
cd x264-snapshot-20100922-2245
./configure --prefix=$PREFIX
make && make install clean
cd $PREFIX/dist


clear
echo -------------------------
echo "Checking out and building xvidcore"
echo -------------------------
${WGET} http://downloads.xvid.org/downloads/xvidcore-1.2.2.tar.gz
tar -xvf xvidcore-1.2.2.tar.gz
cd xvidcore/build/generic
./configure --prefix=$PREFIX
make && make install clean
cd $PREFIX/dist


clear
echo -------------------------
echo "Checking out and building FFMPEG"
echo -------------------------
svn co -r 25296 svn://svn.ffmpeg.org/ffmpeg/trunk ffmpeg 
cd $PREFIX/dist/ffmpeg
patch -p0 -i $PREFIX/share/patches/ffmpeg_framehack_rev25296.patch
./configure --prefix=$PREFIX --disable-asm --disable-stripping --enable-debug --enable-gpl --disable-doc --enable-libmp3lame --enable-libx264 --enable-nonfree --enable-libvorbis --enable-libxvid --enable-version3 --enable-pthreads --enable-libfaac --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libtheora --enable-libxvid --enable-x11grab
make && make install
cd $PREFIX/dist


#clear
# echo -------------------------
# echo "Downloading and building Jasper"
# echo -------------------------
# curl -O http://www.ece.uvic.ca/~mdadams/jasper/software/jasper-1.900.1.zip
# unzip jasper-1.900.1.zip
# cd jasper-1.900.1
# ./configure --prefix=$PREFIX
# make
# make install
# cd $PREFIX/dist
# 
# 
# clear
# echo -------------------------
# echo "Downloading and building JPEG"
# echo -------------------------
# curl -O http://www.ijg.org/files/jpegsrc.v8b.tar.gz
# tar -xvf jpegsrc.v8b.tar.gz
# cd jpeg-8b
# ./configure --enable-shared=no --prefix=$PREFIX
# make
# make install
# cd $PREFIX/dist
# 
# 
# clear
# echo -------------------------
# echo "Downloading and building PNG"
# echo -------------------------
# curl -O ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng-1.4.4.tar.gz
# tar -xvf libpng-1.4.4.tar.gz
# cd libpng-1.4.4
# ./configure --enable-shared=no --prefix=$PREFIX
# make
# make install
# cd $PREFIX/dist
# 
# 
# clear
# echo -------------------------
# echo "Downloading and building TIFF"
# echo -------------------------
# ${WGET} ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.4.tar.gz
# tar -xvf tiff-3.9.4.tar.gz
# cd tiff-3.9.4
# ./configure --enable-shared=no --prefix=$PREFIX
# make && make install
# cd $PREFIX/dist


#test if cmake is installed.
type -P cmake &>/dev/null || { 
clear
echo -------------------------
echo "Checking out and building CMake"
echo -------------------------
${WGET} http://www.cmake.org/files/v2.8/cmake-2.8.2.tar.gz
tar -xvf cmake-2.8.2.tar.gz
cd cmake-2.8.2
./configure --prefix=$PREFIX
make && make install clean
cd $PREFIX/dist;
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
export CFLAGS=""
export CPPFLAGS=""
export LDFLAGS=""

clear
echo -------------------------
echo "Checking out and building OpenCV"
echo -------------------------
svn -r 3713 co https://code.ros.org/svn/opencv/trunk/opencv opencv2.1
cd opencv2.1
patch -p0 -i $PREFIX/share/patches/opencv_framehack_rev3713.patch
cmake -G "Unix Makefiles" -D OPENCV_BUILD_3RDPARTY_LIBS=FALSE -D BUILD_EXAMPLES=OFF -D BUILD_NEW_PYTHON_SUPPORT=OFF -D BUILD_TESTS=OFF -D BUILD_SHARED_LIBS=OFF -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=$PREFIX .
make && make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Done!"
echo -------------------------
exit 0
