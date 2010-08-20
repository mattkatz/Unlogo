
#include <iostream>
#include <highgui.h>
#include "LogoFilter.h"
#include "MatImage.h"


using namespace cv;
using namespace std;

/*
int main (int argc, char * const argv[])
{
	VideoCapture cap(argv[1]);
	cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
	
    if(!cap.isOpened())  
	{
		cout << "Can not open video file." << endl;
        return -1;
	}
	
	MatImage head;
	head.open("share/faces/Chanel.png");

	MatImage frame;

	
	for(int i=0; 1; i++)
    {
        frame = cap;
		frame.convert(CV_RGB2RGBA);
		frame.drawIntoMe( head, 0, 0 );
		
		frame.show( "out" );
		
        char c = (char)cvWaitKey(5);
        if( c == '\x1b' ) // esc
        {
            cout << "Exiting ..." << endl;
            return 0;
        }
    }
}
*/

int main (int argc, char * const argv[])
{
	VideoCapture cap(argv[1]); // open the default camera
    if(!cap.isOpened())  
	{
		cout << "Can not open video file." << endl;
        return -1;
	}
	
	LogoFilter g_filter;
	g_filter.init( "SURF", "SURF", "BruteForce" );
	for(int i=2; i<argc; i+=2)
	{
		g_filter.addLogo(argv[i], argv[i+1]);
	}
	
	Mat frame;
	Mat drawImg;
	
    for(int i=0; 1; i++)
    {
        cap >> frame; 
		drawImg = frame.clone();
		g_filter.filter(frame, drawImg, true);
		
		imshow("out", drawImg);
        char c = (char)cvWaitKey(5);
        if( c == '\x1b' ) // esc
        {
            cout << "Exiting ..." << endl;
            return 0;
        }
    }
}