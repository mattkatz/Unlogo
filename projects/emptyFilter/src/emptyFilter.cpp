
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "Image.h"

using namespace fh;
Image input, output;
string msg;

extern "C" int init( const char* argstr )
{
#ifdef FHDEBUG
	cvNamedWindow("input");		cvMoveWindow("input", 0, 0);
	cvNamedWindow("output");	cvMoveWindow("output", 0, 510);
#endif
	
	msg = argstr;
	return 0;
}

extern "C" int uninit()
{
	return 0;
}


extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
	input.setData( width, height, src[0], src_stride[0]);
	
#ifdef FHDEBUG
	input.show("input");
#endif
	
	output.setData( width, height, dst[0], dst_stride[0] );
	output.copyFromImage(input);								// copy input into the output memory
	output.text(msg.c_str(), 10, height-10, .5);					// A very simple modification to the frame
	
#ifdef FHDEBUG
	output.show( "output" );
	waitKey(1);													// required to update highgi windows
#endif
	
	
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst
	return 0;
}
