/*
 *  testApp.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "BaseApp.h"
#include "FlowFinder.h"
#include "Threshold.h"
#include "Train.h"
#include "Grayscale.h"
#include "Mirror.h"

class testApp : public unlogo::BaseApp {
public:
	
	void setup();
	void process(unlogo::Query* query);
	void key(int key);
	void click(string window, int x, int y);
	void mouseMove(string window, int x, int y);
	void trackbar(string trackbar, int val);
	
	unlogo::Train* train;
	unlogo::FlowFinder flow;
	unlogo::Threshold* threshold;
	unlogo::Grayscale* grayscale;
	unlogo::Mirror* mirror;
	bool findTrackingPoints;
	
};
