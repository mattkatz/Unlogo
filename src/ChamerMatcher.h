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
	ChamerMatcher() : bUseSobel(true){};
	
	bool init(bool useSoebelEdgeDetection=true);
	bool setTrainImage(Image& _train, string datapath);
	void doQuery(Image& _query, bool showCorrespondence=false);
	
protected:
	
	int best;
	bool bUseSobel;
	Mat train, query;
	vector<vector<Point> > results;
	vector<float> costs;
	
};