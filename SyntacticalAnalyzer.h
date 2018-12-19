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
	// squote is used in combination with lparen to verify whether we're
	// evaluating a list of things to be printed.  Rather than encapsulating
	// each quoted lit item as ("item1"), ("item2"), etc, it allows the whole
	// list to be encapsulated (and printed) as "(item1 item2...itemN)"
	bool squote;
	bool lparen;
	// used to hold onto the return type of a function and determine what kind
	// of "return" statement to print
	string returnVal;
	// used to handle arithmetic scheme code where only one operator is seen
	// with more than 2 operands.  Corresponding code will then know to repeat
	// the seen operator for the extra operands in the list
	int numLitsSeen;
	string savedOp; // used with numLitsSeen, stores the operator we saw previously
	// if we're looking at a line of scheme code that isn't enclosed in a call to
	// (display ...) and we encounter arithmetic operators, then it's a line of
	// code/calculations we'll want to assign the result to returnVal for
	bool assignToReturnVal;
	// used with returnVal to determine when something within an if-else stmt needs
	// to be assigned to returnVal
	bool comingFromIfElse;
	bool rparen;
	bool cameFromCond;
};
	
#endif
