/*
 *  GenericMatcher.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/4/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "GenericMatcher.h"

// --------------------------------------------------
bool GenericMatcher::init(string _detector_alg, string _extractor_alg, string _matcher_alg, float _ransacReprojThreshold)
{
	ransacReprojThreshold = _ransacReprojThreshold;
	
	detector_alg=_detector_alg;
	extractor_alg=_extractor_alg;
	matcher_alg=_matcher_alg;
	
	detector = cv::FeatureDetector::create(detector_alg);
	extractor = cv::DescriptorExtractor::create(extractor_alg);
	matcher = cv::DescriptorMatcher::create(_matcher_alg);
	
	if( detector.empty()) {
		cout << _detector_alg << " is not a known detector algorithm" << endl;
		return false;
	}
	if( extractor.empty()) {
		cout << _extractor_alg << " is not a known extractor algorithm" << endl;
		return false;
	}
	if( matcher.empty() ) {
		cout << _matcher_alg << " is not a known matching algorithm" << endl;
		return false;
	}
	
	bInited=true;
	return true;
}

// --------------------------------------------------
bool GenericMatcher::train(Image& _trainImg, string datapath)
{
	assert(_trainImg.channels()==1);
	
	trainImg = _trainImg;

	if(loadTrainingData(datapath))
	{
		bTrainAnalyzed = true;
	}
	else
	{
		// TO DO:  return false if soething here fails.
		detector->detect(trainImg.cvImg, train_kpts);							//Find interest points
		extractor->compute(trainImg.cvImg, train_kpts, train_descriptors);	//Compute descriptors at each keypoint location	
		saveTrainingData(datapath);
		bTrainAnalyzed=true;
	}

	return bTrainAnalyzed;
}

//--------------------------------------------------------------
void GenericMatcher::drawTrainKeypointsIntoImage(Image& img)
{	
	if(bTrainAnalyzed && train_kpts.size() > 0)
	{
		drawKeypoints(img.cvImg, train_kpts, img.cvImg, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	}
}



// --------------------------------------------------
void GenericMatcher::doQuery(Image& queryImg, bool showCorrespondence)
{
	if(!bInited)	{
		cerr << "WARNING: Not inited. Using default values. Please call init before match." << endl;
		init();
	}
	
	if(!bTrainAnalyzed) {
		cerr << "WARNING: Train image was not set." << endl;
		return;
	}
	
	cv::Mat query_descriptors;
	vector<cv::KeyPoint> query_kpts;
	vector<Point2f> query_pts;
	
	detector->detect(queryImg.cvImg, query_kpts);				
	extractor->compute(queryImg.cvImg, query_kpts, query_descriptors); 
	
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
	if(showCorrespondence)
	{
		Mat correspondenceImg;
		drawMatches(trainImg.cvImg, train_kpts, queryImg.cvImg, query_kpts, matches, correspondenceImg, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), matchesMask, DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("generic correspondence", correspondenceImg);
	}
}

// --------------------------------------------------
bool GenericMatcher::loadTrainingData(string base) 
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
	
	
	bTrainAnalyzed=true;
	return true;
}


// --------------------------------------------------
bool GenericMatcher::saveTrainingData(string base) 
{
	char filename[255];
	char nodename[255];
	
	// Save Keypoints
	sprintf(filename, "%s_keypoints.xml", base.c_str());
	FileStorage keypoints_fs;
	keypoints_fs.open(filename, FileStorage::APPEND);
	if(!keypoints_fs.isOpened())
	{
		cout << "Could not open keypoints file for writing." << endl;
		return false;
	}
	
	sprintf(nodename, "%s_keypoints", detector_alg.c_str());
	cv::write(keypoints_fs, nodename, train_kpts);
	keypoints_fs.release();
	
	// Save Descriptors
	char descriptors_filename[255];
	sprintf(descriptors_filename, "%s_descriptors.xml", base.c_str());
	FileStorage descriptors_fs;
	descriptors_fs.open(descriptors_filename, FileStorage::APPEND);
	if(!descriptors_fs.isOpened()) 
	{
		cout << "Could not open descriptors file for writing." << endl;
		return false;
	}
	
	sprintf(nodename, "%s_descriptors", extractor_alg.c_str());
	descriptors_fs << string(nodename) << train_descriptors;
	descriptors_fs.release();
	
	return true;
}

