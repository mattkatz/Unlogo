/*
 *  TrainingSet.cpp
 *  logomunge
 *
 *  Created by Jeffrey Crouse on 4/6/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "TrainingSet.h"


// ----------------------------------------------
// Load TrainingImages from a previoiusly saved archive
void TrainingSet::loadFromArchive(string path)
{
	images.clear();
	
	FileStorage fs;
	fs.open(path.c_str(), FileStorage::READ);
}



// ----------------------------------------------
// Load a bunch of images from a directory and make TrainingImages out of them
bool TrainingSet::loadFromDirectory(string _path)
{
	// TO DO:  check if detector and extractor have been set
	
	images.clear();

	dir = fs::path(_path);
	
	if( !fs::exists(dir) || !fs::is_directory(dir) )
	{
		cout << dir << " is not a directory." << endl;
		return false;
	}
	
	// Loop through the contents of the directory and assemble SURF points of any images found.
	cout << "Opening " << dir << endl;
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_itr( dir ); dir_itr != end_iter;  ++dir_itr )
	{
		cout << "encountered a file: " << dir_itr->path() << endl;
	
		if( is_image(dir_itr->path()) )
		{
			cout << "found an image " << dir_itr->path() << endl;
			
			TrainingImage img;
			img.open(dir_itr->path().string());
			img.generate_cnrfile();
			
			if(fs::exists(img.cnrfile))
			{
				// TO DO: Read in and use the corners from the file!
				// TO DO: pre-process the image! (Soebel, grayscale, etc)
				
				// examine the image.
				cout << "Identifying keypoints" << endl;
				detector->detect(img.cvImg, img.keypoints);
				
				cout << "Extracting descriptors " << endl;
				extractor->compute(img.cvImg, img.keypoints, img.descriptors);
				
				images.push_back( img );
			}
		}
	} // end for(all files in directory)
	
	return true;
}


// ----------------------------------------------
void TrainingSet::save()
{
	// If we found any images, make a datafile.
	if(images.size() > 0)
	{
		// Construct the path to where the data will be stored
		fs::path datafile(dir.string()+".xml.gz");

		cout << "saving data to " << datafile << endl;
		
		// Open up the file so we can write to it.
		FileStorage fs;
		fs.open(datafile.string(), FileStorage::WRITE);
		
		if(fs.isOpened())
		{
			fs << "total" << (int)images.size();
			fs << "logos" << "{";
			
			char nodename[255];
			for(int i=0; i<images.size(); i++)
			{
				fs << "logo" << "{";
				sprintf(nodename, "image_%d", i);
				fs << "filename" << nodename;
				write(fs, "keypoints", images[i].keypoints);
				fs << "descriptors" << images[i].descriptors;
				fs << "}";
			}
			
			fs << "}";
			fs.release();
		}
	}
}


// ----------------------------------------------
void TrainingSet::match(Image img)
{
	for(int i=0; i<images.size(); i++)
	{
		images[i].match( img );
		
		// Did this match? 
	}
}

// ----------------------------------------------
void TrainingSet::setFeatureDetector(FeatureDetector* _detector)
{
	detector = _detector;
	detector.addref();
}

// ----------------------------------------------
void TrainingSet::setDescriptorExtractor(DescriptorExtractor* _extractor)
{
	extractor = _extractor;
	extractor.addref();
}
