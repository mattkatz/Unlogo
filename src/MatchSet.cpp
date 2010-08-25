/*
 *  MatchSet.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "MatchSet.h"

namespace unlogo {

	//--------------------------------------------------
	MatchSet::MatchSet(Image* _a, Image* _b, int ransacReprojThreshold)
	{
		a = _a;
		b = _b;
		
		if(a->empty() || b->empty())
		{
			log(LOG_LEVEL_ERROR, "one of the images is empty");
			return;
		}

		a->findDescriptors();
		b->findDescriptors();
		
		log(LOG_LEVEL_DEBUG, "a (kpts %d, dscrptrs %d x %d) b (kpts %d, dscrptrs %d x %d)", 
			a->keypoints.size(), a->descriptors.rows, a->descriptors.cols, 
			b->keypoints.size(), b->descriptors.rows, b->descriptors.cols);
		
		if(a->descriptors.empty() || b->descriptors.empty())
		{
			log(LOG_LEVEL_ERROR, "one of the images has no descriptors");
			return;
		}
		
		// Load up the matcher with the "training" points from B
		Matcher* matcher = Matcher::Instance();
		matcher->descriptorMatcher->clear();
		matcher->descriptorMatcher->add( b->descriptors ); // Add training points
		matcher->descriptorMatcher->match( a->descriptors, matches );	// Find the best matches B for all points in A
																		// ie: matches.size() == a->keypoints.size()

		//! converts vector of points to the vector of keypoints, where each keypoint is assigned the same size and the same orientation
		vector<Point2f> pointsA;
		KeyPoint::convert(a->keypoints, pointsA);
		
		//! converts vector of keypoints to vector of points
        vector<Point2f> pointsB;
		KeyPoint::convert(b->keypoints, pointsB, matches);
		H12 = cv::findHomography( Mat(pointsA), Mat(pointsB), CV_RANSAC, ransacReprojThreshold );
		
		// Do a perspective transform on the points from Image A using the homography we calculated
		Mat pTransform;
		perspectiveTransform(Mat(pointsA), pTransform, H12);
		
		
		// Clear the mask from any previous calculations
		inlierMask.clear();
		inlierMask.resize( matches.size(), 0 );
		
		// If the corresponding point from Image B is less than 4 away (?), then it is an inlier
		vector<int>::const_iterator mit = matches.begin();
        for(int i = 0; i < (int)pointsA.size(); i++ )
        {
            if( norm(pointsB[i] - pTransform.at<Point2f>(i,0)) < 4 ) // inlier
			{
				inlierMask[i] = 1;
			}
        }
	}
	
	
	//--------------------------------------------------
	float MatchSet::pctMatch()
	{
		// How many unique matches in B do we have?
		vector<int> umatches( matches );
		sort(umatches.begin(), umatches.end());
        umatches.erase(unique(umatches.begin(), umatches.end()), umatches.end());
		float pct = umatches.size() / (float)matches.size();
		
		log(LOG_LEVEL_DEBUG, "in MatchSet(), %d unique matches in B for %0.2f%% match", umatches.size(), pct*100); // matches.size will always equal A->keypoints.size
		return pct;
	}
	
	//--------------------------------------------------
	void MatchSet::drawKeypointsInA()
	{
		vector<Point2f> points;
		KeyPoint::convert(a->keypoints, points);
		
		for(int i=0; i<(int)matches.size(); i++)
		{
			Scalar c = (inlierMask[i]==0) ? CV_RGB(255,0,0) : CV_RGB(0,255,0);
			MatchSet::drawKeypoint( b->cvImage, b->keypoints[i], c, DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
		}
	}
	
	//--------------------------------------------------
	void MatchSet::drawMatchesInB()
	{
		vector<Point2f> points;
		KeyPoint::convert(b->keypoints, points);
		log(LOG_LEVEL_DEBUG, "Drawing %d keypoints", points.size());
		for(int i=0; i<(int)matches.size(); i++)
		{
			int match = matches[i];
			Scalar c = (inlierMask[match]==0) ? CV_RGB(255,0,0) : CV_RGB(0,255,0);
			MatchSet::drawKeypoint( b->cvImage, b->keypoints[match], c, DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
		}
	}
	
	
	//--------------------------------------------------
	void MatchSet::drawKeypoint( Mat& img, const KeyPoint& p, const Scalar& color, int flags )
	{
		Point center( cvRound(p.pt.x), cvRound(p.pt.y) );
		
		if( flags & DrawMatchesFlags::DRAW_RICH_KEYPOINTS )
		{
			int radius = cvRound(p.size/2); // KeyPoint::size is a diameter
			
			// draw the circles around keypoints with the keypoints size
			circle( img, center, radius, color, 1, CV_AA );
			
			// draw orientation of the keypoint, if it is applicable
			if( p.angle != -1 )
			{
				float srcAngleRad = p.angle*(float)CV_PI/180.f;
				Point orient(cvRound(cos(srcAngleRad)*radius), 
							 cvRound(sin(srcAngleRad)*radius));
				line( img, center, center+orient, color, 1, CV_AA );
			}
		}
		else
		{
			// draw center with R=3
			int radius = 3;
			circle( img, center, radius, color, 1, CV_AA );
		}
	}
	
	//--------------------------------------------------
	Point2f MatchSet::avgA(bool includeOutliers)
	{
		Point2f avg;
		vector<Point2f> points;
		KeyPoint::convert(a->keypoints, points);
		
		for(int i=0; i<(int)points.size(); i++)
		{
			if(inlierMask[i]==1 || includeOutliers) avg += points[i];
		}
		return Point2f( avg.x / points.size(), avg.y / points.size() );
	}
	
	
	//--------------------------------------------------
	Point2f MatchSet::avgB()
	{		
		Point2f avg;
		vector<Point2f> points;
		KeyPoint::convert(b->keypoints, points);
		
		for(int i=0; i<(int)matches.size(); i++)
		{
			avg += points[matches[i]];
		}
		return Point2f( avg.x / matches.size(), avg.y / matches.size() );
	}
}