/*
 *  MatchableImage.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 10/6/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "MatchableImage.h"

namespace fh {

	MatchableImage::MatchableImage()
	{
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
	}

	MatchableImage::MatchableImage(int width, int height, uint8_t* data, int channels) : 
	Image(width, height, data, channels)
	{
		
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
	}

#pragma mark UTILITY_METHODS
	
	//--------------------------------------------------
	int MatchableImage::open( const char* path )
	{
		descriptorsCurrent=false;
		featuresCurrent=false;
		return Image::open(path);;
	}
	
	//--------------------------------------------------
	void MatchableImage::copyFromImage( const MatchableImage &other )
	{
		
		features = other.features;	
		other.descriptors.copyTo( descriptors );
		descriptorsCurrent = other.descriptorsCurrent;
		featuresCurrent = other.featuresCurrent;
		Image::copyFromImage( other );
	}
	
	//--------------------------------------------------
	void MatchableImage::setData(int width, int height, uint8_t* data, int stride)
	{
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
		return Image::setData(width, height, data, stride);
	}
	
	
#pragma mark MATCHING
	
	//--------------------------------------------------
	Mat MatchableImage::findDescriptors()
	{
		if(descriptorAlgUsed.empty())
		{
			log(LOG_LEVEL_DEBUG, "Warning: default findDescriptors() called and no previous algorithm set.  Using SURF!");
			return findDescriptors("SURF");
		}
		else
		{
			return findDescriptors(descriptorAlgUsed);
		}
		
	}
	
	//--------------------------------------------------
	// As of writing this, you can use:
	// SIFT, SURF
	Mat MatchableImage::findDescriptors(string alg_name)
	{
		if(descriptorsCurrent && descriptorAlgUsed.compare(alg_name)==0) return descriptors;
		if(!featuresCurrent) findFeatures();
		
		if(empty() || features.size()==0)
		{
			log(LOG_LEVEL_ERROR, "in findDescriptors(), image is empty or there are no features in image");
		}
		
		Mat cvImageGray;
		cvtColor(cvImage, cvImageGray, CV_RGB2GRAY);
		
		Ptr<DescriptorExtractor> descriptorExtractor = createDescriptorExtractor(alg_name);
		if(descriptorExtractor==0)
		{
			log(LOG_LEVEL_ERROR, "Detector not found!");
		}
		
		descriptorExtractor->compute( cvImageGray, features, descriptors );
		log(LOG_LEVEL_DEBUG, "in findDescriptors(), %d x %d descriptors", descriptors.rows, descriptors.cols);
		
		descriptorAlgUsed = alg_name;
		descriptorsCurrent=true;
		matcherTrained=false;
		return descriptors;
	}
	
	//--------------------------------------------------
	vector<KeyPoint> MatchableImage::findFeatures()
	{
		if(featureAlgUsed.empty())
		{
			log(LOG_LEVEL_DEBUG, "Warning: default findFeatures() called and no previous algorithm set.  Using SURF!");
			return findFeatures("SURF");
		}
		else
		{
			return findFeatures(featureAlgUsed);
		}
		
	}
	
	
	//--------------------------------------------------
	vector<KeyPoint> MatchableImage::findFeatures(string alg_name, Mat &bounds)
	{
		vector<KeyPoint> tempFeatures = findFeatures(alg_name);
		features.clear();
		for(int i=0; i<tempFeatures.size(); i++)
		{
			double inside = pointPolygonTest(bounds, features[i].pt, true);
			if(inside>=0)
			{
				features.push_back( tempFeatures[i] );
			}
			else
			{
				double dist = abs(inside);
				if(dist < (tempFeatures[i].size/2.))
				{
					features.push_back( tempFeatures[i] );
				}
			}
		}
		
		log(LOG_LEVEL_DEBUG, "in findFeatures(), after filtering by bounds, there are %d features", features.size());
		
#ifdef DEBUG		
		for(int i=0; i<4; i++)
		{
			float *p1 = bounds.ptr<float>(i%4);
			float *p2 = bounds.ptr<float>((i+1)%4);
			//line(cvImage, Point(p1[0], p1[1]), Point(p2[0],p2[1]), CV_RGB(255,255,255), 3);
		}
#endif
		
		featuresCurrent=true;
		return features;
	}
	
	
	//--------------------------------------------------
	// As of writing this, you can use:
	// FAST, STAR, SIFT, SURF, MSER, GFTT, HARRIS, L
	vector<KeyPoint> MatchableImage::findFeatures(string alg_name)
	{
		log(LOG_LEVEL_DEBUG, "finding features using %s", alg_name.c_str());
		
		
		if(featuresCurrent && featureAlgUsed.compare(alg_name)==0) return features;
		
		if(empty())
		{
			log(LOG_LEVEL_ERROR, "in findFeatures(), image is empty");
		}
		
		
		// "L" isn't in the "createDetector" thingie.
		// TO DO:  Figure out what these vars do & how to pass them in!
		// For that matter, how to pass in parameters to any of the other feature detectors?
		if(alg_name.compare("L")==0)
		{
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
			int maxPoints=100;
			ldetector.getMostStable2D(bw(), features, maxPoints, gen);
		}
		else 
		{
			Ptr<FeatureDetector> detector = createFeatureDetector( alg_name );
			if(detector==0)
			{
				log(LOG_LEVEL_ERROR, "Feature detector %s not found!", alg_name.c_str());
			}
			
			detector->detect( bw(), features );
		}
		
		log(LOG_LEVEL_DEBUG, "in findFeatures(), %d features", features.size());
		featuresCurrent = true;
		descriptorsCurrent = false;
		matcherTrained=false;
		featureAlgUsed=alg_name;
		return features;
	}
	
	//--------------------------------------------------
	void MatchableImage::trainMatcher()
	{
		if(matchAlgUsed.empty())
		{
			log(LOG_LEVEL_DEBUG, "Warning: default trainMatcher() called and no previous algorithm set.  Using BruteForce!");
			return trainMatcher("BruteForce");
		}
		else
		{
			return trainMatcher(matchAlgUsed);
		}
	}
	
	
	//--------------------------------------------------
	// As of writing this, you can use:
	// ONEWAY, FERN, BruteForce, BruteForce-L1, Planar
	void MatchableImage::trainMatcher(string alg_name)
	{
		if(matcherTrained && matchAlgUsed.compare(alg_name)==0) return;
		
		log(LOG_LEVEL_DEBUG, "Training matcher %s", alg_name.c_str());
		
		/*
		// These don't require descriptors.  It's all rolled up in the matcher.
		if(!alg_name.compare("FERN")||!alg_name.compare("ONEWAY")||!alg_name.compare("CALONDER"))
		{
			findFeatures();
			genericDescriptorMatch = createDescriptorMatcher(alg_name, "fern_params.xml");
			
			log(LOG_LEVEL_DEBUG, "training matcher");
			Mat img = bw();
			genericDescriptorMatch->add( img, features );
			
		}
		*/
		
		if(!alg_name.compare("BruteForce")||!alg_name.compare("BruteForce-L1"))
		{
			findDescriptors();
			descriptorMatcher = createDescriptorMatcher(alg_name.c_str());
			descriptorMatcher->clear();
			descriptorMatcher->add( descriptors ); 
		}
		
		if(!alg_name.compare("Planar"))
		{
			if(featureAlgUsed.compare("L")!=0)
			{
				log(LOG_LEVEL_DEBUG, "Planar detector works best with L detector.  Switching!");
				findFeatures("L");
			}
			
			Size patchSize(32, 32);
			planarDetector.setVerbose(true);
			planarDetector.train(pyramid(ldetector.nOctaves-1), features, patchSize.width, 100, 11, 10000, ldetector, gen);
			
		}
		matchAlgUsed = alg_name;
	}
	
	
	
	//--------------------------------------------------
	void MatchableImage::matchTo(MatchableImage &b, vector<int>& featureMatchesAtoB)
	{
		trainMatcher();
		
		log(LOG_LEVEL_DEBUG, "attempting to match using %s", matchAlgUsed.c_str());
		
		// These don't require descriptors.  It's all rolled up in the matcher.
		if(!matchAlgUsed.compare("FERN")||!matchAlgUsed.compare("ONEWAY")||!matchAlgUsed.compare("CALONDER"))
		{
			if(b.descriptorsCurrent)
			{
				log(LOG_LEVEL_DEBUG, "Helpful tip: You don't have to calculate descriptors in B if you are using %s", matchAlgUsed.c_str());
			}
			
			b.findFeatures();
			
			log(LOG_LEVEL_DEBUG, "performing match");
			genericDescriptorMatch->match( b.bw(), b.features, featureMatchesAtoB);
		}
		
		// These require descriptors
		if(!matchAlgUsed.compare("BruteForce")||!matchAlgUsed.compare("BruteForce-L1"))
		{
			b.findDescriptors();
			descriptorMatcher->match( b.descriptors, featureMatchesAtoB );
		}
		
		if(!matchAlgUsed.compare("Planar"))
		{
			if(b.featuresCurrent)
			{
				log(LOG_LEVEL_DEBUG, "Helpful Tip:  When using Planar matcher, don't findFeatures on image B.  It will be done again, wasting resources.");
			}
			if(b.descriptorsCurrent)
			{
				log(LOG_LEVEL_DEBUG, "Helpful tip: You don't have to calculate descriptors if you are using %s", matchAlgUsed.c_str());
			}
			
			vector<Mat> bpyr = b.pyramid(ldetector.nOctaves-1);
			ldetector(bpyr, b.features, 300);
			
			planarDetector(bpyr, b.features, H, dst_corners, &featureMatchesAtoB);
			
		}
	}
	
	//--------------------------------------------------
	vector<KeyPoint> MatchableImage::updateFeatures( MatchableImage& previous, Mat& H )
	{	
		
		vector<Point2f> prevPts; KeyPoint::convert(previous.features, prevPts);
		int nPrevPts = prevPts.size();
		vector<Point2f> nextPts;
		vector<uchar> status;
		vector<float> err;
		calcOpticalFlowPyrLK(previous.cvImage, cvImage, prevPts, nextPts, status, err);
		
		
		
		features.clear();
		for(int i=0; i<nPrevPts; i++)
		{
			if(status[i]>0)
			{
				KeyPoint feature = previous.features[i];
				feature.pt = nextPts[i];
				features.push_back( feature );
			}
		}
		int pctPtsLost=features.size() / (float)nPrevPts;
		
		
		if(nPrevPts>3)
		{
			Mat prevMat = points2mat(prevPts);
			Mat nextMat = points2mat(nextPts);
			
			H = findHomography(prevMat, nextMat, status, CV_RANSAC, 2);
			int ptsUsedInHomography=0;
			for(int i=0; i<status.size(); i++)
			{
				if(status[i]>0) ptsUsedInHomography++;
			}
			//log(LOG_LEVEL_DEBUG, "%f%% points used in homography", (ptsUsedInHomography/(float)npts)*100);
		}
		return features;
	}
	
	
	
}