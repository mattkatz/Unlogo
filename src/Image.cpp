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
	
#pragma mark CONSTRUCTORS
	
	//--------------------------------------------------
	Image::Image()
	{
		descriptorsCurrent=false;
	}
	
	
	//--------------------------------------------------
	Image::Image(int width, int height, uint8_t* data, int channels)
	{
		loadFromData(width, height, data, channels);
		descriptorsCurrent=false;
	}
	
	
	//--------------------------------------------------
	Image::Image(const Image& other)
	{
		other.cvImage.copyTo(cvImage);
		descriptorsCurrent=false;
	}
	
	
#pragma mark ASSIGNMENT
	
	//--------------------------------------------------
	void Image::copyto( Image &other )
	{
		cvImage.copyTo( other.cvImage );
	}
	
	//--------------------------------------------------
	void Image::loadFromData(int width, int height, uint8_t* data, int stride)
	{
		cvImage = Mat(width, height, CV_8UC3, data, stride);
		descriptorsCurrent=false;
	}
	
	//--------------------------------------------------
	void Image::operator = ( const Image& other )
	{
		cvImage = other.cvImage;
		descriptorsCurrent=false;
	}
	

#pragma mark MATCHING
	
	//--------------------------------------------------
	void Image::findDescriptors()
	{
		if(descriptorsCurrent) return;
		
		Matcher* matcher = Matcher::Instance();
		if(empty())
		{
			log(LOG_LEVEL_ERROR, "in findDescriptors(), image is empty");
			return;
		}
		
		Mat cvImageGray;
		cvtColor(cvImage, cvImageGray, CV_RGB2GRAY);
		
		matcher->detector->detect( cvImageGray, keypoints );
		matcher->descriptorExtractor->compute( cvImageGray, keypoints, descriptors );
		//log(LOG_LEVEL_DEBUG, "in findDescriptors(), %d keypoints, %d x %ddescriptors", 
		//	keypoints.size(), descriptors.rows, descriptors.cols);
		descriptorsCurrent=true;
	}
	
	//--------------------------------------------------
	void Image::convert(int code) 
	{
		Mat tmp;
		cvtColor(cvImage, tmp, CV_BGR2BGRA);
		cvImage = tmp;	
	}
	
	//--------------------------------------------------
	int Image::open( const char* path )
	{
		cvImage = imread( path );
		if( cvImage.empty() ) {
			log(LOG_LEVEL_ERROR, "in open() Can not read %s", path);
			return -1;
		}
		descriptorsCurrent=false;
		return 0;
	}
	
	//--------------------------------------------------
	void Image::drawIntoMe( const Image &other, Point2f loc )
	{
		Mat fg = other.cvImage;
		Mat bg = cvImage(Rect(loc.x, loc.y, fg.cols, fg.rows));
		
		// This should be put into Image::drawIntoMe()
		for( int i = 0; i < fg.rows; i++ )
		{
			uchar* ptr_bg = bg.ptr<uchar>(i);
			const uchar* ptr_fg = fg.ptr<uchar>(i);
			
			for( int j = 0; j < fg.step; j += fg.channels() )
			{
				float alpha	= ptr_fg[j+3] / (float)numeric_limits<uchar>::max();
				float inv_alpha = 1.0-alpha;
				
				ptr_bg[j  ]	= saturate_cast<uchar>((ptr_bg[j  ]*inv_alpha) + (ptr_fg[j  ]*alpha));
				ptr_bg[j+1]	= saturate_cast<uchar>((ptr_bg[j+1]*inv_alpha) + (ptr_fg[j+1]*alpha));
				ptr_bg[j+2]	= saturate_cast<uchar>((ptr_bg[j+2]*inv_alpha) + (ptr_fg[j+2]*alpha));
			}
		}
	}
	
	
	
#pragma mark CV_IMAGE_ACCESSOR_METHODS
	
	//--------------------------------------------------
	bool Image::empty()
	{
		return cvImage.empty();
	}
	
	//--------------------------------------------------
	Size Image::size()
	{
		return cvImage.size();
	}
	
}