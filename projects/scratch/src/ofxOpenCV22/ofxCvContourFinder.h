/*
 *  ofxCvContourFinder.h
 *  scratch
 *
 *  Created by Jeffrey Crouse on 1/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once

#include "ofxCvMatImage.h"


struct ofxCvContour {
	vector<cv::Point> pts;
	cv::Rect boundingBox;
	cv::Point center;
	float area;
};

class ofxCvContourFinder {
public:
	
	vector<ofxCvContour> contours;
	vector<vector<cv::Point> > cvContours;
	vector<cv::Vec4i> hierarchy;
	
	
	void findContours( ofxCvMatImage& img, bool bUseAproxPolyDP=true ) {
		if(img.empty()) {
			cerr << "image passed to findContours is empty" << endl;
			return;
		}

		if(img.channels() > 1) {
			cerr << "image passed to findContours has more than 1 channel." << endl;
			return;
		}
		
		// findContours destroys the image.  So let's make a copy
		cv::Mat tmpImg = img.clone();

		cvContours.clear();
		hierarchy.clear();
		contours.clear();
		
	    // Find all of the contours

	
		cv::findContours( tmpImg, cvContours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		
		cvContours.resize(cvContours.size());	// not sure why this is here... it's from the sample code.
		
		for( size_t k = 0; k < cvContours.size(); k++ ) {
			
			ofxCvContour c;
			c.pts = cvContours[k];
			
			if(bUseAproxPolyDP) {
				approxPolyDP(cv::Mat(c.pts), c.pts, 3, true);	
			}
			
			c.area = cv::contourArea( cv::Mat(c.pts) );
		
			contours.push_back( c );
		}
	}
	
#ifdef USING_OF
	void draw(float x, float y) {
		ofPushMatrix();
		ofPushStyle();
		ofTranslate(x, y);
		ofNoFill();
		ofSetColor(150, 50, 77);
		for(int i=0; i<contours.size(); i++) {
			ofBeginShape();
			for(int j=0; j<contours[i].pts.size(); j++) {
				ofVertex(contours[i].pts[j].x, contours[i].pts[j].y);
			}
			ofEndShape(true);
		}
		ofPopStyle();
		ofPopMatrix();
	}
#else
	void draw(ofxCvMatImage img, int _levels=1) {
	    drawContours( img, cvContours, _levels <= 0 ? 3 : -1, cv::Scalar(128,255,255),
					 3, CV_AA, hierarchy, std::abs(_levels) );	
	}
#endif	
	
};