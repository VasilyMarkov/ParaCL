#pragma once
#include <memory>
#include "grammar.tab.hh"
#include <FlexLexer.h>
#include "ast.hpp"

namespace yy {

class NumDriver {
  FlexLexer* plex_;
  std::shared_ptr<entryNode> ast_;
public:
  NumDriver(FlexLexer* plex): plex_(plex) {ast_ = std::make_shared<entryNode>();}

  parser::token_type yylex(parser::semantic_type *yylval) {
    
    parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
    if (tt == yy::parser::token_type::NUMBER) {
      yylval->as<int>() = std::stoi(plex_->YYText());
    }
    if (tt == yy::parser::token_type::ID) {
      // std::string str = plex_->YYText();
      // std::cout << str << ' ' << plex_->YYLeng() << std::endl;
      // yylval->as<std::string>() = plex_->YYText(); 
        std::string name(plex_->YYText());
        parser::semantic_type tmp;
        tmp.as<std::string>() = name;
        yylval->swap<std::string>(tmp);
    }
    return tt;
  }

  std::shared_ptr<entryNode> getAst() const { return ast_;};

  void evalAst() const {
    ast_->eval();
  }

  void dumpAst() const {
    ast_->dump();
  }

  bool parse() {
    parser parser(this);
    bool res = parser.parse();
    return !res;
  }

};

} 