/*
 *  JSONElement.h
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/4/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <iostream>
#include <fstream>
#include "json/json.h"


using namespace std;
using namespace Json;

class JSONElement: public Value {
public:
	JSONElement() {};
	JSONElement(string jsonString);
	JSONElement(Json::Value& v);
	bool parse(string jsonString);
	bool open(string filename);
	bool save(string filename, bool pretty=false);
	string getRawString(bool pretty=true);
	
protected:
	bool openLocal(string filename);
	bool openRemote(string filename);
};