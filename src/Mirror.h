/*
 *  Mirror.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include "Effect.h"
	
class Mirror : public Effect {
public:
	
	Mirror(bool v, bool h) {
		horizontal=h;
		vertical=v;
	}
	
	void apply(Image* image);
	
	
	bool horizontal;
	bool vertical;
};

