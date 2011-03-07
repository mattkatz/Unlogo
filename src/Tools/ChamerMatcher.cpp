/*
 *  ChamerMatcher.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/5/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "ChamerMatcher.h"


// -------------------------------------------
bool ChamerMatcher::train(Image& _trainImg)
{	
	trainImg = _trainImg.cvImg.clone();
	bTrained=true;
	return true;
}


// -------------------------------------------
void ChamerMatcher::doQuery(Image& _queryImg, string windowName)
{
	assert(_queryImg.channels()==1);
	
	if(!bTrained) {
		cout << "Please train ChamerMatcher before using" << endl;
		return;
	}

	
	queryImg = _queryImg.cvImg.clone();
	
	imshow("chamer query", queryImg);
	imshow("chamer train", trainImg);
	
	results.clear();
	costs.clear();
	
	// chamerMatching clobbers the images, so we have to make a copy.
	Mat train = trainImg.clone();
	best = chamerMatching( queryImg, train, results, costs );
	cout << "best=" << best << endl;
	if( best < 0 )
	{
		cout << "not found;\n";
		return;
	}
	
	if(!windowName.empty())
	{
		Mat cimg;
		cvtColor(queryImg, cimg, CV_GRAY2BGR);
		
		size_t i, n = results[best].size();
		for( i = 0; i < n; i++ )
		{
			Point pt = results[best][i];
			if( pt.inside(Rect(0, 0, cimg.cols, cimg.rows)) )
			{
				cimg.at<Vec3b>(pt) = Vec3b(0, 255, 0);
				//circle(cimg, pt, 10, Scalar(0,255,0), 1);
			}
		}
		
		imshow(windowName, cimg);	
	}
}
