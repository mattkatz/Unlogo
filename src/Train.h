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
#include "utils.h"



class Train : public Image {
public:
	
	void analyze();
	void init(string _detector_alg="Sift", string _extractor_alg="Sift", string _matcher_alg="BruteForce");
	void match(Image& query, float ransacReprojThreshold=5.0);
	bool loadMetaData(string base);
	void saveMetaData(string base);
	

};
