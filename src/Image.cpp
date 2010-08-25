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
		loadFromImage( other );
	}
	
	
#pragma mark ASSIGNMENT
	
	//--------------------------------------------------
	void Image::loadFromImage( const Image &other )
	{
		other.cvImage.copyTo( cvImage );
		keypoints = other.keypoints;	
		other.descriptors.copyTo( descriptors );
		descriptorsCurrent = other.descriptorsCurrent;
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
		log(LOG_LEVEL_DEBUG, "in findDescriptors(), %d keypoints, %d x %d descriptors", 
			keypoints.size(), descriptors.rows, descriptors.cols);
		descriptorsCurrent=true;
	}
	
	//--------------------------------------------------
	void Image::convert(int code) 
	{
		Mat tmp;
		cvtColor(cvImage, tmp, code);
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
	// TO DO:  this only works with 4-channel images
	// This should be fixed at some point.
	void Image::drawIntoMe( Image &other, Point2f loc )
	{
		if(loc.x>width() || loc.y > height()) return;
		
		int roi_w = 0;
		int roi_h = 0;
		Point2f fgroipos = Point2f(0, 0);
		
		if(loc.x < 0)
		{
			roi_w = other.width() + loc.x;
			loc.x = 0;
			fgroipos.x = other.width() - roi_w;
		}
		else
		{
			roi_w = std::min(other.width(), (int) std::abs(width() - loc.x));
		}
		
		if(loc.y < 0)
		{
			roi_h = (int) std::abs(other.height() + loc.y);
			loc.y = 0;
			fgroipos.y = other.height() - roi_h;
		}
		else
		{
			roi_h = std::min(other.height(), (int) std::abs(height() - loc.y));
		}
		
		Mat fg = other.cvImage(Rect(fgroipos.x, fgroipos.x, roi_w, roi_h));
		Mat bg = cvImage(Rect(loc.x, loc.y, roi_w, roi_h));

		// This should be put into Image::drawIntoMe()
		for( int i = 0; i < roi_h; i++ )
		{
			uchar* ptr_bg = bg.ptr<uchar>(i);
			const uchar* ptr_fg = fg.ptr<uchar>(i);
			
			for( int j = 0; j < roi_w * fg.channels(); j += fg.channels() )
			{
				float alpha	= ptr_fg[j+3] / (float)numeric_limits<uchar>::max();
				float inv_alpha = 1.0-alpha;
				
				ptr_bg[j  ]	= saturate_cast<uchar>((ptr_bg[j  ]*inv_alpha) + (ptr_fg[j  ]*alpha));
				ptr_bg[j+1]	= saturate_cast<uchar>((ptr_bg[j+1]*inv_alpha) + (ptr_fg[j+1]*alpha));
				ptr_bg[j+2]	= saturate_cast<uchar>((ptr_bg[j+2]*inv_alpha) + (ptr_fg[j+2]*alpha));
			}
		}
	}
	
	
	//--------------------------------------------------
	void Image::show(const char* win_name)
	{
		imshow( win_name, cvImage );	
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
	
	//--------------------------------------------------
	int Image::channels()
	{
		return cvImage.channels();
	}
	
	int Image::width()
	{
		return cvImage.cols;
	}
	
	int Image::height() 
	{
		return cvImage.rows;
	}
}