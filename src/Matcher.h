/*
 *  Matcher.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef MATCHER_H
#define MATCHER_H

#include <string>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <highgui.h>
#include <cvaux.h>
#include "ulUtils.h"



using namespace std;
using namespace cv;

namespace unlogo {
	class Matcher {
	public:
		static Matcher* Instance(string detector_type, string descriptor_extractor_type, string descriptor_matcher_type);
		static Matcher* Instance();
		
		Ptr<FeatureDetector> detector;
		Ptr<DescriptorExtractor> descriptorExtractor;
		Ptr<DescriptorMatcher> descriptorMatcher;
		
	private:
		Matcher(string detector_type, string descriptor_extractor_type, string descriptor_matcher_type); // Private so that it can  not be called
		//Matcher(Matcher const&) {};					// copy constructor is private
		//Matcher& operator=(Matcher const&) {};		// assignment operator is private
		
		string detector_type;
		string descriptor_extractor_type;
		string descriptor_matcher_type;
		
		static Matcher* m_pInstance;

	};
}

#endif