#!/bin/bash

set -e
set -u
SCRIPT=$(cd ${0%/*} && echo $PWD/${0##*/})
PREFIX=`dirname $SCRIPT`
rm -rf $PREFIX/bin
rm -rf $PREFIX/lib
rm -rf $PREFIX/doc
rm -rf $PREFIX/include
rm -rf $PREFIX/dist
rm -rf $PREFIX/man
rm -rf $PREFIX/share/doc
rm -rf $PREFIX/share/man
rm -rf $PREFIX/share/opencv
rm -rf $PREFIX/share/aclocal
rm -rf $PREFIX/share/cmake-2.8
rm -rf $PREFIX/share/ffmpeg

exit 0
