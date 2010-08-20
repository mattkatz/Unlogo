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
enum MatImageColorspaces
{
	MATIMG_CSPACE_YCrCb,
	MATIMG_CSPACE_RGB,
	MATIMG_CSPACE_BGR,
	MATIMG_CSPACE_GRAY,
	MATIMG_CSPACE_RGBA,
	MATIMG_CSPACE_BGRA,
	MATIMG_CSPACE_HSV
};

using namespace cv;
using namespace std;

class MatImage {
public:
	
	MatImage();

	int type;
	Mat cvImage;
	
	
	void	changeTo( int type );
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