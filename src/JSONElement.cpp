/*
 *  JSONElement.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/4/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "JSONElement.h"


//--------------------------------------------------------------
JSONElement::JSONElement(Json::Value& v) : Value(v)
{
	
}

//--------------------------------------------------------------
JSONElement::JSONElement(string jsonString)
{
	parse(jsonString);
}

//--------------------------------------------------------------
bool JSONElement::parse(string jsonString)
{
	Reader reader;
	if(!reader.parse( jsonString, *this )) {
		cout << "Unable to parse string" << endl;
		return false;
	}
	return true;
}


//--------------------------------------------------------------
bool JSONElement::open(string filename)
{
	if(filename.find("http://")==0) {
		return openRemote(filename);
	} else {
		return openLocal(filename);
	}
}


//--------------------------------------------------------------
bool JSONElement::openLocal(string filename) 
{
	ifstream myfile(filename.c_str());
	
	if (!myfile.is_open()) {
		cout << "Could not open " << filename << endl;
		return false;
	}
	
	string str,strTotal;
	getline(myfile, str);
	while ( myfile ) {
		strTotal += str;
		getline(myfile, str);
	}
	myfile.close();
	
	Reader reader;
	if(!reader.parse( strTotal, *this )) {
		cout << "Unable to parse " << filename << endl;
		return false;
	}
	return true;
}


//--------------------------------------------------------------
bool JSONElement::openRemote(string filename) {
	string result;
	
	// TO DO -- put some CURL shit here
	
	Reader reader;
	if(!reader.parse( result, *this )) {
		cout << "Unable to parse " << filename << endl;
		return false;
	}
	return true;
}


//--------------------------------------------------------------
bool JSONElement::save(string filename, bool pretty)
{
	ofstream file_key(filename.c_str());
	if (!file_key.is_open()) {
		cout << "Unable to open " << filename << endl;
		return false;
	}
	
	if(pretty) {
		StyledWriter writer;
		file_key << writer.write( *this ) << endl;
	} else {
		FastWriter writer;
		file_key << writer.write( *this ) << endl;
	}
	file_key.close();	
	return true;
}


//--------------------------------------------------------------
string JSONElement::getRawString(bool pretty) {
	string raw;
	if(pretty) {
		StyledWriter writer;
		raw = writer.write(*this);
	} else {
		FastWriter writer;
		raw = writer.write(*this);
	}
	return raw;
}
