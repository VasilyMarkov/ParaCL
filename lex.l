%option c++

%{

#include "grammar.tab.hh"

%}

WS      [ \t\n\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
%%

{WS}              /* skip blanks and tabs */
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
"("               return yy::parser::token_type::LBRAC;
")"               return yy::parser::token_type::RBRAC;
";"               return yy::parser::token_type::SCOLON;
{DIGIT1}{DIGIT}*  return yy::parser::token_type::NUMBER;
.			      return yy::parser::token_type::ERR;

%%