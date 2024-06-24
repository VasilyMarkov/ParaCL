%option c++

%{

#include "grammar.tab.hh"

%}

WS      [ \t\n\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
ID      [a-zA-Z_][a-zA-Z0-9_]*
IF      "if"
ELSE    "else"
WHILE   "while"
INPUT   "?"
OUTPUT  "print"
%%

{WS}              /* skip blanks and tabs */
{IF}              return yy::parser::token_type::IF;
{ELSE}            return yy::parser::token_type::ELSE;
{WHILE}           return yy::parser::token_type::WHILE;
{INPUT}           return yy::parser::token_type::INPUT;
{OUTPUT}          return yy::parser::token_type::OUTPUT;
"+"               return yy::parser::token_type::PLUS;
"-"               return yy::parser::token_type::MINUS;
"*"               return yy::parser::token_type::MULT;
"/"               return yy::parser::token_type::DIV;
"="               return yy::parser::token_type::ASSIGN;
">"               return yy::parser::token_type::GR;
">="              return yy::parser::token_type::GRE;
"<"               return yy::parser::token_type::LW;
"<="              return yy::parser::token_type::LWE;
"=="              return yy::parser::token_type::EQ;
"!="              return yy::parser::token_type::NEQ;
"&&"              return yy::parser::token_type::AND;
"||"              return yy::parser::token_type::OR;
"!"               return yy::parser::token_type::NOT;
"("               return yy::parser::token_type::LBRAC;
")"               return yy::parser::token_type::RBRAC;
"{"               return yy::parser::token_type::LBRACE;
"}"               return yy::parser::token_type::RBRACE;
";"               return yy::parser::token_type::SCOLON;
{DIGIT1}{DIGIT}*  return yy::parser::token_type::NUMBER;
{DIGIT}           return yy::parser::token_type::NUMBER;
{ID}              return yy::parser::token_type::ID;
.			      return yy::parser::token_type::ERR;

%%