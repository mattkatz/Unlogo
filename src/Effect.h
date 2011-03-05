/*
 *  Filter.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/8/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"


class Image;

class Effect {
public:
	virtual void apply(Image* image)=0;
};

