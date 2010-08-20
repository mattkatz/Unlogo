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

int LogoFilter::init(string detector_type, string descriptor_extractor_type, string descriptor_matcher_type)
{
	ransacReprojThreshold = 2;
	ransacMethod = CV_RANSAC;
	
	// Construct the detector, extractor, and matcher.
	detector = createDetector( detector_type );
	descriptorExtractor = createDescriptorExtractor( descriptor_extractor_type );
	descriptorMatcher = createDescriptorMatcher( descriptor_matcher_type );
	if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
	{
		log(LOG_LEVEL_ERROR, "Can not create detector or descriptor extractor or descriptor matcher of given types");
		return -1;
	}
	
	log(LOG_LEVEL_DEBUG, "Detector Type: %s", detector_type.c_str());
	log(LOG_LEVEL_DEBUG, "Extractor Type: %s", descriptor_extractor_type.c_str());
	log(LOG_LEVEL_DEBUG, "Matcher Type: %s", descriptor_matcher_type.c_str());
	return 0;
}

int LogoFilter::addLogo(string search, string replace)
{
	Logo logo;
	logo.search = search;
	logo.img = imread( logo.search, CV_LOAD_IMAGE_GRAYSCALE );
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
		
		logo.replace = replace;
		if( logo.replace.find_first_of("0x")==0 && logo.replace.length() == 8)
		{
			char* p;
			int r = strtol( logo.replace.substr(2,2).c_str(), &p, 16 );
			int g = strtol( logo.replace.substr(3,2).c_str(), &p, 16 );
			int b = strtol( logo.replace.substr(6,2).c_str(), &p, 16 );
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

		log(LOG_LEVEL_DEBUG, "%s (%d keypoints) --> %s", logo.search.c_str(), 
				logo.keypoints.size(), logo.replace.c_str() );
		
		return 0;
	}
	return -1;
}


// in_image is the one to analyze
// out_img is the one to draw on
int LogoFilter::filter(Mat &in_img, Mat &out_img, bool draw_matches)
{
	if(in_img.empty() || out_img.empty()) {
		log(LOG_LEVEL_DEBUG, "Frame is empty.");
		return -1;
	}
	
	Mat gray_img; 
	cvtColor(in_img, gray_img, CV_RGB2GRAY);
	//blur(gray_img, gray_img, Size(3,3), Size(0,0));

    vector<KeyPoint> keypoints2;
    detector->detect( gray_img, keypoints2 );
	log(LOG_LEVEL_DEBUG, "%d keypoints in frame", keypoints2.size());
	
    Mat descriptors2;
    descriptorExtractor->compute( gray_img, keypoints2, descriptors2 );
	
	for(size_t i=0; i<logos.size(); i++)
	{
		log(LOG_LEVEL_DEBUG, "Matching descriptors for %s", logos[i].search.c_str());
		vector<int> matches;
		descriptorMatcher->clear();
		descriptorMatcher->add( descriptors2 );
		descriptorMatcher->match( logos[i].descriptors, matches );
		
		Mat H12;
		vector<Point2f> points2;
		
		if(ransacReprojThreshold >= 0)
		{
			log(LOG_LEVEL_DEBUG, "Computing homography (RANSAC)");
			KeyPoint::convert(keypoints2, points2, matches);
			H12 = findHomography( Mat(logos[i].points), Mat(points2), ransacMethod, ransacReprojThreshold );
		}
		
		vector<char> matchesMask( matches.size(), 0 );
		
		if( H12.empty() )
		{
			log(LOG_LEVEL_WARNING, "No homography found...");
		}
		else
		{
			log(LOG_LEVEL_DEBUG, "Homography found...");
			
			Mat points1t;
			perspectiveTransform(Mat(logos[i].points), points1t, H12);
			vector<int>::const_iterator mit = matches.begin();
			vector<Point2f> inliers;
			Point2f center;
			for( size_t j = 0; j < logos[i].points.size(); j++ )
			{
				if( norm(points2[j] - points1t.at<Point2f>(j,0)) < 4) // inlier
				{
					matchesMask[j] = 1;
					inliers.push_back( points2[j] );
					circle(out_img, points2[j], 4, CV_RGB(255, 0, 0), 1);
					center += points2[j];
				}
			}
			
			center = Point2d(center.x/inliers.size(), center.y/inliers.size());
			circle(out_img, center, 10, logos[i].replace_color, 5, CV_AA);
			log(LOG_LEVEL_DEBUG, "%d matches, %d inliers", matches.size(), inliers.size() );
		}
		
		if(draw_matches)
		{
			Mat drawImg;
			drawMatches(logos[i].img, logos[i].keypoints, gray_img, keypoints2, matches, drawImg, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), matchesMask,
						DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			imshow( logos[i].search, drawImg );
		}
	}
	
	return 0;
}