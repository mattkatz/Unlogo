/*
 *  ChamerMatcher.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/5/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"
using namespace cv;

class ChamerMatcher {
public:
	ChamerMatcher() : bTrained(false) {};
	
	bool train(Image& _trainImg);
	void doQuery(Image& _queryImg, string windowName="");

protected:

	// Other stuff
	int best;
	bool bTrained;
	Mat trainImg, queryImg;
	vector<vector<Point> > results;
	vector<float> costs;
	
};