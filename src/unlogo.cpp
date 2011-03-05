/*
 *  unlogo.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/3/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "unlogo.h"

// ------------------------------
unlogo::~unlogo()
{

}


// ------------------------------
int unlogo::init(const char* argstr)
{	

	threshold	= new Threshold(50);

	findTrackingPoints=true;

	train.open(argstr, true);
	
	if(asift.init()){
		asift.setTrainImage(train, argstr);
	} else {
		return 1;
	}

	if(gmatcher.init()) {
		gmatcher.setTrainImage(train, argstr);
	} else {
		return 1;
	}

	flow.setMaxPoints(500);
	return 0;
}

// ------------------------------
void unlogo::process(Mat frame)
{	
	query.useMat( frame );
	assert(query.channels()==3);
	
	gray = query.getGrayscale();
	assert(gray.channels()==1);
	
	// Equalize histograms here
	
	if(findTrackingPoints)
	{
		cout << "finding new tracking points" << endl;
		flow.findTrackingPoints( gray );
		findTrackingPoints=false;
	}
	flow.updateTrackingPoints( gray );
	
	
	//Mat H12 = flow.getHomography();
	
	

	gmatcher.doQuery( gray, true );
	asift.doQuery( gray, true );

	

	imshow("preview", frame);
	
	cvWaitKey(1);
}

