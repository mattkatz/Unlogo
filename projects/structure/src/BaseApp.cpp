/*
 *  BaseApp.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "BaseApp.h"

namespace unlogo
{
	static BaseApp* m_pInstance;
	
	// -------------------------------------------------------
	BaseApp::BaseApp() {
		frameno=0;
		bDoQuit=false;
		m_pInstance = this;
	}
	
	// TO DO: I should use some other method where the user doesn't always have to 
	// refer to windows with strings. Like maybe this method returns an int ID and
	// the user can use that ID to refer to the window in the future.
	// -------------------------------------------------------
	void BaseApp::makeWindow(string name, int x, int y)
	{
		cv::namedWindow(name.c_str(), CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO|CV_GUI_EXPANDED);
		cvMoveWindow(name.c_str(), x, y);
		
		string* window = new string(name);
		cv::setMouseCallback(name.c_str(), BaseApp::onMouse, window);
		
		windows.push_back(name);
	}


	// -------------------------------------------------------
	void BaseApp::onMouse( int event, int x, int y, int flags, void* param )
	{
		if(m_pInstance==NULL) return;
		
		//	bool shift =	flags & CV_EVENT_FLAG_SHIFTKEY;
		//	bool ctrl =		flags & CV_EVENT_FLAG_CTRLKEY;
		//	bool alt =		flags & CV_EVENT_FLAG_ALTKEY;
		//	bool lbutton =	flags & CV_EVENT_FLAG_LBUTTON;
		//	bool rbutton =	flags & CV_EVENT_FLAG_RBUTTON;
		//	bool mbutton =	flags & CV_EVENT_FLAG_MBUTTON;
		
		string* window = (string*)param;
		switch(event)
		{
			case CV_EVENT_MOUSEMOVE:  m_pInstance->mouseMove(*window, x, y); break;
			case CV_EVENT_LBUTTONDOWN: break;
			case CV_EVENT_RBUTTONDOWN: break;
			case CV_EVENT_MBUTTONDOWN: break;
			case CV_EVENT_LBUTTONUP: m_pInstance->click(*window, x, y); break;
			case CV_EVENT_RBUTTONUP: break;
			case CV_EVENT_MBUTTONUP: break;
			case CV_EVENT_LBUTTONDBLCLK: break;
			case CV_EVENT_RBUTTONDBLCLK: break;
			case CV_EVENT_MBUTTONDBLCLK: break;
		}
	}

	// -------------------------------------------------------
	void BaseApp::onTrackbar(int val, void* param)
	{
		string* trackbar = (string*)param;
		m_pInstance->trackbar(*trackbar, val);
	}

	// -------------------------------------------------------
	void BaseApp::show(string window, Image* image, bool clean)
	{
		imshow(window, (clean) ? image->clean : image->dirty);
	}
	
	// -------------------------------------------------------
	void BaseApp::addTrackbar(string window, string trackbarName, int startVal, int maxVal)
	{
		string* trackbar = new string(trackbarName);
		cv::createTrackbar(trackbarName.c_str(), window.c_str(), &startVal, maxVal, BaseApp::onTrackbar, trackbar);	
	}
}