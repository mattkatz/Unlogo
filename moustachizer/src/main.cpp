/*
 *  main.cpp
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/23/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "moustachizer.h"

extern "C" 
{
	Moustachizer* plugin;
	
	int init( const char* argstr )
	{
		try {
			plugin = new Moustachizer();
			return plugin->init(argstr);
		}
		catch ( ... ) {
			return -1;
		}
	}
	
	int process(uint8_t* dst[4], int dst_stride[4], uint8_t* src[4], int src_stride[4], int width, int height)
	{
		// To do:
		// Do we need to calculate #channels using stride & width?
		
		Mat input(Size(width, height), CV_8UC3, src[0], src_stride[0]);
		
		uint8_t* data = src[0];	// save a reference to the data so we can make sure it is the same array

		plugin->process(input); //src, src_stride, width, height
		
		if(src[0] != data) {
			cout << "ERROR! Data has been reallocated.  This will be problematic" << endl;
		}
		
		for(int i=0; i<4; i++) {
			memcpy(dst[i], src[i], sizeof(uint8_t) * src_stride[i] * height);
		}
		
        int c = cvWaitKey(1);
        if( (char)c == 27 )
		{
            exit(0);
		}	
		return 0;
	}
	
	int uninit()
	{
		delete plugin;
		return 0;
	}
}
