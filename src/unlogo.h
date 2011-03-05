/*
 *  unlogo.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/3/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include "Image.h"
#include "FlowFinder.h"
#include "Threshold.h"
#include "ASIFTMatcher.h"
#include "GenericMatcher.h"


using namespace std;
using namespace cv;

class unlogo {
public:
	unlogo() {};
	~unlogo();
	int init(const char* argstr);
	void process(Mat frame);
	
	
private:
	
	/*
	In a CV application, the query is usually a single 
	 frame of a video.  It is defined in contrast to the "train" image, which is 
	 usually analyzed once at the beginning of the app and then compared against the query image.
	 */
	Image query;
	Image gray;
	
	/*
	 The "train" image is usually created at the beginning of the app and compared
	 against a 'query' image every frame.
	 */
	Image train;
	
	
	ASIFTMatcher asift;
	GenericMatcher gmatcher;
	
	FlowFinder flow;
	Threshold* threshold;
	bool findTrackingPoints;
};