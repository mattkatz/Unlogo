/*
 *  OpticalFlow.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/25/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "OpticalFlow.h"

namespace unlogo {

	//--------------------------------------------------
	OpticalFlow::OpticalFlow(Image& _prev, Image& _next) 
	{
		prev = _prev;
		next = _next;
		
		if(prev.channels()>1 || next.channels()>1)
		{
			log(LOG_LEVEL_ERROR, "in OpticalFlow(), both images must be 1 channel.");
			return;
		}
		
		if(prev.size() != next.size())
		{
			log(LOG_LEVEL_ERROR, "in OpticalFlow(), both images must be the same size");
			return;
		}
		
		double pyrScale = 0.5;	// Specifies the image scale (<1) to build the pyramids for each image. pyrScale=0.5 means the classical pyramid, where each next layer is twice smaller than the previous
		int levels = 3;			// The number of pyramid layers, including the initial image. levels=1 means that no extra layers are created and only the original images are used
		int winsize = 15;		// The averaging window size; The larger values increase the algorithm robustness to image noise and give more chances for fast motion detection, but yield more blurred motion field
		int iterations = 3;		// The number of iterations the algorithm does at each pyramid level
		int polyN = 5;			// Size of the pixel neighborhood used to find polynomial expansion in each pixel. The larger values mean that the image will be approximated with smoother surfaces, yielding more robust algorithm and more blurred motion field. Typically, polyN =5 or 7
		int polySigma = 1.2;	// Standard deviation of the Gaussian that is used to smooth derivatives that are used as a basis for the polynomial expansion. For polyN=5 you can set polySigma=1.1 , for polyN=7 a good value would be polySigma=1.5
		calcOpticalFlowFarneback(prev.cvImage,next.cvImage,flow,pyrScale,levels,winsize,iterations,polyN,polySigma,0);
	}

	
	//--------------------------------------------------
	void OpticalFlow::draw(const char* win_name)
	{
		
		int step=16;
		for(int y = 0; y < next.height(); y += step)
			for(int x = 0; x < next.width(); x += step)
			{
				const Point2f& fxy = flow.at<Point2f>(y, x);
				line(next.cvImage, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), CV_RGB(0,255,255));
				circle(next.cvImage, Point(x,y), 2, CV_RGB(0,255,255), -1);
			}
		imshow(win_name, next.cvImage);
	}
	
	//--------------------------------------------------
	Point2f OpticalFlow::avg( Point2f thresh, int sampling )
	{
		return inRegion(Rect(0,0,flow.cols,flow.rows), thresh, sampling);
	}
	

	//--------------------------------------------------
	Point2f OpticalFlow::at( Point2f pos )
	{
		return flow.at<Point2f>(pos.x, pos.y);
	}
	
	
	//--------------------------------------------------
	Point2f OpticalFlow::inRegion( Rect region, Point2f thresh, int sampling )
	{
		region = region & Rect(0,0,flow.size().width,flow.size().height);
		
		int max_x = region.x+region.width;
		int max_y = region.y+region.height;
		
		if(region.x<0) region.x=0;
		if(region.y<0) region.y=0;
		if(max_x > flow.cols) region.width=flow.cols-region.x-1;
		if(max_y > flow.rows) region.height=flow.rows-region.y-1;
			
		Mat roi = flow( region );
		
		// TO DO:  use Mat::inrange and Mat::mean instead of stuff below
		
		Point2f total;
		int nPts=0;
		for(int y = 0; y < roi.size().height; y += sampling)
			for(int x = 0; x < roi.size().width; x += sampling)
			{
				const Point2f& fxy = roi.at<Point2f>(y,x);
				if(fxy.x > thresh.x || fxy.y > thresh.y)
				{
					total += fxy;
					nPts++;
				}
			}
		
		if(nPts>0)
		{
			return Point2f( total.x/nPts, total.y/nPts );
		}
		else
		{
			return Point2f(0,0);
		}
	}
}