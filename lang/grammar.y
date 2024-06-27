
%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define parse.error custom
%param {yy::Driver* driver}

%locations

%code requires
{
#include <iostream>
#include <string>
#include <ast.hpp>

namespace yy { class Driver; }
using namespace ast;
}

%code
{
#include "driver.hpp"

namespace yy {
  parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* location, Driver* driver);
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
  INPUT   "?"
  OUTPUT  "print"
  ERR
;

%token <int> NUMBER
%token <std::string> ID
%nterm <std::unique_ptr<iNode>> term factor expr 
%nterm <std::unique_ptr<iNode>> expr_statement if_statement while_statement statement statements     

%left '+' '-' 
%left '*' '/' 
%nonassoc  UMINUS
%nonassoc  UNOT

%start program

%%

program: statements {driver->setAST(std::move($1));}

statements:   statement             {$$ = std::move($1);}
            | statements statement  {$$ = newScope(std::move($1), std::move($2));}
;

statement:  expr_statement    {$$ = std::move($1);}
          | if_statement      {$$ = std::move($1);}
          | while_statement   {$$ = std::move($1);}
;

if_statement:   IF "(" expr ")" "{" statements "}" {$$ = newIf(std::move($3), std::move($6), nullptr);}
              | IF "(" expr ")" "{" statements "}" ELSE "{" statements "}" {$$ = newIf(std::move($3), std::move($6), std::move($10));}
;

while_statement: WHILE "(" expr ")" "{" statements "}" {$$ = newWhile(std::move($3), std::move($6));}

expr_statement: expr ";" {$$ = std::move($1);}   

expr:    term "+"  expr            {$$ = newArith(arith_t::PLUS, std::move($1), std::move($3));}
      |  term "-"  expr            {$$ = newArith(arith_t::MINUS, std::move($1), std::move($3));}  
      |  term ">"  expr            {$$ = newPred(pred_t::GR, std::move($1), std::move($3));}  
      |  term ">=" expr            {$$ = newPred(pred_t::GRE, std::move($1), std::move($3));}  
      |  term "<"  expr            {$$ = newPred(pred_t::LW, std::move($1), std::move($3));}  
      |  term "<=" expr            {$$ = newPred(pred_t::LWE, std::move($1), std::move($3));}  
      |  term "==" expr            {$$ = newPred(pred_t::EQ, std::move($1), std::move($3));}  
      |  term "!=" expr            {$$ = newPred(pred_t::NEQ, std::move($1), std::move($3));}    
      |  term "&&" expr            {$$ = newPred(pred_t::AND, std::move($1), std::move($3));}    
      |  term "||" expr            {$$ = newPred(pred_t::OR, std::move($1), std::move($3));}    
      |  term "="  expr            {$$ = newAssign(std::move($1), std::move($3));} 
      |  "-" expr %prec UMINUS     {$$ = newMinus(std::move($2));} 
      |  "!" expr %prec UNOT       {$$ = newNot(std::move($2));} 
      |  "print" ID                {$$ = newOutput($2);} 
      |  term                      {$$ = std::move($1);}       
;

term :  term "*" factor              {$$ = newArith(arith_t::MULT, std::move($1), std::move($3));}   
      | term "/" factor              {$$ = newArith(arith_t::DIV, std::move($1), std::move($3));}   
      | factor                       {$$ = std::move($1);}
;    

factor :  NUMBER                      {$$ = newNumber($1);}
        | "(" expr ")"                {$$ = std::move($2);}
        | ID                          {$$ = newVar($1);}
        | "?"                         {$$ = newInput();}
;  

%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, Driver* driver) 
{
  return driver->yylex(yylval, location);
}

void parser::error(const parser::location_type& location, const std::string& message){
  std::cerr << message << " in line " << location.begin.line << std::endl;
}

void yy::parser::report_syntax_error(const yy::parser::context& context) const {
  driver->report_syntax_error(context);
}

}
