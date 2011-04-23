/*
 *  FaceFinder.h
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/23/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Face.h"

using namespace std;
using namespace cv;

typedef vector<Face> FaceSet;

class FaceTracker {
public:
	
	int init()
	{
		if(!faceFinder.load("/usr/local/share/opencv/haarcascades/haarcascade_frontalface_default.xml"))
		{
			cout << "Could not load face classifier." << endl;
			return 1;
		}
		if(!nestedFinder.load("/usr/local/share/opencv/haarcascades/haarcascade_eye.xml"))
		{
			cout << "Could not load nested classifier." << endl;
			return 1;
		}
		return 0;
	}
	
	void search(Mat frame)
	{
		faces.clear();
		
		vector<cv::Rect> matches;
		faceFinder.detectMultiScale(frame, matches, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(30, 30));

		for(int i=0; i<matches.size(); i++)
		{
			Face f;
			f.boundingBox = matches[i];
	
			Mat face_roi = frame(f.boundingBox);
			nestedFinder.detectMultiScale(face_roi, f.eyes, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(30, 30));
		
			if(f.eyes.size() > 0)
			{
				faces.push_back( f );
			}
		}
		//history.push_back( faces );
	}
	
		
	FaceSet faces;
	
protected:
	
	//vector<FaceSet> history;
	CascadeClassifier faceFinder, nestedFinder;
};