/*
 *  MatchableImage.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "MatchableImage.h"

namespace unlogo {

	MatchableImage::MatchableImage()
	{
		
	}
	
	
	void MatchableImage::makeDescriptors()
	{
		Matcher* matcher = Matcher::Instance();
		if(cvImage.empty())
		{
			log(LOG_LEVEL_ERROR, "in matchTo(), image is empty");
		}
		
		matcher->detector->detect( cvImage, keypoints );
		cout << keypoints.size() << " points" << endl << ">" << endl;
		
		cout << "< Computing descriptors for keypoints..." << endl;
		matcher->descriptorExtractor->compute( cvImage, keypoints, descriptors );
	}
	
	MatchSet MatchableImage::matchTo(MatchableImage* other)
	{
		makeDescriptors();
		MatchSet m(this, other);
		return m;
	}
	
	
}