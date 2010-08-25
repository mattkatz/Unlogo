/*
 *  unlogo.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse and Kashif Balil
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

#include "Image.h"
#include "Matcher.h"
#include "MatchSet.h"
#include "OpticalFlow.h"
#include "Logo.h"


#define MATCHING_DELAY 10
#define MATCHING_PCT_THRESHOLD 0.1
#define GHOST_FRAMES_ALLOWED 50
#define RANSAC_PROJECTION_THRESH 2

using namespace unlogo;
vector<Logo> logos;
vector<Logo*> detected_logos;
Image input, output, prev;
int framenum=0;


extern "C" int init( const char* argstr )
{
	try {
		/* print a welcome message, and the OpenCV version */
		printf ("Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
				CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
		
		// Parse arguments.
		vector<string> argv = split(argstr, ":");
		int argc = argv.size();
		
		if(argc<5 || argc%2!=1)
		{
			cout << "Usage" << endl;
			cout << "[detector]:[descriptor]:[matcher]:[search:replace]..." << endl;
			cout << "where [search:replace] is a list of images to look for and an image to replace it with. " << endl;
			cout << "Detector Types: FAST, STAR, SIFT, SURF, MSER, GFTT, HARRIS" << endl;
			cout << "Descriptor Types: SIFT, SURF" << endl;
			cout << "Matcher Types: BruteForce, BruteForce-L1" << endl;
			return -1;
		}

		// Construct the matcher singleton the way we want it.
		// Otherwise, the instance constructor will do the default.
		Matcher::Instance(argv[0], argv[1], argv[2]);
	
		// Load in all of the logos from the arguments
		for(int i=3; i<argc; i+=2)
		{
			Logo l;
			l.name = argv[i].c_str();
			l.logo.open( l.name );
			l.replacement.open( argv[i+1].c_str() );
			l.ghostFrames=0;
			l.pos = Point2f(-1,-1);
			logos.push_back( l );
			log(LOG_LEVEL_DEBUG, "Loaded logo %s", l.name);
		}
		
		
		cvNamedWindow("input");
		cvMoveWindow("input", 0, 0);
		
		cvNamedWindow("output");
		cvMoveWindow("output", 0, 510);
		
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
	log(LOG_LEVEL_DEBUG, "=== Frame %d ===", framenum);

	input.loadFromData( height, width, src[0], src_stride[0]); // Why are width and height reversed?
	output.loadFromData( height, width, dst[0], dst_stride[0]);
	input.convert(CV_BGR2BGRA);
	output = Image(input);										// copy input into the output memory
	
	
	if(input.empty() || output.empty()) return 1;
	
	
	// Doing matching is expensive. So we only do it every X frames
	// The rest of the time we just calculate the Optical Flow and 
	// move any matched logos accordingly
	bool doMatching = (framenum>0 && framenum%MATCHING_DELAY==0) || detected_logos.size()==0;
	if( doMatching )
	{
		detected_logos.clear();
		
		// Make a MatchSet for each frame/logo pair
		for(int i=0; i<(int)logos.size(); i++)
		{
			MatchSet ms = MatchSet(&logos[i].logo, &output, RANSAC_PROJECTION_THRESH);
	
			// If thet are a match, reset the ghost frames counter
			// Otherwise, increase the ghost frames counter
			if(ms.pctMatch() > MATCHING_PCT_THRESHOLD)
			{
				logos[i].ghostFrames=0;
				logos[i].homography = ms.H12.clone();
				lerp(logos[i].pos, ms.avgB(), 4.f);
				detected_logos.push_back( &logos[i] );
			}
			else if(logos[i].ghostFrames<GHOST_FRAMES_ALLOWED)
			{
				logos[i].ghostFrames += MATCHING_DELAY;
				detected_logos.push_back( &logos[i] );
			}
			else
			{
				// Don't put it into the detected_logos set
			}
		}
		
		// At this point, detected_logos is updated
		
	}
	else // do optical flow
	{
		// Do optical flow. Then just move detected_logos according to that.
		Image next( input );
		next.convert( CV_BGRA2GRAY );
		prev.convert( CV_BGRA2GRAY );
		
		OpticalFlow flow = OpticalFlow(prev, next);
		
		Point2f offset = flow.avg( Point2f(6,6), 16 );
		for(int i=0; i<(int)detected_logos.size(); i++)
		{
			log(LOG_LEVEL_DEBUG, "offset = (%f,%f)", offset.x, offset.y);
			detected_logos[i]->pos += offset;
		}
	}
	
	
	// Now draw detected_logos into input
	for(int i=0; i<(int)detected_logos.size(); i++)
	{
		// Make it center-based.
		Point2f draw_loc = detected_logos[i]->pos;
		Size s = detected_logos[i]->replacement.size();
		draw_loc.x -= (s.width / 2.);
		draw_loc.y -= (s.height / 2.);
		
		// TO DO: Use logos[i].homography to warp the image...
		
		output.drawIntoMe( detected_logos[i]->replacement, draw_loc );
	}
	
	
	// Debugging windows
	input.show("input");
	output.show("output");
	//if(framenum>0) prev.show("prev");
	
	// Keep a copy of this frame for optical flow detection
	prev = Image( input );
	
	waitKey(1);
	framenum++;
	return 0;
}

