/*
 *  ofxCvMatImage.h
 *  scratch
 *
 *  Created by Jeffrey Crouse on 1/3/11.
 *
 */

#pragma once

#include <opencv2/opencv.hpp>

class ofxCvMatImage {
	
	public:

	/*
	 I am trying to stick with the way that OpenCV2.2 uses operators/constructors as much as possible.
	 Note that this is quite different than they are used in the original OF wrapper.
	 */
	
	ofxCvMatImage(){	cout << "0\n";	}
	
	// CONSTRUCTOR COPY OPERATORS (data is copied)
	//ofxCvMatImage( const IplImage* iplimg )		: img(iplimg)	{cout << "1\n";}
	ofxCvMatImage( const cv::Mat& mom )			: img(mom)		{cout << "2\n";}
	ofxCvMatImage( const ofxCvMatImage& mom )	: img(mom.img)	{cout << "3\n";}

	// ASSIGNMENT OPERATORS (no data is copied, new header is made)
	//void operator = ( const IplImage* mom );
	void operator = ( const cv::Mat& mom );
	void operator = ( const ofxCvMatImage& mom );
	
	// CAST OPERATORS (no data is copied)
    //operator IplImage*();
	operator cv::Mat();
	operator cv::Mat*();
	
	// () OPERATOR  
	// NOTE:  Don't confuse this operator with the constructor.
	// This is confusing, so I am hesitant to put it in here, but it is a very
	// useful feature of the cv::Mat, so I will.
	ofxCvMatImage& operator()(cv::Rect r);					// no data is copied
	

	void copyFrom( ofxCvMatImage& other, int numChannels );	
	ofxCvMatImage subimg( cv::Rect r ) {	return (ofxCvMatImage)img(r); 	}
	void setFromPixels(unsigned char* pix, int width, int height, int channels);
	void setChannels( int nChannels );
	void flip(bool v, bool h);
	
	
	
	float height()	{ return img.size().height;	}
	float width()	{ return img.size().width;	}
	void threshold(int amt, bool inv=false) {	cv::threshold( img, img, amt, 255, (inv) ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);	}
	void blur()		{ cv::blur( img, img, cv::Size(3,3) );	}
	bool empty()	{ return img.empty();		}
	int channels()	{ return img.channels(); }
	IplImage* ipl_image();
	cv::Mat mat_image();
	
protected:
	int getConversionCode(int fromChannels, int toChannels);
	cv::Mat img;

};