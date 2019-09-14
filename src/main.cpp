#include "interpreter.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

int main() {
  std::ifstream programFile ("/home/kpfromer/programming/cpp/interpreter/program.pascal");
  std::vector<std::string> lines;
  std::string line;
  if (programFile.is_open()) {
    while (std::getline(programFile, line)) {
      lines.emplace_back(line);
      std::cout << line << std::endl;
    }

    interpreter::Parser parser (lines);
    interpreter::PrintVisitor printVisitor;
    interpreter::Interpreter interpreter1;
    interpreter::AST* node = parser.parse();
    node->accept(interpreter1);
    std::cout << interpreter1.getVal() << std::endl;
//    node->accept(printVisitor);
//    std::cout << printVisitor.getVal();
  } else {
    std::cout << "No File" << std::endl;
  }








//  lines.emplace_back("BEGIN BEGIN number := 2; a := number; b := 10 * a + 10 * number / 4; c := a - - b END; x := 11; END.");




  return 0;
}