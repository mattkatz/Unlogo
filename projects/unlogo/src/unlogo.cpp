
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "MatchableImage.h"

using namespace fh;


typedef struct Logo
{
	const char* name;  // Kept for convenience and debugging
	Image logo;
	Image replacement;
	Point2f pos;
	int ghostFrames;
	Mat homography;
};


MatchableImage prev;						// The last frame -- for optical flow.
int framenum=0;					// Current frame number

extern "C" int init( const char* argstr )
{
	try {
		log(LOG_LEVEL_DEBUG, "Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
			CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
		
		// Parse arguments.
		vector<string> argv = split(argstr, ":");
		int argc = argv.size();
		if(argc < 9)
		{
			log(LOG_LEVEL_ERROR, "You must supply at least 9 arguments.");
			exit(-1);
		}
		
		
		
#ifdef DEBUG		
		namedWindow("input");		cvMoveWindow("input", 0, 0);
		namedWindow("output");		cvMoveWindow("output", 650, 0);
#endif 
		
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
	MatchableImage input( width, height, src[0], src_stride[0]);
	if(input.empty()) return 1;
	
	
#ifdef DEBUG
	input.show("input");
#endif
	
	
	Image output(width, height, dst[0], dst_stride[0]);			// point the 'output' image to the FFMPEG data array	
	output.copyFromImage(input);								// copy input into the output memory
	
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst
	
	
#ifdef DEBUG	
	output.show("output");
	waitKey(3);	// needed to update windows.
#endif
	
	framenum++;
	return 0;
}