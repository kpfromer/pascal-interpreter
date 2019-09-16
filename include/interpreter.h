#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <map>
#include <string>
#include <vector>

namespace interpreter {

enum TokenType {
  // Interpreter Only
  NULLTOKEN,
  ENDLINE,
  // Reserved Keywords
  BEGIN,
  END,
  ASSIGN, // :=
  SEMI,
  DOT,
  ID,
  // Numbers
  INTEGER,
  LEFT_PAREN,
  RIGHT_PAREN,
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE
};
enum TokenGroup { OPERATOR };

bool isDigit(const char &character);

class Token {
public:
  TokenType type;
  int value = 0;
  std::string name;
  Token();
  explicit Token(const TokenType &type);
  Token(const TokenType &type, const int &value);
  Token(const TokenType &type, const std::string &value);
};

class Num;
class BinaryOp;
class UnaryOp;
class Compound;
class Assign;
class Var;
class NoOp;

class Visitor {
public:
  virtual void visit(Num &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(UnaryOp &) = 0;
  virtual void visit(Compound &) = 0;
  virtual void visit(Assign &) = 0;
  virtual void visit(Var &) = 0;
  virtual void visit(NoOp &) = 0;
  //  virtual void visit(  number );
};

class AST {
public:
  virtual void accept(Visitor &) = 0;
};

class Num : public AST {
public:
  Token number;
  int value;
  Num(const Token &token);

  //  virtual void accept(const Visitor &v);
  virtual void accept(Visitor &v) override { v.visit(*this); }
};

class UnaryOp : public AST {
public:
  Token op;
  AST *node;
  UnaryOp(const Token &op, AST *node);
  ~UnaryOp();

  virtual void accept(Visitor &v) override { v.visit(*this); }
};

class BinaryOp : public AST {
public:
  AST *left;
  Token op;
  AST *right;
  BinaryOp(AST *left, const Token &op, AST *right);
  ~BinaryOp();

  //  virtual void accept(const Visitor &v);
  virtual void accept(Visitor &v) override { v.visit(*this); }
};

class Compound : public AST {

public:
  std::vector<AST *> children;
  Compound();
  void append(AST* node);
  ~Compound();

  virtual void accept(Visitor &v) override { v.visit(*this); }
};

class Assign : public AST {

public:
  Var* left;
//  AST *left;
  AST *right;
  Assign(Var* left, AST *right);
  ~Assign();

  virtual void accept(Visitor &v) override { v.visit(*this); }
};

class Var : public AST {
public:
  Token value;
  explicit Var(Token var);

  virtual void accept(Visitor &v) override { v.visit(*this); }
};

class NoOp: public AST {
public:
  virtual void accept(Visitor &v) override { v.visit(*this); }
};


class Lexer {
  std::vector<std::string> lines;
  unsigned int line = 0;
  unsigned int position = 0;

  bool isMoreTokens();
  void skipWhitespace();
  void advance();
  int integer();

public:
  Lexer(std::vector<std::string> lines);

  Token id();
  // const bool &peek = false
  Token peek(const unsigned int &count = 1);
  char peekCharacter(const unsigned int &count = 1);
  Token getNextToken();
};

class Parser {
  Lexer lexer;

public:
  explicit Parser(const std::vector<std::string> &lines);
  Token eat(TokenType type);
  // factor : PLUS  factor
  //              | MINUS factor
  //              | INTEGER
  //              | LPAREN expr RPAREN
  //              | variable
  AST *factor();
  // term : factor ((MUL | DIV) factor)*
  AST *term();
  // expr : term ((PLUS | MINUS) term)*
  AST *expr();

  // program : compound_statement DOT
  AST *program();
  // compoundStatement: BEGIN statementList END
  AST *compoundStatement();
  // statementList : statement
  //                   | statement SEMI statementList
  std::vector<AST*> statementList();
  //     statement : compoundStatement
  //              | assignmentStatement
  //              | empty
  AST *statement();
  // assignmentStatement : variable ASSIGN expr
  AST *assignmentStatement();
  // variable : ID
  Var *variable();
  AST* empty();

  AST *parse();
};

template<typename VisitablePtr, typename ResultType>
class ValueGetter : public interpreter::Visitor {
public:

  ResultType GetValue(VisitablePtr n)
  {
    n->accept(*this); // this call fills the return value
    return this->value;
  }

  void Return(ResultType value_)
  {
    value = value_;
  }
private:
  ResultType value;
};

class BasicVisitor : public Visitor {
  std::vector<int> vals;

public:
  void visit(Num &number);
  void visit(BinaryOp &opNode);
  void visit(UnaryOp &opNode);
  int getVal();
};

class PrintVisitor : public Visitor {
  std::string xml;
  unsigned int tabCount = 0;
  std::string getTabs();

public:
  void visit(Num &number);
  void visit(BinaryOp &opNode);
  void visit(UnaryOp &opNode);
  void visit(Compound &node);
  void visit(Assign &node);
  void visit(Var &node);
  void visit(NoOp &node);
  std::string getVal();
};

//class SymbolicTable {
//
//};

class Interpreter : public Visitor {
  std::map<std::string, int> symbolicTable;
  std::vector<int> vals;
  bool needVal = false;
  std::string xml;
  std::string varName;

  unsigned int tabCount = 0;
  std::string getTabs();

public:
  void visit(Num &number);
  void visit(BinaryOp &opNode);
  void visit(UnaryOp &opNode);
  void visit(Compound &node);
  void visit(Assign &node);
  void visit(Var &node);
  void visit(NoOp &node);
  std::string getVal();
};

} // namespace interpreter

#endif