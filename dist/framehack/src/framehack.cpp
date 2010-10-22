/*
 *  framehack.cpp
 *	This is a small program that opens a video using HighGUI, 
 *  loads a framehack filter, and applys it to the movie.
 *	It is meant to be a simplified replacement for FFMPEG 
 *	used for testing only.
 *
 *  Created by Jeffrey Crouse on 10/11/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <highgui.h>
#include <cvaux.h>
#include <dlfcn.h>

/*
vector<string> split(const string& str, const string& delimiters)
{
	vector<string> tokens;
	
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
	
	return tokens;
}
*/

int main(int argc, char * const argv[])
{
	if(argc<3)
	{
		std::cout << "You must specify a plugin and a movie file." << std::endl;
		std::cout << "eg: framehack myfilter.dylib resources/FullHouse.mp4 [semicolon:separated:args]" << std::endl;
		return -1;
	}
	
	char* pluginPath = argv[1];
	char* videoPath = argv[2];
	char* args = (argc>3) ? argv[3] : NULL;
	//vector<string> strArgs = split(args, ":");
	
	std::cout << "[framehack] Loading Plugin: " << pluginPath << std::endl;
	
	void* handle = dlopen( pluginPath, RTLD_NOW );
	
	if ( !handle ) {
		std::cout << "[framehack] " << dlerror() << std::endl;
		return -1;
	}
    
	dlerror();
	
	int (*init)( const char *args );
	int (*uninit)();
	int (*process)(uint8_t *dst[4], int dst_stride[4],
				   uint8_t *src[4], int src_stride[4],
				   int width, int height );
	
	*(void **) (&init) = dlsym(handle, "init");
	if ( dlerror() != NULL ) {
		std::cout << "cannot find function init" << std::endl;
		return -1;
	}
	*(void **) (&uninit)  = dlsym(handle, "uninit");
	if ( dlerror() != NULL ) {
		std::cout << "cannot find function uninit" << std::endl;
		return -1;
	}
	*(void **) (&process) = dlsym(handle, "process");
	if ( dlerror() != NULL ) {
		std::cout << "cannot find function process" << std::endl;
		return -1;
	}
	
	
	// Imitating the arguments that FFMPEG gives us through AVFilter.
	int width, height;
	uint8_t* src[4];
	uint8_t* dst[4];
	int src_stride[4];
	int dst_stride[4];
	
	// Open the video
	cv::VideoCapture cap(videoPath);
	cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
	
    if(!cap.isOpened())  
	{
		std::cout << "Can not open video source: " <<  videoPath << std::endl;
        return -1;
	}
	width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	dst[0]= new uint8_t[ width* height * 3 ];
	dst_stride[0] = width * 3;
	
	init(args);													
	
	cv::Mat frame;
	for(;;)
    {
        cap >> frame; // get a new frame from video source
		if(frame.empty()) break;
		
		src[0] = frame.data;
		src_stride[0] = frame.step;
		
		process(dst, dst_stride, src, src_stride, width, height);
	}
	
	uninit();
	
	std::cout << "[framehack] Exiting ..." << std::endl;
	return 0;
}