/*
 *  Grayscale.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/10/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "Grayscale.h"

void unlogo::Grayscale::apply(Image* image)
{
	cv::cvtColor(image->clean, image->dirty, CV_RGB2GRAY);
}
