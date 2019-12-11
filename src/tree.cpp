#include <utility>
#include "interpreter.h"

Num::Num(int value): value(value) {}
Boolean::Boolean(bool value): value(value) {}
Var::Var(std::string val): name(std::move(val)) {}

UnaryOp::UnaryOp(Token op, std::shared_ptr<AST> node): op(std::move(op)), node(std::move(node)) {}

BinaryOp::BinaryOp(std::shared_ptr<AST> left, Token op, std::shared_ptr<AST> right): op(std::move(op)) {
  this->left = std::move(left);
  this->right = std::move(right);
}

Program::Program(std::shared_ptr<AST> programName, std::shared_ptr<AST> block): name(std::move(programName)), block(std::move(block)) {}

Block::Block(std::vector<std::shared_ptr<AST>> declarations, std::shared_ptr<AST> compoundStatement): declarations(std::move(declarations)), compoundStatement(std::move(compoundStatement)) {}

Assign::Assign(std::shared_ptr<AST> left, std::shared_ptr<AST> right): left(std::move(left)), right(std::move(right)) {}