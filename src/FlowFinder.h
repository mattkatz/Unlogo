/*
 *  FlowFinder.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"

class FlowFinder  {
public:
	
	FlowFinder();
	void findTrackingPoints( Image& frame );
	void updateTrackingPoints( Image& frame );
	void addTrackingPoint(float x, float y);
	void setMaxPoints(int max_pts) { MAX_COUNT = max_pts; };
	cv::Mat getHomography() {  return H12; }

	void drawIntoImage( Image& img ) {
		
	}
	
private:
	cv::Mat H12;
	
	vector<float> err;
	vector<uchar> status;
	cv::TermCriteria termcrit;	
	int MAX_COUNT;					// Maximum number of points to track
	cv::Size winSize;
	cv::Mat gray, prevGray;			// Input frames
	
	vector<cv::Point2f> prev_points;
	vector<cv::Point2f> curr_points;
	
	
	// Temporary holder for a new point to be added 
	cv::Point2f pt;
	bool addRemovePt;
};
