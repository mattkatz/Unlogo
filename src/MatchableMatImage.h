/*
 *  MatchableMatImage.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATCHABLE_MAT_IMAGE_H
#define MATCHABLE_MAT_IMAGE_H

#include "MatImage.h"

class MatchableMatImage : public MatImage {
public:
	
protected:
	vector<KeyPoint> keypoints;	
	Mat descriptors;
	vector<Point2f> points;
};

#endif