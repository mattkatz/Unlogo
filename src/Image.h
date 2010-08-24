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

	class MatchSet;
	
	class Image {
	public:
		
		Image();
		Image(const Image& mother);
		Image( Mat& matimg );
		Image(int width, int height, int nChannels, uint8_t* src, int stride);
		
		void	convert( int conversion_code );
		int		open( const char* filename );
		void	drawIntoMe(Image* child, Point2f pos);
		void	drawIntoMe( Image* child, int x, int y );
		void	warp( Mat& homography );
		void	makeKeypointsAndDescriptors();
		void	setFromMat( Mat &m );					// just points cvImage to new Mat
		
		// Operator Overloads
		//
		void operator << ( VideoCapture &cap );
		void operator = ( Mat &other );
		
		
	//protected:
		Mat cvImage;

		bool descriptorsAndKeypointsUpdated;
		vector<KeyPoint> keypoints;	
		Mat descriptors;
	};
}




#endif