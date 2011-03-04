/*
 *  BaseApp.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Query.h"


using namespace std;
using namespace cv;

namespace unlogo
{		
	
	
	class BaseApp{
	public:	

		BaseApp();		
		virtual void setup()=0;
		virtual void process(Query* q)=0;
		virtual void click(string window, int x, int y)=0;
		virtual void mouseMove(string window, int x, int y)=0;
		virtual void trackbar(string trackbar, int val)=0;
		
		void makeWindow(string name, int x=0, int y=0);
		void show(string window, Image* image, bool clean=false);
		void addTrackbar(string window, string trackbarName, int startVal, int maxVal);

		
		void quit()			{	bDoQuit=true;				}
		bool shouldQuit()	{	return bDoQuit;				}	
		void incFrame()		{	frameno++;					}
		bool hasSource()	{	return capture.isOpened();	}
		int getFrameNumber(){	return frameno;				}
		void setVideoSource(string filename)	{	capture.open(filename);	}
		void setVideoSource(int device)			{	capture.open(0);	}

		VideoCapture capture;
		
		static void onMouse( int event, int x, int y, int flags, void* param );
		static void onTrackbar(int val, void* param);
		
	private:

		vector<string> windows;
		bool bDoQuit;
		int frameno;
	};
	
	
}