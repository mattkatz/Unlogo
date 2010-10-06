#!/bin/bash

set -u
SCRIPT=$(cd ${0%/*} && echo $PWD/${0##*/})
PREFIX=`dirname $SCRIPT`
rm -rf $PREFIX/bin
rm -rf $PREFIX/lib
rm -rf $PREFIX/dist
rm -rf $PREFIX/include
rm -rf $PREFIX/man
rm -rf $PREFIX/share/doc
rm -rf $PREFIX/share/man
rm -rf $PREFIX/share/opencv

exit 0
