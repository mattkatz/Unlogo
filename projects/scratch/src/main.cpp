//
//  main.m
//  scratch
//
//  Created by Jeffrey Crouse on 2/1/11.
//  Copyright 2011 Eyebeam. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ofxOpenCV22.h"

using namespace std;
using namespace cv;

#define MODE_CONTOURS 1
#define MODE_CHAMER 2

int main(int argc, char *argv[])
{
	ofxCvMatImage source;
	ofxCvMatImage graySource;
	ofxCvContourFinder contourFinder;
    CvCapture* cap = cvCaptureFromCAM(0);;

    if( !cap )   {
        printf("can not open camera or video file\n");
        return -1;
    }
    
    source = cvQueryFrame(cap);
    if(source.empty()) {
        printf("can not read data from the video source\n");
        return -1;
    }
	
	cvNamedWindow("color");
	cvNamedWindow("gray");
	cvNamedWindow("threshold");
	
	while(cvWaitKey(1000/60) != 27)
	{
		source = cvQueryFrame(cap);
		
		graySource = source.clone();
		graySource.setChannels(1);
		
		cvShowImage("gray", &((IplImage)graySource));
		
		graySource.threshold(50);
		
		cvShowImage("threshold", &((IplImage)graySource));
		
		
		
		contourFinder.findContours( graySource );
		contourFinder.draw(source);
		
		
		
		source.flip(true, true);

		cvShowImage("color", &((IplImage)source));
	}

    cvReleaseCapture(&cap);
	
    return 0;
}
