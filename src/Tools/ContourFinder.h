/*
 *  ContourFinder.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */
// WARNING: IN PROGRESS

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"

struct Contour {
	vector<cv::Point> pts;
	cv::Rect boundingBox;
	cv::Point center;
	float area;
};

class ContourFinder {
public:
	
	void findContours( Image& img, bool bUseAproxPolyDP=true ) {
		if(img.empty()) {
			cerr << "image passed to findContours is empty" << endl;
			return;
		}
		
		if(img.channels() > 1) {
			cerr << "image passed to findContours has more than 1 channel." << endl;
			return;
		}
		
		// findContours destroys the image.  So let's make a copy
		cv::Mat tmpImg = img.cvImg.clone();
		
		hierarchy.clear();
		contours.clear();
		cvContours.clear();
		
		
	    // Find all of the contours
		cv::findContours( tmpImg, cvContours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		
		cvContours.resize(cvContours.size());	// not sure why this is here... it's from the sample code.
		
		for( size_t k = 0; k < cvContours.size(); k++ ) {
			
			Contour c;
			c.pts = cvContours[k];
			
			if(bUseAproxPolyDP) {
				approxPolyDP(cv::Mat(c.pts), c.pts, 3, true);	
			}
			
			c.area = cv::contourArea( cv::Mat(c.pts) );
			
			contours.push_back( c );
		}
	}
	
	void drawIntoImage(Image& img)
	{
		for(int i=0; i<contours.size(); i++)
		{
			for(int j=0; j<contours[i].pts.size(); j++)
			{
				cv::line(img.cvImg, contours[i].pts[j], contours[i].pts[j+1], Scalar(255,0,0), 1);
			}
		}	
	}
	
protected:
	vector<Contour> contours;
	vector<vector<cv::Point> > cvContours;
	vector<cv::Vec4i> hierarchy;
};