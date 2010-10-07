/*
 *  MatchableImage.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 10/6/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */


#include <iostream>
#include <highgui.h>
#include <cvaux.h>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "fhUtils.h"
#include "Image.h"

namespace fh {

	class MatchableImage : public Image {
		
	public:
		MatchableImage();
		MatchableImage(int width, int height, uint8_t* data, int channels);
		
		vector<KeyPoint> findFeatures(string alg_name);
		vector<KeyPoint> findFeatures(string alg_name, Mat &bounds);
		vector<KeyPoint> findFeatures();
		vector<KeyPoint> updateFeatures( MatchableImage& previous, Mat& H ); // use optical flow to move features
		Mat findDescriptors(string alg_name);
		Mat findDescriptors();
		void trainMatcher(string alg_name);
		void trainMatcher();
		
		
		void matchTo(MatchableImage &b, vector<int>& featureMatchesAtoB);
		
		static void drawFeature( Mat& img, const KeyPoint& p, const Scalar& color, int flags );
		void drawFeatures();
		
		int open( const char* path );
		void copyFromImage( const MatchableImage &other );
		void setData(int width, int height, uint8_t* data, int stride);

		//protected:
		vector<KeyPoint> features;	
		Mat descriptors;
		
		bool featuresCurrent;
		bool descriptorsCurrent;
		bool matcherTrained;
		
		string featureAlgUsed;
		string descriptorAlgUsed;
		string matchAlgUsed;
		
		// Stuff used for matching
		// stuff used specifically for Planar matching
		LDetector ldetector;
		PatchGenerator gen;
		PlanarObjectDetector planarDetector;
		Mat H;
		vector<Point2f> dst_corners;
		// Other matchers
		Ptr<GenericDescriptorMatch> genericDescriptorMatch;
		Ptr<DescriptorMatcher> descriptorMatcher;
		
	};

}