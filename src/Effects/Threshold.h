/*
 *  Threshold.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/8/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include "Effect.h"


class Threshold : public Effect {
public:
	
	Threshold(int amt) {
		thresholdType = CV_THRESH_BINARY;
		threshold = amt;
		maxValue = 255;
	}
	
	void setAmount(int amt) {
		threshold = amt;
	}
	
	void apply(Image* image);

private:
	double threshold;
	double maxValue;
	int thresholdType;
};

