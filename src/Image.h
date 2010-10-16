/*
 *  Image.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef FHIMAGE_H
#define FHIMAGE_H

#include <iostream>
#include <highgui.h>
#include <cvaux.h>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "fhUtils.h"


namespace fh {

	using namespace cv;
	using namespace std;
	
	class Image {
	public:
		
		Image();
		Image(Mat& _cvImage);
		Image( int width, int height, uint8_t* data, int channels );
		//Image( const Image& other ); // The copy constructor is for creating a new object. It copies a existing object to a newly constructed object. 
		void operator = ( const Image &other ); // The assignment operator is to deal with an already existing object. 
		Image operator()(const Rect roi);
		
		void setData(int width, int height, uint8_t* data, int stride);
		void copyFromImage( const Image &other );
		int open( const char* path );
		void convert( int code );
		void equalizeHist();
		
		
		void drawIntoMe( Image &other, Point2f loc );
		void pixelize(int pixelSize);
		void show(const char* win_name);
		Point2f opticalFlowAvg( const Image& prev, Point2f thresh );
		Point2f opticalFlowAt( const Image& prev, Point2f pos );
		void opticalFlow( const Image& prev, Mat& flow );
		void text( const char* text, int x, int y, double scale=1, Scalar color=CV_RGB(255,255,255) );

		vector<Mat> pyramid(int maxLevel);
		Mat bw();
		
		void save(const char* filename);
		
		// cvImage accessor convenience methods
		bool empty();
		Size size();
		int channels();
		int width();
		int height();
		
		
		Mat cvImage;

	};
}




#endif