#!/bin/bash

../ffmpeg/ffmpeg -qscale 2 -i IMG_0092.MOV -vf plugin=./logo.so:Dior.jpg -y out.mp4
