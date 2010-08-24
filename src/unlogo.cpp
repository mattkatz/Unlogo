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
Image frame;


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
			l.pos = Point2f( -1, -1 );
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
	
	// This should be put into an Image::convert function
	Mat dst_tmp;
	cvtColor(dst_img, dst_tmp, CV_BGR2BGRA);
	dst_img = dst_tmp;

	frame.setFromMat( dst_img );

	// Loop through all loaded logos
	for(int i=0; i<(int)logos.size(); i++)
	{
		// Find all matches between the frame and the logo
		// the logo is A and the frame is B
		MatchSet* ms = new MatchSet(&logos[i].logo, &frame, 2);

		//ms->drawMatchesInB();
		
		// Get the middle point of all of the keypoint matches.
		// Then move the replacement towards that point.
		Point2f pos_actual = ms->avgB();
		if(logos[i].pos.x == -1 || logos[i].pos.y == -1)
		{
			logos[i].pos = pos_actual;
			logos[i].located = true;
		}
		else
		{
			Point2f	diff = pos_actual - logos[i].pos;
			Point2f move = Point2f(diff.x/10., diff.y/10.);
			logos[i].pos += move;
		}
		
		// Just for debugging.
		circle(dst_img, logos[i].pos, 20, CV_RGB(255,255,255), 5, CV_AA);
		
		// Adjust so that it is center-based
		Point2f p = logos[i].pos;
		p.x -= (logos[i].replacement.cvImage.cols / 2.);
		p.y -= (logos[i].replacement.cvImage.rows / 2.);
		
		// What are we going to replace it with?
		//replace.warp( ms->H12 );
		//imshow( "replace", replace.cvImage );

		
		// Finally, draw the replacement
		Mat fg = logos[i].replacement.cvImage;
		Mat bg = dst_img(Rect(p.x, p.y, fg.cols, fg.rows));
		
		// This should be put into Image:;drawIntoMe()

		for( int i = 0; i < fg.rows; i++ )
		{
			uchar* ptr_bg = bg.ptr<uchar>(i);
			uchar* ptr_fg = fg.ptr<uchar>(i);
			
			for( int j = 0; j < fg.step; j += fg.channels() )
			{
				float alpha	= ptr_fg[j+3] / (float)numeric_limits<uchar>::max();
				float inv_alpha = 1.0-alpha;
				
				ptr_bg[j  ]	= saturate_cast<uchar>((ptr_bg[j  ] * inv_alpha) + (ptr_fg[j  ] * alpha));
				ptr_bg[j+1]	= saturate_cast<uchar>((ptr_bg[j+1] * inv_alpha) + (ptr_fg[j+1] * alpha));
				ptr_bg[j+2]	= saturate_cast<uchar>((ptr_bg[j+2] * inv_alpha) + (ptr_fg[j+2] * alpha));
			}
		}
	}

	
	//imshow( "frame", frame.cvImage );
	imshow( "dst", dst_img );
	waitKey(1);
	
	framenum++;
	return 0;
}

