

#include "Tools/ChamerMatcher.h"

using namespace cv;
using namespace std;

Image trainImage;
ChamerMatcher chamerMatcher;
VideoCapture capture;

int main( int argc, char** argv )
{

	
	trainImage.open("data/logo.png", true);
	if(trainImage.empty())
	{
		cout << "couldn't open " << argv[1] << endl;
		return 1;
	}
	
	chamerMatcher.train(trainImage);
	
	

	capture.open( "data/apple_logo.mov" );
	if (!capture.isOpened())
	{
		cout << "couldn't open video" << endl;
		return 1;
	}
	
	int frameno=0;
	Image frame, gray;
	for (;;)
	{
		cout << "frame " << frameno << endl;
		frame.grabFrame(capture);
		frame.show("clean");
		
		if (frame.empty())
			continue;
		
		// Do some stuff to the image to make it easier to match.
		gray = frame.getGrayscale();
		gray.equalize();
		//gray.show("equalized");
		//gray.threshold(150);
		//gray.adaptiveThreshold();
		gray.canny(100, 255, 3);
		//gray.soebel(false);
		gray.show("soebel");
		
		chamerMatcher.doQuery(gray, "chamer");
		
		char key = waitKey(2);
		switch (key)
		{
			case 27:
			case 'q':
				return 0;
				break;
		}
		
		frameno++;
	}
}
