//
//  main.m
//  scratch
//
//  Created by Jeffrey Crouse on 2/1/11.
//  Copyright 2011 Eyebeam. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "utils.h"

using namespace std;
using namespace cv;

#define MODE_CONTOURS 1
#define MODE_CHAMER 2
#define MODE_FEATURES 3


#define MODE_MAX MODE_FEATURES
#define MODE_FIRST MODE_CONTOURS

enum { NONE_FILTER = 0, CROSS_CHECK_FILTER = 1 };

int main(int argc, char *argv[])
{
	// Images
	// TERMINOLOGY:  "train" = template image, "query" = video frame
	cv::Mat queryColor, queryGray, queryProcessed;
	cv::Mat train = imread("data/logo.png", 0);
	cv::Mat trainTemp;
	
	
	// Stuff used by different processes
	
	// FEATURE
	// Fast, Star, Sift, Surf, Mser, Gftt, Harris, GridAdapted, PyramidAdapted
	cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SURF");
	// Sift, Surf, Brief, OpponentColor
	cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SURF");
	// BruteForce, FlannBased
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");
	if( detector.empty() || extractor.empty() || matcher.empty()  )
    {
        cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;
        return -1;
    }
	
	bool doCrossCheckFilter=true;
	double ransacReprojThreshold = 5;
	cv::Mat correspondenceImg;
	
	
	Mat train_descriptors, query_descriptors, H12;
	vector<Point2f> train_pts, query_pts;
	vector<KeyPoint> train_kpts, query_kpts;
	vector<DMatch> matches;
	vector<int> queryIdxs, trainIdxs;
	
	// Find the keypoints in the train image
	detector->detect(train, train_kpts);								//Find interest points
	extractor->compute(train, train_kpts, train_descriptors );			//Compute descriptors at each keypoint location
	

	
	//CONTOURS
	cv::Mat contourFinderTmp;		// the contourFinder destroys the image, so we make a copy before feeding it in.
	int _levels=3;
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> contourHierarchy;
	
	
	
	//CHAMER
	vector<vector<Point> > chamerResults;
	vector<float> chamerCosts;
	int bestChamer=0;
	
	
	
	
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened()) {
		cout << "capture device 0 failed to open!" << endl;
		return 1;
	}
	
	namedWindow("query");		cvMoveWindow("query", 0, 0);
	namedWindow("gray");		cvMoveWindow("gray", 640, 0);
	namedWindow("processed");	cvMoveWindow("processed", 0, 490);
	namedWindow("train");		cvMoveWindow("train", 640, 490);
	
	
	int mode = MODE_FEATURES;
	bool quit = false;
	while(!quit)
	{
		capture >> queryColor;
		if (queryColor.empty())
			continue;
		
		
		//cv::flip( source, source, 1);	
		cv::cvtColor(queryColor, queryGray, CV_RGB2GRAY );
		imshow("gray", queryGray);
		imshow("train", train);
		
		switch (mode)
		{
				
			case MODE_CONTOURS:
				putText(queryColor, "CONTOURS", cv::Point(10, 20), FONT_HERSHEY_SIMPLEX, .5, cv::Scalar(255,255,255));
				
				cv::threshold( queryGray, queryProcessed, 50, 255, cv::THRESH_BINARY);
				imshow("processed", queryProcessed);
				contours.clear();
				contourHierarchy.clear();
				contourFinderTmp = queryProcessed.clone();
				cv::findContours( contourFinderTmp, contours, contourHierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
				drawContours( queryColor, contours, _levels <= 0 ? 3 : -1, cv::Scalar(128,255,255),
							 2, CV_AA, contourHierarchy, std::abs(_levels) );	
				break;
			
				
			case MODE_CHAMER:
				putText(queryColor, "CHAMER", cv::Point(10, 20), FONT_HERSHEY_SIMPLEX, .5, cv::Scalar(255,255,255));
				
				trainTemp = train.clone();
				//Canny(templTemp, templTemp, 5, 50, 3);
				
				//Canny(const Mat& image, Mat& edges, double threshold1, double threshold2, int apertureSize=3, bool L2gradient=false)
				//Canny(graySource, processed, 5, 50, 3);
				
				//Sobel(const Mat& src, Mat& dst, int ddepth, int xorder, int yorder, int ksize=3, double scale=1, double delta=0, int borderType=BORDER_DEFAULT)¶
				Sobel(queryGray, queryProcessed, CV_8U, 1, 0);
				
				
				imshow("processed", queryProcessed);
				
				bestChamer = chamerMatching( queryProcessed, trainTemp, chamerResults, chamerCosts );
				if( bestChamer > 0 )
				{
					size_t n = chamerResults[bestChamer].size();
					for(size_t i = 0; i < n; i++ )
					{
						Point pt = chamerResults[bestChamer][i];
						if( pt.inside(Rect(0, 0, queryColor.cols, queryColor.rows)) )
						{
							queryColor.at<Vec3b>(pt) = Vec3b(0, 255, 0);
						}
					}
				}
				
				char msg[255];
				sprintf(msg, "bestChamer=%d", bestChamer);
				putText(queryColor, msg, cv::Point(10, 40), FONT_HERSHEY_SIMPLEX, .5, cv::Scalar(255,255,255));
				
				break;
				
				
			case MODE_FEATURES:
				putText(queryColor, "FEATURES", cv::Point(10, 20), FONT_HERSHEY_SIMPLEX, .5, cv::Scalar(255,255,255));
				
				// Find the keypoints and descriptors for the new frame
				detector->detect(queryGray, query_kpts);				
				extractor->compute(queryGray, query_kpts, query_descriptors); 
				
				
				// Match the keypoints
				if(doCrossCheckFilter) {
					crossCheckMatching(matcher, train_descriptors, query_descriptors, matches, 1 );
				} else {
					matcher->match( train_descriptors, query_descriptors, matches );
				}
				
			
				// Yank out just the indicies of the matches
				queryIdxs.resize( matches.size() );
				trainIdxs.resize( matches.size() );
				for( size_t i = 0; i < matches.size(); i++ )
				{
					queryIdxs[i] = matches[i].queryIdx;
					trainIdxs[i] = matches[i].trainIdx;
				}
				
				// Convert the keypoints to regular 2D points
				KeyPoint::convert(query_kpts, query_pts, queryIdxs);
				KeyPoint::convert(train_kpts, train_pts, trainIdxs);
				
					
				// Calculate the homography
				if(ransacReprojThreshold >= 0 )
				{
					H12 = findHomography( Mat(query_pts), Mat(train_pts), CV_RANSAC, ransacReprojThreshold );
				}
				
				// Make a mask of the points in the query image that survive the perspective transform
				// of the homography
				if( !H12.empty() )
				{
					vector<char> matchesMask( matches.size(), 0 );
					Mat query_points_transformed; 
					perspectiveTransform(Mat(query_pts), query_points_transformed, H12);
					for( size_t i1 = 0; i1 < query_pts.size(); i1++ ) {
						if( norm(train_pts[i1] - query_points_transformed.at<Point2f>((int)i1,0)) < 4 ) // inlier
							matchesMask[i1] = 1;
					}
					drawMatches( train, train_kpts, queryColor, query_kpts, matches, correspondenceImg, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), matchesMask, DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
					
				}
				cvMoveWindow("correspondences", 0, 490);
				imshow("correspondences", correspondenceImg);
				break;
				
				
			default:
				break;
		}
		
		imshow("query", queryColor);

		
		int key = cvWaitKey(1000/60);
		if(key != -1)
		{
			switch(key) {
				case 27:	// escape
					quit = true;
					break;
				case ' ':
					mode++;
					if(mode > MODE_MAX) 
						mode = MODE_FIRST;
					break;
				default:
					cout << key << endl;
					break;
			}
		}
	}


    return 0;
}
