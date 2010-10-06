/*
 *  Image.cpp
 *  framehack
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "Image.h"

namespace fh {
	
#pragma mark CONSTRUCTORS
	
	//--------------------------------------------------
	Image::Image()
	{
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
	}
	
	
	//--------------------------------------------------
	Image::Image(int width, int height, uint8_t* data, int stride)
	{
		setData(width, height, data, stride);
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
	}
	
	//--------------------------------------------------
	/*
	Image::Image(const Image& other)
	{
		copyFromImage( other );
	}
	*/
	
#pragma mark ASSIGNMENT

	//--------------------------------------------------
	void Image::copyFromImage( const Image &other )
	{
		if(cvImage.channels()!=other.cvImage.channels() )
		{
			log(LOG_LEVEL_ERROR, "in copyFromImage(), WARNING: channels are different. Mat being reallocated.");
			log(LOG_LEVEL_ERROR, "in copyFromImage(), this.channels=%d other.channels=%d", cvImage.channels(), other.cvImage.channels());
		}
		
		if(cvImage.size()!=other.cvImage.size())
		{
			Size s = cvImage.size();
			log(LOG_LEVEL_ERROR, "in copyFromImage(), WARNING: size are different. Mat may be reallocated.");
			log(LOG_LEVEL_ERROR, "in copyFromImage(), this.size=%dx%d other.size=%dx%d", 
				s.width, s.height, other.cvImage.size().width, other.cvImage.size().height);
		}
		
		other.cvImage.copyTo( cvImage );
		features = other.features;	
		other.descriptors.copyTo( descriptors );
		descriptorsCurrent = other.descriptorsCurrent;
		featuresCurrent = other.featuresCurrent;
	}
	
	//--------------------------------------------------
	void Image::setData(int width, int height, uint8_t* data, int stride)
	{
		
		cvImage = Mat(Size(width, height), CV_8UC3, data, stride);
		//cvImage = Mat(width, height, CV_8UC3, data, stride);
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
	}
	
	//--------------------------------------------------
	void Image::operator = ( const Image& other )
	{
		cvImage = other.cvImage;
		descriptorsCurrent=false;
		featuresCurrent=false;
		matcherTrained=false;
	}

#pragma mark MATCHING
	
	//--------------------------------------------------
	Mat Image::findDescriptors()
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
	Mat Image::findDescriptors(string alg_name)
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
	vector<KeyPoint> Image::findFeatures()
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
	// As of writing this, you can use:
	// FAST, STAR, SIFT, SURF, MSER, GFTT, HARRIS, L
	vector<KeyPoint> Image::findFeatures(string alg_name)
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
			Ptr<FeatureDetector> detector = createDetector( alg_name );
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
	void Image::trainMatcher()
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
	void Image::trainMatcher(string alg_name)
	{
		if(matcherTrained && matchAlgUsed.compare(alg_name)==0) return;
		
		log(LOG_LEVEL_DEBUG, "Training matcher %s", alg_name.c_str());
		
		// These don't require descriptors.  It's all rolled up in the matcher.
		if(!alg_name.compare("FERN")||!alg_name.compare("ONEWAY")||!alg_name.compare("CALONDER"))
		{
			findFeatures();
			genericDescriptorMatch = createGenericDescriptorMatch(alg_name, "fern_params.xml");

			log(LOG_LEVEL_DEBUG, "training matcher");
			Mat img = bw();
			genericDescriptorMatch->add( img, features );
			
		}
		
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
	void Image::matchTo(Image &b, vector<int>& featureMatchesAtoB)
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
	
	vector<Mat> Image::pyramid(int maxLevel) 
	{
		vector<Mat> imagePyr;
		buildPyramid(cvImage, imagePyr, maxLevel);
		return imagePyr;
	}
	
	//--------------------------------------------------
	Mat Image::bw()
	{
		if(cvImage.channels()==1) return cvImage;
		else {
			Mat bwImage;
			cvtColor(cvImage, bwImage, CV_RGB2GRAY);
			return bwImage;
		}
	}
	
	//--------------------------------------------------
	void Image::convert(int code) 
	{
		Mat tmp;
		cvtColor(cvImage, tmp, code);
		cvImage = tmp;	
	}
	
	//--------------------------------------------------
	int Image::open( const char* path )
	{
		cvImage = imread( path );
		if( cvImage.empty() ) {
			log(LOG_LEVEL_ERROR, "in open() Can not read %s", path);
			return -1;
		}
		descriptorsCurrent=false;
		featuresCurrent=false;

		return 0;
	}
	
	
	//--------------------------------------------------
	void Image::equalizeHist() 
	{
		cv::equalizeHist(cvImage, cvImage);
	}
	
	//--------------------------------------------------
	// TO DO:  this only works with 4-channel images
	// This should be fixed at some point.
	void Image::drawIntoMe( Image &other, Point2f loc )
	{
		if(loc.x > width() || loc.y > height()) return;
		
		// Get intersection of 2 images
		Rect intersection = Rect(loc, other.size()) & Rect(Point(0,0), size());
								   
		// Get the sub-section of each image that can be written to.
		Mat fg = other.cvImage(Rect(abs(intersection.x - loc.x), abs(intersection.y - loc.y), intersection.width, intersection.height));
		Mat bg = cvImage(intersection);
		
		
		// This should be put into Image::drawIntoMe()
		for( int i = 0; i < intersection.height; i++ )
		{
			uchar* ptr_bg = bg.ptr<uchar>(i);
			const uchar* ptr_fg = fg.ptr<uchar>(i);
			
			for( int j = 0; j < intersection.width * fg.channels(); j += fg.channels() )
			{
				float alpha	= ptr_fg[j+3] / (float)numeric_limits<uchar>::max();
				float inv_alpha = 1.0-alpha;
				
				ptr_bg[j  ]	= saturate_cast<uchar>((ptr_bg[j  ]*inv_alpha) + (ptr_fg[j  ]*alpha));
				ptr_bg[j+1]	= saturate_cast<uchar>((ptr_bg[j+1]*inv_alpha) + (ptr_fg[j+1]*alpha));
				ptr_bg[j+2]	= saturate_cast<uchar>((ptr_bg[j+2]*inv_alpha) + (ptr_fg[j+2]*alpha));
			}
		}
	}
	

	//--------------------------------------------------
	void Image::text(const char* text, int x, int y, double scale, Scalar color )
	{
		int thickness=1;
		int lineType=8;
		putText(cvImage, text, Point(x,y), FONT_HERSHEY_SIMPLEX, scale, color, thickness, lineType);
	}
	
	//--------------------------------------------------
	void Image::show(const char* win_name)
	{
		imshow( win_name, cvImage );	
	}

	
#pragma mark CV_IMAGE_ACCESSOR_METHODS
	
	//--------------------------------------------------
	bool Image::empty()
	{
		return cvImage.empty();
	}
	
	//--------------------------------------------------
	Size Image::size()
	{
		return cvImage.size();
	}
	
	//--------------------------------------------------
	int Image::channels()
	{
		return cvImage.channels();
	}
	
	int Image::width()
	{
		return cvImage.size().width;
	}
	
	int Image::height() 
	{
		return cvImage.size().height;
	}
}