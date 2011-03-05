/*
 *  ASIFTMatcher.h
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
#include "demo_lib_sift.h"
#include "library.h"
#include "frot.h"
#include "fproj.h"
#include "compute_asift_keypoints.h"
#include "compute_asift_matches.h"
#include <sys/stat.h> 
#include "JSONElement.h"

using namespace std;

class ASIFTMatcher {
public:
	ASIFTMatcher() : bKeysGenerated(false), tilts(7), bInited(false) {
		default_sift_parameters(params);
		_h = cvMat(3, 3, CV_64F, h);
	}
	bool init(float InitSigma=1.6, int BorderDist=5, int Scales=3, float PeakThresh=255.0*0.04/3.0, 
			  float EdgeThresh=0.06, float EdgeThresh1=0.08, int OriBins=36, float OriSigma=1.5,
			  float OriHistThresh=0.8, float MaxIndexVal=0.2, int MagFactor=3, float IndexSigma=1.0,
			  int IgnoreGradSign=0, float MatchRatio=0.73, float MatchXradius=1000000.0f,
			  float MatchYradius=1000000.0f);
	bool setTrainImage(Image& train, string datapath);
	void doQuery(Image& query, bool showCorrespondence=false);

	bool saveTrainingData(string basepath);
	bool loadTrainingData(string basepath);
	
	void drawTrainKeypointsIntoImage(Image& img);

	
protected:
	bool bInited;
	bool bKeysGenerated;
	
	CvMat _h;
	double h[9];
	matchingslist matchings;
	int num_matchings;
	Image train;
	vector< vector< keypointslist > > train_keys;
	int tilts;
	siftPar params;
	int numKeypoints;	
};