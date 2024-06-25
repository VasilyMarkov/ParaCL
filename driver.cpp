#include <iostream>
#include <fstream>
#include <memory>
#include "driver.hpp"

// int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char *argv[]) {

  if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
      return 1;
  }
  std::ifstream file (argv[1]);
	if (!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 0;
	}
  yy::Driver driver(file);
  driver.parse();
  file.close();
  return 0;
}