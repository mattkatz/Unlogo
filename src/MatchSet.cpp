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

	MatchSet::MatchSet(Image* _a, Image* _b, int ransacReprojThreshold)
	{
		a = _a;
		b = _b;
		
		if(a->cvImage.empty() || b->cvImage.empty())
		{
			log(LOG_LEVEL_ERROR, "one of the images is empty");
			return;
		}

		a->makeKeypointsAndDescriptors();
		b->makeKeypointsAndDescriptors();
		log(LOG_LEVEL_DEBUG, "a (kpts %d, dscts %d x %d) b (kpts %d, dscts %d x %d)", 
			a->keypoints.size(), a->descriptors.rows, a->descriptors.cols, 
			b->keypoints.size(), b->descriptors.rows, b->descriptors.cols);
		
		
		// Load up the matcher with the "training" points from B
		Matcher* matcher = Matcher::Instance();
		matcher->descriptorMatcher->clear();
		matcher->descriptorMatcher->add( b->descriptors ); // Add training points
		matcher->descriptorMatcher->match( a->descriptors, matches );	// Find the best matches B for all points in A
																		// ie: matches.size() == a->keypoints.size()
		
		// How many unique matches do we have?
		vector<int> umatches( matches );
		sort(umatches.begin(), umatches.end());
        umatches.erase(unique(umatches.begin(), umatches.end()), umatches.end());
		float pct = umatches.size() / (float)matches.size();
		
		log(LOG_LEVEL_DEBUG, "in MatchSet(), %d unique matches in B for %f%% match", umatches.size(), pct); // matches.size will always equal A->keypoints.size
		
		
		
		//! converts vector of points to the vector of keypoints, where each keypoint is assigned the same size and the same orientation
		vector<Point2f> pointsA;
		KeyPoint::convert(a->keypoints, pointsA);
		
		//! converts vector of keypoints to vector of points
        vector<Point2f> pointsB;
		KeyPoint::convert(b->keypoints, pointsB, matches);
		H12 = findHomography( Mat(pointsA), Mat(pointsB), CV_RANSAC, ransacReprojThreshold );
		
		// Do a perspective transform on the points from Image A using the homography we calculated
		Mat pTransform;
		perspectiveTransform(Mat(pointsA), pTransform, H12);
		
		
		// Clear the mask from any previous calculations
		inlierMask.clear();
		inlierMask.resize( matches.size(), 0 );
		
		// If the corresponding point from Image B is less than 4 away (?), then it is an inlier
		vector<int>::const_iterator mit = matches.begin();
        for( size_t i = 0; i < pointsA.size(); i++ )
        {
            if( norm(pointsB[i] - pTransform.at<Point2f>(i,0)) < 4 ) // inlier
			{
				inlierMask[i] = 1;
			}
        }
	}
	
	void MatchSet::drawMatchesInA()
	{
		vector<Point2f> points;
		KeyPoint::convert(a->keypoints, points);
		
		for(int i=0; i<matches.size(); i++)
		{
			if(inlierMask[i]==0) {
				circle(a->cvImage, points[i], a->keypoints[i].size/2., CV_RGB(0,255,0), 1);
			} else {
				circle(a->cvImage, points[i], a->keypoints[i].size/2., CV_RGB(255,0,0), 1);
			}
		}
	}
	
	void MatchSet::drawMatchesInB()
	{
		vector<Point2f> points;
		KeyPoint::convert(b->keypoints, points);
		
		for(int i=0; i<matches.size(); i++)
		{
			int match = matches[i];
			if(inlierMask[match]==0) {
				circle(b->cvImage, points[match], b->keypoints[match].size/2., CV_RGB(0,255,0), 1);
			} else {
				circle(b->cvImage, points[match], b->keypoints[match].size/2., CV_RGB(255,0,0), 1);
			}
		}
		
	}
}