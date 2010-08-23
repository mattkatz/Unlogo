/*
 *  MatchTracker.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "MatchTracker.h"

namespace unlogo {

	MatchTracker::MatchTracker()
	{
		
	}
	
	void MatchTracker::track(MatchSet* set)
	{
		history.push_back( set );
	}

	
	Point2f MatchTracker::avg()
	{
		return Point2f(0, 0);
	}
}