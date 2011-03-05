/*
 *  ofxCvFeatureFinder.h
 *  scratch
 *
 *  Created by Jeffrey Crouse on 1/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once

#include "ofxCvMatImage.h"

class ofxCvFeatureFinder {
public:
	
	bool bInited;
	cv::Ptr<cv::FeatureDetector> detector;
	vector<cv::KeyPoint> keypoints;
	
	ofxCvFeatureFinder() {
		bInited = false;
	}
	
	void init(string detectorType) {
		detector = cv::FeatureDetector::create( detectorType );
		bInited=true;
	}
	
	void findFeatures(ofxCvMatImage& img) {
		if(!bInited) {
			cerr << "ofxCvFeatureFinder has not been inited!" << endl;
			return;
		}
		detector->detect( img, keypoints );
	}
	
#ifdef USING_OF	
	void draw(float _x, float _y) {
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(_x, _y);
		ofNoFill();

		float x, y, x2, y2, angle, scale;
		for(int i=0; i<keypoints.size(); i++) {

				x = keypoints[i].pt.x;
				y = keypoints[i].pt.y;
				scale = keypoints[i].size;
				angle = keypoints[i].angle;
				x2 = (cos(angle)*scale)+x;
				y2 = (sin(angle)*scale)+y;
				
				ofSetColor(0, 150, 150);
				ofCircle(x, y, scale);
				
				ofSetColor(210, 100, 0);
				ofLine(x, y, x2, y2);
		}
		ofPopMatrix();
		ofPopStyle();
	}
#endif	
};