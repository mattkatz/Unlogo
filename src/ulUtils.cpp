/*
 *  ulUtils.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/19/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#include "ulUtils.h"

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
