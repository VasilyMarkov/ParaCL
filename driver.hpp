#pragma once
#include <fstream>
#include <memory>
#include "grammar.tab.hh"
#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif
#include "ast.hpp"
// #include "ast_small.hpp"

namespace yy {

class Lexer: public yyFlexLexer {
public:
  Lexer(){}
  int yylex() override;
};

class Driver {
  std::unique_ptr<Lexer> lex_ = nullptr;
  std::unique_ptr<iNode> ast_ = nullptr;
  evalVisitor evaluator_;
  bool isParsed = false;
public:
  Driver(std::ifstream& file): lex_(std::make_unique<Lexer>()) {
    lex_->switch_streams(file, std::cout);
  }
  
  parser::token_type yylex(parser::semantic_type *yylval) {
    
    parser::token_type tt = static_cast<parser::token_type>(lex_->yylex());
    if (tt == yy::parser::token_type::NUMBER) {
      yylval->as<int>() = std::stoi(lex_->YYText());
    }
    if (tt == yy::parser::token_type::ID) {
        std::string name(lex_->YYText());
        parser::semantic_type tmp;
        tmp.as<std::string>() = name;
        yylval->swap<std::string>(tmp);
    }

    return tt;
  }
  
  void setAST(std::unique_ptr<iNode> ast) noexcept {ast_ = std::move(ast);}

  void eval() {
    if (isParsed) {
      ast_->eval(evaluator_);
    }
    else {
      std::cout << "AST not yet or correct parsed" << std::endl;
    }
  }

  bool parse() {
    parser parser(this);
    bool res = parser.parse();
    isParsed = !res;
    return !res;
  }

};

} 