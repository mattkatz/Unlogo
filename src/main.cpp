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


// Imitating the stuff that FFMPEG gives us.
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
	
	init(argv[2]);  // from unlogo.cpp
	
	width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	for(;;)
    {
		cv::Mat frame;
        cap >> frame; // get a new frame from camera
	
		src[0] = frame.data;
		src_stride[0] = width * 3;
		
		process(dst, dst_stride, src, src_stride, width, height);  // from unlogo.cpp
	}
	
	uninit();  // from unlogo.cpp

}


/*
#include "Image.h"
#include "Matcher.h"
#include "MatchSet.h"
#include "MatchTracker.h"
#include "Logo.h"

using namespace cv;
using namespace std;
using namespace unlogo;

int main (int argc, char * const argv[])
{
	const char* window_name = "output";
	
	// Open the video
	VideoCapture cap(argv[1]);
    if(!cap.isOpened())  
	{
		cout << "Can not open video source" << endl;
        return -1;
	}

	// Construct the matcher singleton the way we want it.
	// Otherwise, the instance constructor will do the default.
	Matcher::Instance("SURF", "SURF", "BruteForce");

	// Load in all of the logos from the arguments
	vector<Logo> logos;
	for(int i=2; i<argc; i+=2)
	{
		Logo l;
		l.name = argv[i];
		l.logo.open( argv[i] );
		l.replacement.open( argv[i+1] );
		l.tracker = MatchTracker();
		logos.push_back( l );
	}
	
	
	Image frame;
	for(int fn=0;1;fn++)
    {
		cout << "=== Frame " << fn << " ===" <<  endl;
		
		frame << cap;
		
		// escape or end of video exit program
		if(frame.cvImage.empty() || waitKey(10) == '\x1b') break;  
		
		for(int i=0; i<logos.size(); i++)
		{
			// Find all matches between the frame and the logo
			// the logo is A and the frame is B
			MatchSet* ms = new MatchSet(&logos[i].logo, &frame, 2);

			ms->drawMatchesInB();
			
			// Keep track of the history of the matches so we can ease/average
			logos[i].tracker.track( ms );
			
			// What are we going to replace it with?
			Image replace = logos[i].replacement;
			cout << "cols: " << ms->H12.cols << " rows: " << ms->H12.rows << endl;
			replace.warp( ms->H12 );
			imshow( logos[i].name, replace.cvImage );
			
			circle(frame.cvImage, ms->avgB(), 20, CV_RGB(255, 255, 255), 5);
			
			// Finally, draw the replacement
			//Point2f loc = logos[i].tracker.avg();
			frame.drawIntoMe( &replace, ms->avgB() );
			
			//imshow( logos[i].name, logos[i].logo.cvImage );
		}
		
		imshow( window_name, frame.cvImage );
		
	}
	
	cout << "Exiting ..." << endl;
	return 0;
}

*/