/*
 *  ofxCvMatImage.cpp
 *  scratch
 *
 *  Created by Jeffrey Crouse on 1/3/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "ofxCvMatImage.h"

//--------------------------------------------------------------------------------
void ofxCvMatImage::operator = ( const cv::Mat& mom ) {
    unsigned char* oldData = data;	// keep a copy of the data pointer
	
	cv::Mat::operator=(mom);
	
	if( data != oldData  ) {
		cerr <<  "ofxCvMatImage::operator = ( const cv::Mat& _mom ) Mat has been reallocated.  This usually means you are doing some transformation on an image that it isn't allocated to deal with, and it could lead to unexpected results." << endl;
	}
}


//--------------------------------------------------------------------------------
// When calling the equals operator, use the 
void ofxCvMatImage::operator = ( const IplImage* mom ) {
	unsigned char* oldData = data;	// keep a copy of the data pointer
	
    cv::Mat::operator=(mom);
	
	if( data != oldData  ) {
		cerr <<  "ofxCvMatImage::::operator = ( const IplImage* _mom ) Mat has been reallocated.  This usually means you are doing some transformation on an image that it isn't allocated to deal with, and it could lead to unexpected results." << endl;
	}
}


// -------------------------------------------------------------
void ofxCvMatImage::setFromPixels(unsigned char* pix, int width, int height, int channels) {
	unsigned char* oldData = data;	// keep a copy of the data pointer
	
	int type;
	switch(channels) {
		case 1:	type = CV_8UC1;	break;
		case 3:	type = CV_8UC3;	break;
		case 4:	type = CV_8UC4;	break;
		default: cerr << "channels not supported" << endl; return;
	}
	create(height, width, type);
	if(isContinuous()) {
		memcpy(data, pix, total()*channels);
	} else {
		for(int i=0; i<size().height; i++) {
			unsigned char* row = ptr<unsigned char>(i);
			memcpy(row, pix + (i*step), step);
		}
	}
	
	if( data != oldData  ) {
		cerr <<  "ofxCvMatImage::setFromPixels() Mat has been reallocated.  This usually means you are doing some transformation on an image that it isn't allocated to deal with, and it could lead to unexpected results." << endl;
	}
}


// -------------------------------------------------------------
int ofxCvMatImage::getCode(int fromChannels, int toChannels) {
	int code = -1;
	switch(fromChannels) {
		case 1: switch(toChannels) {
			case 3: code = CV_GRAY2RGB; break;
			case 4: code = CV_GRAY2RGBA; break;
		}
			break;
		case 3: switch(toChannels) {
			case 1: code = CV_RGB2GRAY; break;
			case 4: code = CV_RGB2RGBA; break;
		}
			break;
		case 4: switch(toChannels) {
			case 1: code = CV_RGBA2GRAY; break;
			case 3: code = CV_RGBA2RGB; break;
		}
			break;
	}
	return code;
}


/*
 CvtColor(src,dst,code)
 src		The source 8-bit (8u), 16-bit (16u) or single-precision floating-point (32f) image 
 dst		The destination image of the same data type as the source. The number of channels may be different
 code		Color conversion operation that can be specifed using CV_ src-color-space 2 dst-color-space constants
 
 The function converts the input image from one color space to another. 
 */
// -------------------------------------------------------------
void ofxCvMatImage::setChannels( int nChannels ) {
	uchar* ptr = data;	// keep a copy of the data pointer
	
	int code = getCode(channels(), nChannels);
	
	if(code == -1) {
		cerr << "conversion is not supported." << endl;
		return;
	}
	Mat tmp;
	cv::cvtColor( *this, tmp, code  );
	tmp.copyTo( *this );
	
	if( data != ptr ) {
		cerr << "ofxCvMatImage::setChannels() Mat has been reallocated." << endl;
	}
}


// -------------------------------------------------------------
void ofxCvMatImage::flip(bool v, bool h){ 
	int flipMode = 0;
	
	if( v && !h ) flipMode = 0;
	else if( !v && h ) flipMode = 1;
	else if( v && h ) flipMode = -1;
	else {
		cerr << "that flip code doesn't exist" << endl;
		return;
	}

	cv::flip( *this, *this, flipMode);	
}
