/*
 *  main.cpp
 *  unlogo testing program
 *
 *  Created by Jeffrey Crouse on 8/18/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 *	This small program simply opens a cv::VideoCapture with the first argument
 *	and then starts feeding the frames to the unlogo.cpp FFMPEG callbacks.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <highgui.h>

// Bring in the functions from unlogo.cpp
extern "C" {
	int init( const char* argstr );
	int uninit();
	int process( uint8_t* dst[4], int dst_stride[4],
				uint8_t* src[4], int src_stride[4],
				int width, int height);
} 


// Imitating the arguments that FFMPEG gives us through AVFilter.
// see process() in unlogo.cpp
int width, height;
uint8_t* src[4];
uint8_t* dst[4];
int src_stride[4];
int dst_stride[4];


int main(int argc, char * const argv[])
{
	// Open the video
	cv::VideoCapture cap(argv[1]);
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

