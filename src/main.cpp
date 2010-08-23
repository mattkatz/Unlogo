

#include "Matcher.h"
#include "Image.h"
#include "MatchSet.h"
#include "MatchTracker.h"


using namespace cv;
using namespace std;
using namespace unlogo;


typedef struct Logo {
	const char* name;
	Image logo;
	Image replacement;
	MatchTracker tracker;
};

int main (int argc, char * const argv[])
{
	const char* window_name = "output";
	
	// Open the video
	VideoCapture cap(argv[1]);
    if(!cap.isOpened())  
	{
		cout << "Can not open video source" << endl;
        return -1;
	}

	// Construct the matcher singleton the way we want it.
	// Otherwise, the instance constructor will do the default.
	Matcher::Instance("SURF", "SURF", "BruteForce");

	
	// Load in all of the logos from the arguments
	vector<Logo> logos;
	for(int i=2; i<argc; i+=2)
	{
		Logo l;
		l.name = argv[i];
		l.logo.open( argv[i] );
		l.replacement.open( argv[i+1] );
		l.tracker = MatchTracker();
		logos.push_back( l );
	}
	
	
	Image frame;
	for(int framenum=0; 1; framenum++)
    {
		cout << "===Frame " << framenum << "===" <<  endl;
		
		frame << cap;
		if(frame.cvImage.empty() || waitKey(10) == '\x1b') break;
		
		for(int i=0; i<logos.size(); i++)
		{
			// Find all matches between the frame and the logo
			// the logo is A and the frame is B
			MatchSet* ms = new MatchSet(&logos[i].logo, &frame, 2);

			ms->drawMatchesInB();
			
			// Keep track of the history of the matches so we can ease/average
			logos[i].tracker.track( ms );
			
			// What are we going to replace it with?
			//Image replace = logos[i].replacement;
			//replace.warp( &h12 );
			
			// Finally, draw the replacement
			//Point2f loc = logos[i].tracker.avg();
			//frame.drawIntoMe( &replace, loc );
			
			//imshow( logos[i].name, logos[i].logo.cvImage );
		}
		
		imshow( window_name, frame.cvImage );
		
	}
	
	cout << "Exiting ..." << endl;
	return 0;
}


/*
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
	
	Image frame;
	
    for(int i=0; 1; i++)
    {
        frame = cap;
		
		g_filter.filter(frame, frame, true);
		
		frame.show( "out" );

        char c = (char)cvWaitKey(5);
        if( c == '\x1b' ) // esc
        {
            cout << "Exiting ..." << endl;
            return 0;
        }
    }
}



int main (int argc, char * const argv[])
{
	VideoCapture cap(argv[1]);
	cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
	
    if(!cap.isOpened())  
	{
		cout << "Can not open video file." << endl;
        return -1;
	}
	
	Image head;
	head.open("share/faces/Chanel.png");

	Image frame;

	
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