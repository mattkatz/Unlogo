/*
 *  Image.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "Image.h"

namespace unlogo
{
	// ------------------------------------------
	void Image::open(string filename)
	{
		// TO DO:  use cvtColor here instead of loading each individually
		clean = cv::imread(filename, 1);
		clean.copyTo(dirty);
	}
	
	// ------------------------------------------
	void Image::grabFrame( cv::VideoCapture &cap )
	{
		unsigned char* data = clean.data;
		cap >> clean;
		
		// Just a check to protect against leaks.  
		// There probably isn't a good reason why the Mat should
		// be making a new data array frequently (like every frame)
		// so if you see this a lot, you are probably doing something wrong.
		if( data != clean.data) {
			cout << "INFO: Reallocated Image::clean in Image::grabFrame" << endl;
		}
		clean.copyTo(dirty);
	}
	
	// ------------------------------------------
	void Image::apply(Effect* effect)
	{
		unsigned char* data = clean.data;
		effect->apply(this);
		if( data != clean.data) {
			cout << "INFO: Reallocated Image::clean in Image::apply" << endl;
		}
	}
	
	// ------------------------------------------
	bool Image::empty()
	{
		return clean.empty() || dirty.empty();
	}

}