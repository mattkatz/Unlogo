/*
 *  Image.h
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Effect.h"


using namespace cv;
using namespace std;

class Effect;

class Image {
public:
	
	// Constructors
	Image(){}
	Image(Mat m)							{	m.copyTo(cvImg);						}
	Image(Size size, int type)				{	cvImg.create(size, type);				}
	Image(Size size)						{	cvImg.create(size, CV_8UC3);			}
	Image(int width, int height, int type)	{	cvImg.create(height, width, type);		}
	Image(int width, int height)			{	cvImg.create(height, width, CV_8UC3);	}
	
	// Operator overloading
	Image& operator=(const Image &other);
	Image clone() {	return *(new Image(cvImg.clone()));	}
	
	
	// Convenience methods
	Size size()				{	return cvImg.size();		}
	int width()				{	return cvImg.size().width;	}
	int height()			{	return cvImg.size().height; }
	int channels()			{	return cvImg.channels();	};	
	void convert(int code)	{	cvtColor(cvImg, cvImg, code);			}
	void add(Image &img)	{	cv::add(img.cvImg, cvImg, cvImg);		}
	void dilate(int amt)	{	cv::dilate(cvImg, cvImg, Mat(), Point(-1,-1), amt);	}
	void erode(int amt)		{	cv::erode(cvImg, cvImg, Mat(), Point(-1,-1), amt);	}
	void blur(int amt)		{	cv::blur(cvImg, cvImg, Size(amt,amt));		}
	void invert()			{	cv::bitwise_not(cvImg, cvImg);	}
	void show(string wn)	{	cv::imshow(wn, cvImg);		}
	bool empty()			{	return cvImg.empty();		}	

	
	// Point this image to the data in m
	void useMat( cv::Mat m );
	
	// Open an image from disk
	bool open(string filename, bool forceGrayscale=false);
	
	// Grab a frame from a VideoCapture object
	void grabFrame( cv::VideoCapture &cap );
	
	// Apply an effect
	void apply(Effect* filter);
	
	// Get a section from another image
	void copySectionFrom(Image src, Rect src_rect);
	void copySectionFrom(Image src, Rect src_rect, Rect dst_rect);
	
	// Copy this image into another at a particular location
	void copyInto(Image background, int x, int y, Image mask);
	void copyInto(Image background, int x, int y);
	
	// Show this image using HighGUI
	void show(string windowname, int x, int y);
	
	// Get an image for each channel in this image
	vector<Image> split();
	
	// Get a matrix of the image as a list of pixels (ie: 1 column, w*h rows)
	Mat asList();
	
	// Set the depth of values  eg. CV_8U, CV_32F
	void setDepth(int newType);
	
	// Get a particular number of kmeans clusters of colors
	float* getClusters(int clusterCount);
	
	// Get some copies of this image
	vector<Image> copies(int num);
	
	// Get a grayscale version of this image.
	Image getGrayscale();
	
	// Get a reference to the cv::Mat
	Mat* getCvImg() {	return &cvImg; }
	
//protected:	
	cv::Mat cvImg;
};
