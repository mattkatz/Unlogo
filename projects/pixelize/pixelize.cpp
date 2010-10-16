/*
 *  pixelize.cpp
 *
 *  Created by Jeffrey Crouse
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "Image.h"

using namespace fh;
using namespace std;

int framenum=0;

extern "C" int init( const char* argstr )
{
	try {
		log(LOG_LEVEL_DEBUG, "Welcome to framehack, using OpenCV version %s (%d.%d.%d)\n",
			CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
		
		// Parse arguments.
		vector<string> argv = split(argstr, ":");
		int argc = argv.size();
		if(argc < 1)
		{
			log(LOG_LEVEL_ERROR, "You must supply at least 1 arguments.");
			exit(-1);
		}
		
		namedWindow("input");		cvMoveWindow("input", 0, 0);
		namedWindow("output");		cvMoveWindow("output", 650, 0);
		
		return 0;
	}
	catch ( ... ) {
		return -1;
	}
}

extern "C" int uninit()
{
	return 0;
}


extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
	cout << "(frame " << framenum << ")  ";
	Image input( width, height, src[0], src_stride[0]);
	if(input.empty()) return 1;
	
	input.show("input");
	
	
	Image output(width, height, dst[0], dst_stride[0]);			// point the 'output' image to the FFMPEG data array	
	output.copyFromImage(input);								// copy input into the output memory
	output.pixelize(width/40.0);
	
	output.text("pixelize", 20, 20);
	
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst
	
	output.show("output");
	waitKey(3);	// needed to update windows.
	
	framenum++;
	return 0;
}
