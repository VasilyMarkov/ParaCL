#include <iostream>
#include <fstream>
#include <memory>
#include "driver.hpp"
#include <exception>

using namespace yy;

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
  try {
    Driver driver(file);
    driver.parse();
    print(ast::var_store);
  } 
  catch(std::exception& exp) {
    std::cout << exp.what() << std::endl;
  }
  file.close();
  return 0;
}