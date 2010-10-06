
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "Image.h"

using namespace fh;
Image input, output, gray, moustache;
int framenum=0;
CascadeClassifier faceFinder, eyeFinder;
vector<Rect> faces;

extern "C" int init( const char* argstr )
{
#ifdef FHDEBUG
	printf ("Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
			CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
	cvNamedWindow("input");		cvMoveWindow("input", 0, 0);
	cvNamedWindow("output");	cvMoveWindow("output", 0, 510);
#endif
	
	if( !faceFinder.load( "../../share/opencv/haarcascades/haarcascade_frontalface_alt.xml" ) ) {
		log(LOG_LEVEL_ERROR, "Could not load classifier cascade");
		return -1;
	}
	
	if( !eyeFinder.load( "../../share/opencv/haarcascades/haarcascade_eye_tree_eyeglasses.xml" ) ) {
		log(LOG_LEVEL_WARNING, "Could not load classifier cascade for nested objects");
		return -1;
	}
	
	moustache.open("resources/moustache.png");
	
	return 0;
}

extern "C" int uninit()
{
	return 0;
}


extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
	log(LOG_LEVEL_DEBUG, "=== Frame %d ===", framenum);
	
	input.setData( width, height, src[0], src_stride[0]);
	
#ifdef FHDEBUG
	input.show("input");
#endif
	
	gray = Image( input );
	gray.convert(CV_RGB2GRAY);
	input.convert(CV_RGB2RGBA);
	gray.equalizeHist();
	faces.clear();
	faceFinder.detectMultiScale( gray.cvImage, faces,
								1.1, 2, 0
								//|CV_HAAR_FIND_BIGGEST_OBJECT
								//|CV_HAAR_DO_ROUGH_SEARCH
								|CV_HAAR_SCALE_IMAGE
								,
								Size(30, 30) );
	
	
	for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++ )
    {
        Mat graySubImg;
        vector<Rect> eyes;
        Point center;
        int radius;
        center.x = r->x + r->width * 0.5;
        center.y =r->y + r->height * 0.5;
        radius = cvRound((r->width + r->height)*0.25);
        circle( input.cvImage, center, radius, CV_RGB(255,0,0), 3, 8, 0 );
		
		
        graySubImg = gray.cvImage(*r);
        eyeFinder.detectMultiScale( graySubImg, eyes,
								   1.1, 2, 0
								   //|CV_HAAR_FIND_BIGGEST_OBJECT
								   //|CV_HAAR_DO_ROUGH_SEARCH
								   //|CV_HAAR_DO_CANNY_PRUNING
								   |CV_HAAR_SCALE_IMAGE
								   ,
								   Size(30, 30) );
		
		if(eyes.size()>0)
		{
			Point2f pos = center;
			pos.x -= moustache.width() / 2.;
			pos.y += 25;
			input.drawIntoMe(moustache, pos);
		}
		
        for(vector<Rect>::const_iterator eye = eyes.begin(); eye != eyes.end(); eye++ )
        {
            center.x = cvRound(r->x + eye->x + eye->width * 0.5);
            center.y = cvRound(r->y + eye->y + eye->height * 0.5);
            radius = cvRound((eye->width + eye->height)*0.25);
            circle( input.cvImage, center, radius, CV_RGB(0,255,0), 3, 8, 0 );
        }
    }
	
	
	
	input.convert(CV_RGBA2RGB);	
	
	
	output.setData( width, height, dst[0], dst_stride[0] );
	output.copyFromImage(input);								// copy input into the output memory
	output.text("sample", 10, height-10, .5);					// A very simple modification to the frame
	
#ifdef FHDEBUG
	output.show( "output" );
	waitKey(1);													// required to update highgi windows
#endif
	
	
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst
	framenum++;
	return 0;
}
