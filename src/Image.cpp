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
	Image::Image(int width, int height, uint8_t* data, int stride)
	{
		setData(width, height, data, stride);
		descriptorsCurrent=false;
	}
	
	//--------------------------------------------------
	Image::Image(const Image& other)
	{
		copyFromImage( other );
	}
	
	
#pragma mark ASSIGNMENT

	//--------------------------------------------------
	void Image::copyFromImage( const Image &other )
	{
		if(cvImage.channels()!=other.cvImage.channels() )
		{
			log(LOG_LEVEL_ERROR, "in copyFromImage(), WARNING: channels are different. Mat being reallocated.");
			log(LOG_LEVEL_ERROR, "in copyFromImage(), this.channels=%d other.channels=%d", cvImage.channels(), other.cvImage.channels());
		}
		
		if(cvImage.size()!=other.cvImage.size())
		{
			log(LOG_LEVEL_ERROR, "in copyFromImage(), WARNING: size are different. Mat may be reallocated.");
			log(LOG_LEVEL_ERROR, "in copyFromImage(), this.size=%dx%d other.size=%dx%d", 
				cvImage.size().width, cvImage.size().height, other.cvImage.size().width, other.cvImage.size().height);
		}
		
		other.cvImage.copyTo( cvImage );
		keypoints = other.keypoints;	
		other.descriptors.copyTo( descriptors );
		descriptorsCurrent = other.descriptorsCurrent;
	}
	
	//--------------------------------------------------
	void Image::setData(int width, int height, uint8_t* data, int stride)
	{
		
		cvImage = Mat(Size(width, height), CV_8UC3, data, stride);
		//cvImage = Mat(width, height, CV_8UC3, data, stride);
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
		
		// Get intersection of 2 images
		Rect roi = Rect(loc, other.size()) & Rect(Point(0,0), size()); 

		Point2f fgroipos = Point2f(0, 0);
		
		if(loc.x < 0)
		{
			fgroipos.x = other.width() - roi.width;
		}
		
		if(loc.y < 0)
		{
			fgroipos.y = other.height() - roi.height;
		}
		
		// Get the sub-section of each image that can be written to.
		Mat fg = other.cvImage(Rect(fgroipos.x, fgroipos.y, roi.width, roi.height));
		Mat bg = cvImage(Rect(max(0, (int)loc.x), max(0, (int)loc.y), roi.width, roi.height));
		
		
		// This should be put into Image::drawIntoMe()
		for( int i = 0; i < roi.height; i++ )
		{
			uchar* ptr_bg = bg.ptr<uchar>(i);
			const uchar* ptr_fg = fg.ptr<uchar>(i);
			
			for( int j = 0; j < roi.width * fg.channels(); j += fg.channels() )
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
	void Image::text(const char* text, int x, int y, double scale, Scalar color )
	{
		int thickness=1;
		int lineType=8;
		putText(cvImage, text, Point(x,y), FONT_HERSHEY_SIMPLEX, scale, color, thickness, lineType);
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
		return cvImage.size().width;
	}
	
	int Image::height() 
	{
		return cvImage.size().height;
	}
}