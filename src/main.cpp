#include "interpreter.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class Evaluator : public interpreter::ValueGetter<interpreter::AST*, int> {
public:
  std::map<std::string, int> symbolicTable;
  void visit(interpreter::Num & n) override {
    std::cout << "Return num value " << n.value << std::endl;
    Return(n.value);
  }

  void visit(interpreter::BinaryOp& n) override {
    // GetValue visits children and returns computed values
    if (n.op.type == interpreter::MULTIPLY) {
      Return(GetValue(n.left) * GetValue(n.right));
    } else if (n.op.type == interpreter::DIVIDE) {
      Return(GetValue(n.left) / GetValue(n.right));
    } else if (n.op.type == interpreter::PLUS) {
      Return(GetValue(n.left) + GetValue(n.right));
    } else if (n.op.type == interpreter::MINUS) {
      Return(GetValue(n.left) - GetValue(n.right));
    }
  }

  void visit(interpreter::UnaryOp& op) override {
    int v = GetValue(op.node);
    if (op.op.type == interpreter::MINUS) {
      Return(-v);
    } else {
      Return(v);
    }
  }

  void visit(interpreter::Compound & node) override {
    for (interpreter::AST* sub : node.children) {
      sub->accept(*this);
    }
  }
  void visit(interpreter::Assign & node) override {
    int val = GetValue(node.right);
    std::cout << "Set " << node.left->value.name << " = " << val << std::endl;
    symbolicTable[node.left->value.name] = GetValue(node.right);
  };
  void visit(interpreter::Var & node) override {
    // TODO: check node.value.name exists
    std::cout << "Getting " << node.value.name << " value: " << symbolicTable[node.value.name] << std::endl;
    Return(symbolicTable[node.value.name]);
//    return
  }
  void visit(interpreter::NoOp & node) override {}

  static std::string getVal(interpreter::AST* node) {
    Evaluator vis;
    node->accept(vis); // this call fills the return value
//    return vis.value;
    for( auto const& [key, val] : vis.symbolicTable )
    {
      std::cout << key         // string (key)
                << ':'
                << val        // string's value
                << std::endl ;
    }
    return "";
  }
};

int main() {

//  std::vector<std::string> test;
//  test.emplace_back("4 * 2 + 3");
//  interpreter::Parser p1 (test);
//  interpreter::AST* node = p1.term();
//  std::cout << Evaluator::GetValue(node) << std::endl;

  std::ifstream programFile ("/home/kpfromer/programming/cpp/interpreter/program.pascal");
  std::vector<std::string> lines;
  std::string line;
  if (programFile.is_open()) {
    while (std::getline(programFile, line)) {
      lines.emplace_back(line);
    }

    interpreter::Parser parser (lines);
    interpreter::PrintVisitor printVisitor;
    interpreter::Interpreter interpreter1;

    interpreter::AST* node = parser.parse();

    Evaluator eval1;

    std::cout << Evaluator::getVal(node) << "\nDONE" << std::endl;

//    std::cout << Evaluator::getVal(node) << std::endl;


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