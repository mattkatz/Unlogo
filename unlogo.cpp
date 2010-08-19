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
#include "opencv2/core/core.hpp"
#include <boost/algorithm/string.hpp>

#include "LogoFilter.h"

using namespace cv;
using namespace std;
using namespace boost;


LogoFilter* g_filter = 0;

extern "C" int init( const char* argstr )
{
	try {
		// Parse arguments.
		vector<string> args;
		split(args, argstr, is_any_of(":"));
		
		if(args.size()<5 || args.size()%2!=1)
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

		g_filter = new LogoFilter( );
		int init = g_filter->init( args[0], args[1], args[2] );
		if(init != 0)
		{
			return -1;
		}
		
		// Add all of the search/replace pairs
		for(size_t i=3; i<args.size(); i+=2)
		{
			g_filter->addLogo(args[i], args[i+1]);
		}
		return 0;
	}
	catch ( ... ) {
		return -1;
	}
}

extern "C" int uninit()
{
	delete g_filter;
	g_filter = 0;
    
	return 0;
}

extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
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
	
	Mat dst_img(width, height, CV_8UC3, dst[0], dst_stride[0]);
	Mat rgb_image; cvtColor(dst_img, rgb_image, CV_YCrCb2RGB);
	return g_filter->filter( rgb_image, dst_img );
}