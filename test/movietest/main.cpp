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


int main (int argc, char * const argv[]) {

	cout << "< Creating detector, descriptor extractor and descriptor matcher ..." << endl;
    Ptr<FeatureDetector> detector = createDetector( argv[1] );
    Ptr<DescriptorExtractor> descriptorExtractor = createDescriptorExtractor( argv[2] );
    Ptr<DescriptorMatcher> descriptorMatcher = createDescriptorMatcher( "BruteForce" );
    cout << ">" << endl;
    if( detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty()  )
    {
        cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;
        return -1;
	}
	
	cout << "< Reading the template..." << endl;
    Mat img1 = imread( argv[3], CV_LOAD_IMAGE_GRAYSCALE), img2;
	
	if( img1.empty() )
    {
        cout << "Can not read template image" << endl;
        return -1;
    }
	
	vector<KeyPoint> keypoints1;
    detector->detect( img1, keypoints1 );
    cout << keypoints1.size() << " points" << endl << ">" << endl;
	
    cout << "< Computing descriptors for keypoints from first image..." << endl;
    Mat descriptors1;
    descriptorExtractor->compute( img1, keypoints1, descriptors1 );
    cout << ">" << endl;
	
	// If srcPoints and dstPoints are measured in pixels, 
	// it usually makes sense to set this parameter somewhere in the range 1 to 10.
	int ransacReprojThreshold = 1;

	
	
	VideoCapture cap(argv[4]); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
	
	Mat frame;
	Mat bwframe;
	Mat lastframe;
	//RNG rng = theRNG();
	
    for(int i=0; 1; i++)
    {
        cap >> frame; // get a new frame from camera
		cvtColor(frame, bwframe, CV_BGR2GRAY);
		
		
		if(i==0) {
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


/*
void warpPerspectiveRand( const Mat& src, Mat& dst, Mat& H, RNG& rng )
{
    H.create(3, 3, CV_32FC1);
    H.at<float>(0,0) = rng.uniform( 0.8f, 1.2f);
    H.at<float>(0,1) = rng.uniform(-0.1f, 0.1f);
    H.at<float>(0,2) = rng.uniform(-0.1f, 0.1f)*src.cols;
    H.at<float>(1,0) = rng.uniform(-0.1f, 0.1f);
    H.at<float>(1,1) = rng.uniform( 0.8f, 1.2f);
    H.at<float>(1,2) = rng.uniform(-0.1f, 0.1f)*src.rows;
    H.at<float>(2,0) = rng.uniform( -1e-4f, 1e-4f);
    H.at<float>(2,1) = rng.uniform( -1e-4f, 1e-4f);
    H.at<float>(2,2) = rng.uniform( 0.8f, 1.2f);
	
    warpPerspective( src, dst, H, src.size() );
}
*/

void doIteration( const Mat& img1, Mat& img2, Mat& lastframe,
				 vector<KeyPoint>& keypoints1, const Mat& descriptors1,
				 Ptr<FeatureDetector>& detector, Ptr<DescriptorExtractor>& descriptorExtractor,
				 Ptr<DescriptorMatcher>& descriptorMatcher,
				 double ransacReprojThreshold )
{
    assert( !img1.empty() );
    Mat H12;

	assert( !img2.empty()/* && img2.cols==img1.cols && img2.rows==img1.rows*/ );
	
    cout << endl << "< Extracting keypoints from second image..." << endl;
    vector<KeyPoint> keypoints2;
    detector->detect( img2, keypoints2 );
    cout << keypoints2.size() << " points" << endl << ">" << endl;
	
    if( !H12.empty() )
    {
        cout << "< Evaluate feature detector..." << endl;
        float repeatability;
        int correspCount;
        evaluateFeatureDetector( img1, img2, H12, &keypoints1, &keypoints2, repeatability, correspCount );
        cout << "repeatability = " << repeatability << endl;
        cout << "correspCount = " << correspCount << endl;
        cout << ">" << endl;
    }
	
    cout << "< Computing descriptors for keypoints from second image..." << endl;
    Mat descriptors2;
    descriptorExtractor->compute( img2, keypoints2, descriptors2 );
    cout << ">" << endl;
	
    cout << "< Matching descriptors..." << endl;
    vector<int> matches;
    descriptorMatcher->clear();
    descriptorMatcher->add( descriptors2 );
    descriptorMatcher->match( descriptors1, matches );
    cout << ">" << endl;
	
    if( !H12.empty() )
    {
        cout << "< Evaluate descriptor match..." << endl;
        vector<Point2f> curve;
        Ptr<GenericDescriptorMatch> gdm = new VectorDescriptorMatch( descriptorExtractor, descriptorMatcher );
        evaluateDescriptorMatch( img1, img2, H12, keypoints1, keypoints2, 0, 0, curve, gdm );
        for( float l_p = 0; l_p < 1 - FLT_EPSILON; l_p+=0.1 )
            cout << "1-precision = " << l_p << "; recall = " << getRecall( curve, l_p ) << endl;
        cout << ">" << endl;
    }
	
    if(ransacReprojThreshold >= 0 )
    {
        cout << "< Computing homography (RANSAC)..." << endl;
        vector<Point2f> points1; KeyPoint::convert(keypoints1, points1);
        vector<Point2f> points2; KeyPoint::convert(keypoints2, points2, matches);
        H12 = findHomography( Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold );
        cout << ">" << endl;
    }
	
    Mat drawImg;
	
    if( !H12.empty() ) // filter outliers
    {
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
