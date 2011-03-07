/*
 *  Threshold.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/8/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "Threshold.h"


void Threshold::apply(Image* image)
{
	cv::threshold(image->cvImg, image->cvImg, threshold, maxValue, thresholdType);
}

