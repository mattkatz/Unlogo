//
//  main.m
//  scratch
//
//  Created by Jeffrey Crouse on 2/1/11.
//  Copyright 2011 Eyebeam. All rights reserved.
//

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "Query.h"
#include "testApp.h"


int main(int argc, char *argv[])
{
	// Make ourselves a testApp
	testApp* app = new testApp();
	app->setup();

	// Make sure the video source opened.
	if (!app->hasSource()) {
		printf("Capture failed to open.  Did you call setVideoSource()?\n");
		return 1;
	}
	
	// Create a holder for the incoming frames
	unlogo::Query query;	
	
	// Do the loop
	while(!app->shouldQuit())
	{
		query.grabFrame( app->capture );
		if (query.empty())
			continue;

		// Process the frame we got
		app->process( &query );
	
		// Pass any key presses to the app
		int key = cvWaitKey(100);
		if(key != -1) app->key(key);
		
		// Increment the frame 
		app->incFrame();
	}

    return 0;
}
