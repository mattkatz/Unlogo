/*
 *  ofxCvHaarFinder.h
 *  scratch
 *
 *  Created by Jeffrey Crouse on 1/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once

#include "ofxCvMatImage.h"

struct ofxCvHaarFinderMatch {
	cv::Rect boundingBox;
	vector<cv::Rect> submatches;
};

class ofxCvHaarFinder {
public:
	
	cv::CascadeClassifier cc1, cc2;
	vector<ofxCvHaarFinderMatch> matches;

	
	void loadCascade(string s) {
		if( !cc1.load( s ) ) {
			cout << "Could not load classifier cascade: " << s << endl;
		}
	}
	
	void loadSubCascade(string s) {
		if( !cc2.load( s )  ) {
			cout << "Could not load classifier cascade for nested objects: " << s << endl;
		}
	}
#ifdef USING_OF	
	void draw(float x, float y) {
		ofPushMatrix();
		ofNoFill();
		ofTranslate(x, y);
		ofPushStyle();
		for(int i=0; i<matches.size(); i++) {
			ofSetColor(200, 200, 100);
			ofPushMatrix();
			ofTranslate(matches[i].boundingBox.x, matches[i].boundingBox.y);
			ofRect(0, 0, matches[i].boundingBox.width, matches[i].boundingBox.height);
			
			ofSetColor(100, 80, 200);
			for(int j=0; j<matches[i].submatches.size(); j++) {
				ofRectangle sm = matches[i].submatches[j];
				ofRect(sm.x, sm.y, sm.width, sm.height);
			}
			ofPopMatrix();
		}
		ofPopStyle();
		ofPopMatrix();
	}
#endif
	
	
	void search(ofxCvMatImage& img, float scaleFactor, float minw, float minh) {
		matches.clear();
		/*
		 void CascadeClassifier::detectMultiScale( const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size());
		 image				Matrix of type CV_8U containing the image in which to detect objects.
		 objects			Vector of rectangles such that each rectangle contains the detected object.
		 scaleFactor		Specifies how much the image size is reduced at each image scale.
		 minNeighbors		Speficifes how many neighbors should each candiate rectangle have to retain it.
		 flags				This parameter is not used for new cascade and have the same meaning for old cascade as in function cvHaarDetectObjects.
		 minSize			The minimum possible object size. Objects smaller than that are ignored.
		 */
		
		vector<cv::Rect> superMatches;
		cc1.detectMultiScale( img, superMatches, scaleFactor, 2, CV_HAAR_SCALE_IMAGE, cv::Size(minw, minh) );
		
		for(int i=0; i<superMatches.size(); i++)
		{
			ofxCvHaarFinderMatch m;
			m.boundingBox.x = superMatches[i].x;
			m.boundingBox.y = superMatches[i].y;
			m.boundingBox.width = superMatches[i].width;
			m.boundingBox.height = superMatches[i].height;
			

			vector<cv::Rect> subMatches;
			cc2.detectMultiScale( img(superMatches[i]), subMatches, scaleFactor, 2, CV_HAAR_SCALE_IMAGE, cv::Size(minw, minh) );

			for(int j=0;j<subMatches.size(); j++)
			{
				m.submatches.push_back( subMatches[i] );
			}

			matches.push_back( m );
		}
	}
	
};