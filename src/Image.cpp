/*
 *  Image.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "Image.h"
namespace unlogo {
	//--------------------------------------------------
	Image::Image()
	{
		descriptorsAndKeypointsUpdated=false;
	}


	//--------------------------------------------------
	int Image::open( const char* path )
	{
		cvImage = imread( path );
		log(LOG_LEVEL_DEBUG, "in open() image has %d channels", cvImage.channels());
		if( cvImage.empty() )
		{
			log(LOG_LEVEL_ERROR, "in open() Can not read %s", path);
			return -1;
		}
		descriptorsAndKeypointsUpdated=false;
		return 0;
	}

	
	//--------------------------------------------------
	void Image::drawIntoMe(Image* child, Point2f pos)
	{
		drawIntoMe(child, pos.x, pos.y);
	}
	
	
	//--------------------------------------------------
	void Image::drawIntoMe(Image* child, int x, int y )
	{
		log(LOG_LEVEL_DEBUG, "in drawIntoMe() Mother %d channels, Child %d channels", cvImage.channels(), child->cvImage.channels());
		if(cvImage.type() != child->cvImage.type())
		{
			log(LOG_LEVEL_ERROR, "in drawIntoMe() Images must be of the same type. Mother=%d  Child=%d", cvImage.type(), child->cvImage.type());
			return;
		}
		if(cvImage.empty())
		{
			log(LOG_LEVEL_ERROR, "in drawIntoMe() Target image is empty");
			return;
		}
		// Make a Mat that points to the child-sized region of the image, starting at x,y
		Mat out_roi = cvImage(Rect(x, y, child->cvImage.cols, child->cvImage.rows));
		
		// Different operations depending on whether there is alpha involved
		if(child->cvImage.channels()<4)
		{
			child->cvImage.copyTo(out_roi);
		}
		else
		{
			// How can you determine the type?  it won't always be uchar
			alphaBlendRGBA<uchar>(out_roi, child->cvImage, out_roi);
		}
		
		descriptorsAndKeypointsUpdated=false;
	}

	
	//--------------------------------------------------
	void Image::warp( Mat* m )
	{
		descriptorsAndKeypointsUpdated=false;
	}


	//--------------------------------------------------
	void Image::convert( int conversion_code )
	{
		if(cvImage.empty())
		{
			log(LOG_LEVEL_ERROR, "in convert() Image is empty.");
			return;
		}

		Mat cvImageTemp;
		cvtColor(cvImage, cvImageTemp, conversion_code);
		cvImage = cvImageTemp;
	}


	//--------------------------------------------------
	void Image::operator << ( VideoCapture &cap )
	{
		cap >> cvImage;
		descriptorsAndKeypointsUpdated=false;
	}

	//--------------------------------------------------
	void Image::operator = ( Mat &other )
	{
		other.copyTo( cvImage );
		descriptorsAndKeypointsUpdated=false;
	}

	//--------------------------------------------------
	void Image::makeKeypointsAndDescriptors()
	{
		if(descriptorsAndKeypointsUpdated) return;
		
		Matcher* matcher = Matcher::Instance();
		if(cvImage.empty())
		{
			log(LOG_LEVEL_ERROR, "in makeKeypointsAndDescriptors(), image is empty");
		}
		
		Mat cvImageGray;
		cvtColor(cvImage, cvImageGray, CV_RGB2GRAY);
		
		matcher->detector->detect( cvImageGray, keypoints );
		//log(LOG_LEVEL_DEBUG, "in makeKeypointsAndDescriptors(), Found %d keypoints", keypoints.size());
		
		matcher->descriptorExtractor->compute( cvImageGray, keypoints, descriptors );
		//log(LOG_LEVEL_DEBUG, "in makeKeypointsAndDescriptors(), Descriptors: %d x %d", descriptors.rows, descriptors.cols);
		
		descriptorsAndKeypointsUpdated=true;
	}
	
}