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
#include <boost/algorithm/string.hpp>  // Take this out eventually

#include "Image.h"
#include "Matcher.h"
#include "MatchSet.h"
#include "Logo.h"


using namespace cv;
using namespace std;
using namespace boost;
using namespace unlogo;


vector<Logo> logos;
int framenum=0;


extern "C" int init( const char* argstr )
{
	try {
		// Parse arguments.
		vector<string> argv;
		split(argv, argstr, is_any_of(":"));
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
			l.located = false;
			l.pos = Point2f(-1,-1);
			logos.push_back( l );
			log(LOG_LEVEL_DEBUG, "Loaded logo %s", l.name);
		}
		
		namedWindow("dst", CV_WINDOW_AUTOSIZE);
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




void lerp(Point2f& current, const Point2f& desired, float ease)
{
	if(current.x == -1 || current.y == -1) {
		current = desired;
	} else {
		Point2f	diff = desired - current;
		current += Point2f(diff.x/ease, diff.y/ease);
	}
}

extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
	log(LOG_LEVEL_DEBUG, "=== Frame %d ===", framenum);

	// Point some Image objects at the data being passed in
	Image input( height, width, src[0], src_stride[0]); // Why are width and height reversed?
	Image output( height, width, dst[0], dst_stride[0]);
	input.convert(CV_BGR2BGRA);
	if(input.empty() || output.empty()) return 1;

	
	// Loop through all loaded logos
	// TO DO:  All matches should be kept until the end and then drawn after we have looked for all logos.
	for(int i=0; i<(int)logos.size(); i++)
	{
		// Find all matches between the logo (A) and the frame (B)
		MatchSet ms = MatchSet(&logos[i].logo, &input, 2);
		
		//ms.drawMatchesInB();  // for debugging
		
		if(ms.pctMatch() > 0.2)
		{
			// Get the middle point of all of the keypoint matches.
			// Then move the replacement towards that point.
			lerp(logos[i].pos,  ms.avgB(), 10.f);
			
			Point2f draw_loc = logos[i].pos;
			Size s = logos[i].replacement.size();
			draw_loc.x -= (s.width / 2.);
			draw_loc.y -= (s.height / 2.);
		
			// use ms.H12 to warp the replacement image...
			
			input.drawIntoMe( logos[i].replacement, draw_loc );
		}
		
	}
	
	input.copyto( output );
	imshow( "input", input.cvImage );
	imshow( "output", output.cvImage );

	waitKey(1);
	framenum++;
	return 0;
}

