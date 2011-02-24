/*
 *  Image.cpp
 *  framehack
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "Image.h"

namespace fh {
	
#pragma mark CONSTRUCTORS
	
	//--------------------------------------------------
	Image::Image()
	{
	
	}
	
	//--------------------------------------------------
	Image::Image(int width, int height, uint8_t* data, int stride)
	{
		setData(width, height, data, stride);
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
			Size s = cvImage.size();
			log(LOG_LEVEL_ERROR, "in copyFromImage(), WARNING: size are different. Mat may be reallocated.");
			log(LOG_LEVEL_ERROR, "in copyFromImage(), this.size=%dx%d other.size=%dx%d", 
				s.width, s.height, other.cvImage.size().width, other.cvImage.size().height);
		}
		
		other.cvImage.copyTo( cvImage );
	}
	
	//--------------------------------------------------
	void Image::setData(int width, int height, uint8_t* data, int stride)
	{
		cvImage = Mat(Size(width, height), CV_8UC3, data, stride);
		//cvImage = Mat(width, height, CV_8UC3, data, stride);
	}
	
	//--------------------------------------------------
	void Image::operator = ( const Image& other )
	{
		cvImage = other.cvImage;
	}

	
#pragma mark UTILITIES
	
	//--------------------------------------------------
	int Image::open( const char* path )
	{
		cvImage = imread( path );
		if( cvImage.empty() ) {
			log(LOG_LEVEL_ERROR, "in open() Can not read %s", path);
			return -1;
		}
		return 0;
	}
	
	//--------------------------------------------------
	void Image::show(const char* win_name)
	{
		imshow( win_name, cvImage );	
	}
	



#pragma mark EFFECTS_AND_MANIPULATION

	//--------------------------------------------------
	void Image::equalizeHist() 
	{
		cv::equalizeHist(cvImage, cvImage);
	}
	
	
	//--------------------------------------------------
	// TO DO:  this only works with 4-channel images
	// This should be fixed at some point.
	void Image::drawIntoMe( Image &other, Point2f loc )
	{
		if(loc.x > width() || loc.y > height()) {
			cout << "ERROR" << endl;
			return;
		}
		
		// Get intersection of 2 images
		Rect intersection = Rect(loc, other.size()) & Rect(Point(0,0), size());
		
		// Get the sub-section of each image that can be written to.
		Mat fg = other.cvImage(Rect(abs(intersection.x - loc.x), abs(intersection.y - loc.y), intersection.width, intersection.height));
		Mat bg = cvImage(intersection);
		
		
		// This should be put into Image::drawIntoMe()
		for( int i = 0; i < intersection.height; i++ )
		{
			uchar* ptr_bg = bg.ptr<uchar>(i);
			const uchar* ptr_fg = fg.ptr<uchar>(i);
			
			for( int j = 0; j < intersection.width * fg.channels(); j += fg.channels() )
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
	void Image::pixelize(int pixel_size) {
		
		Mat tmp;
		cvImage.copyTo(tmp);
		
		for(int y=0; y<cvImage.size().height; y+=pixel_size)
		{
			for(int x=0; x<cvImage.size().width; x+=pixel_size)
			{
				int i = (y * cvImage.size().width + x) * cvImage.channels();
				float r = cvImage.data[ i + 0 ];
				float g = cvImage.data[ i + 1 ];
				float b = cvImage.data[ i + 2 ];
				rectangle(tmp, Point(x,y), Point(x+pixel_size, y+pixel_size), CV_RGB(r,g,b), CV_FILLED);
			}
		}
		
		tmp.copyTo(cvImage);
	}
	
	//--------------------------------------------------
	void Image::text(const char* text, int x, int y, double scale, Scalar color )
	{
		int thickness=1;
		int lineType=8;
		putText(cvImage, text, Point(x,y), FONT_HERSHEY_SIMPLEX, scale, color, thickness, lineType);
	}
	
	//--------------------------------------------------
	Mat Image::bw()
	{
		if(cvImage.channels()==1) return cvImage;
		else {
			Mat bwImage;
			cvtColor(cvImage, bwImage, CV_RGB2GRAY);
			return bwImage;
		}
	}
	
	//--------------------------------------------------
	void Image::convert(int code) 
	{
		Mat tmp;
		cvtColor(cvImage, tmp, code);
		cvImage = tmp;	
	}

	
#pragma mark CV_IMAGE_ACCESSOR_METHODS
	
	//--------------------------------------------------
	bool Image::empty()
	{
		return cvImage.empty();
	}
	
	//--------------------------------------------------
	vector<Mat> Image::pyramid(int maxLevel) 
	{
		vector<Mat> imagePyr;
		buildPyramid(cvImage, imagePyr, maxLevel);
		return imagePyr;
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
