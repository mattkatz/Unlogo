/*
 *  Matcher.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/22/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "Matcher.h"

namespace unlogo{
	
	// Global static pointer used to ensure a single instance of the class.
	Matcher* Matcher::m_pInstance = NULL;  
	
	/** This function is called to create an instance of the class. 
	 Calling the constructor publicly is not allowed. The constructor 
	 is private and is only called by this Instance function.
	 */
	
	Matcher* Matcher::Instance(string detector_type, string descriptor_extractor_type, string descriptor_matcher_type)
	{
		if (!m_pInstance)   // Only allow one instance of class to be generated.
			m_pInstance = new Matcher(detector_type, descriptor_extractor_type, descriptor_matcher_type);
		
		return m_pInstance;
	}
	
	Matcher* Matcher::Instance()
	{
		if (!m_pInstance){
			return Instance("SURF", "SURF", "BruteForce");
		}
		return m_pInstance;
	}
	
	
	Matcher::Matcher(string detector_type, string descriptor_extractor_type, string descriptor_matcher_type)
	{ 
		// Construct the detector, extractor, and matcher.
		detector = createDetector( detector_type );
		descriptorExtractor = createDescriptorExtractor( descriptor_extractor_type );
		descriptorMatcher = createDescriptorMatcher( descriptor_matcher_type );
		if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
		{
			log(LOG_LEVEL_ERROR, "Can not create detector or descriptor extractor or descriptor matcher of given types");
			return;
		}
		
		log(LOG_LEVEL_DEBUG, "Detector Type: %s", detector_type.c_str());
		log(LOG_LEVEL_DEBUG, "Extractor Type: %s", descriptor_extractor_type.c_str());
		log(LOG_LEVEL_DEBUG, "Matcher Type: %s", descriptor_matcher_type.c_str());
	}
	
}