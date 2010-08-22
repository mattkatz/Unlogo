/*
 *  MatImage.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "MatImage.h"

//--------------------------------------------------
MatImage::MatImage()
{

}



//--------------------------------------------------
void MatImage::show( const char* window_name )
{
	if(!cvImage.empty())
	{
		imshow( window_name, cvImage );
	}
}



//--------------------------------------------------
int MatImage::open( const char* path )
{
	cvImage = imread( path );
	log(LOG_LEVEL_DEBUG, "in open() image has %d channels", cvImage.channels());
	if( cvImage.empty() )
	{
		log(LOG_LEVEL_ERROR, "in open() Can not read %s", path);
		return -1;
	}
	log(LOG_LEVEL_DEBUG, "in open() Image has %d channels", cvImage.channels());
	return 0;
}

//--------------------------------------------------
void MatImage::drawIntoMe(MatImage &child, int x, int y )
{
	log(LOG_LEVEL_DEBUG, "in drawIntoMe() Mother %d channels, Child %d channels", cvImage.channels(), child.cvImage.channels());
	if(cvImage.type() != child.cvImage.type())
	{
		log(LOG_LEVEL_ERROR, "in drawIntoMe() Images must be of the same type. Mother=%d  Child=%d", cvImage.type(), child.cvImage.type());
		return;
	}
	if(cvImage.empty())
	{
		log(LOG_LEVEL_ERROR, "in drawIntoMe() Target image is empty");
		return;
	}
	// Make a Mat that points to the child-sized region of the image, starting at x,y
	Mat out_roi = cvImage(Rect(x, y, child.cvImage.cols, child.cvImage.rows));
	
	// Different operations depending on whether there is alpha involved
	if(child.cvImage.channels()<4)
	{
		child.cvImage.copyTo(out_roi);
	}
	else
	{
		// How can you determine the type?  it won't always be uchar
		alphaBlendRGBA<uchar>(out_roi, child.cvImage, out_roi);
	}
}




//--------------------------------------------------
void MatImage::convert( int conversion_code )
{
	if(cvImage.empty())
	{
		log(LOG_LEVEL_ERROR, "in convert() Image is empty.");
		return;
	}

	Mat cvImageTemp;
	cvtColor(cvImage, cvImageTemp, conversion_code);
	cvImage = cvImageTemp;
}



//--------------------------------------------------
void MatImage::operator = ( VideoCapture &cap )
{
	cap >> cvImage;
}

//--------------------------------------------------
void MatImage::operator = ( Mat &other )
{
	other.copyTo( cvImage );
}

