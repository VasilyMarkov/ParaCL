#include <iostream>
#include <memory>
#include "driver.hpp"
#include "ast.hpp"
int yyFlexLexer::yywrap() { return 1; }

int main() {
  FlexLexer *lexer = new yyFlexLexer;
  yy::NumDriver driver(lexer);
  
  driver.parse();
  driver.dumpAst();
  driver.evalAst();
  print(var_store);
  delete lexer;
  return 0;
}