#pragma once
#include <fstream>
#include <memory>
#include <algorithm>
#include "grammar.tab.hh"
#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif
#include "ast.hpp"

namespace yy {

class Lexer: public yyFlexLexer {
  yy::parser::location_type current_location_;
  std::size_t current_line_;
public:
  Lexer():current_location_(0), current_line_(1){}
  
  yy::parser::location_type getLocation() const noexcept {return current_location_;}

  std::size_t getCurrentLine() const noexcept {return current_line_;}

  void updateLocation() {
    std::size_t line = lineno(); 
    current_location_.lines(line - current_line_);
    current_location_.step();
    if (line - current_line_ == 0) {
      current_location_.columns(YYLeng());
    }
    current_line_ = line;
  }

  int yylex() override;
};

class Driver {
  std::unique_ptr<Lexer> lexer_ = nullptr;
  std::unique_ptr<ast::iNode> ast_ = nullptr;
  EvalVisitor evaluator_;
  DumpVisitor dumper_;
public:
  Driver(std::ifstream& file): lexer_(std::make_unique<Lexer>()) {
    lexer_->switch_streams(file, std::cout);
  }
  
  parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location) {
    
    parser::token_type tt = static_cast<parser::token_type>(lexer_->yylex());
    if (tt == yy::parser::token_type::NUMBER) {
      yylval->as<int>() = std::stoi(lexer_->YYText());
    }
    if (tt == yy::parser::token_type::ID) {
        std::string name(lexer_->YYText());
        parser::semantic_type tmp;
        tmp.as<std::string>() = name;
        yylval->swap<std::string>(tmp);
    }
    *location = lexer_->getLocation ();
    return tt;
  }
  
  void report_syntax_error(const parser::context& context) const {
    size_t lineno = lexer_->getCurrentLine();
    yy::parser::location_type location = context.location();
    size_t end_column = location.end.column;

    constexpr const int token_max = 10;
    yy::parser::symbol_kind_type expected[token_max];
    int num = context.expected_tokens(expected, token_max);

    std::cerr << lineno << ":" << end_column << ":" << " error: " << "expected \"" << yy::parser::symbol_name(expected[0]) << "\" \n";

    for (int i = 1; i < num; ++i)
    {
        std::cout << " or \"" << yy::parser::symbol_name(expected[i]) << "\" \n";
    }

    throw std::runtime_error("Compilation failed!");
  }

  void setAST(std::unique_ptr<ast::iNode> ast) noexcept {ast_ = std::move(ast);}

  bool parse() {
    parser parser(this);
    bool res = parser.parse();
    if (!res) { 
      ast_->eval(evaluator_);
      // ast_->eval(dumper_);
    }
    return !res;
  }

};

} 