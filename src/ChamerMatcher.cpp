/*
 *  ChamerMatcher.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/5/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "ChamerMatcher.h"

bool ChamerMatcher::init(bool useSoebelEdgeDetection)
{
	bUseSobel = useSoebelEdgeDetection;
	return true;
}

// -------------------------------------------
bool ChamerMatcher::setTrainImage(Image& _train, string datapath)
{
	string edgeImagePath;
	if(bUseSobel)
	{
		edgeImagePath = datapath+"_sobel.jpg";
		train = imread(edgeImagePath, 0);
		if(train.empty())
		{
			train = _train.cvImg.clone();
			//Sobel(train, train, CV_8U, 1, 0);
			Sobel(train, train, CV_8U, 0, 1);
			imwrite(edgeImagePath, train);
		}
	}
	else 
	{
		edgeImagePath = datapath+"_canny.jpg";
		train = imread(edgeImagePath, 0);
		if(train.empty())
		{
			train = _train.cvImg.clone();
			Canny(train, train, 5, 50, 3);
			imwrite(edgeImagePath, train);
		}
	}
	return true;
}


// -------------------------------------------
void ChamerMatcher::doQuery(Image& _query, bool showCorrespondence)
{
	assert(_query.channels()==1);
	
	query = _query.cvImg.clone();
	
	if(bUseSobel)
	{
		Sobel(query, query, CV_8U, 1, 0);
		//Sobel(train, train, CV_8U, 0, 1); 
	}
	else 
	{
		Canny(query, query, 5, 50, 3);
	}
	
	best = chamerMatching( query, train, results, costs );
	if( best < 0 )
	{
		cout << "not found;\n";
		return;
	}
	
	if(showCorrespondence)
	{
		Mat cimg;
		cvtColor(query, cimg, CV_GRAY2BGR);
		
		size_t i, n = results[best].size();
		for( i = 0; i < n; i++ )
		{
			Point pt = results[best][i];
			if( pt.inside(Rect(0, 0, cimg.cols, cimg.rows)) )
			{
				//cimg.at<Vec3b>(pt) = Vec3b(0, 255, 0);
				circle(cimg, pt, 10, Scalar(0,255,0), 1);
			}
		}
		imshow("Chamer Result", cimg);	
	}
}
