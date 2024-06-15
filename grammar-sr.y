
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

parser::token_type yylex(parser::semantic_type* yylval,                         
                         NumDriver* driver);
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
  SCOLON  ";"
  ERR
;

%token <int> NUMBER
%token <std::string> ID
%nterm <std::shared_ptr<iNode>> expr term factor variable

%left '+' '-' 
%left '*' '/' 
%left UMINUS

%start programm

%%

programm: lines;

lines: line | lines line;

line: statement '\n' | '\n'; 

statement:  expr SCOLON {
                  #ifdef DEBUG_GRAMMAR
                    dumpTree($1, 0);  
                  #else
                    std::cout << eval($1) << std::endl;
                    print(var_store);
                  #endif
            }
;

expr:      expr "+" term            { $$ = newArith(arith_t::PLUS, $1, $3); }
        |  expr "-" term            { $$ = newArith(arith_t::MINUS, $1, $3); }  
        |  expr ">" term            { $$ = newPred(pred_t::GR, $1, $3); }  
        |  expr ">=" term           { $$ = newPred(pred_t::GRE, $1, $3); }  
        |  expr "<" term            { $$ = newPred(pred_t::LW, $1, $3); }  
        |  expr "<=" term           { $$ = newPred(pred_t::LWE, $1, $3); }  
        |  expr "==" term           { $$ = newPred(pred_t::EQ, $1, $3); }  
        |  expr "!=" term           { $$ = newPred(pred_t::NEQ, $1, $3); }    
        |  variable "=" expr        { $$ = newAssign($1, $3);} 
        |  term                                  
;

term : term "*" factor             { $$ = newArith(arith_t::MULT, $1, $3); }   
     | term "/" factor              { $$ = newArith(arith_t::DIV, $1, $3); }   
     | factor
;    

factor :  NUMBER                    { $$ = newNumber($1); }
        | "(" expr ")"          { $$ = $2; }
        | "MINUS" NUMBER %prec UMINUS { $$ = newNumber(-$2); }  
;

variable: ID {$$ = newVar($1);} 

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         NumDriver* driver)
{
  return driver->yylex(yylval);
}

void parser::error(const std::string&){}
}