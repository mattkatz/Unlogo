/*
 *  OpticalFlow.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/25/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef OPTICAL_FLOW_H
#define OPTICAL_FLOW_H

#include "Image.h"

namespace unlogo {
	class OpticalFlow {
	public:
		OpticalFlow(Image& prev, Image& next);
		Point2f avg( Point2f thresh);
		Point2f at( Point2f pos );
		Point2f inRegion( Rect roi, Point2f thresh);
		void draw(const char* win_name);
		
		
		Mat flow;
		Image prev, next;
		
	};
}

#endif