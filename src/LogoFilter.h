/*
 *  LogoFilter.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/18/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <highgui.h>
#include <cvaux.h>
#include "Image.h"
#include "ulUtils.h"


using namespace cv;
using namespace std;
using namespace unlogo;


typedef struct Logo {
	string search;	// The path to the image to search for
	Mat search_img;
	
	
	string replace;	// The raw replace input (either hex color or image) 
	Mat replace_img;
	Scalar replace_color;
	vector<KeyPoint> keypoints;	
	Mat descriptors;
	vector<Point2f> points;
};

class LogoFilter {
public:
	LogoFilter();
	int init( string detector_type, string descriptor_extractor_type, string descriptor_matcher_type );
	int filter(Mat &in_image, Mat &out_img, bool draw_matches=false);
	int filter(Image &in_img, Image &out_img, bool draw_matches=false);
	int addLogo(string search, string replace);

protected:

	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> descriptorExtractor;
	Ptr<DescriptorMatcher> descriptorMatcher;
	
	int ransacReprojThreshold;
	int ransacMethod;
	vector<Logo> logos;
};