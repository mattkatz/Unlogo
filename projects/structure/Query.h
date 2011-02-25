/*
 *  Query.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"

namespace unlogo
{
	
	/*
	 Represents a "query" image. In a CV application, the query is usually a single 
	 frame of a video.  It is defined in contrast to the "train" image, which is 
	 usually analyzed once at the beginning of the app and then compared against the query image.
	 */
	class Query : public Image {
	public:
		
		Query* clone() {
			Query* q = new Query();
			q->clean = clean.clone();
			q->dirty = dirty.clone();
			return q;
		}
	};
	
}