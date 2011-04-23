/*
 *  moustachizer.h
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/22/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "FaceTracker.h"

using namespace std;
using namespace cv;

class Moustachizer {
	public:
		Moustachizer() {};
		~Moustachizer() {};
		int init(const char* argstr);
		void process(Mat frame);
	
	private:
		FaceTracker faceTracker;
		Mat stache, mask;
};
