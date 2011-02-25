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

#include "utils.h"
#include "Train.h"
#include "Query.h"
#include "Window.h"

using namespace std;
using namespace cv;



void onThreshold(int pos)
{
	thresholdFilter->threshold = pos;
}

void onTrackingPoints(int pos)
{
	flow.MAX_COUNT = pos;
	findTrackingPoints=true;
	
}

int main(int argc, char *argv[])
{
	// Create a holder for the incoming frames
	unlogo::Query query;
	thresholdFilter = new unlogo::Threshold();
	query.addFilter( thresholdFilter );
	
	// Load in the image we are looking for
	unlogo::Train train("data/logo.png");
	train.init("SURF", "SURF", "BruteForce");
	if(!train.load("data/apple")) {
		train.analyze();
		train.save("data/apple");
		cout << "analyzing and saving" << endl;
	}
	train.show("train", unlogo::IMAGE_COLOR);
	
	// Open the video source
	// TO DO:  Add option to play video
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened()) {
		cout << "capture device 0 failed to open!" << endl;
		return 1;
	}


	// Create some windows to show the various stages
	unlogo::Window queryWin, filterWin, corrWin;
	
	queryWin.init("query", 0, 0);
	queryWin.setImage(&query, unlogo::IMAGE_COLOR);
	queryWin.setCallback(QueryWinOnMouse);
	queryWin.addTrackbar("trackingPoints", onTrackingPoints);
	
	filterWin.init("filtered", 640, 0);
	filterWin.setImage(&query, unlogo::IMAGE_FILTERED);
	filterWin.addTrackbar("threshold",  onThreshold);

	corrWin.init("correspondence", 500, 0);
	corrWin.setImage(&train, unlogo::IMAGE_CORRESPONDENCE);
	
	
	bool quit = false;
	int key;
	int frameno=0;
	while(!quit)
	{
		query.grabFrame( capture );
		if (query.empty())
			continue;



	
		
		flow.draw( query );
		queryWin.show();
		filterWin.show();
		corrWin.show();
		
		
		key = cvWaitKey(100);
		if(key != -1)
		{
			switch(key) {
				case 27:	// escape
					quit = true;
					break;
				default:
					cout << key << endl;
					break;
			}
		}
		
		frameno++;
	}

    return 0;
}
