/*
 *  Image.cpp
 *  structure
 *
 *  Created by Jeffrey Crouse on 2/7/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "Image.h"


// ------------------------------------------
void Image::useMat( cv::Mat m )
{
	cvImg = m;
}


// ------------------------------------------
Image& Image::operator=(const Image &other)
{
	if (this == &other)      // Same object?
		return *this;        // Yes, so skip assignment, and just return *this.
	other.cvImg.copyTo( cvImg );
	return *this;
}

// ------------------------------------------
bool Image::open(string filename, bool forceGrayscale)
{
	int gray = (forceGrayscale) ? 0 : 1;
	cvImg = cv::imread(filename, gray);
	return !cvImg.empty();
}

// ------------------------------------------
void Image::grabFrame( cv::VideoCapture &cap )
{
	unsigned char* data = cvImg.data;
	cap >> cvImg;
	
	// Just a check to protect against leaks.  
	// There probably isn't a good reason why the Mat should
	// be making a new data array frequently (like every frame)
	// so if you see this a lot, you are probably doing something wrong.
	if( data != cvImg.data) {
		cout << "INFO: Reallocated cvImg in Image::grabFrame" << endl;
	}
}

// ------------------------------------------
/*
void Image::apply(Effect* effect)
{
	unsigned char* data = cvImg.data;
	effect->apply(this);
	
	// Just a check to protect against leaks.  
	// There probably isn't a good reason why the Mat should
	// be making a new data array frequently (like every frame)
	// so if you see this a lot, you are probably doing something wrong.
	if( data != cvImg.data) {
		cout << "INFO: Reallocated Image::clean in Image::apply" << endl;
	}
}
*/


// ------------------------------------------------------
void Image::threshold(int t, bool inverse)
{
	int thresholdType = inverse ? THRESH_BINARY_INV : THRESH_BINARY;
	cv::threshold(cvImg, cvImg, t, 255, thresholdType); 
}

// ------------------------------------------------------
void Image::adaptiveThreshold(bool inverse, bool gaussian)
{
	int adaptiveMethod = gaussian ? ADAPTIVE_THRESH_GAUSSIAN_C : ADAPTIVE_THRESH_MEAN_C;
	int thresholdType = inverse ? THRESH_BINARY_INV : THRESH_BINARY;
	cv::adaptiveThreshold(cvImg, cvImg, 255, adaptiveMethod, thresholdType, 5, 5); 
}

// ------------------------------------------------------
void Image::soebel(bool xfirst, int ksize)
{	
	Sobel(cvImg, cvImg, CV_8U, (xfirst)?1:0, (xfirst)?0:1, ksize);
}

// ------------------------------------------------------
void Image::canny(double thresh1, double thresh2, int aperatureSize, bool moreAccurate)
{
	Canny(cvImg, cvImg, thresh1, thresh2, aperatureSize, moreAccurate);
}


// ------------------------------------------------------
void Image::text(const string txt, int x, int y, float scale)
{
	putText(cvImg, txt, Point(x,y), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255));
}

// ------------------------------------------------------
void Image::copySectionFrom(Image src, Rect src_rect)
{
	copySectionFrom(src, src_rect, src_rect);
}

// ------------------------------------------------------
Image Image::getGrayscale()
{
	int code;
	switch(channels())
	{
		case 1: return this->clone();
		case 3: code=CV_RGB2GRAY; break;
		case 4: code=CV_RGBA2GRAY; break;
	}
	
	Mat gray;
	cvtColor(cvImg, gray, code);
	Image* grayscale = new Image(gray);
	return *grayscale;
}

// ------------------------------------------------------
void Image::copySectionFrom(Image src, Rect src_rect, Rect dst_rect)
{
	Mat src_roi = src.cvImg(src_rect);
	Mat dst_roi = cvImg(dst_rect);
	src_roi.copyTo(dst_roi);
}

// ------------------------------------------------------
void Image::copyInto(Image background, int x, int y, Image mask)
{
	Rect bg = Rect(0, 0, background.width(), background.height());
	Rect fg = Rect(x, y, width(), height());
	Rect intersect = fg & bg;
	
	Mat foreground_roi = cvImg( Rect(0, 0, intersect.width, intersect.height) );
	Mat mask_roi = mask.cvImg( Rect(0, 0, intersect.width, intersect.height) );
	Mat background_roi = background.cvImg(Rect(x, y, intersect.width, intersect.height));
	
	
	foreground_roi.copyTo(background_roi, mask_roi);
}


// ------------------------------------------------------
void Image::copyInto(Image background, int x, int y)
{
	copyInto(background, x, y, Image(Mat::ones(size(), CV_8UC1)));
}


// ------------------------------------------------------
void Image::show(string windowname, int x, int y)
{
	namedWindow(windowname);
	cvMoveWindow(windowname.c_str(), x, y);
	imshow(windowname, cvImg);
}


// ------------------------------------------------------
vector<Image> Image::split()
{
	vector<Mat> cvPlanes;
	vector<Image> planes;
	cv::split(cvImg, cvPlanes);
	for(int i=0; i<cvPlanes.size(); i++)
	{
		Image img(cvPlanes[i]);
		planes.push_back( img );
	}
	return planes;
}

// ------------------------------------------------------
// TO DO: There must be a better way to do this.  Maybe post on opencv list
// Maybe we can elongate it to make an extremely wide image and then rotate it.
// NOTE: Not sure this works with >1 channel images
Mat Image::asList()
{
	int rows = cvImg.total();
	int cols = 1;
	Mat list(rows, cols, cvImg.type());
	uchar* src;
	uchar* dest = list.ptr(0);
	for(int i=0; i<cvImg.size().height; i++) {
		src = cvImg.ptr(i);
		memcpy(dest, src, cvImg.step);
		dest += cvImg.step;
	}
	return list;
}


// ------------------------------------------------------
void Image::setDepth(int newType)
{
	cvImg.convertTo(cvImg, newType);
}


// ------------------------------------------------------
float* Image::getClusters(int clusterCount)
{
	Mat list = asList();
	list.convertTo(list, CV_32F);
	
	Mat labels(list.size(), CV_8UC1);
	Mat centers(clusterCount, 1, list.type());
	TermCriteria termcrit(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0);
	kmeans(list, clusterCount, labels, termcrit, 3, KMEANS_PP_CENTERS, &centers);
	float* c = new float[clusterCount];
	memcpy(c, centers.ptr<float>(0), sizeof(float)*clusterCount);
	return c;
}

// ------------------------------------------------------
vector<Image> Image::copies(int num)
{
	vector<Image> clones;
	for(int i=0; i<num; i++) {
		Image aCopy( cvImg );
		clones.push_back( aCopy );
	}
	return clones;
}
