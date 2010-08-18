#include <highgui.h>
#include <cvaux.h>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>


using namespace cv;
using namespace std;

const string winName = "correspondences";


#define DRAW_RICH_KEYPOINTS_MODE     0
#define DRAW_OUTLIERS_MODE           0

//void warpPerspectiveRand( const Mat& src, Mat& dst, Mat& H, RNG& rng );
void doIteration( const Mat& img1, Mat& img2, Mat& lastframe,
				 vector<KeyPoint>& keypoints1, const Mat& descriptors1,
				 Ptr<FeatureDetector>& detector, Ptr<DescriptorExtractor>& descriptorExtractor,
				 Ptr<DescriptorMatcher>& descriptorMatcher,
				 double ransacReprojThreshold);

// Detector Types: FAST, STAR, SIFT, SURF, MSER, GFTT, HARRIS
// Descriptor Types: SIFT, SURF
// DescriptorMatcher Types: BruteForce, BruteForce-L1


int main(int argc, char * const argv[]) {

	
	char* detector_name = argv[1];
	char* descriptor_extractor_name = argv[2];
	char* descriptor_matcher_name = argv[3];
	char* video_name = argv[4];
	char* template_name = argv[5];
	
	cout << "< Creating detector, descriptor extractor and descriptor matcher ..." << endl;
    Ptr<FeatureDetector> detector = createDetector( detector_name );
    Ptr<DescriptorExtractor> descriptorExtractor = createDescriptorExtractor( descriptor_extractor_name );
    Ptr<DescriptorMatcher> descriptorMatcher = createDescriptorMatcher( descriptor_matcher_name );
    if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
    {
        cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;
        return -1;
	}
	
	cout << "< Reading the template..." << endl;
    Mat img1 = imread( template_name, CV_LOAD_IMAGE_GRAYSCALE);
	if( img1.empty() )
    {
        cout << "Can not read template image" << endl;
        return -1;
    }
	
	cout << "< Opening video..." << endl;
	VideoCapture cap(video_name); // open the default camera
    if(!cap.isOpened())  
	{
		 cout << "Can not open video file." << endl;
        return -1;
	}
	
	vector<KeyPoint> keypoints1;
    detector->detect( img1, keypoints1 );
    cout << keypoints1.size() << " points" << endl << ">" << endl;

	
    cout << "< Computing descriptors for keypoints from first image..." << endl;
    Mat descriptors1;
    descriptorExtractor->compute( img1, keypoints1, descriptors1 );
	
	
	// If srcPoints and dstPoints are measured in pixels, 
	// it usually makes sense to set this parameter somewhere in the range 1 to 10.
	int ransacReprojThreshold = 1;

	
	Mat frame;
	Mat bwframe;
	Mat lastframe;
	
    for(int i=0; 1; i++)
    {
        cap >> frame; // get a new frame from camera
		cvtColor(frame, bwframe, CV_BGR2GRAY);
		
		
		if(i==0)
		{
			lastframe = bwframe;
		}
		
		
		doIteration( img1, bwframe, lastframe, keypoints1, descriptors1,
					detector, descriptorExtractor, descriptorMatcher,
					ransacReprojThreshold );
		
		/*
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		imshow("edges", frame);
		*/

		lastframe = bwframe.clone();
        if(waitKey(30) >= 0) break;
    }
	
	
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

void doIteration( const Mat& img1, Mat& img2, Mat& lastframe,
				 vector<KeyPoint>& keypoints1, 
				 const Mat& descriptors1,
				 Ptr<FeatureDetector>& detector, 
				 Ptr<DescriptorExtractor>& descriptorExtractor,
				 Ptr<DescriptorMatcher>& descriptorMatcher,
				 double ransacReprojThreshold )
{
    assert( !img1.empty() && !img2.empty() );
   

    cout << "< Extracting keypoints from second image..." << endl;
    vector<KeyPoint> keypoints2;
    detector->detect( img2, keypoints2 );
    cout << keypoints2.size() << " points" << endl << ">" << endl;
	
    cout << "< Computing descriptors for keypoints from second image..." << endl;
    Mat descriptors2;
    descriptorExtractor->compute( img2, keypoints2, descriptors2 );

    cout << "< Matching descriptors..." << endl;
    vector<int> matches;
    descriptorMatcher->clear();
    descriptorMatcher->add( descriptors2 );
    descriptorMatcher->match( descriptors1, matches );
	
	
	 Mat H12;
	
    if(ransacReprojThreshold >= 0 )
    {
        cout << "< Computing homography (RANSAC)..." << endl;
        vector<Point2f> points1; KeyPoint::convert(keypoints1, points1);
        vector<Point2f> points2; KeyPoint::convert(keypoints2, points2, matches);
        H12 = findHomography( Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold );
    }
	
	
    Mat drawImg;
	
    if( !H12.empty() ) // filter outliers
    {
		cout << "< Homography found..." << endl;
		
        vector<char> matchesMask( matches.size(), 0 );
        vector<Point2f> points1; KeyPoint::convert(keypoints1, points1);
        vector<Point2f> points2; KeyPoint::convert(keypoints2, points2, matches);
        Mat points1t; perspectiveTransform(Mat(points1), points1t, H12);
        vector<int>::const_iterator mit = matches.begin();
        for( size_t i1 = 0; i1 < points1.size(); i1++ )
        {
            if( norm(points2[i1] - points1t.at<Point2f>(i1,0)) < 4 ) // inlier
                matchesMask[i1] = 1;
        }
        // draw inliers
        drawMatches( img1, keypoints1, img2, keypoints2, matches, drawImg, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), matchesMask
#if DRAW_RICH_KEYPOINTS_MODE
					, DrawMatchesFlags::DRAW_RICH_KEYPOINTS
#endif
					);
		
#if DRAW_OUTLIERS_MODE
        // draw outliers
        for( size_t i1 = 0; i1 < matchesMask.size(); i1++ )
            matchesMask[i1] = !matchesMask[i1];
		
        drawMatches( img1, keypoints1, img2, keypoints2, matches, drawImg, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), matchesMask,
					DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
#endif
    }
    else
        drawMatches( img1, keypoints1, img2, keypoints2, matches, drawImg );
	
	
	imshow( winName, drawImg );
	
	
	
	// Jeff's Optical Flow Stuff
	
	Mat flow;
	cvtColor(img2, flow, CV_GRAY2RGB);
	
	vector<Point2f> prevPts;
	vector<Point2f> nextPts;
	for(int i=0; i<keypoints2.size(); i++) {
		prevPts.push_back( keypoints2[i].pt );
	}
	
	vector<uchar> status;
	vector<float> err;
	
	calcOpticalFlowPyrLK(lastframe, img2, prevPts, nextPts, status, err);

	for(int i=0; i<prevPts.size(); i++) {
		circle(flow, prevPts[i], 4, CV_RGB(255, 0, 0));
		line(flow, prevPts[i], nextPts[i],  CV_RGB(200, 0, 255));
	}
    
	imshow( "optflow", flow );
}
