/*
 *  Logo.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/23/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 *	Convenience struct for storing everything associated with a logo
 */

#include "Image.h"
#include "MatchTracker.h"

namespace unlogo
{
	typedef struct Logo
	{
		const char* name;  // Kept for convenience and debugging
		Image logo;
		Image replacement;
		MatchTracker tracker;
	};
}