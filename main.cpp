
#include <iostream>
#include <highgui.h>
#include "opencv2/core/core.hpp"
#include "LogoFilter.h"

using namespace cv;
using namespace std;

int main (int argc, char * const argv[])
{
	VideoCapture cap(argv[1]); // open the default camera
    if(!cap.isOpened())  
	{
		cout << "Can not open video file." << endl;
        return -1;
	}
	
	LogoFilter g_filter;
	g_filter.init( argv[2] );
	
	Mat frame;
	Mat drawImg;
    for(int i=0; 1; i++)
    {
        cap >> frame; 
		drawImg = frame.clone();
			
		g_filter.filter(frame, drawImg);
		
		imshow("out", drawImg);
        if(waitKey(30) >= 0) break;
    }
}	
