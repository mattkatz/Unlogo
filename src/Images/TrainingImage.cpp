/*
 *  TrainingImage.cpp
 *  logomunge
 *
 *  Created by Jeffrey Crouse on 4/6/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "TrainingImage.h"

// ----------------------------------------------------
// Loads in the "corners" from the cnr file, or if it doesn't exist, pops open
// a window that allows the user to mark the corners
void TrainingImage::generate_cnrfile()
{
	corners.clear();
	cnrfile = imgfile;
	cnrfile.replace_extension(".cnr");
	
	// TO DO: Haven't checked this loading process.
	ifstream myfile( cnrfile.string().c_str() );
	if (myfile.is_open())
	{
		string line;
		while( myfile.good() )
		{
			getline(myfile, line);
			if(!line.empty())
			{
				Point p;
				stringstream ss(line);
				ss >> p.x >> p.y;
				corners.push_back( p );
			}
		}
		myfile.close();
		return;
	}

	cout << "No corners file found.  Lets create one." << endl;

	// We need to construct an image with a margin around it
	// This is because the bounds of the "plane" on which the logo exists might
	// extend outside of the image.
	image_w_margin = Mat(Size(cvImg.size().width+(MARGIN*2), cvImg.size().height+(MARGIN*2)), cvImg.type());
	image_w_margin = Scalar(255,255,255);
	
	// Copy the image from the file into the one with the margin
	Mat roi = image_w_margin(Rect(50, 50, cvImg.size().width, cvImg.size().height));
	cvImg.copyTo( roi );
	
	// Show the image
	imshow("image", image_w_margin);
	setMouseCallback("image", on_mouse, this);
	
	bool done=false;
	do {
		int key = waitKey(100);
		if(key==27 || corners.size() > 3)
		{
			done=true;
		}
		if(key=='c')
		{
			generate_cnrfile();
		}
		imshow("image", image_w_margin);
	} while(!done);
	
	if(corners.size()==4)
	{
		ofstream myfile;
		myfile.open(cnrfile.string().c_str(), ios::trunc);
		if(myfile.is_open())
		{
			for(int i=0; i<corners.size(); i++)
			{
				myfile << corners[i].x << " " << corners[i].y << endl;
			}
			myfile.close();
		}
	}
}


// ----------------------------------------------------
// If an image has a name like myimage.jpg?fooblah=boo, rename it myimage.jpg 
void TrainingImage::remove_query_string()
{
	// If there is a query string after the extension, take it off!
	string old_name = imgfile.string();
	size_t found = old_name.find("&");
	if (found!=string::npos)
	{
		string new_name = old_name.substr(0, found);
		fs::rename(old_name, new_name);
		imgfile = fs::path(new_name);
	}	
}




// ----------------------------------------------------
// We need to keep track of the image filename, so we override the open method from Image
void TrainingImage::open(string _imgfile)
{
	imgfile = fs::path(_imgfile);
	Image::open(_imgfile);
}


// ----------------------------------------------------
// OpenCV mouse listener
void TrainingImage::on_mouse( int event, int x, int y, int flags, void* param )
{
	TrainingImage* img = (TrainingImage*)param;
	if(event==CV_EVENT_LBUTTONUP)
	{
		circle(img->image_w_margin, Point(x,y), 2, Scalar(0,0,255), 2);
		img->corners.push_back(Point(x-MARGIN, y-MARGIN));
	}
}

