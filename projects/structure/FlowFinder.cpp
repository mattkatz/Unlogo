/*
 *  FlowFinder.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "FlowFinder.h"

namespace unlogo
{
	
	FlowFinder::FlowFinder()
	{
		winSize = cv::Size(10,10);
		MAX_COUNT = 500;
		termcrit = cv::TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
		addRemovePt = false;
	}
	
	void FlowFinder::findTrackingPoints( Query* frame )
	{				
		// automatic initialization
		frame->dirty.copyTo(gray);
		goodFeaturesToTrack( gray, curr_points, MAX_COUNT, 0.01, 10, cv::Mat(), 3, 0, 0.04);
		cornerSubPix(gray, curr_points, winSize, cv::Size(-1,-1), termcrit);
		addRemovePt = false;
	}
	
	void FlowFinder::updateTrackingPoints( Query* frame )
	{
		frame->dirty.copyTo(gray);
		
		if( !prev_points.empty() )
		{
			vector<float> err;
			if(prevGray.empty())
				gray.copyTo(prevGray);
			
			calcOpticalFlowPyrLK(prevGray, gray, prev_points, curr_points, status, err, winSize,
								 3, termcrit, 0);
			
			//H12 = cv::findHomography(cv::Mat(prev_points), cv::Mat(curr_points), status, cv::RANSAC, 4);
			
			size_t i, k;
			for( i = 0; i < curr_points.size(); i++ )
			{
				if( !status[i] )
					continue;
				
				circle( frame->clean, curr_points[i], 3, cv::Scalar(0,255,0), -1, 8);
				line( frame->clean, curr_points[i], prev_points[i], cv::Scalar(0,0,255), 1);
			}
			
			
			for( i = k = 0; i < curr_points.size(); i++ )
			{
				if( addRemovePt && norm(pt - curr_points[i]) <= 5)
				{
					addRemovePt = false;
					continue;
				}
				
				if( !status[i] )
					continue;
				
				curr_points[k++] = curr_points[i];
				//circle( frame->color,curr_points[i], 3, cv::Scalar(0,255,0), -1, 8);
				//line( frame->color, curr_points[i], prev_points[i], cv::Scalar(0,0,255), 1);
			}

			curr_points.resize(k);
		}
		
		if( addRemovePt && curr_points.size() < (size_t)MAX_COUNT )
		{
			vector<cv::Point2f> tmp;
			tmp.push_back(pt);
			cornerSubPix( gray, tmp, winSize, cvSize(-1,-1), termcrit);
			curr_points.push_back(tmp[0]);
			addRemovePt = false;
		}
		
		std::swap(curr_points, prev_points);
		cv::swap(gray, prevGray);
	}
	
	void FlowFinder::addTrackingPoint(float x, float y) 
	{
		pt = cv::Point2f(x, y);
		addRemovePt = true;
	}
}