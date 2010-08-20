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
	type = -1;
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
	if( cvImage.empty() )
	{
		log(LOG_LEVEL_ERROR, "Can not read %s", path);
		return -1;
	}
	log(LOG_LEVEL_DEBUG, "Image has %d channels", cvImage.channels());
	switch(cvImage.channels())
	{
		case 1:	type = MATIMG_CSPACE_GRAY;	break;
		case 3:	type = MATIMG_CSPACE_RGB;	break;
		case 4:	type = MATIMG_CSPACE_RGBA;	break;
		default: log(LOG_LEVEL_DEBUG, "Warning:  unknown image type."); break;
	}
	return 0;
}


//--------------------------------------------------
void MatImage::drawIntoMe( MatImage &child, int x, int y )
{
	if(cvImage.type() != child.cvImage.type())
	{
		log(LOG_LEVEL_ERROR, "Images must be of the same type. Mother=%d  Child=%d", cvImage.type(), child.cvImage.type());
		return;
	}
	if(cvImage.empty())
	{
		log(LOG_LEVEL_ERROR, "Target image is empty");
		return;
	}
	Rect roi(x, y, child.cvImage.cols, child.cvImage.rows);
	Mat out_roi = cvImage(roi);
	child.cvImage.copyTo(out_roi);
}

//--------------------------------------------------
void MatImage::changeTo( int new_type )
{
	if(cvImage.empty())
	{
		log(LOG_LEVEL_ERROR, "Image is empty."); 
	}
	if(type == -1)
	{
		log(LOG_LEVEL_WARNING, "Unknown image type!");
		return;
	}
	if(type == new_type)
	{
		log(LOG_LEVEL_WARNING, "Trying to convert to same type.");
		return;
	}
	
	int code = -1;
	switch(type) {
		case MATIMG_CSPACE_YCrCb: switch(new_type) {
			case MATIMG_CSPACE_RGB:		code = CV_YCrCb2RGB;	break;
			case MATIMG_CSPACE_BGR:		code = CV_YCrCb2BGR;	break;
		} break;
		case MATIMG_CSPACE_RGB: switch(new_type) {
			case MATIMG_CSPACE_YCrCb:	code = CV_RGB2YCrCb;	break;
			case MATIMG_CSPACE_BGR:		code = CV_RGB2BGR;		break;
			case MATIMG_CSPACE_GRAY:	code = CV_RGB2GRAY;		break;
			case MATIMG_CSPACE_RGBA:	code = CV_RGB2RGBA;		break;
			case MATIMG_CSPACE_BGRA:	code = CV_RGB2BGRA;		break;
			case MATIMG_CSPACE_HSV:		code = CV_RGB2HSV;		break;
		} break;
		case MATIMG_CSPACE_BGR: switch(new_type) {
			case MATIMG_CSPACE_YCrCb:	code = CV_BGR2YCrCb;	break;
			case MATIMG_CSPACE_RGB:		code = CV_BGR2RGB;		break;
			case MATIMG_CSPACE_GRAY:	code = CV_BGR2GRAY;		break;
			case MATIMG_CSPACE_RGBA:	code = CV_BGR2RGBA;		break;
			case MATIMG_CSPACE_BGRA:	code = CV_BGR2BGRA;		break;
			case MATIMG_CSPACE_HSV:		code = CV_BGR2HSV;		break;
		} break;
		case MATIMG_CSPACE_GRAY: switch(new_type) {
			case MATIMG_CSPACE_RGB:		code = CV_GRAY2RGB;		break;
			case MATIMG_CSPACE_BGR:		code = CV_GRAY2BGR;		break;
			case MATIMG_CSPACE_RGBA:	code = CV_GRAY2RGBA;	break;
			case MATIMG_CSPACE_BGRA:	code = CV_GRAY2BGRA;	break;
		} break;
		case MATIMG_CSPACE_RGBA: switch(new_type) {
			case MATIMG_CSPACE_RGB:		code = CV_RGBA2RGB;		break;
			case MATIMG_CSPACE_BGR:		code = CV_RGBA2BGR;		break;
			case MATIMG_CSPACE_GRAY:	code = CV_RGBA2GRAY;	break;
			case MATIMG_CSPACE_BGRA:	code = CV_RGBA2BGRA;	break;
		} break;
		case MATIMG_CSPACE_BGRA: switch(new_type) {
			case MATIMG_CSPACE_RGB:		code = CV_BGRA2RGB;		break;
			case MATIMG_CSPACE_BGR:		code = CV_BGRA2BGR;		break;
			case MATIMG_CSPACE_GRAY:	code = CV_BGRA2GRAY;	break;
			case MATIMG_CSPACE_RGBA:	code = CV_BGRA2RGBA;	break;
		} break;
		case MATIMG_CSPACE_HSV: switch(new_type) {
			case MATIMG_CSPACE_RGB:		code = CV_HSV2RGB;		break;
			case MATIMG_CSPACE_BGR:		code = CV_HSV2BGR;		break;
		} break;
	}
	if(code == -1)
	{
		log(LOG_LEVEL_ERROR, "The conversion you requested cannot be completed");
	}
	else
	{
		Mat cvImageTemp;
		cvtColor(cvImage, cvImageTemp, code);
		cvImage = cvImageTemp;
	}
}



//--------------------------------------------------
void MatImage::operator = ( VideoCapture &cap )
{
	cap >> cvImage;
	if(cap.get(CV_CAP_PROP_CONVERT_RGB)==0)
	{
		type = MATIMG_CSPACE_BGR;
	}
	else
	{
		type = MATIMG_CSPACE_RGB;
	}

}

//--------------------------------------------------
void MatImage::operator = ( Mat &other )
{
	other.copyTo( cvImage );
}

