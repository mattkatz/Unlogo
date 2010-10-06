/*
 *  fhUtils.cpp
 *  framehack
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "fhUtils.h"

int log( int level, const char * format, ... )
{
	using namespace std;	

	switch(level) {
		default:
		case LOG_LEVEL_DEBUG: 	cerr << "[unlogo debg] "; break;
		case LOG_LEVEL_WARNING:	cerr << "[unlogo warn] "; break;
		case LOG_LEVEL_ERROR:	cerr << "[unlogo ERROR] "; break;
	}
	va_list args;
	va_start(args, format);
	string fstr(format+string("\n"));
	int status = vfprintf(stderr, fstr.c_str(), args);
	va_end(args);
	return status;
}



void lerp(Point2f& current, const Point2f& desired, float ease)
{
	if(current.x == -1 || current.y == -1) {
		current = desired;
	} else {
		Point2f	diff = desired - current;
		current += Point2f(diff.x/ease, diff.y/ease);
	}
}

void lerp(Point& current, Point& desired, float ease)
{
	if(current.x == -1 || current.y == -1) {
		current = desired;
	} else {
		Point	diff = desired - current;
		current += Point(diff.x/ease, diff.y/ease);
	}
}

vector<string> split(const string& str, const string& delimiters)
{
	vector<string> tokens;
	
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
	
	return tokens;
}


Mat points2mat(vector<Point2f> pts)
{
	Mat out(pts.size(), 1, CV_32FC2);
	for(int i=0; i<pts.size(); i++)
	{
		float* ptr = out.ptr<float>(i);
		ptr[0] = pts[i].x;
		ptr[1] = pts[i].y;
	}
	return out;
}

Mat points2mat(vector<Point> pts)
{
	Mat out(pts.size(), 1, CV_32FC2);
	for(int i=0; i<pts.size(); i++)
	{
		float* ptr = out.ptr<float>(i);
		ptr[0] = pts[i].x;
		ptr[1] = pts[i].y;
	}
	return out;
}

void Sort4PointsClockwise(Point2f points[4]){
    Point2f& a = points[0];
    Point2f& b = points[1];
    Point2f& c = points[2];
    Point2f& d = points[3];
	
    double abc = a.x * b.y - a.y * b.x + b.x * c.y - b.y * c.x + c.x * a.y - c.y * a.x;
    if(abc < 0.0){
        double acd = a.x * c.y - a.y * c.x + c.x * d.y - c.y * d.x + d.x * a.y - d.y * a.x;
        if(acd < 0.0){
			// Cool!
			return;
        }
        else {
			double abd = a.x * b.y - a.y * b.x + b.x * d.y - b.y * d.x + d.x * a.y - d.y * a.x;
			if(abd < 0){
				std::swap(d, c);
			}
			else{
				std::swap(a, d);
			}
        }
    }
    else {
        double acd = a.x * c.y - a.y * c.x + c.x * d.y - c.y * d.x + d.x * a.y - d.y * a.x;
        if(acd < 0.0){
			double abd = a.x * b.y - a.y * b.x + b.x * d.y - b.y * d.x + d.x * a.y - d.y * a.x;
			if(abd < 0.0){
				std::swap(b, c);
			}
			else{
				std::swap(a, b);
			}
        }
        else {
			std::swap(a, c);
        }
    }
}

