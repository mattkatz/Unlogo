#include <highgui.h>
#include <cvaux.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>


#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"


using namespace cv;
using namespace std;
using namespace boost;

string detector_type;
string descriptor_extractor_type;
string descriptor_matcher_type;
int ransacReprojThreshold = 2;

Ptr<FeatureDetector> detector;
Ptr<DescriptorExtractor> descriptorExtractor;
Ptr<DescriptorMatcher> descriptorMatcher;


typedef struct {
	Mat img;
	vector<KeyPoint> keypoints;	
	Mat descriptors;
	vector<Point2f> points;
} Logo;

vector<Logo> logos;

extern "C" int init( const char* argstr )
{
	try {
		vector<string> args;
		split(args, argstr, is_any_of(":"));
		if(args.size() < 4) {
			cout << "[detectorType] [descriptorType] [matcherType] [video] [logo(s)]" << endl;
			return -1;
		}

		detector_type = args[0];
		descriptor_extractor_type = args[1];
		descriptor_matcher_type = args[2];

		
		cout << "< Creating detector, descriptor extractor and descriptor matcher ..." << endl;
		detector = createDetector( detector_type );
		descriptorExtractor = createDescriptorExtractor( descriptor_extractor_type );
		descriptorMatcher = createDescriptorMatcher( descriptor_matcher_type );
		if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
		{
			cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;
			return -1;
		}
	
		for(int i=3; i<(int)args.size(); i++)
		{
			Logo logo;
			logo.img = imread( args[i], CV_LOAD_IMAGE_GRAYSCALE);
			if( logo.img.empty() )
			{
				cout << "Can not read template image " << args[i] << endl;
				return -1;
			}
			detector->detect( logo.img, logo.keypoints );
    		cout << args[i] << ": " << logo.keypoints.size() << " keypoints" << endl;
	
    		if(logo.keypoints.size() > 0)
    		{
    			if(ransacReprojThreshold >= 0 )
    				KeyPoint::convert(logo.keypoints, logo.points);
    			descriptorExtractor->compute( logo.img, logo.keypoints, logo.descriptors );
				logos.push_back( logo );
			}
		}
			
		cout << "-----------------" << endl;
		cout << "Detector Type: " << detector_type << endl;
		cout << "Extractor Type: " << descriptor_extractor_type << endl;
		cout << "Matcher Type: " << descriptor_matcher_type << endl;
		cout << "Num Logos: " << logos.size() << endl;
		cout << "-----------------" << endl;

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
	

    cout << "< Extracting keypoints from second image..." << endl;
    vector<KeyPoint> keypoints2;
    detector->detect( bw_image, keypoints2 );
    cout << keypoints2.size() << " points" << endl << ">" << endl;
	
    cout << "< Computing descriptors for keypoints from second image..." << endl;
    Mat descriptors2;
    descriptorExtractor->compute( bw_image, keypoints2, descriptors2 );

    cout << "< Matching descriptors..." << endl;
    vector<int> matches;
    descriptorMatcher->clear();
    descriptorMatcher->add( descriptors2 );
    descriptorMatcher->match( logos[0].descriptors, matches );
	
	
	Mat H12;
	vector<Point2f> points2;

    if(ransacReprojThreshold >= 0 )
    {
        cout << "< Computing homography (RANSAC)..." << endl;
        KeyPoint::convert(keypoints2, points2, matches);
        H12 = findHomography( Mat(logos[0].points), Mat(points2), CV_RANSAC, ransacReprojThreshold );
    }
	
    if( !H12.empty() ) // filter outliers
    {
		cout << "< Homography found..." << endl;
		
        vector<char> matchesMask( matches.size(), 0 );
        Mat points1t; perspectiveTransform(Mat(logos[0].points), points1t, H12);
        vector<int>::const_iterator mit = matches.begin();
        for( size_t i1 = 0; i1 < logos[0].points.size(); i1++ )
        {
            if( norm(points2[i1] - points1t.at<Point2f>(i1,0)) < 4 ) // inlier
                matchesMask[i1] = 1;
        }
        
        for(size_t i=0; i<matches.size(); i++) {
			if(matchesMask[i]>0) {
				int match = matches[i];
				Point2f p = points2[match];
				circle(img, p, 4, CV_RGB(0, 255, 0), 1);
			}
		}

    }

	return 0;
}