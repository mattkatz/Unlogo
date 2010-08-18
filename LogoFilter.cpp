/*
 *  LogoFilter.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/18/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "LogoFilter.h"

LogoFilter::LogoFilter() { }

int LogoFilter::init( const char* argstr )
{
	framenum=0;
	
	// Parse arguments.
	vector<string> args;
	split(args, argstr, is_any_of(":"));
	
	if(args.size()<4 || args.size()%2!=1)
	{
		cout << "Usage" << endl;
		cout << "[detector]:[descriptor]:[matcher]:[search:replace]..." << endl;
		cout << "where [search:replace] is a list of images to look for and either image " << endl;
		cout << "or hex color to replace it with.  Hex colors must begin with # and be 6 digits" << endl;
		cout << endl;
		cout << "Detector Types: FAST, STAR, SIFT, SURF, MSER, GFTT, HARRIS" << endl;
		cout << "Descriptor Types: SIFT, SURF" << endl;
		cout << "Matcher Types: BruteForce, BruteForce-L1" << endl;
		return -1;
	}
	
	string detector_type = args[0];
	string descriptor_extractor_type = args[1];
	string descriptor_matcher_type = args[2];
	ransacReprojThreshold = 2;
	
	// Construct the detector, extractor, and matcher.
	detector = createDetector( detector_type );
	descriptorExtractor = createDescriptorExtractor( descriptor_extractor_type );
	descriptorMatcher = createDescriptorMatcher( descriptor_matcher_type );
	if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
	{
		log(LOG_LEVEL_ERROR, "Can not create detector or descriptor extractor or descriptor matcher of given types");
		return -1;
	}
	
	// Construct all of the Logo objects.
	for(size_t i=3; i<args.size(); i+=2)
	{
		Logo logo;
		logo.search = args[i];
		logo.img = imread( logo.search, CV_LOAD_IMAGE_GRAYSCALE);
		if( logo.img.empty() )
		{
			log(LOG_LEVEL_ERROR, "Can not read template image: %s\n", logo.search.c_str());
			return -1;
		}
		
		detector->detect( logo.img, logo.keypoints );
		
		if(logo.keypoints.size() > 0)
		{
			if(ransacReprojThreshold >= 0 )
			{
				KeyPoint::convert(logo.keypoints, logo.points);
			}
			descriptorExtractor->compute( logo.img, logo.keypoints, logo.descriptors );
			
			logo.replace = args[i+1];
			if( starts_with(logo.replace, "#") && logo.replace.length() == 7)
			{
				char* p;
				int r = strtol( logo.replace.substr(1,2).c_str(), &p, 16 );
				int g = strtol( logo.replace.substr(3,2).c_str(), &p, 16 );
				int b = strtol( logo.replace.substr(5,2).c_str(), &p, 16 );
				logo.replace_color = Scalar(r, g, b);
			}
			else
			{
				logo.replace_img = imread(logo.replace, CV_LOAD_IMAGE_COLOR);
				if( logo.replace_img.empty() )
				{
					log(LOG_LEVEL_ERROR, "Can not read replacement image: %s",  logo.replace.c_str());
					return -1;
				}
			}
			
			logos.push_back( logo );
		}
	}
	
	log(LOG_LEVEL_DEBUG, "-----------------");
	log(LOG_LEVEL_DEBUG, "Detector Type: %s", detector_type.c_str());
	log(LOG_LEVEL_DEBUG, "Extractor Type: %s", descriptor_extractor_type.c_str());
	log(LOG_LEVEL_DEBUG, "Matcher Type: %s", descriptor_matcher_type.c_str());
	log(LOG_LEVEL_DEBUG, "Num Logos: %d", logos.size());
	for(size_t i=0; i<logos.size(); i++)
	{
		log(LOG_LEVEL_DEBUG, "\t%s (%d keypoints) --> %s", logos[i].search.c_str(), 
			 logos[i].keypoints.size(), logos[i].replace.c_str());
	}
	log(LOG_LEVEL_DEBUG, "-----------------");
	return 0;
}

int LogoFilter::filter(Mat &in_image, Mat &out_img)
{
	Mat gray_image; 
	cvtColor(in_image, gray_image, CV_RGB2GRAY);
	
    vector<KeyPoint> keypoints2;
    detector->detect( gray_image, keypoints2 );
	log(LOG_LEVEL_DEBUG, "%d keypoints in frame", keypoints2.size());
	
    log(LOG_LEVEL_DEBUG, "Computing descriptors for keypoints from frame...");
    Mat descriptors2;
    descriptorExtractor->compute( gray_image, keypoints2, descriptors2 );
	
	for(size_t i=0; i<logos.size(); i++)
	{
		log(LOG_LEVEL_DEBUG, "Matching descriptors for %s", logos[i].search.c_str());
		vector<int> matches;
		descriptorMatcher->clear();
		descriptorMatcher->add( descriptors2 );
		descriptorMatcher->match( logos[i].descriptors, matches );
		
		Mat H12;
		vector<Point2f> points2;
		
		if(ransacReprojThreshold >= 0 )
		{
			log(LOG_LEVEL_DEBUG, "Computing homography (RANSAC)");
			KeyPoint::convert(keypoints2, points2, matches);
			H12 = findHomography( Mat(logos[i].points), Mat(points2), CV_RANSAC, ransacReprojThreshold );
		}
		
		if( H12.empty() )
		{
			log(LOG_LEVEL_WARNING, "No homography found...");
		}
		else
		{
			log(LOG_LEVEL_DEBUG, "Homography found...");
			
			vector<char> matchesMask( matches.size(), 0 );
			Mat points1t; perspectiveTransform(Mat(logos[i].points), points1t, H12);
			vector<int>::const_iterator mit = matches.begin();
			for( size_t i1 = 0; i1 < logos[i].points.size(); i1++ )
			{
				if( norm(points2[i1] - points1t.at<Point2f>(i1,0)) < 4) // inlier
					matchesMask[i1] = 1;
			}
			
			for(size_t i2=0; i2<matches.size(); i2++)
			{
				if(matchesMask[i2]>0)
				{
					int match = matches[i2];
					Point2f p = points2[match];
					circle(out_img, p, 4, CV_RGB(0, 255, 0), 1);
				}
			}
		}
	}
	framenum++;
	return 0;
}

int LogoFilter::log( int level, const char * format, ... )
{
	string color;
	switch(level) {
		default:
		case LOG_LEVEL_DEBUG: 	color="\033[01;33m"; break;
		case LOG_LEVEL_WARNING:	color="\033[01;34m"; break;
		case LOG_LEVEL_ERROR:	color="\033[01;31m"; break;
	}
	va_list args;
	va_start(args, format);
	string fstr(color+string("[unlogo] ")+format+"\n");
	int status = vfprintf(stderr, fstr.c_str(), args);
	va_end(args);
	return status;
}