// Project: CS 460 Fall 2017 Project 3
// File: Object.h
// Author: Dr. Watts
// Desciption: Object class for Scheme to C++ translation
/********************************************************************************/
/* CS460 Project 3 - Team 1: Justin Moore, Robert Hubbell, Justin Bernard       */
/********************************************************************************/

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;


/********************************************************************************/
/* Functions and variables for Object class                                     */
/********************************************************************************/

enum obj_type {NONE, INT, REAL, STRING, LIST};
	
class Object
{
    public:
	Object();
	Object (const int & value);
	Object (const double & value);
	Object (const string & value);
	bool operator == (const Object & O) const;
	bool operator != (const Object & O) const;
	bool operator < (const Object & O) const;
	bool operator <= (const Object & O) const;
	bool operator > (const Object & O) const;
	bool operator >= (const Object & O) const;
	Object operator + (const Object & O) const;
	Object operator - (const Object & O) const;
	Object operator * (const Object & O) const;
	Object operator / (const Object & O) const;
	Object operator % (const Object & O) const;
	friend bool numberp (const Object & O);
	friend bool symbolp (const Object & O);
	friend bool listp (const Object & O);
	friend bool zerop (const Object & O);
	friend bool nullp (const Object & O);
	friend bool stringp (const Object & O);
	friend Object listop (const string & S, const Object & O);
	friend Object cons (const Object & O1, const Object O2);
	friend Object round (const Object & O);
	friend ostream & operator << (ostream & outs, const Object & O);
    private:
	Object (stringstream & ss);
	void MakeName ();
	obj_type type;
	string name;
	int intval;
	double realval;
	string strval;
	vector <Object> listval;
};
