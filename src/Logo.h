/*
 *  Logo.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/23/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "Image.h"

using namespace unlogo;

// Just a little convenience struct for storing everything
// associated with a particular logo
typedef struct Logo {
	const char* name;  // Kept for convenience and debugging
	Image logo;
	Image replacement;
	MatchTracker tracker;
};