#include <iostream>
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <stdio.h>
#include <ostream>

using namespace std;
using namespace cv;

int				g_slider_position = 0;
CvCapture*		g_capture         = NULL;
CvMemStorage*	g_storage  = NULL;
void			onTrackbarSlide(int pos);
IplImage*		doCanny(IplImage* in, double lowThresh, double highThresh, double aperture);


int main (int argc, char * const argv[]) {
	
	g_storage = cvCreateMemStorage(0); 
	
	const char* vid_win_name = "Video";
	const char* object_win_name = "Template";
	
	// Show the template
	const char* object_filename = argv[1];
	Mat object = imread( object_filename );
	cvNamedWindow( object_win_name, CV_WINDOW_AUTOSIZE );
	imshow(object_win_name, object );
	
	
	// Load the keypoint data for the template
	Size patchSize(32, 32);
    LDetector ldetector(7, 20, 2, 2000, patchSize.width, 2);
    ldetector.setVerbose(true);
    PlanarObjectDetector detector;
    
    vector<Mat> objpyr, imgpyr;
    int blurKSize = 3;
    double sigma = 0;
    GaussianBlur(object, object, Size(blurKSize, blurKSize), sigma, sigma);
    buildPyramid(object, objpyr, ldetector.nOctaves-1);

    
    vector<KeyPoint> objKeypoints, imgKeypoints;
	PatchGenerator gen(0,256,5,true,0.8,1.2,-CV_PI/2,CV_PI/2,-CV_PI/2,CV_PI/2);
    
    string model_filename = format("%s_model.xml.gz", object_filename);
    printf("Trying to load %s ...\n", model_filename.c_str());
    FileStorage fs(model_filename, FileStorage::READ);
    if( fs.isOpened() )
    {
        detector.read(fs.getFirstTopLevelNode());
        printf("Successfully loaded %s.\n", model_filename.c_str());
    }
    else
    {
        printf("The file not found and can not be read. Let's train the model.\n");
        printf("Step 1. Finding the robust keypoints ...\n");
        ldetector.setVerbose(true);
        ldetector.getMostStable2D(object, objKeypoints, 100, gen);
        printf("Done.\nStep 2. Training ferns-based planar object detector ...\n");
        detector.setVerbose(true);
		
        detector.train(objpyr, objKeypoints, patchSize.width, 100, 11, 10000, ldetector, gen);
        printf("Done.\nStep 3. Saving the model to %s ...\n", model_filename.c_str());
        if( fs.open(model_filename, FileStorage::WRITE) )
            detector.write(fs, "ferns_model");
    }
	printf("Now find the keypoints in the image, try recognize them and compute the homography matrix\n");
    fs.release();
	

	
	// Take care of loading the video
	cvNamedWindow( vid_win_name, CV_WINDOW_AUTOSIZE );
    g_capture = cvCreateFileCapture( argv[2] );
    
	
	int frames = (int) cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_COUNT);
    int tmpw = (int) cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_WIDTH);
    int tmph = (int) cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_HEIGHT);

    printf("opencv frames %d w %d h %d\n",frames,tmpw,tmph);

	cvCreateTrackbar("Position", vid_win_name, &g_slider_position, frames, onTrackbarSlide);
    Mat frame = cvQueryFrame( g_capture );
	Mat gray_frame, thresh_frame, contours_frame, canny_frame, last_frame, rgb_tmp, composite;

	CvSeq* contours = 0;

	
	VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
	
    Mat edges;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
	
	/*
	
	while(1)
	{
		if( !(frame = cvQueryFrame( g_capture )) ) break;
		
		// Make a gray version of the frame
		cvCvtColor(frame, gray_frame ,CV_BGR2GRAY);
		

		GaussianBlur(frame, frame, Size(blurKSize, blurKSize), sigma, sigma);
		buildPyramid(frame, frame, ldetector.nOctaves-1);
		
		
		// Make the threshold version
		cvThreshold(gray_frame, thresh_frame, 80, 255, CV_THRESH_BINARY);
		
		// Draw the contours onto the frame
		cvFindContours( thresh_frame, g_storage, &contours );
		cvCopy(frame, contours_frame);
		if( contours )
		{
			cvDrawContours(contours_frame, contours, cvScalar(100, 200, 50), cvScalar(60, 20, 200), 100);
		}

		// Make the canny image
		canny_frame = doCanny(gray_frame, 10, 100, 3);
		
		
		// Copy the contoursframe into the composite image
		cvSetImageROI(composite, cvRect(0, 0, frame->width, frame->height));
		cvCopy(contours_frame, composite);
		
		
		// Copy the threshold image into the composite
		cvSetImageROI(composite, cvRect(frame->width, 0, frame->width, frame->height));
		cvCvtColor(thresh_frame, rgb_tmp, CV_GRAY2BGR);
		cvCopy(rgb_tmp, composite);
		
		// Copy the Canny image into the composite
		cvSetImageROI(composite, cvRect(0, frame->height, frame->width, frame->height));
		cvCvtColor(canny_frame, rgb_tmp, CV_GRAY2BGR);
		cvCopy(rgb_tmp, composite);
		
		
		cvResetImageROI(composite);
		// Interface stuff
       
		frames++;
		last_frame = cvCloneImage(frame);
        cvSetTrackbarPos("Position",vid_win_name,frames);
        cvShowImage( vid_win_name, composite );
        char c = (char)cvWaitKey(0);
        if( c == 27 ) break;
    }
	
	cvClearMemStorage( g_storage );
    cvReleaseCapture( &g_capture );
    cvDestroyWindow( vid_win_name );
	 */
    return(0);
}

void onTrackbarSlide(int pos)
{
    cvSetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES, pos);
}

IplImage* doCanny(IplImage* in, double lowThresh, double highThresh,  double aperture)
{
    if (in->nChannels != 1)
        return(0); // Canny only handles gray scale images
    IplImage* out = cvCreateImage( 
								  cvGetSize( in ),
								  in->depth, //IPL_DEPTH_8U,    
								  1);
    cvCanny( in, out, lowThresh, highThresh, aperture );
    return( out );
};