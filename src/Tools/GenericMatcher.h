/*
 *  GenericMatcher.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/4/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"
#include "utils.h"


class GenericMatcher {
public:
	
	GenericMatcher() : bTrainAnalyzed(false), bInited(false) {}
	
	bool init(string _detector_alg="SIFT", string _extractor_alg="SIFT", string _matcher_alg="BruteForce", float _ransacReprojThreshold=5.0);
	bool train(Image& train, string datapath);
	void doQuery(Image& query, bool showCorresponence=false);
	
	bool saveTrainingData(string basepath);
	bool loadTrainingData(string basepath);
	
	void drawTrainKeypointsIntoImage(Image& img);
	
private:
	
	Image trainImg;
	
	float ransacReprojThreshold;
	
	bool bTrainAnalyzed;
	bool bInited;
	bool bDoCrossCheckFilter;
	
	Mat train_descriptors, H12;
	vector<Point2f> train_pts;
	vector<KeyPoint> train_kpts;
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