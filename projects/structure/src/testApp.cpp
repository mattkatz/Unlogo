/*
 *  testApp.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "testApp.h"

// ---------------------------------------
void testApp::setup()
{
	setVideoSource(0);
	
	threshold	= new unlogo::Threshold(50);
	mirror		= new unlogo::Mirror(false, true);
	grayscale	= new unlogo::Grayscale();
	
	findTrackingPoints=true;
	
	makeWindow("query");
	addTrackbar("query", "tracking points", 500, 500);
	
	makeWindow( "threshold", 650, 0);
	addTrackbar("threshold", "threshold", 50, 255);
	
	makeWindow("correspondence", 0, 500);
	
	// Load in the image we are looking for
	train = new unlogo::Train("data/logo.png");
	train->init("SURF", "SURF", "BruteForce");
	if(!train->load("data/apple")) {
		train->analyze();
		train->save("data/apple");
		cout << "analyzing and saving" << endl;
	}
	
	flow.setMaxPoints(500);
}


// ---------------------------------------
void testApp::process(unlogo::Query* query)
{
	query->apply( mirror );
	query->apply( grayscale );
	
	if(findTrackingPoints)
	{
		cout << "finding new tracking points" << endl;
		flow.findTrackingPoints( query );
		findTrackingPoints=false;
	}
	
	flow.updateTrackingPoints( query );

	Mat H12 = flow.getHomography();
	
	show("query", query, true);
		
	query->apply( threshold );
	
	
	show("threshold", query, false);

	
	// Match the query image to logo
	//train->match(query);
	
	
	show("correspondence", train);
}


// ---------------------------------------
void testApp::key(int key)
{
	switch(key) {
		case 27:	// escape
			quit();
			break;
		default:
			cout << key << endl;
			break;
	}
}


// ---------------------------------------
void testApp::click(string window, int x, int y)
{
	if(window.compare("query")==0)
	{
		flow.addTrackingPoint(x, y);
	}
}


// ---------------------------------------
void testApp::trackbar(string tb, int val)
{
	if(tb.compare("tracking points")==0)
	{
		cout << "setting #tp to " << val << endl;
		flow.setMaxPoints(val);
		findTrackingPoints=true;
	}
	
	if(tb.compare("threshold")==0)
	{
		threshold->setAmount( val );
	}
}

// ---------------------------------------
void testApp::mouseMove(string window, int x, int y)
{
	
}

