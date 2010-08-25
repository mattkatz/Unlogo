/*
 *  MatchSet.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATCH_SET_H
#define MATCH_SET_H

#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "ulUtils.h"
#include "Image.h"


namespace unlogo {
	
	class Image;
	
	class MatchSet {
	public:
		MatchSet(Image* a, Image* b, int ransacReprojThreshold);
		void drawKeypointsInA();
		void drawMatchesInB();
		Point2f avgA(bool includeOutliers=false);
		Point2f avgB();
		float pctMatch();
		
		
		static void drawKeypoint( Mat& img, const KeyPoint& p, const Scalar& color, int flags );
		
		
		
	//protected:
		
		Mat H12;
		Image* a;
		Image* b;
		vector<int> matches;		// contains the best match in B for all keypoints in A
									// ie: a->keypoints.size() == matches.size()
		vector<char> inlierMask;	// ditto with inlierMask -- 1 for inlier, 0 for not
		
	};
}

#endif