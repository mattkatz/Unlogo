/*
 *  TrainingImage.h
 *  logomunge
 *
 *  Created by Jeffrey Crouse on 4/6/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */
#pragma once
#define MARGIN 50

#include "Image.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;


class TrainingImage : public Image {
public:
	
	void remove_query_string();
	void generate_cnrfile();
	void open(string _imgfile);
	void match(Image image)
	{
		// Does this image match the 
	}
	
	static void on_mouse( int event, int x, int y, int flags, void* param );

//protected:
	fs::path imgfile;			// path to the image file
	fs::path cnrfile;			// path to the corners file
	vector<KeyPoint> keypoints;	
	Mat descriptors;
	
	Mat image_w_margin;
	vector<Point> corners;
};

