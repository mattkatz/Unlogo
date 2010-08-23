/*
 *  MatchTracker.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATCH_TRACKER_H
#define MATCH_TRACKER_H

#include "MatchSet.h"

namespace unlogo {
	class MatchTracker {
	public:
		MatchTracker();
		void track(MatchSet* set);
		Point2f avg();
		
		
	protected:
		vector<MatchSet*> history;
	};
}


#endif