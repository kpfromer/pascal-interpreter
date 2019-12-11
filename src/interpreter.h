//
// Created by kpfro on 12/9/2019.
//

#ifndef INTERPRETER_INTERPRETER_H
#define INTERPRETER_INTERPRETER_H

#include <memory>
#include <utility>
#include<vector>
#include<string>
#include <memory>
#include <iostream>
#include<map>
#include "stack.h"

// Helper

bool isDigit(const char & value);

// Core

class Token {
public:
  enum Type {
    PROGRAM,
    BEGIN,
    END,
    DOT,
    SEMI,
    COLON,
    COMMA,
    VAR,

    ASSIGN,

    EQUAL,
    NOT_EQUAL,
    AND,
    OR,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    NOT,
    PLUS,
    MINUS,
    MULTPLY,
    DIVIDE,
    LEFT_PAREN,
    RIGHT_PAREN,

    ID,
    TRUE,
    FALSE,
    INTEGER_CONST,

    INTEGER,
    BOOLEAN,
    FUNCTION,
    PROCEDURE,

    END_OF_FILE
  };


  std::string name;
  Type type;
  int value = 0;
  Token(Type type);
  Token(Type type, int value);
  Token(std::string name, Type type, int value);
};

//std::map<std::string, Token> createReserved() {
//  std::map<std::string, Token> reserved;
//  reserved["TRUE"] = Token(Token::Type::TRUE, 1);
//  reserved["FALSE"] = Token(Token::Type::FALSE, 0);
//  return reserved;
//}

const std::map<std::string, Token> RESERVED_KEYWORDS = {
    {"TRUE", Token(Token::Type::TRUE, 1)},
    {"FALSE", Token(Token::Type::FALSE, 0)},
    {"PROGRAM", Token(Token::Type::PROGRAM)},
    {"BEGIN", Token(Token::Type::BEGIN)},
    {"END", Token(Token::Type::END)},
    {"VAR", Token(Token::Type::VAR)},
    {"BOOLEAN", Token(Token::Type::BOOLEAN)},
    {"INTEGER", Token(Token::Type::INTEGER)},
    {"FUNCTION", Token(Token::Type::FUNCTION)},
    {"PROCEDURE", Token(Token::Type::PROCEDURE)}
};

class Lexer {
private:
  std::vector<std::string> lines;
  int pos = 0;
  int line = 0;

  char peekChar();
public:
  explicit Lexer(std::vector<std::string> lines);
  bool advance();
  void skipWhitespace();
  void skipComment();
  bool isEmpty();
  void print();
  Token _id();
  Token getNextToken();
  Token peek(const int & count = 1);
  Token number();
};

class AST;

class Parser {
private:
  std::vector<std::string> lines;
  Lexer lexer;
public:
  Parser(std::vector<std::string> lines);
  Token eat(Token::Type type);

  // Literals
  std::shared_ptr<AST> number();
  std::shared_ptr<AST> boolean();
  std::shared_ptr<AST> variable();

  // Expressions
  std::shared_ptr<AST> factor();
  std::shared_ptr<AST> multDivFactor();
  std::shared_ptr<AST> addSubFactor();
  std::shared_ptr<AST> relationalFactor();
  std::shared_ptr<AST> andFactor();
  std::shared_ptr<AST> orFactor();
  std::shared_ptr<AST> comparisonFactor();
  std::shared_ptr<AST> expression();

  // Program Structure
  std::shared_ptr<AST> program();
  std::shared_ptr<AST> compoundStatement();
  std::shared_ptr<AST> empty();
  std::shared_ptr<AST> block();

  // Statements
  std::vector<std::shared_ptr<AST>> statementList();
  std::shared_ptr<AST> statement();
  std::shared_ptr<AST> assignmentStatement();

//  std::shared_ptr<AST> ifStatement();
//  std::shared_ptr<AST> whileStatement();
  std::shared_ptr<AST> callStatement();

  // Function/Procedure Setup
  std::vector<std::shared_ptr<AST>> declarations();
  std::vector<std::shared_ptr<AST>> formalParameterList();
  std::vector<std::shared_ptr<AST>> formalParameter();
  std::vector<std::shared_ptr<AST>> variableDeclaration();
  std::shared_ptr<AST> typeSpec();

};

class AST;
class Num;
class Boolean;
class Var;

class UnaryOp;
class BinaryOp;
class Program;
class Compound;
class Assign;
class NoOp;
class Block;
class VarDecl;
class Type;
//class ProcedureDecl;
class FunctionDecl;
class Param;
//class IfAST;
//class WhileAST;
class Call;



class Visitor {
public:
//  virtual void visit(AST& el) = 0;
  virtual void visit(Num& el) = 0;
  virtual void visit(Boolean& el) = 0;
  virtual void visit(Var& el) = 0;
  virtual void visit(UnaryOp& el) = 0;
  virtual void visit(BinaryOp& el) = 0;

  virtual void visit(Program& el) = 0;
  virtual void visit(Compound& el) = 0;
  virtual void visit(Assign& el) = 0;
  virtual void visit(NoOp& el) = 0;
  virtual void visit(Block& el) = 0;

  virtual void visit(VarDecl& el) = 0;
  virtual void visit(Type& el) = 0;
//  virtual void visit(ProcedureDecl& el) = 0;
  virtual void visit(FunctionDecl& el) = 0;
  virtual void visit(Param& el) = 0;
  virtual void visit(Call& el) = 0;
};

class AST {
public:
  virtual void accept(Visitor& v) = 0;
};

class Num : public AST {
public:
  int value;
  Num(int value);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Boolean : public AST {
public:
  bool value;
  explicit Boolean(bool value);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Var : public AST {
public:
  std::string name;
  explicit Var(std::string name);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class UnaryOp : public AST {
public:
  Token op;
  std::shared_ptr<AST> node;
  UnaryOp(Token op, std::shared_ptr<AST> node);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class BinaryOp: public AST {
public:
  std::shared_ptr<AST> left;
  Token op;
  std::shared_ptr<AST> right;
  BinaryOp(std::shared_ptr<AST> left, Token op, std::shared_ptr<AST> right);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Program : public AST {
public:
  std::shared_ptr<AST> name;
  std::shared_ptr<AST> block;
  Program(std::shared_ptr<AST> programName, std::shared_ptr<AST> block);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Compound : public AST {
public:
  std::vector<std::shared_ptr<AST>> children;
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Block : public AST {
public:
  std::vector<std::shared_ptr<AST>> declarations;
  std::shared_ptr<AST> compoundStatement;
  Block(std::vector<std::shared_ptr<AST>> declarations, std::shared_ptr<AST> compoundStatement);

  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Assign : public AST {
public:
  std::shared_ptr<AST> left;
  std::shared_ptr<AST> right;
  Assign(std::shared_ptr<AST> left, std::shared_ptr<AST> right);
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class NoOp : public AST {
public:
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class VarDecl : public AST {
public:
  std::shared_ptr<AST> var;
  std::shared_ptr<AST> type;
  VarDecl(std::shared_ptr<AST> var, std::shared_ptr<AST> type): var(std::move(var)), type(std::move(type)) {}
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Type : public AST {
public:
  std::string name;
  explicit Type(std::string name): name(std::move(name)) {}
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Param : public AST {
public:
  std::shared_ptr<AST> var;
  std::shared_ptr<AST> type;
  Param(std::shared_ptr<AST> var, std::shared_ptr<AST> type): var(std::move(var)), type(std::move(type)) {}
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class FunctionDecl : public AST {
public:
  std::shared_ptr<AST> name;
  std::vector<std::shared_ptr<AST>> params;
  std::shared_ptr<AST> block;
  FunctionDecl(std::shared_ptr<AST> name, std::vector<std::shared_ptr<AST>> params, std::shared_ptr<AST> block): name(std::move(name)), params(std::move(params)), block(std::move(block)) {}
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

class Call : public AST {
public:
  std::shared_ptr<AST> name;
  std::vector<std::shared_ptr<AST>> actualParams;
  explicit Call(std::shared_ptr<AST> name, std::vector<std::shared_ptr<AST>> actualParams): name(std::move(name)), actualParams(std::move(actualParams)) {}
  void accept(Visitor& v) override {
    v.visit(*this);
  }
};

// Call Stack

class NumberValue;
class BooleanValue;
class ASTValue;

class RecordValueVisitor {
public:
  virtual void visit(NumberValue& el) = 0;
  virtual void visit(BooleanValue& el) = 0;
  virtual void visit(ASTValue& el) = 0;
};

class RecordValue {
public:
  virtual void accept(RecordValueVisitor& v) = 0;
};

class NumberValue: public RecordValue {
public:
  int value;
  NumberValue(int value): value(value) {}
  void accept(RecordValueVisitor& v) override {
    v.visit(*this);
  }
};

class BooleanValue: public RecordValue {
public:
  bool value;
  BooleanValue(bool value): value(value) {}
  void accept(RecordValueVisitor& v) override {
    v.visit(*this);
  }
};

class ASTValue: public RecordValue {
public:
  FunctionDecl value;
  ASTValue(FunctionDecl value): value(std::move(value)) {}
  void accept(RecordValueVisitor& v) override {
    v.visit(*this);
  }
};

class ActivationRecord {
private:
  std::map<std::string, std::shared_ptr<RecordValue>> members;
public:
  enum Type {
    PROGRAM,
    PROCEDURE,
    FUNCTION
  };
  std::string name;
  Type type;
  int level;
  std::shared_ptr<ActivationRecord> parent;

  ActivationRecord(std::string name, Type type, int level, std::shared_ptr<ActivationRecord> parent): name(std::move(name)), type(type), level(level), parent(std::move(parent)) {}

  void print();
  std::shared_ptr<RecordValue> get(std::string name);
  void set(std::string name, bool value);
  void set(std::string name, int value);
  void set(std::string name, FunctionDecl value);
};

// Interpreter

class PrintVisitor: public Visitor, public RecordValueVisitor {
public:
  int value = 0;
  std::string name;
//  std::map<std::string, int> values;
  std::shared_ptr<ActivationRecord> callstack = nullptr;

  void print() {
    if (callstack != nullptr) {
      callstack->print();
    }
//    for(const auto & item : values) {
//      std::cout << item.first << " " << item.second << std::endl;
//    }
  }

  void visit(Num& el) override {
    value = el.value;
  }

  // Record Values
  void visit(BooleanValue& val) override {
    value = val.value ? 1 : 0;
  }
  void visit(NumberValue& val) override {
    value = val.value;
  }
  void visit(ASTValue& val) override {
    // TODO: set param name to val.value.params value
    val.value.block->accept(*this);
//    throw std::invalid_argument("NOT CREATED");
//    value = val.value ? 1 : 0;
  }

  void visit(Var& el) override {
    name = el.name;
    if (callstack != nullptr) {
      auto ar = callstack->get(name);
      if (ar != nullptr) {
        ar->accept(*this);
      }
    }
  }

  void visit(Boolean& el) override {
    value = el.value ? 1 : 0;
  }

  void visit(UnaryOp& el) override {
    el.node->accept(*this);
    int nodeValue = value;
    if (el.op.type == Token::Type::PLUS) {
    } else if (el.op.type == Token::Type::MINUS) {
      value = -nodeValue;
    } else if (el.op.type == Token::Type::NOT) {
      value = nodeValue != 1 ? 1 : 0;
    } else {
      throw std::invalid_argument("Invalid unary op.");
    }
  }

  void visit(BinaryOp& el) override {
    // TODO: type check
    el.left->accept(*this);
    int leftValue = value;
    // TODO: type check
    el.right->accept(*this);
    int rightValue = value;
    if (el.op.type == Token::Type::PLUS) {
      value = leftValue + rightValue;
    } else if (el.op.type == Token::Type::MINUS) {
      value = leftValue - rightValue;
    } else if (el.op.type == Token::Type::MULTPLY) {
      value = leftValue * rightValue;
    } else if (el.op.type == Token::Type::DIVIDE) {
      value = leftValue / rightValue;
    } else if (el.op.type == Token::Type::OR) {
      value = (leftValue == 1) || (rightValue == 1) ? 1 : 0;
    } else if (el.op.type == Token::Type::AND) {
      value = (leftValue == 1) && (rightValue == 1) ? 1 : 0;
    } else if (el.op.type == Token::Type::EQUAL) {
      value = leftValue == rightValue ? 1 : 0;
    } else if (el.op.type == Token::Type::NOT_EQUAL) {
      value = leftValue != rightValue ? 1 : 0;
    } else if (el.op.type == Token::Type::GREATER) {
      value = leftValue > rightValue ? 1 : 0;
    } else if (el.op.type == Token::Type::GREATER_EQUAL) {
      value = leftValue >= rightValue ? 1 : 0;
    } else if (el.op.type == Token::Type::LESS) {
      value = leftValue < rightValue ? 1 : 0;
    } else if (el.op.type == Token::Type::LESS_EQUAL) {
      value = leftValue <= rightValue ? 1 :0;
    } else {
      throw std::invalid_argument("Invalid binary op.");
    }
  }

  void visit(Assign& el) override {
    // todo; type check
//    token.name
    el.left->accept(*this);
    auto varName = name;
    el.right->accept(*this);
    callstack->set(varName, value);
//    values[name] = value;
  }

  void visit(Program& el) override {
    el.name->accept(*this);
    auto programName = name;
    auto ar = std::make_shared<ActivationRecord>(programName, ActivationRecord::Type::PROGRAM, 1, nullptr);

    callstack = ar;
    el.block->accept(*this);
    print();
    callstack = nullptr;
  }

  void visit(Block& el) override {
    // TODO: decls
    for (auto decl : el.declarations) {
      decl->accept(*this);
    }
    el.compoundStatement->accept(*this);
  }

  void visit(Compound& el) override {
    for (const auto& subEl : el.children) {
      subEl->accept(*this);
    }
  }

  void visit(NoOp& el) override {}

  void visit(VarDecl& el) override {
//    el.var->accept(*this);
//    el.type->accept(*this);
  }

  void visit(Type& el) override {

  }

  void visit(Param& el) override {

  }

  void visit(FunctionDecl& el) override {
//    const functionName = node.name.value;
//
//    this.callStack.peek().set(functionName, node);
//
//    const functionSymbol = new FunctionSymbol(functionName);
//    for (let param of node.params) {
//      const paramType = this.currentScope.lookup(param.typeNode.token.value);
//      const paramName = param.varNode.token.value;
//      if (!paramType) throw new Error('Not defined paramType');
//      const varSymbol = new VarSymbol(paramName, paramType);
//      functionSymbol.params.push(varSymbol);
//    }
//    this.currentScope.define(functionSymbol);
    el.name->accept(*this);
    std::string functionName = name;

    callstack->set(functionName, el);
    // TODO: symbol table creation
  }

  void visit(Call& el) override {
//    const typeSymbol = this.currentScope.lookup(node.name);
//
//    if (typeSymbol === undefined || !(typeSymbol instanceof ProcedureSymbol || typeSymbol instanceof FunctionSymbol)) {
//      throw new Error(`Invalid procedure call called "${node.name}"`);
//    }
//
//    // BUILT IN FUNCTIONS
//    if (typeSymbol.name === 'print') {
//      console.log(chalk.green(`PASCAL: "${this.visit(node.actualParams[0])}"`));
//      return;
//    }
//
//    const procedureCallAR = new ActivationRecord(
//        node.name,
//        ARType.PROCEDURE,
//        this.callStack.peek().nestingLevel + 1,
//        this.callStack.peek()
//    );
//    this.callStack.push(procedureCallAR);
//
//    // TODO: setup scope for procedure (set args for procedure to args provided and create new scope LOOK ABOVE)
//
//    const procedureScope = new ScopedSymbolTable(node.name, this.currentScope.scopeLevel + 1, this.currentScope);
//    for (let i = 0; i < typeSymbol.params.length; ++i) {
//      const actualParam = node.actualParams[i];
//      const paramSymbol = typeSymbol.params[i];
//      // TODO: type check
//      // TODO: other callStack
//      this.callStack.peek().set(paramSymbol.name, this.visit(actualParam));
//    }
//    this.currentScope = procedureScope;
//    const procedureAST = this.callStack.peek().get(node.name);
//
//    this.visit(procedureAST.block);
//    // console.log(this.callStack.toString());
//
//    const finalValue = this.callStack.peek().get(ReturnValue);
//
//    this.currentScope = this.currentScope.upperTable!;
//    this.callStack.pop();
//    return finalValue;
    el.name->accept(*this);
    std::string callName = name;
    auto funcAST = callstack->get(callName);
    auto callAR = std::make_shared<ActivationRecord>(name, ActivationRecord::Type::FUNCTION, callstack->level, callstack);
    callstack = callAR;
    funcAST->accept(*this);
    // Return value:
    // TODO:
//    callstack->get(RETURN_VALUE);
    callstack->print();
    callstack = callstack->parent;
  }

};

#endif //INTERPRETER_INTERPRETER_H
