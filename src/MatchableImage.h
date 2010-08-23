/*
 *  MatchableImage.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATCHABLE_IMAGE_H
#define MATCHABLE_IMAGE_H

#include "Image.h"
#include "Matcher.h"
#include "MatchSet.h"
#include "ulUtils.h"


namespace unlogo {
	class MatchSet;
	class MatchableImage : public Image {
	public:
		MatchableImage();
		MatchSet matchTo( MatchableImage* other );
		void makeDescriptors();
		
	//protected:
		Matcher* matcher;

	};
	
}


#endif