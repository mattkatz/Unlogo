#include <highgui.h>
#include <cvaux.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/xml_parser.hpp>

#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"


using namespace cv;
using namespace std;
using namespace boost;


typedef struct Logo
{
	string search;
	string replace;
	Mat img;
	Mat replace_img;
	Scalar replace_color;
	vector<KeyPoint> keypoints;	
	Mat descriptors;
	vector<Point2f> points;
};

/*
struct job_settings
{
	string detector_type;
	string descriptor_extractor_type;
	string descriptor_matcher_type;
	int ransacReprojThreshold;
	std::set<std::set<string> > logos;
	
    std::string m_file;          // log filename
    int m_level;                 // debug level
    std::set<string> m_modules;  // modules where logging is enabled
    void load(const string &filename);
    void save(const string &filename);
};

void job_settings::load(const string &filename)
{
	using boost::property_tree::ptree;
	ptree pt;
	read_xml(filename, pt);
	m_file = pt.get<string>("debug.filename");
	m_level = pt.get("debug.level", 0);
	BOOST_FOREACH(ptree::value_type &v,  pt.get_child("debug.modules"))
	{
		m_modules.insert(v.second.data());
	}
}

// Saves the debug_settings structure to the specified XML file
void job_settings::save(const std::string &filename)
{
	using boost::property_tree::ptree;
	ptree pt;
	pt.put("debug.filename", m_file);
	pt.put("debug.level", m_level);
	BOOST_FOREACH(const std::string &name, m_modules)
	{
		pt.put("debug.modules.module", name);
	}
	write_xml(filename, pt);
}

*/


string detector_type;
string descriptor_extractor_type;
string descriptor_matcher_type;
int ransacReprojThreshold;

Ptr<FeatureDetector> detector;
Ptr<DescriptorExtractor> descriptorExtractor;
Ptr<DescriptorMatcher> descriptorMatcher;

vector<Logo> logos;
int framenum;


#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3

int ulog(int level, const char * format, ...) {
	string color;
	switch(level) {
		default:
		case LOG_LEVEL_DEBUG: 	color="\033[01;33m"; break;
		case LOG_LEVEL_WARNING:	color="\033[01;34m"; break;
		case LOG_LEVEL_ERROR:	color="\033[01;31m"; break;
	}
	va_list args;
	va_start( args, format);
	std::string fstr( color+string( "[unlogo] ") + format + "\n");
	int status =  vfprintf( stderr, fstr.c_str(), args);
	va_end(args);
	return status;
}

extern "C" int init( const char* argstr )
{
	try {
		
		framenum=0;
		
		// Parse arguments.
		vector<string> args;
		split(args, argstr, is_any_of(":"));
		
		if(args.size()<4 || args.size()%2!=1)
		{
			cout << "Usage" << endl;
			cout << "[detector]:[descriptor]:[matcher]:[search:replace]..." << endl;
			cout << "where [search:replace] is a list of images to look for and either image " << endl;
			cout << "or hex color to replace it with.  Hex colors must begin with # and be 6 digits" << endl;
			return -1;
		}

	
		detector_type = args[0];
		descriptor_extractor_type = args[1];
		descriptor_matcher_type = args[2];
		ransacReprojThreshold = 2;

		// Construct the detector, extractor, and matcher.
		detector = createDetector( detector_type );
		descriptorExtractor = createDescriptorExtractor( descriptor_extractor_type );
		descriptorMatcher = createDescriptorMatcher( descriptor_matcher_type );
		if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
		{
			ulog(LOG_LEVEL_ERROR, "Can not create detector or descriptor exstractor or descriptor matcher of given types");
			return -1;
		}
	
		// Construct all of the Logo objects.
		for(size_t i=3; i<args.size(); i+=2)
		{
			Logo logo;
			logo.search = args[i];
			logo.img = imread( logo.search, CV_LOAD_IMAGE_GRAYSCALE);
			if( logo.img.empty() )
			{
				ulog(LOG_LEVEL_ERROR, "Can not read template image: %s\n", logo.search.c_str());
				return -1;
			}
		
			detector->detect( logo.img, logo.keypoints );
	
    		if(logo.keypoints.size() > 0)
    		{
    			if(ransacReprojThreshold >= 0 )
    			{
    				KeyPoint::convert(logo.keypoints, logo.points);
    			}
    			descriptorExtractor->compute( logo.img, logo.keypoints, logo.descriptors );
    			
    			logo.replace = args[i+1];
    			if( starts_with(logo.replace, "#") && logo.replace.length() == 7)
    			{
    				char* p;
					int r = strtol( logo.replace.substr(1,2).c_str(), &p, 16 );
    				int g = strtol( logo.replace.substr(3,2).c_str(), &p, 16 );
    				int b = strtol( logo.replace.substr(5,2).c_str(), &p, 16 );
    				logo.replace_color = Scalar(r, g, b);
    			}
				else
				{
					logo.replace_img = imread(logo.replace, CV_LOAD_IMAGE_COLOR);
					if( logo.replace_img.empty() )
					{
						ulog(LOG_LEVEL_ERROR, "Can not read replacement image: %s",  logo.replace.c_str());
						return -1;
					}
				}
    			
				logos.push_back( logo );
			}
		}
		
		ulog(LOG_LEVEL_DEBUG, "-----------------");
		ulog(LOG_LEVEL_DEBUG, "Detector Type: %s", detector_type.c_str());
		ulog(LOG_LEVEL_DEBUG, "Extractor Type: %s", descriptor_extractor_type.c_str());
		ulog(LOG_LEVEL_DEBUG, "Matcher Type: %s", descriptor_matcher_type.c_str());
		ulog(LOG_LEVEL_DEBUG, "Num Logos: %d", logos.size());
		for(size_t i=0; i<logos.size(); i++)
		{
			ulog(LOG_LEVEL_DEBUG, "\t%s (%d keypoints) --> %s", logos[i].search.c_str(), 
				logos[i].keypoints.size(), logos[i].replace.c_str());
		}
		ulog(LOG_LEVEL_DEBUG, "-----------------");
	}
	catch ( ... ) {
		return -1;
	}
	
	return 0;
}


extern "C" int uninit()
{
	// delete stuff here
	return 0;
}




    
extern "C" int process( uint8_t* dst[4], int dst_stride[4],
                        uint8_t* src[4], int src_stride[4],
                        int width, int height )
{
	
	ulog(LOG_LEVEL_DEBUG, "FRAME %d", framenum);
	
	for ( int i(0); i<3; ++i )
	{
		uint8_t* pdst = dst[i];
		uint8_t* psrc = src[i];
		int w = width;
		int h = height;
		if ( i > 0 )
			w >>= 1, h >>= 1;
		
		if ( dst_stride[i] == src_stride[i] )
			memcpy( pdst, psrc, src_stride[i] * h );
		else 
		{
			for ( int y(0); y<h; ++y )
			{
				memcpy( pdst, psrc, w );
				pdst += dst_stride[i], psrc += src_stride[i];
			}
		}
	}         
  
  
	Mat img(width, height, CV_8UC3, dst[0], dst_stride[0]);
	Mat rgb_image; cvtColor(img, rgb_image, CV_YCrCb2RGB);
	Mat bw_image; cvtColor(rgb_image, bw_image, CV_RGB2GRAY);
	

    vector<KeyPoint> keypoints2;
    detector->detect( bw_image, keypoints2 );
	ulog(LOG_LEVEL_DEBUG, "%d keypoints in frame", keypoints2.size());
	
	
    ulog(LOG_LEVEL_DEBUG, "Computing descriptors for keypoints from frame...");
    Mat descriptors2;
    descriptorExtractor->compute( bw_image, keypoints2, descriptors2 );


	for(size_t i=0; i<logos.size(); i++)
	{
		ulog(LOG_LEVEL_DEBUG, "Matching descriptors for %s", logos[i].search.c_str());
		vector<int> matches;
		descriptorMatcher->clear();
		descriptorMatcher->add( descriptors2 );
		descriptorMatcher->match( logos[i].descriptors, matches );
		
		
		Mat H12;
		vector<Point2f> points2;
	
		if(ransacReprojThreshold >= 0 )
		{
			ulog(LOG_LEVEL_DEBUG, "Computing homography (RANSAC)");
			KeyPoint::convert(keypoints2, points2, matches);
			H12 = findHomography( Mat(logos[i].points), Mat(points2), CV_RANSAC, ransacReprojThreshold );
		}
		
		if( !H12.empty() ) // filter outliers
		{
			ulog(LOG_LEVEL_DEBUG, "Homography found...");
			
			vector<char> matchesMask( matches.size(), 0 );
			Mat points1t; perspectiveTransform(Mat(logos[i].points), points1t, H12);
			vector<int>::const_iterator mit = matches.begin();
			for( size_t i1 = 0; i1 < logos[i].points.size(); i1++ )
			{
				if( norm(points2[i1] - points1t.at<Point2f>(i1,0)) < 4 ) // inlier
					matchesMask[i1] = 1;
			}
			
			for(size_t i2=0; i2<matches.size(); i2++)
			{
				if(matchesMask[i2]>0)
				{
					int match = matches[i2];
					Point2f p = points2[match];
					circle(img, p, 4, CV_RGB(0, 255, 0), 1);
				}
			}
		}
	}
	
	framenum++;
	return 0;
}