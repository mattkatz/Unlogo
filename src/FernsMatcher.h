/*
 *  FernsMatcher.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/5/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "Image.h"
using namespace cv;

class FernsMatcher {
public:
	FernsMatcher() : bTrainSet(false), bInited(false), patchSize(32,32), 
		ldetector(7, 20, 2, 2000, patchSize.width, 2), 
		gen(0,256,5,true,0.8,1.2,-CV_PI/2,CV_PI/2,-CV_PI/2,CV_PI/2) {
		
		ldetector.setVerbose(true);
		blurKSize = 3;
		sigma = 0;
		maxPoints=100;
	}
	
	// TO DO -- make this take arguments so it can actually be used as an initializer!
	bool init() {
		Size patchSize(32, 32);
		int radius = 7;
		int threshold = 20;
		int nOctaves=2;
		int nViews=2000;
		int clusteringDistance = 2;
		ldetector = LDetector(radius, threshold, nOctaves, nViews, patchSize.width, clusteringDistance);
		ldetector.setVerbose(true);
		
		double backgroundMin=0;
		double backgroundMax=256;
		double noiseRange=5;
		bool randomBlur=true;
		double lambdaMin=0.8;
		double lambdaMax=1.2;
		double thetaMin=-CV_PI/2;
		double thetaMax=CV_PI/2;
		double phiMin=-CV_PI/2;
		double phiMax=CV_PI/2;
		gen = PatchGenerator(backgroundMin, backgroundMax, noiseRange, randomBlur, 
							 lambdaMin, lambdaMax, thetaMin, thetaMax, phiMin, phiMax);
		maxPoints=100;
		
		bInited=true;
		return true;
	}
	
	
	bool setTrainImage(Image& train, string object_filename) {
		
		object = train.cvImg.clone();
		
		GaussianBlur(object, object, Size(blurKSize, blurKSize), sigma, sigma);
		buildPyramid(object, objpyr, ldetector.nOctaves-1);
		
		
		string model_filename = format("%s_model.xml.gz", object_filename.c_str());
		printf("Trying to load %s ...\n", model_filename.c_str());
		FileStorage fs(model_filename, FileStorage::READ);
		if( fs.isOpened() )
		{
			detector.read(fs.getFirstTopLevelNode());
			printf("Successfully loaded %s.\n", model_filename.c_str());
		}
		else
		{
			printf("The file not found and can not be read. Let's train the model.\n");
			printf("Step 1. Finding the robust keypoints ...\n");
			ldetector.setVerbose(true);
			ldetector.getMostStable2D(object, objKeypoints, 100, gen);
			printf("Done.\nStep 2. Training ferns-based planar object detector ...\n");
			detector.setVerbose(true);
			
			detector.train(objpyr, objKeypoints, patchSize.width, 100, 11, 10000, ldetector, gen);
			printf("Done.\nStep 3. Saving the model to %s ...\n", model_filename.c_str());
			if( fs.open(model_filename, FileStorage::WRITE) )
				detector.write(fs, "ferns_model");
		}
		fs.release();
		
		return true;
	}
	

	void doQuery(Image& query, bool showCorrespondence=false) {
	
		image = query.cvImg.clone();
		
		GaussianBlur(image, image, Size(blurKSize, blurKSize), sigma, sigma);
		buildPyramid(image, imgpyr, ldetector.nOctaves-1);
		
		
		vector<Point2f> dst_corners;
		Mat correspond( object.rows + image.rows, std::max(object.cols, image.cols), CV_8UC3);
		correspond = Scalar(0.);
		Mat part(correspond, Rect(0, 0, object.cols, object.rows));
		cvtColor(object, part, CV_GRAY2BGR);
		part = Mat(correspond, Rect(0, object.rows, image.cols, image.rows));
		cvtColor(image, part, CV_GRAY2BGR);
		
		
		double t = (double)getTickCount();
		objKeypoints = detector.getModelPoints();
		ldetector(imgpyr, imgKeypoints, 300);
		
		std::cout << "Object keypoints: " << objKeypoints.size() << "\n";
		std::cout << "Image keypoints: " << imgKeypoints.size() << "\n";
		bool found = detector(imgpyr, imgKeypoints, H, dst_corners, &pairs);
		t = (double)getTickCount() - t;
		printf("%gms\n", t*1000/getTickFrequency());
		
		int i;
		if( found )
		{
			for( i = 0; i < 4; i++ )
			{
				Point r1 = dst_corners[i%4];
				Point r2 = dst_corners[(i+1)%4];
				line( correspond, Point(r1.x, r1.y+object.rows),
					 Point(r2.x, r2.y+object.rows), Scalar(0,0,255) );
			}
		}
		
		for( i = 0; i < (int)pairs.size(); i += 2 )
		{
			line( correspond, objKeypoints[pairs[i]].pt,
				 imgKeypoints[pairs[i+1]].pt + Point2f(0,object.rows),
				 Scalar(0,255,0) );
		}
		
		if(showCorrespondence)
		{
			imshow( "Ferns Correspondence", correspond );
		}
		
		return;		
	}
	
	void drawTrainKeypointsIntoImage(Image& img)
	{
		Mat objectColor = img.cvImg;
		cvtColor(object, objectColor, CV_GRAY2BGR);
		for(int i = 0; i < (int)objKeypoints.size(); i++ )
		{
			circle( objectColor, objKeypoints[i].pt, 2, Scalar(0,0,255), -1 );
			circle( objectColor, objKeypoints[i].pt, (1 << objKeypoints[i].octave)*15, Scalar(0,255,0), 1 );
		}
	}
	
	void drawQueryKeypointsIntoImage(Image& img)
	{
		Mat imageColor = img.cvImg;
		cvtColor(image, imageColor, CV_GRAY2BGR);
		for(int i = 0; i < (int)imgKeypoints.size(); i++ )
		{
			circle( imageColor, imgKeypoints[i].pt, 2, Scalar(0,0,255), -1 );
			circle( imageColor, imgKeypoints[i].pt, (1 << imgKeypoints[i].octave)*15, Scalar(0,255,0), 1 );
		}
	}
	
protected:
	
	bool bInited;
	bool bTrainSet;
	Mat H;
	
	LDetector ldetector;
    PlanarObjectDetector detector;
	PatchGenerator gen;
	Size patchSize;
	int maxPoints;
	vector<int> pairs;
	int blurKSize;
	double sigma;
	Mat object, image;
	vector<Mat> objpyr, imgpyr;
	vector<KeyPoint> objKeypoints, imgKeypoints;
};