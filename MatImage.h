/*
 *  MatImage.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATIMG_H
#define MATIMG_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <highgui.h>
#include <cvaux.h>
#include "ulUtils.h"

using namespace cv;
using namespace std;

class MatImage {
public:
	
	MatImage();

	Mat cvImage;	
	
	void	convert( int conversion_code );
	int		open( const char* filename );
	void	show( const char* window_name );
	void	drawIntoMe( MatImage &child, int x, int y );
	
	// Operator Overloads
	//
	void operator = ( VideoCapture &cap );
	void operator = ( Mat &other );
protected:
	
	
	
};

#endif