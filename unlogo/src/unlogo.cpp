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
	for(int i=0; i<trainingSets.size(); i++)
	{
		delete trainingSets[i];
	}	
}


// ------------------------------
int unlogo::init(const char* argstr)
{	
	findTrackingPoints=true;

	vector<string> archives;
	split( archives, argstr, boost::algorithm::is_any_of(":,") );
	for(int i=0; i<archives.size(); i++)
	{
		TrainingSet* set = new TrainingSet();
		set->loadFromArchive(archives[i]);
		
		trainingSets.push_back( set );
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
	
	// TO DO: pre-process image here (equalize historgrams, soebel, threshold, etc?) 
	
	if(findTrackingPoints)
	{
		flow.findTrackingPoints( gray );
		findTrackingPoints=false;
	}
	else
	{
		flow.updateTrackingPoints( gray );
	}

	Mat H12 = flow.getHomography();

	
	// Use each of the training sets to find logos in the frame
	for(int i=0; i<trainingSets.size(); i++)
	{
		trainingSets[i]->match(gray);
		
		// TO DO:  If we find a match, we need to draw something into 'query'
	}
	
	
	query.text("unlogo", 10, query.height()-20);
	
	imshow("preview", frame);
	cvWaitKey(1);
}

