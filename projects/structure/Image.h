/*
 *  Image.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Effect.h"

namespace unlogo {

	class Effect;
	
	class Image {
	public:
		
		void open(string filename);
		void grabFrame( cv::VideoCapture &cap );
		bool empty();
		void apply(Effect* filter);
		
		cv::Mat clean;
		cv::Mat dirty;
	};
}