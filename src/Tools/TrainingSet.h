/*
 *  TrainingSet.h
 *  logomunge
 *
 *  Created by Jeffrey Crouse on 4/6/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include "TrainingImage.h"


using namespace std;
using namespace cv;
namespace fs = boost::filesystem;


class TrainingSet  {
public:
	
	TrainingSet(){}
	
	void loadFromArchive(string path);
	bool loadFromDirectory(string _path);
	void save();
	void match(Image img);
	void setFeatureDetector(FeatureDetector* _detector);
	void setDescriptorExtractor(DescriptorExtractor* _extractor);

	
protected:
	
	// ----------------------------------------------------
	static bool is_image(fs::path p)
	{
		if(!is_regular_file(p))
		{
			return false;
		}
		string ext = extension(p);
		boost::to_lower(ext);
		const char *image_exts[] = {".jpg", ".jpeg", ".png", ".tiff", ".tif"};
		for(int i=0; i<5; i++)
		{
			if(boost::starts_with(ext, image_exts[i]))	
			{
				return true;
			}
		}
		return false;
	}
	
	fs::path dir;
	Ptr<FeatureDetector> detector;			// Use generic container for detector so we can use different types
	Ptr<DescriptorExtractor> extractor;		// same for extractor
	vector<TrainingImage> images;
};

