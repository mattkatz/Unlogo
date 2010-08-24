/*
 *  unlogo.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/18/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "Image.h"
#include "Matcher.h"
#include "MatchSet.h"
#include "MatchTracker.h"
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
			cout << "where [search:replace] is a list of images to look for and either image " << endl;
			cout << "or hex color to replace it with.  Hex colors must begin with 0x and be 6 digits" << endl;
			cout << endl;
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
			l.tracker = MatchTracker();
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


Image frame;
extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
	log(LOG_LEVEL_DEBUG, "=== Frame %d ===", framenum);
	
	// Copy src pixels to dest
	for ( int i(0); i<3; ++i )
	{
		uint8_t* pdst = dst[i];
		uint8_t* psrc = src[i];
		int w = width;
		int h = height;
		if ( i > 0 )
			w >>= 1, h >>= 1;
		
		if ( dst_stride[i] == src_stride[i] )
			memcpy( pdst, psrc, src_stride[i] * h );
		else 
		{
			for ( int y(0); y<h; ++y )
			{
				memcpy( pdst, psrc, w );
				pdst += dst_stride[i], psrc += src_stride[i];
			}
		}
	}
	
	// Make a cv:Mat with the data.
	Mat dst_img(height, width, CV_8UC3, dst[0], dst_stride[0]);  // why are w/h reversed here?
	if(dst_img.empty()) return 1;
	
	frame.setFromMat( dst_img );

	// Loop through all loaded logos
	for(int i=0; i<(int)logos.size(); i++)
	{
		// Find all matches between the frame and the logo
		// the logo is A and the frame is B
		MatchSet* ms = new MatchSet(&logos[i].logo, &frame, 2);

		ms->drawMatchesInB();
		
		// Keep track of the history of the matches so we can ease/average
		//logos[i].tracker.track( ms );
		
		// What are we going to replace it with?
		//Image replace = logos[i].replacement;
	
		//replace.warp( ms->H12 );
		//imshow( logos[i].name, replace.cvImage );
		
		//circle(frame.cvImage, ms->avgB(), 20, CV_RGB(255, 255, 255), 5);
		
		// Finally, draw the replacement
		//Point2f loc = logos[i].tracker.avg();
		//frame.drawIntoMe( &replace, ms->avgB() );	
	}

	waitKey(1);
	imshow( "dst", dst_img );

	framenum++;
	return 0;
}