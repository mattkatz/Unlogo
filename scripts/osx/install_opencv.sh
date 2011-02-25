#!/bin/bash

set -e
set -o pipefail
SCRIPT=$(cd ${0%/*} && echo $PWD/${0##*/})
PREFIX=`dirname $SCRIPT`


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