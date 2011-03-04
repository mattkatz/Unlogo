/*
 *  Train.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "Train.h"

namespace unlogo
{
	// --------------------------------------------------
	void Train::init(string _detector_alg, string _extractor_alg, string _matcher_alg)
	{
		detector_alg=_detector_alg;
		extractor_alg=_extractor_alg;
		matcher_alg=_matcher_alg;
		
		detector = cv::FeatureDetector::create(detector_alg);
		extractor = cv::DescriptorExtractor::create(extractor_alg);
		matcher = cv::DescriptorMatcher::create(_matcher_alg);
		
		if( detector.empty())		throw "Detector algorithm was not recognized.";
		if( extractor.empty())		throw "Extractor algorithm was not recognized.";
		if( matcher.empty())		throw "Matcher algorithm was not recognized.";
		
		bInited=true;
	}
	
	// --------------------------------------------------
	void Train::analyze()
	{
		if(dirty.empty()) throw "You must load an image before analyzing it.";
		
		detector->detect(dirty, train_kpts);							//Find interest points
		extractor->compute(dirty, train_kpts, train_descriptors);	//Compute descriptors at each keypoint location	
		
		bAnalyzed=true;
	}

	// --------------------------------------------------
	void Train::match(Query* query, float ransacReprojThreshold)
	{
		if(!bInited)	{
			cerr << "WARNING: Train not inited. Using default values. Please call init before match." << endl;
			init();
		}
		
		if(!bAnalyzed) {
			cerr << "WARNING: Train image was not analyzed. Analyzing now. Please call analyze before match." << endl;
			analyze();
		}
		
		cv::Mat query_descriptors;
		vector<cv::KeyPoint> query_kpts;
		
		detector->detect(query->dirty, query_kpts);				
		extractor->compute(query->dirty, query_kpts, query_descriptors); 
		
		// Match the keypoints
		if(bDoCrossCheckFilter) {
			crossCheckMatching(matcher, train_descriptors, query_descriptors, matches, 1 );
		} else {
			matcher->match( train_descriptors, query_descriptors, matches );
		}
		
		vector<int> queryIdxs(matches.size());
		vector<int> trainIdxs(matches.size());
		for( size_t i = 0; i < matches.size(); i++ )
		{
			queryIdxs[i] = matches[i].queryIdx;
			trainIdxs[i] = matches[i].trainIdx;
		}
		
		
		KeyPoint::convert(query_kpts, query_pts, queryIdxs);
		KeyPoint::convert(train_kpts, train_pts, trainIdxs);
		
		
		// Calculate the homography
		if(ransacReprojThreshold >= 0 )
		{
			H12 = findHomography( Mat(query_pts), Mat(train_pts), CV_RANSAC, ransacReprojThreshold );
		}
		
		// Make a mask of the points in the query image that survive the perspective transform
		// of the homography
		vector<char> matchesMask(matches.size(), 0);
		if( !H12.empty() )
		{
			Mat query_points_transformed; 
			perspectiveTransform(Mat(query_pts), query_points_transformed, H12);
			for( size_t i1 = 0; i1 < query_pts.size(); i1++ )
			{
				if( norm(train_pts[i1] - query_points_transformed.at<Point2f>((int)i1,0)) < 4 ) // inlier
					matchesMask[i1] = 1;
			}				
		}
		
		drawMatches( clean, train_kpts, query->clean, query_kpts, matches, correspondenceImg, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), matchesMask, DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	}
		
	
	// --------------------------------------------------
	bool Train::load(string base) 
	{
		if(!bInited)	{
			cerr << "WARNING: Train not inited. Using default values. Please call init before load." << endl;
			init();
		}
		
		// TO DO: Think more about the logic here.
		// Should I check if all of the files exist before loading any?
		char filename[255];
		char nodename[255];
		
		// Open Keypoints file
		sprintf(filename, "%s_keypoints.xml", base.c_str());
		FileStorage keypoints_fs;
		keypoints_fs.open(filename, FileStorage::READ);
		if(!keypoints_fs.isOpened())
			return false;
		
		// Load keypoints for set keypoint detection method
		sprintf(nodename, "%s_keypoints", detector_alg.c_str());
		if(keypoints_fs[nodename].empty())
			return false;
		cv::read(keypoints_fs[nodename], train_kpts);
		keypoints_fs.release();
		
		
		// Open descriptor file
		sprintf(filename, "%s_descriptors.xml", base.c_str());
		FileStorage descriptors_fs;
		descriptors_fs.open(filename, FileStorage::READ);
		if(!descriptors_fs.isOpened())
			return false;
		
		// Load descriptors for given extractor method
		sprintf(nodename, "%s_descriptors", extractor_alg.c_str());
		if(descriptors_fs[nodename].empty()) 
			return false;
		descriptors_fs[nodename] >> train_descriptors;
		descriptors_fs.release();
		
		
		bAnalyzed=true;
		return true;
	}
	
	
	// --------------------------------------------------
	void Train::save(string base) 
	{
		char filename[255];
		char nodename[255];
		
		// Save Keypoints
		sprintf(filename, "%s_keypoints.xml", base.c_str());
		FileStorage keypoints_fs;
		keypoints_fs.open(filename, FileStorage::APPEND);
		if(!keypoints_fs.isOpened()) throw "Could not open keypoints file for writing.";
		
		sprintf(nodename, "%s_keypoints", detector_alg.c_str());
		cv::write(keypoints_fs, nodename, train_kpts);
		keypoints_fs.release();
		
		// Save Descriptors
		char descriptors_filename[255];
		sprintf(descriptors_filename, "%s_descriptors.xml", base.c_str());
		FileStorage descriptors_fs;
		descriptors_fs.open(descriptors_filename, FileStorage::APPEND);
		if(!descriptors_fs.isOpened()) throw "Could not open descriptors file for writing.";
		
		sprintf(nodename, "%s_descriptors", extractor_alg.c_str());
		descriptors_fs << string(nodename) << train_descriptors;
		descriptors_fs.release();
	}
	
}