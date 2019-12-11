//
// Created by kpfro on 12/10/2019.
//

#include <utility>

#include "interpreter.h"

class ValuePrinter : public RecordValueVisitor {
  void visit(NumberValue& el) override {
    std::cout << el.value << std::endl;
  }
  void visit(BooleanValue& el) override {
    std::cout << (el.value ? "TRUE" : "FALSE") << std::endl;
  }
  void visit(ASTValue& el) override {
    std::cout << "AST" << std::endl;
  }
};

void ActivationRecord::print() {
  ValuePrinter printer;
  for(const auto & item : members) {
    std::cout << item.first << " ";
    item.second->accept(printer);
//    std::cout << item.first << " " << item.second << std::endl;
  }
}

std::shared_ptr<RecordValue> ActivationRecord::get(std::string name) {
  // TODO: error if not found
  if (members.find(name) == members.end() && parent != nullptr) {
    parent->get(name);
  }
  return members[name];
}

void ActivationRecord::set(std::string name, bool value) {
  members[name] = std::shared_ptr<RecordValue>(new BooleanValue(value));
}

void ActivationRecord::set(std::string name, int value) {
  members[name] = std::shared_ptr<RecordValue>(new NumberValue(value));
}

void ActivationRecord::set(std::string name, FunctionDecl value) {
  members[name] = std::shared_ptr<RecordValue>(new ASTValue(value));
}