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
	string color;
	switch(level) {
		default:
		case LOG_LEVEL_DEBUG: 	color="\033[01;33m"; break;
		case LOG_LEVEL_WARNING:	color="\033[01;34m"; break;
		case LOG_LEVEL_ERROR:	color="\033[01;31m"; break;
	}
	va_list args;
	va_start(args, format);
	string fstr(color+string("[unlogo] ")+format+"\n");
	int status = vfprintf(stderr, fstr.c_str(), args);
	va_end(args);
	return status;
}
