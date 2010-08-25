/*
 *  Image.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATIMG_H
#define MATIMG_H

#include <iostream>
#include <highgui.h>
#include <cvaux.h>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "ulUtils.h"
#include "Matcher.h"
#include "MatchSet.h"

namespace unlogo {

	using namespace cv;
	using namespace std;
	
	class Image {
	public:
		
		Image();
		Image( int width, int height, uint8_t* data, int channels );
		Image( const Image& other ); // The copy constructor is for creating a new object. It copies a existing object to a newly constructed object. 
		void operator = ( const Image &other ); // The assignment operator is to deal with an already existing object. 
		
		void loadFromData(int width, int height, uint8_t* data, int channels);
		int open( const char* path );
		void copyto( Image &other );
		void convert( int code );
		void findDescriptors();
		void drawIntoMe( const Image &other, Point2f loc );
		
		// cvImage accessor convenience methods
		bool empty();
		Size size();
		
	//protected:
		Mat cvImage;
		vector<KeyPoint> keypoints;	
		Mat descriptors;
		bool descriptorsCurrent;
	};
}




#endif