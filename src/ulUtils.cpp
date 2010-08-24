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
	cerr << set_bold( TRUE ) << "[unlogo] " << set_bold(FALSE); 
	switch(level) {
		default:
		case LOG_LEVEL_DEBUG: 	cout << set_colors(VT_DEFAULT,VT_DEFAULT); break;
		case LOG_LEVEL_WARNING:	cout << set_colors(VT_YELLOW,VT_DEFAULT); break;
		case LOG_LEVEL_ERROR:	cout << set_colors(VT_WHITE,VT_RED); break;
	}
	va_list args;
	va_start(args, format);
	string fstr(format+string("\n"));
	int status = vfprintf(stderr, fstr.c_str(), args);
	va_end(args);
	cout << finalize;
	return status;
}


