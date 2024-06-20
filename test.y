%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%param {yy::NumDriver* driver}

%code requires
{
#include <iostream>
#include <string>
#include <ast.hpp>

namespace yy { class NumDriver; }

}

%code
{
#include "driver.hpp"

namespace yy {
parser::token_type yylex(parser::semantic_type* yylval, NumDriver* driver);
}

}

%token
  ASSIGN  "="
  PLUS    "+"
  MINUS   "-"
  MULT    "*"
  GR      ">"
  GRE     ">="
  LW      "<"
  LWE     "<="
  EQ      "!="
  NEQ     "=="
  DIV     "/"
  LBRAC   "("
  RBRAC   ")"
  LBRACE   "{"
  RBRACE   "}"
  SCOLON  ";"
  IF      "if"
  ERR
;

%token <int> NUMBER
%token <std::string> ID
%nterm <std::shared_ptr<iNode>> statements stmt expr

%start program

%%

program: statements {dumpTree($1);}

statements: stmt 
            | statements stmt {$$ = newStmt($1, $2);}

stmt: expr ";" {$$ = $1;}   

expr:   NUMBER                  {$$ = newNumber($1);}   
      | expr "+" NUMBER         {$$ = newArith(arith_t::PLUS, $1, newNumber($3));}                            
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         NumDriver* driver)
{
  return driver->yylex(yylval);
}

void parser::error(const std::string&){}
}