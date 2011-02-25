/*
 *  Grayscale.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include "Effect.h"

namespace unlogo
{
	
	class Grayscale : public Effect {
	public:
		void apply(Image* image);
	};
}

