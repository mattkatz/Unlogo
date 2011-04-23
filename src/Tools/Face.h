/*
 *  Face.h
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/23/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include "opencv2/core/core.hpp"

using namespace cv;

class Face {
public:
	
	void draw(Mat img)
	{
		Point p1(boundingBox.x, boundingBox.y);
		Point p2(boundingBox.x+boundingBox.width, boundingBox.y+boundingBox.height);
		rectangle(img, p1, p2, Scalar(255,0,0), 1);

		Mat face_roi = img(boundingBox);
		for(int j=0; j<eyes.size(); j++)
		{
			Point p1(eyes[j].x, eyes[j].y);
			Point p2(eyes[j].x+eyes[j].width, eyes[j].y+eyes[j].height);
			rectangle(face_roi, p1, p2, Scalar(0,255,0), 1);
		}
	}
	
	Rect boundingBox;
	vector<Rect> eyes;
};