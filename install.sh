#!/bin/bash

set -e
set -u
set -o pipefail
SCRIPT=$(cd ${0%/*} && echo $PWD/${0##*/})
PREFIX=`dirname $SCRIPT`

clear
echo -------------------------
echo "Welcome to the Framehack Installer"
echo "Install location: $PREFIX"
echo -------------------------


read -p "Press any key to start install"
echo "Making the distribution directory..."
mkdir -p $PREFIX/dist
cd $PREFIX/dist



clear
echo -------------------------
echo "Downloading and Building Jasper"
echo -------------------------
curl -O http://www.ece.uvic.ca/~mdadams/jasper/software/jasper-1.900.1.zip
unzip jasper-1.900.1.zip
cd jasper-1.900.1
./configure --prefix=$PREFIX
make
make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Downloading and Building JPEG"
echo -------------------------
curl -O http://www.ijg.org/files/jpegsrc.v8b.tar.gz
tar -xvf jpegsrc.v8b.tar.gz
cd jpeg-8b
./configure --enable-shared=no --prefix=$PREFIX
make
make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Downloading and Building PNG"
echo -------------------------
curl -O ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng-1.4.4.tar.gz
tar -xvf libpng-1.4.4.tar.gz
cd libpng-1.4.4
./configure --enable-shared=no --prefix=$PREFIX
make
make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Downloading and Building TIFF"
echo -------------------------
curl -L -O ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.4.tar.gz
tar -xvf tiff-3.9.4.tar.gz
cd tiff-3.9.4
./configure --enable-shared=no --prefix=$PREFIX
make
make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Downloading and Building LAME"
echo -------------------------
curl -L -O http://downloads.sourceforge.net/project/lame/lame/3.98.4/lame-3.98.4.tar.gz
tar -xvf lame-3.98.4.tar.gz
cd lame-3.98.4
./configure --enable-shared=no --prefix=$PREFIX
make
make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Checking out and Building FFMPEG"
echo -------------------------
svn co -r 25296 svn://svn.ffmpeg.org/ffmpeg/trunk ffmpeg 
cd ffmpeg
patch -p0 -i $PREFIX/share/patches/ffmpeg_framehack_rev25296.patch
./configure --arch=x86 --enable-libmp3lame --enable-libx264 --enable-nonfree --enable-gpl --disable-doc --prefix=$PREFIX
make
make install
cd $PREFIX/dist



clear
echo -------------------------
echo "Checking out and Building OpenCV"
echo -------------------------
svn co -r 3713 https://code.ros.org/svn/opencv/trunk/opencv 
cd opencv
patch -p0 -i $PREFIX/share/patches/opencv_framehack_rev3713.patch
cmake -G "Unix Makefiles" -D BUILD_TESTS=OFF -D BUILD_SHARED_LIBS=OFF -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_INSTALL_PREFIX=$PREFIX .
make
make install
cd $PREFIX/dist




clear
echo -------------------------
echo "Done!"
echo -------------------------
exit 0
