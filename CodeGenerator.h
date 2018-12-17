/********************************************************************************/
/* CS460 Project 3 - Team 1: Justin Moore, Robert Hubbell, Justin Bernard       */
/********************************************************************************/

#ifndef CG_H
#define CG_H

#include <iostream>
#include <fstream>

using namespace std;

class CodeGen 
{
 public:
  CodeGen (string filename);
  ~CodeGen ();
  void WriteCode (int tabs, string code);
 private:
  ofstream cpp;
};
	
#endif
