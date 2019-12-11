#include "interpreter.h"
#include<vector>
#include<iostream>
#include<fstream>

std::vector<std::string> readLines(const std::string& filename) {
  std::vector<std::string> lines;
  std::ifstream file (filename);
  std::string line;
  while(std::getline(file, line)) {
    // TODO: remove tabs
    lines.push_back(line);
  }
  return lines;
}

int main() {
  auto lines = readLines(R"(C:\Users\kpfro\CLionProjects\Interpreter\programs\first.pas)");
//  std::vector<std::string> lines;
//  lines.emplace_back("PROGRAM test; BEGIN x:=3; x:=x*x+x; y := (x+3)/3 END.");

//  std::cout << "Start" << std::endl;
//  Lexer lexer (lines);
//  for (Token token = lexer.getNextToken(); token.type != Token::Type::END_OF_FILE; token = lexer.getNextToken()) {
//    std::cout << token.value << std::endl;
//  }

  PrintVisitor printer;

  Parser parser (lines);
  parser.program()->accept(printer);

  printer.print();

  std::cout << printer.value << std::endl;
}