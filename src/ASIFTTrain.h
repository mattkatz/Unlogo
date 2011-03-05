/*
 *  ASIFTTrain.h
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
#include <sys/stat.h> 
#include "JSONElement.h"

using namespace std;



class ASIFTTrain : public Image {
public:



	
	void init(float InitSigma=1.6, int BorderDist=5, int Scales=3, float PeakThresh=255.0*0.04/3.0, 
			   float EdgeThresh=0.06, float EdgeThresh1=0.08, int OriBins=36, float OriSigma=1.5,
			   float OriHistThresh=0.8, float MaxIndexVal=0.2, int MagFactor=3, float IndexSigma=1.0,
			   int IgnoreGradSign=0, float MatchRatio=0.73, float MatchXradius=1000000.0f,
			   float MatchYradius=1000000.0f);
	int calculateKeypoints(bool verbose=false);		// Calculates the keypoints.
	int findKeypoints();	// the preferred way to load keypoints.  it looks for saved data first
	
	void matchTo(Image& query);
	void drawKeypointsIntoImage(Image& img);
	void drawKeypointsIntoImage()			{ drawKeypointsIntoImage(*this); }
	bool open(string filename);
	int saveKeypoints(string filename);
	int loadKeypoints(string filename);
	

	
};