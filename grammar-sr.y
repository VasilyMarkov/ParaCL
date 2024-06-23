
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
  EQ      "=="
  NEQ     "!="
  AND     "&&"
  OR      "||"
  NOT     "!"
  DIV     "/"
  LBRAC   "("
  RBRAC   ")"
  LBRACE   "{"
  RBRACE   "}"
  SCOLON  ";"
  IF      "if"
  ELSE    "else"
  WHILE   "while"
  ERR
;

%token <int> NUMBER
%token <std::string> ID
%nterm <std::shared_ptr<iNode>> term factor statements statement expr_statement if_statement while_statement expr

%left '+' '-' 
%left '*' '/' 
%nonassoc  UMINUS
%nonassoc  UNOT

%start program

%%

program: statements {$1->eval(); dumpTree($1); print(var_store);}

statements:   statement
            | statements statement  {$$ = newScope($1, $2);}

statement:  expr_statement   
          | if_statement  
          | while_statement
;

if_statement:   IF "(" expr ")" "{" statements "}" {$$ = newIf($3, $6, nullptr);}
              | IF "(" expr ")" "{" statements "}" ELSE "{" statements "}" {$$ = newIf($3, $6, $10);}

while_statement: WHILE "(" expr ")" "{" statements "}" {$$ = newWhile($3, $6);}


expr_statement:   expr ";" {$$ = $1;}    

expr:      term "+"  expr            {$$ = newArith(arith_t::PLUS, $1, $3);}
        |  term "-"  expr            {$$ = newArith(arith_t::MINUS, $1, $3);}  
        |  term ">"  expr            {$$ = newPred(pred_t::GR, $1, $3);}  
        |  term ">=" expr           {$$ = newPred(pred_t::GRE, $1, $3);}  
        |  term "<"  expr            {$$ = newPred(pred_t::LW, $1, $3);}  
        |  term "<=" expr           {$$ = newPred(pred_t::LWE, $1, $3);}  
        |  term "==" expr           {$$ = newPred(pred_t::EQ, $1, $3);}  
        |  term "!=" expr           {$$ = newPred(pred_t::NEQ, $1, $3);}    
        |  term "&&" expr           {$$ = newPred(pred_t::AND, $1, $3);}    
        |  term "||" expr           {$$ = newPred(pred_t::OR, $1, $3);}    
        |  term "="  expr            {$$ = newAssign($1, $3);} 
        |  term                                  
;

term : term "*" factor              {$$ = newArith(arith_t::MULT, $1, $3);}   
     | term "/" factor              {$$ = newArith(arith_t::DIV, $1, $3);}   
     | factor
;    

factor :  NUMBER                      {$$ = newNumber($1);}
        | "(" expr ")"                {$$ = $2;}
        |  ID                         {$$ = newVar($1);}
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
