/********************************************************************************/
/* CS460 P3 - Team1: Justin Moore, Robert Hubbell, Justin Bernard               */
/********************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include "SyntacticalAnalyzer.h"
#include "CodeGenerator.h"

using namespace std;

/********************************************************************************/
/* Function: SyntacticalAnalyzer constructor                                    */
/* Parameters: char * filename                                                  */
/* Return type: None                                                            */
/* Description: Reads in the filename to set up output file name with           */
/* appropriate filetype extension.  Also sets up LexicalAnalyzer and CodeGen    */
/* with the filename, and sets some private variables                           */
/********************************************************************************/
SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
  string name = filename;
  lex = new LexicalAnalyzer (filename);
  gen = new CodeGen (name);
  string p2name = name.substr (0, name.length()-3) + ".p2";
  p2file.open(p2name);
  token_type t;
  numStmtCalls = 0;
  numTabs = 0;
  int ifCalls= 0;
  squote = false; // used with lparen to determine how to arrange quoted lits
  lparen = false; // if an lparen is found, quoted lit will be one big list, else not
  returnVal = ""; // variable used to hold value of what a function will be returning
  numLitsSeen = 0; // tracks NUMLITs to determine how to arrange arithmetic operations
  savedOp = ""; // used with numLitsSeen, variable to hold the operator last seen
  assignToReturnVal = true; // when DISPLAY is encountered, this will be set false
  int errors = program();
}

/********************************************************************************/
/* Function: SyntacticalAnalyzer destructor                                     */
/* Parameters: None                                                             */
/* Return type: None                                                            */
/* Description: Delete lex and gen                                              */
/********************************************************************************/
SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{
  delete lex;
  delete gen;
}

/********************************************************************************/
/* Function: program                                                            */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Looks for the first LPAREN to begin grammar checking and reports*/
/* an error where appropriate                                                   */
/********************************************************************************/
int SyntacticalAnalyzer::program()
{
  int errors = 0;
  token = lex->GetToken();
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Program function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  // apply rule 1
  if (token == LPAREN_T)
    {
      p2file << "Using Rule 1\n";
      token = lex->GetToken();
      errors += define();
      if (token == LPAREN_T)
	{
	  token = lex->GetToken();
	  errors += more_defines();
	}
      else
	{
	  lex->ReportError ("left parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
	  errors++;
	}
      if (token == EOF_T)
	{
	  // do nothing 
	}
      else
	{
	  lex->ReportError ("EOF token expected, '" + lex->GetTokenName(token) + "' found.");
          errors++;
	}
    }
  else
    {
      lex->ReportError ("left parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Program function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: more_defines                                                       */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for more_defines               */
/********************************************************************************/
int SyntacticalAnalyzer::more_defines()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering More_Defines function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == DEFINE_T)
    { // apply rule 2
      p2file << "Using Rule 2\n";
      errors += define();
      if (token == LPAREN_T)
	{
	  token = lex->GetToken();
	  errors += more_defines();
	}
      else
	{
	  lex->ReportError ("left parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
          errors++;
	}
    }
 
  else if (token == IDENT_T)
    { // apply rule 3
      p2file << "Using Rule 3\n";
      token = lex->GetToken();
      numStmtCalls = 0;
      errors += stmt_list("");
      if (token == RPAREN_T)
	token = lex->GetToken();
      else
	{
	  lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
          errors++;
	}
    }

  if (token == EOF_T)
    { // apply rule 1
    }
  else
    {
      lex->ReportError ("IDENT_T or DEFINE_T expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
      }
  tok = lex->GetTokenName(token);
  p2file << "Exiting More_Defines function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: define                                                             */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for define                     */
/********************************************************************************/
int SyntacticalAnalyzer::define()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Define function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == DEFINE_T)
    { // apply rule 4
      p2file << "Using Rule 4\n";
      token = lex->GetToken();
      if (token == LPAREN_T)
	{
	  token = lex->GetToken();
	  if (token == IDENT_T)
	    {
	      lexeme = lex-> GetLexeme();
	      if(lexeme == "main")
		{
		  returnVal = "0";
		  gen->WriteCode(numTabs, "int " + lexeme + "(");
		}
	      else
		{
		  returnVal = "returnVal"; // changes here
		  gen->WriteCode(numTabs, "Object " + lexeme + "(");
		}
	      token = lex->GetToken();
	      errors += param_list(true);
	      if (token == RPAREN_T)
		{
		  gen->WriteCode(numTabs,")\n{\n");
		  numTabs++;
		  if (returnVal != "0") // changes here
		    gen->WriteCode(numTabs, "Object returnVal;\n"); // changes here
		  token = lex->GetToken();
		  numStmtCalls = 0;
		  errors += stmt();
		  //gen->WriteCode(numTabs, ";\n");
		  errors += stmt_list("");
		  //gen->WriteCode(numTabs, ";\n");
		  if (token == RPAREN_T)
		    {
		      gen->WriteCode(numTabs, "return " + returnVal + ";\n"); // changes here
		      numTabs--;
		      gen->WriteCode(numTabs, "}\n\n");
		      token = lex->GetToken();
		    }
		  else
		    {
		      lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
		      errors++;
		    }
		}
	      else
		{
		  lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
		  errors++;
		}
	    }
	  else
	    {
	      lex->ReportError ("IDENT_T token expected, '" + lex->GetTokenName(token) + "' found.");
	      errors++;
	    }
	}
      else
	{
	  lex->ReportError ("left parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
          errors++;
	}
    }
  else
    {
      lex->ReportError ("DEFINE_T token expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Define function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: stmt_list                                                          */
/* Parameters: string op                                                        */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for stmt_list                  */
/********************************************************************************/
int SyntacticalAnalyzer::stmt_list(string op)
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Stmt_List function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T
      || token == IDENT_T || token == LPAREN_T)
    { // apply rule 5
      p2file << "Using Rule 5\n";
      if (token == NUMLIT_T) // changes here
	numLitsSeen++;
      //errors += stmt();
      //gen->WriteCode(0, op);
      if (numLitsSeen <= 2) // changes here, added if-else
	{
	  errors += stmt();
	  gen->WriteCode(0, op);
	}
      else
	{
	  gen->WriteCode(0, savedOp);
	  errors += stmt();
	}
      errors += stmt_list(""); // changes here
    }
  else if (token == RPAREN_T)
    { // apply rule 6
      p2file << "Using Rule 6\n";
    }
  else
    {
      lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Stmt_List function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: stmt                                                               */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for stmt                       */
/********************************************************************************/
int SyntacticalAnalyzer::stmt()
{
  numStmtCalls++;
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Stmt function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T)
    { // apply rule 7
      p2file << "Using Rule 7\n";
      errors += literal();
    }
  else if (token == IDENT_T)
    { // apply rule 8
      p2file << "Using Rule 8\n";
      lexeme = lex->GetLexeme();
      //gen->WriteCode(0, lexeme + "()"); changes here
      gen->WriteCode(0, lexeme); // changes here
      token = lex->GetToken();
    }
  else if (token == LPAREN_T)
    { // apply rule 9
      p2file << "Using Rule 9\n";
      token = lex->GetToken();
      errors += action();
      
      if (token == RPAREN_T)
	{
	  token = lex->GetToken();
	}
      else
	{
	  lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
          errors++;
	}
    }
  else
    {
      lex->ReportError ("NUMLIT_T, STRLIT_T, SQUOTE_T, IDENT_T, or LPAREN_T expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  if (--numStmtCalls == 0)
    gen->WriteCode(0, ";\n");
  tok = lex->GetTokenName(token);
  p2file << "Exiting Stmt function; current token is: " << tok << endl;
  return errors;
}
/********************************************************************************/
/* Function: literal                                                            */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for literal                    */
/********************************************************************************/
int SyntacticalAnalyzer::literal()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Literal function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (numStmtCalls == 1)
    gen->WriteCode(1, "");
  if (token == NUMLIT_T)
    { // apply rule 10
      p2file << "Using Rule 10\n";
      gen->WriteCode(0, "Object(" + lexeme + ")");
      token = lex->GetToken();
    }
  else if (token == STRLIT_T)
    { // apply rule 11
      p2file << "Using Rule 11\n";
      gen->WriteCode(0, "Object(" + lexeme + ")");
      token = lex->GetToken();
    }
  else if (token == SQUOTE_T)
    { // apply rule 12
      p2file << "Using Rule 12\n";
      token = lex ->GetToken();
      squote = true;  // changes here
      errors += quoted_lit();
      squote = false;  // changes here
    }
  else
    {
      lex->ReportError ("NUMLIT_T, STRLIT_T or SQUOTE_T expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Literal function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: quoted_lit                                                         */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for quoted_lit                 */
/********************************************************************************/
int SyntacticalAnalyzer::quoted_lit()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Quoted_Lit function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token >= IDENT_T && token <= LPAREN_T || token == SQUOTE_T)
    { // apply rule 13
      p2file << "Using Rule 13\n";
      errors += any_other_token();
    }
  else
    {
      lex->ReportError ("any other token expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Quoted_Lit function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: more_tokens                                                        */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for more_tokens                */
/********************************************************************************/
int SyntacticalAnalyzer::more_tokens()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering More_Tokens function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token >= IDENT_T && token <= LPAREN_T || token == SQUOTE_T)
    { // apply rule 14
      p2file << "Using Rule 14\n";
      if (token == SQUOTE_T) // changes here
	squote = true;
      errors += any_other_token();
      errors += more_tokens();
      squote = false;
    }
  else if (token == RPAREN_T)
    { // apply rule 15
      p2file << "Using Rule 15\n";
    }
  else
    {
      lex->ReportError ("any other token expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting More_Tokens function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: param_list                                                         */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for param_list                 */
/********************************************************************************/
int SyntacticalAnalyzer::param_list(bool firstCall)
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Param_List function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == IDENT_T)
    { // apply rule 16
      p2file << "Using Rule 16\n";
      if (firstCall == true)
	gen->WriteCode(0, "Object " + lex->GetLexeme());
      else
	gen->WriteCode(0, ", Object " + lex->GetLexeme());
      token = lex->GetToken();
      errors += param_list(false);
    }
  else if (token == RPAREN_T)
    { // apply rule 17
      p2file << "Using Rule 17\n";
    }
  else
    {
      lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Param_List function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: else_part                                                          */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for else_part                  */
/********************************************************************************/
int SyntacticalAnalyzer::else_part()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Else_Part function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T
      || token == IDENT_T || token == LPAREN_T)
    { // apply rule 18
      p2file << "Using Rule 18\n";
      if (token == SQUOTE_T) // changes here
	squote = true;
      errors += stmt();
      squote = false;
    }
  else if (token == RPAREN_T)
    { // apply rule 19
      p2file << "Using Rule 19\n";
    }
  else
    {
      lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Else_Part function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: stmt_pair                                                          */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for stmt_pair                  */
/********************************************************************************/
int SyntacticalAnalyzer::stmt_pair()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Stmt_Pair function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == LPAREN_T)
    { // apply rule 20
      p2file<< "Using Rule 20\n";
      token = lex->GetToken();
      gen->WriteCode(numTabs, "else if(");
      errors += stmt_pair_body();
    }
  else if (token == RPAREN_T)
    { // apply rule 21
      p2file << "Using Rule 21\n";
    }
  else
    {
      lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Stmt_Pair function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: stmt_pair_body                                                     */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for stmt_pair_body             */
/********************************************************************************/
int SyntacticalAnalyzer::stmt_pair_body()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Stmt_Pair_Body function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T
      || token == IDENT_T || token == LPAREN_T)
    { // apply rule 22
      p2file << "Using Rule 22\n";
      if (token == SQUOTE_T) // changes here
	squote = true;
      errors += stmt();
      squote = false;
      gen->WriteCode(0, ")\n");
      gen->WriteCode(numTabs++, "{\n");
      gen->WriteCode(numTabs, "");
      if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T
	  || token == IDENT_T || token == LPAREN_T)
	{
	  if (token == SQUOTE_T) // changes here
	    squote = true;
	  errors += stmt();
	  squote = false;
	  gen->WriteCode(0, ";\n");
	  gen->WriteCode(--numTabs, "}\n");
	  if (token == RPAREN_T)
	    {
	      token = lex->GetToken();
      	      errors += stmt_pair();
	    }
	  else
	    {
	      lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
	    }
	}
      else
	{
	  lex->ReportError ("NUMLIT_T, STRLIT_T, SQUOTE_T, IDENT_T, or LPAREN_T expected, '" + lex->GetTokenName(token) + "' found.");
	  errors++;
	}
    }
  else if (token == ELSE_T)
    { // apply rule 23
      p2file << "Using Rule 23\n";
      token = lex->GetToken();
      gen->WriteCode(numTabs, "else\n");
      gen->WriteCode(numTabs++, "{\n");
      gen->WriteCode(numTabs, "");
      errors += stmt();
      gen->WriteCode(0, ";\n");
      gen->WriteCode(--numTabs, "}\n");
      if (token == RPAREN_T)
	{
	  token = lex->GetToken();
	}
    }
  else
    {
      lex->ReportError ("NUMLIT_T, STRLIT_T, SQUOTE_T, IDENT_T, LPAREN_T or ELSE_T expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Stmt_Pair_Body function; current token is: " << tok << endl;
  return errors;
}

/********************************************************************************/
/* Function: action                                                             */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for action                     */
/********************************************************************************/
int SyntacticalAnalyzer::action()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Action function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;
  numLitsSeen = 0;
  switch(token)
    {
      
    case IF_T:
      // apply rule 24
      p2file << "Using Rule 24\n";
      gen->WriteCode(numTabs, "if(");
      token = lex->GetToken();
      errors += stmt();
      gen->WriteCode(0, ")\n");
      gen->WriteCode(numTabs++, "{\n");
      numStmtCalls = 0;
      errors += stmt();
      gen->WriteCode(0, "\n");
      gen->WriteCode(--numTabs, "}\n");
      gen->WriteCode(numTabs, "else\n");
      gen->WriteCode(numTabs++, "{\n");
      errors += else_part();
      gen->WriteCode(0, ";\n");
      gen->WriteCode(--numTabs, "}\n");
      break;

    case COND_T:
      // apply rule 25
      p2file << "Using Rule 25\n";
      token = lex->GetToken();
      if (token == LPAREN_T)
	{
	  token = lex->GetToken();
	  gen->WriteCode(numTabs, "if(");
	  errors += stmt_pair_body();
	}
      else
	{
	  lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
	  errors++;
	}
      break;

    // apply rules 26, 30-35, 41, 48
    case LISTOP_T:
      p2file << "Using Rule 26\n";
      //gen->WriteCode(0, "(" + lexeme + " ("); // changes here
      gen->WriteCode(0, "listop (\"" + lexeme + "\", ");
      token = lex->GetToken();
      errors += stmt();
      gen->WriteCode(0, "))");
      break;
    case NOT_T:
      p2file << "Using Rule 30\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(!");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case NUMBERP_T:
      p2file << "Using Rule 31\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(numberp(");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case LISTP_T:
      p2file << "Using Rule 32\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(listp(");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case ZEROP_T:
      p2file << "Using Rule 33\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(zerop(");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case NULLP_T:
      p2file << "Using Rule 34\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(nullp(");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case STRINGP_T:
      p2file << "Using Rule 35\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(stringp(");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case ROUND_T:
      p2file << "Using Rule 41\n";
      token = lex->GetToken();
      gen->WriteCode(0, "round(");
      //gen->WriteCode(0, "(round("); // changes here
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case DISPLAY_T:
      p2file << "Using Rule 48\n";
      assignToReturnVal = false; // changes here
      gen->WriteCode(0, "cout << ");
      token = lex->GetToken();
      errors += stmt();
      break;

    // apply rules 27, 40
    case CONS_T:
      p2file << "Using Rule 27\n";
      token = lex->GetToken();
      if (assignToReturnVal)  // changes here
	gen->WriteCode(0, "returnVal = ");
      //gen->WriteCode(0, "(cons ");
      gen->WriteCode(0, "cons ("); // changes here
      errors += stmt();
      gen->WriteCode(0, ", "); // changes here
      errors += stmt();
      gen->WriteCode(0, ")");
      break;
    case MODULO_T:
      p2file << "Using Rule 40\n";
      token = lex->GetToken();
      if (assignToReturnVal) // changes here
        gen->WriteCode(0, "returnVal = ");
      gen->WriteCode(0, "(");
      errors += stmt();
      gen->WriteCode(0, " % ");
      errors += stmt();
      gen->WriteCode(0, ")");
      break;

    // apply rules 28, 29, 36, 39, 42-47
    case AND_T:
      p2file << "Using Rule 28\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" && ");
      gen->WriteCode(0, ")");
      break;
    case OR_T:
      p2file << "Using Rule 29\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" || ");
      gen->WriteCode(0, ")");
      break;
    case PLUS_T:
      p2file << "Using Rule 36\n";
      token = lex->GetToken();
      if (assignToReturnVal) // changes here
        gen->WriteCode(0, "returnVal = ");
      gen->WriteCode(0, "(");
      errors += stmt_list(" + ");
      gen->WriteCode(0, ")");
      break;
    case MULT_T:
      p2file << "Using Rule 39\n";
      token = lex->GetToken();
      if (assignToReturnVal) // changes here
	gen->WriteCode(0, "returnVal = ");
      gen->WriteCode(0, "(");
      savedOp = " * ";
      errors += stmt_list(" * ");
      gen->WriteCode(0, ")");
      break;
    case EQUALTO_T:
      p2file << "Using Rule 42\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" == ");
      gen->WriteCode(0, ")");
      break;
    case GT_T:
      p2file << "Using Rule 43\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" > ");
      gen->WriteCode(0, ")");
      break;
    case LT_T:
      p2file << "Using Rule 44\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" < ");
      gen->WriteCode(0, ")");
      break;
    case GTE_T:
      p2file << "Using Rule 45\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" >= ");
      gen->WriteCode(0, ")");
      break;
    case LTE_T:
      p2file << "Using Rule 46\n";
      token = lex->GetToken();
      gen->WriteCode(0, "(");
      errors += stmt_list(" <= ");
      gen->WriteCode(0, ")");
      break;
    case IDENT_T:
      p2file << "Using Rule 47\n";
      lexeme = lex->GetLexeme();
      gen->WriteCode(0, lexeme + "(");
      token = lex->GetToken();
      errors += stmt_list("");
      gen->WriteCode(0, ")");
      break;

    // apply rules 37, 38
    case MINUS_T:
      p2file << "Using Rule 37\n";
      token = lex->GetToken();
      if (assignToReturnVal) // changes here
        gen->WriteCode(0, "returnVal = ");
      gen->WriteCode(0, "(");
      errors += stmt();
      gen->WriteCode(0, " - ");
      errors += stmt_list("");
      gen->WriteCode(0, ")");
      break;
    case DIV_T:
      p2file << "Using Rule 38\n";
      token = lex->GetToken();
      if (assignToReturnVal) // changes here
        gen->WriteCode(0, "returnVal = ");
      gen->WriteCode(0, "(");
      errors += stmt();
      gen->WriteCode(0, " / ");
      errors += stmt_list("");
      gen->WriteCode(0, ")");
      break;

    case NEWLINE_T:
      // apply rule 49
      p2file << "Using Rule 49\n";
      gen->WriteCode(numTabs, "cout << endl");
      token = lex->GetToken();
      break;

    case RPAREN_T:
      // do nothing
      break;
      
    default:
      lex->ReportError ("action token expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Action function; current token is: " << tok << endl;
  numLitsSeen = 0; // changes here
  assignToReturnVal = true; // changes here
  return errors;
}

/********************************************************************************/
/* Function: any_other_token                                                    */
/* Parameters: None                                                             */
/* Return type: int                                                             */
/* Description: Follows and checks grammar rules for any_other_token            */
/********************************************************************************/
int SyntacticalAnalyzer::any_other_token()
{
  string tok = lex->GetTokenName(token), lexeme = lex->GetLexeme();
  p2file << "Entering Any_Other_Token function; current token is: " << tok << ", lexeme: " << lexeme << endl;
  int errors = 0;

  if (squote && token == LPAREN_T)
    lparen = true;
  if (token == LPAREN_T)
    { // apply rule 50
      p2file << "Using Rule 50\n";
      token = lex->GetToken();
      lexeme = lex->GetLexeme();
      //gen->WriteCode(0,"(\"" + lexeme); made change here, below
      gen->WriteCode(0,"\"(" + lexeme);
      if (squote)
	token = lex->GetToken();
      errors += more_tokens();
      //gen->WriteCode(0, ")"); made changes below too
      gen->WriteCode(0, ")\"");
      lparen = false;
      if (token == RPAREN_T)
	{
	  token = lex->GetToken();
	}
      else
	{
	  lex->ReportError ("right parenthesis expected, '" + lex->GetTokenName(token) + "' found.");
	  errors++;
	}
    }
  else if (token >= IDENT_T && token <= LTE_T)
    { // apply rules 51-78, 80, 81
      if(token >= IDENT_T && token <= IF_T)
	p2file << "Using Rule " << token + 50 << endl;
      else if(token == COND_T)
	p2file << "Using Rule 80\n";
      else if(token >= DISPLAY_T && token <= STRINGP_T)
	p2file << "Using Rule " << token + 49 << endl;
      else if(token == MODULO_T || token == ROUND_T)
	p2file << "Using Rule " << token + 53 << endl;
      else if(token == ELSE_T)
	p2file << "Using Rule 81\n";
      else if(token >= PLUS_T && token <= MULT_T)
	p2file << "Using Rule " << token + 46 << endl;
      else if(token >= EQUALTO_T && token <= LTE_T)
	p2file << "Using Rule " << token + 48 << endl;
      lexeme = lex->GetLexeme();
      if (squote && lparen)
	gen->WriteCode(0, " " + lexeme);
      else
	gen->WriteCode(numTabs, "(\"" + lexeme + "\")"); // changes here, and above
      token = lex->GetToken();
    }
  else if (token == SQUOTE_T)
    { // apply rule 79
      p2file << "Using Rule 79\n";
      token = lex->GetToken();
      squote = true;
      errors += any_other_token();
      squote = false;
    }
  else
    {
      lex->ReportError ("any other token expected, '" + lex->GetTokenName(token) + "' found.");
      errors++;
    }
  tok = lex->GetTokenName(token);
  p2file << "Exiting Any_Other_Token function; current token is: " << tok << endl;
  return errors;
}
