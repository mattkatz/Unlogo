/*
 *  utils.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include <opencv2/opencv.hpp>
using namespace cv;

namespace unlogo
{
	//Takes a descriptor and turns it into an xy point
	void keypoints2points(const vector<KeyPoint>& in, vector<Point2f>& out);

	//Takes an xy point and appends that to a keypoint structure
	void points2keypoints(const vector<Point2f>& in, vector<KeyPoint>& out);

	// filters keypoints
	void crossCheckMatching( cv::Ptr<DescriptorMatcher>& descriptorMatcher,
							const Mat& descriptors1, const Mat& descriptors2,
							vector<DMatch>& filteredMatches12, int knn );
}