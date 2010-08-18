/*
 *  unlogo.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 8/18/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef UNLOGO_H
#define UNLOGO_H



extern "C" int init( const char* argstr );
extern "C" int uninit();
extern "C" int process( uint8_t* dst[4], int dst_stride[4],
			uint8_t* src[4], int src_stride[4],
			int width, int height);



#endif