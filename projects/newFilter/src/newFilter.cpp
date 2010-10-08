#include <iostream>

/*
 *  unlogo.cpp
 *  unlogo
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
Image prev;			// The last frame -- for optical flow.
int framenum=0;					// Current frame number

extern "C" int init( const char* argstr )
{
	try {
		log(LOG_LEVEL_DEBUG, "Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
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
	output.text("unlogo", 20, 20);
	
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst
	
	output.show("output");
	waitKey(3);	// needed to update windows.
	
	framenum++;
	return 0;
}



int main(int argc, char * const argv[])
{
	// Imitating the arguments that FFMPEG gives us through AVFilter.
	// see process() in unlogo.cpp
	int width, height;
	uint8_t* src[4];
	uint8_t* dst[4];
	int src_stride[4];
	int dst_stride[4];
	
	// Open the video
	cv::VideoCapture cap("resources/FullHouse.mp4");
	cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
	
    if(!cap.isOpened())  
	{
		std::cout << "Can not open video source" << std::endl;
        return -1;
	}
	width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	dst[0]= new uint8_t[ width* height * 3 ];
	dst_stride[0] = width * 3;
	
	init(argv[2]);													// from unlogo.cpp
	
	cv::Mat frame;
	for(;;)
    {
        cap >> frame; // get a new frame from camera
		if(frame.empty()) break;
		
		src[0] = frame.data;
		src_stride[0] = frame.step;
		
		process(dst, dst_stride, src, src_stride, width, height);  // from unlogo.cpp
	}
	
	uninit();														// from unlogo.cpp
	
	std::cout << "Exiting ..." << std::endl;
	return 0;
}