/********************************************************************************/
/* CS460 P3 - Team1: Justin Moore, Robert Hubbell, Justin Bernard               */
/********************************************************************************/

#ifndef SYN_H
#define SYN_H

#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "CodeGenerator.h"

using namespace std;

/********************************************************************************/
/* SyntacticalAnalyzer class functions and variables                            */
/********************************************************************************/
class SyntacticalAnalyzer 
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
    private:
	LexicalAnalyzer * lex;
	CodeGen * gen;
	token_type token;
	ofstream p2file;
	int program();
	int more_defines();
	int define();
	int stmt_list(string op);
	int stmt();
	int literal();
	int quoted_lit();
	int more_tokens();
	int param_list(bool firstCall);
	int else_part();
	int stmt_pair();
	int stmt_pair_body();
	int action();
	int any_other_token();
	int numStmtCalls;
	int numTabs;
	int ifCalls;
};
	
#endif
