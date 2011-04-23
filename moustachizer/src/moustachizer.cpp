/*
 *  moustachizer.cpp
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/22/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */


#include "FaceTracker.h"

using namespace std;
using namespace cv;

class Moustachizer {
public:
    Moustachizer() {};
    ~Moustachizer() {};
    int init(const char* argstr);
    void process(Mat frame);
	
private:
    FaceTracker faceTracker;
    Mat stache, mask;
};



// ------------------------------
int Moustachizer::init(const char* argstr) {

	faceTracker.init();
	
	const char* fileName = "images/moustache4.jpg";
	stache = imread(fileName, 1);
	
	// OpenCV can't load 4 channel images, which is a huge pain
	// so I am pulling out the Value channel from the moustache image
	// to use as a mask for drawing the moustache into the main frame.
	Mat hsvimg;
	cvtColor(stache, hsvimg, CV_RGB2HSV);
	vector<Mat> hsvchannels;
	split(hsvimg, hsvchannels);	
	bitwise_not(hsvchannels[2], mask); 
	erode(mask, mask, Mat(), Point(-1,-1), 4);
	dilate(mask, mask, Mat(), Point(-1,-1), 2);
	
	return 0;
}

// ------------------------------
void Moustachizer::process(Mat frame) {
	
	//circle(frame, Point(300,300), 300, Scalar(255,0,0), 3);
	Mat grayFrame = frame.clone();
	cvtColor(frame, grayFrame, CV_RGB2GRAY);
	equalizeHist(grayFrame, grayFrame);
	imshow("grayFrame", grayFrame);
	faceTracker.search( grayFrame );
	
	
	
	for(int i=0; i<faceTracker.faces.size(); i++)
	{
		Face face = faceTracker.faces[i];
		face.draw( frame );
		
		float scale =  (float)face.boundingBox.width / stache.size().width;
		
		Mat stache_resized;
		Mat mask_resized;
		resize(stache, stache_resized, Size(), scale, scale);
		resize(mask, mask_resized, Size(), scale, scale);
		
		float xpos = face.boundingBox.x;
		float ypos = face.boundingBox.y + (face.boundingBox.height * .60);
		Rect pos = Rect(xpos, ypos, stache_resized.size().width, stache_resized.size().height);
		
		/*
		 Rect frame = Rect(0, 0, input.size().width, input.size().height);
		 Rect intersection = pos & frame;
		 Mat fg = stache_resized(Rect(0,0,intersection.width,intersection.height));
		 Mat bg = input(Rect(xpos,ypos,intersection.width,intersection.height));
		 */
		
		Mat bg = frame(pos);
		stache_resized.copyTo(bg, mask_resized);	
	}
	
	//cvtColor(input, input, CV_GRAY2RGB);
	imshow("preview", frame);
	
	cvWaitKey(1);
}

