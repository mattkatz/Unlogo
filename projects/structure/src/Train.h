/*
 *  Train.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"
#include "Query.h"
#include "utils.h"

namespace unlogo
{

	/*
	 The "train" image is usually created at the beginning of the app and compared
	 against a 'query' image every frame.
	*/
	class Train : public Image {
	public:

		Train() : bAnalyzed(false), bDoCrossCheckFilter(false), bInited(false) { }
		Train(string filename) : bAnalyzed(false), bDoCrossCheckFilter(false), bInited(false) {
			open(filename);
		}
		
		void analyze();
		void init(string _detector_alg="Sift", string _extractor_alg="Sift", string _matcher_alg="BruteForce");
		void match(Query* query, float ransacReprojThreshold=5.0);
		bool load(string base);
		void save(string base);
		
	private:
		
		bool bAnalyzed;
		bool bInited;
		bool bDoCrossCheckFilter;
		
		Mat train_descriptors, query_descriptors, H12;
		vector<Point2f> train_pts, query_pts;
		vector<KeyPoint> train_kpts, query_kpts;
		vector<DMatch> matches;
		vector<int> queryIdxs, trainIdxs;
		
		string detector_alg;		// Fast, Star, Sift, Surf, Mser, Gftt, Harris, GridAdapted, PyramidAdapted
		string extractor_alg;		// Sift, Surf, Brief, OpponentColor
		string matcher_alg;			// BruteForce, FlannBased
		
		cv::Ptr<cv::FeatureDetector> detector;		
		cv::Ptr<cv::DescriptorExtractor> extractor; 
		cv::Ptr<cv::DescriptorMatcher> matcher;
		
		Mat correspondenceImg;
	};
}