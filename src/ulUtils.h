/*
 *  ulUtils.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef UL_UTILS_H
#define UL_UTILS_H

#include <iostream>
#include <string>
#include <stdarg.h>
#include <cstdio>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <highgui.h>
#include <cvaux.h>
#include "vt100.h"


using namespace std;
using namespace cv;

#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3


int log( int level, const char* format, ... );
void lerp(Point2f& current, const Point2f& desired, float ease=10.);
vector<string> split(const string& str, const string& delimiters = " ");

template<typename T>
void alphaBlendRGBA(const Mat& src1, const Mat& src2, Mat& dst)
{
    const float alpha_scale = (float)std::numeric_limits<T>::max();
	//const float inv_scale = 1.f/alpha_scale;
	
    CV_Assert( src1.type() == src2.type() );
	CV_Assert( src1.type() == CV_MAKETYPE(DataType<T>::depth, 4) );
	CV_Assert( src1.size() == src2.size() );
	
    Size size = src1.size();
    dst.create(size, src1.type());
	
    // here is the idiom: check the arrays for continuity and,
    // if this is the case,
    // treat the arrays as 1D vectors
    if( src1.isContinuous() && src2.isContinuous() && dst.isContinuous() )
    {
        size.width *= size.height;
        size.height = 1;
    }
    size.width *= 4;
	
    for( int i = 0; i < size.height; i++ )
    {
        // when the arrays are continuous,
        // the outer loop is executed only once
        const T* ptr1 = src1.ptr<T>(i);
        const T* ptr2 = src2.ptr<T>(i);
        T* dptr = dst.ptr<T>(i);
		
        for( int j = 0; j < size.width; j += 4 )
        {
            float alpha = ptr1[j+3] / alpha_scale; 
			float beta	= ptr2[j+3] / alpha_scale;
            dptr[j  ]	= saturate_cast<T>(ptr1[j  ] * alpha + ptr2[j  ] * beta);
            dptr[j+1]	= saturate_cast<T>(ptr1[j+1] * alpha + ptr2[j+1] * beta);
            dptr[j+2]	= saturate_cast<T>(ptr1[j+2] * alpha + ptr2[j+2] * beta);
            dptr[j+3]	= saturate_cast<T>(ptr1[j+3] + ptr2[j+3]); //((1 - (1-alpha)*(1-beta))*alpha_scale);
        }
    }
}

#endif