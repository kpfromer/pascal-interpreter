#include "interpreter.h"
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

bool interpreter::isDigit(const char &character) {
  return character == '0' || character == '1' || character == '2' ||
         character == '3' || character == '4' || character == '5' ||
         character == '6' || character == '7' || character == '8' ||
         character == '9';
}

interpreter::Token::Token() { type = NULLTOKEN; }

interpreter::Token::Token(const TokenType &type) { this->type = type; }

interpreter::Token::Token(const TokenType &type, const int &value) {
  this->type = type;
  this->value = value;
}
interpreter::Token::Token(const interpreter::TokenType &type,
                          const std::string &value) {
  this->type = type;
  this->name = value;
}

interpreter::Num::Num(const Token &token) {
  if (token.type != INTEGER) {
    throw std::invalid_argument("Invalid token provided to Num.");
  }
  this->number = token;
  this->value = token.value;
}

interpreter::UnaryOp::UnaryOp(const Token &op, AST *node) {
  this->op = op;
  this->node = node;
}

interpreter::UnaryOp::~UnaryOp() {
  delete node;
}

interpreter::BinaryOp::BinaryOp(interpreter::AST *left, const interpreter::Token &op, interpreter::AST *right) {
  this->left = left;
  this->op = op;
  this->right = right;
}

interpreter::BinaryOp::~BinaryOp() {
  delete left;
  delete right;
}

interpreter::Token interpreter::Lexer::getNextToken() {
  while (isMoreTokens()) {
    char current = lines[line][position];

    if (current == ' ') {
      skipWhitespace();
      continue;
    }

    if (std::isalpha(current)) {
      return id();
    }

    if (current == ':' && peekCharacter() == '=') {
      advance();
      advance();
      Token token (ASSIGN);
      return token;
    } else if (current == ';') {
      advance();
      Token token (SEMI);
      return token;
    } else if (current == '.') {
      advance();
      Token token (DOT);
      return token;
    }

    if (isDigit(current)) {
      Token number(INTEGER, integer());
      return number;
    }

    if (current == '+') {
      advance();
      Token op(PLUS, '+');
      return op;
    } else if (current == '-') {
      advance();
      Token op(MINUS, '+');
      return op;
    } else if (current == '*') {
      advance();
      Token op(MULTIPLY, '*');
      return op;
    } else if (current == '/') {
      advance();
      Token op(DIVIDE, '-');
      return op;
    } else if (current == '(') {
      advance();
      Token op(LEFT_PAREN, '(');
      return op;
    } else if (current == ')') {
      advance();
      Token op(RIGHT_PAREN, ')');
      return op;
    }

    throw std::invalid_argument("Invalid token.");
  }
  Token eof(ENDLINE);
  return eof;
}

interpreter::Token interpreter::Lexer::peek(const unsigned int &count) {
  unsigned int oldLine = line;
  unsigned int oldPosition = position;
  if (count < 1) {
    throw std::invalid_argument("Count for peek must be 1 or more.");
  }
  Token token;
  for (unsigned int i = 0; i < count && isMoreTokens(); i++) {
    token = getNextToken();
  }
  line = oldLine;
  position = oldPosition;
  return token;
}

int interpreter::Lexer::integer() {
  std::string numberStr;
  while (isDigit((lines[line][position]))) {
    numberStr += lines[line][position];
    advance();
  }
  return std::stoi(numberStr);
}

interpreter::Lexer::Lexer(std::vector<std::string> lines) : lines(lines) {

}

bool interpreter::Lexer::isMoreTokens() {
  if (line == lines.size() - 1) { // Last Line
    return position < lines[line].size();
  }
  return line < lines.size() - 1;
}

void interpreter::Lexer::skipWhitespace() {
  while (isMoreTokens() && lines[line][position] == ' ') {
    advance();
  }
}

interpreter::Token interpreter::Lexer::id() {
  std::string val;
  while (isMoreTokens() && std::isalpha(lines[line][position])) {
    val += lines[line][position];
    advance();
  }

  boost::algorithm::to_lower(val);

  // TODO: error if lowercase
  if (val == "begin")  {
    Token token (BEGIN);
    return token;
  } else if (val == "end") {
    Token token (END);
    return token;
  } else {
    Token token(ID, val);
    return token;
//    throw std::invalid_argument("Invalid id.");
  }
}

char interpreter::Lexer::peekCharacter(const unsigned int & count) {
  if (position + count < lines[line].size()) {
    return lines[line][position + count];
  } else {
    throw std::invalid_argument("Invalid peek count \"" + std::to_string(count) + "\" when line is \"" + std::to_string(lines[line].size()) +"\". Count out of bounds.");
  }
}

void interpreter::Lexer::advance() {
  position++;
  if (position >= lines[line].size()) {
    line++;
    position = 0;
  }
}

interpreter::Token interpreter::Parser::eat(TokenType type) {
  Token next = lexer.getNextToken();
  if (next.type == type) {
    return next;
  } else {
    throw std::invalid_argument("Invalid Token");
  }
}

// factor : (PLUS | MINUS) factor | INTEGER | LPAREN expr RPAREN
interpreter::AST *interpreter::Parser::factor() {
  Token a = lexer.peek();
  if (a.type == PLUS) {
    AST *newNode = new UnaryOp(lexer.getNextToken(), factor());
    return newNode;
  } else if (a.type == MINUS) {
    AST *newNode = new UnaryOp(lexer.getNextToken(), factor());
    return newNode;
  } else if (a.type == INTEGER) {
    AST *newNode = new Num(lexer.getNextToken());
    return newNode;
  } else if (a.type == LEFT_PAREN) {
    AST *val = expr();
    eat(RIGHT_PAREN);
    return val;
  } else {
    return variable();
//    throw std::invalid_argument("Invalid factor");
  }
}

// term : factor ((MUL | DIV) factor)*
interpreter::AST *interpreter::Parser::term() {
  AST *node = factor();

  while (lexer.peek().type == MULTIPLY || lexer.peek().type == DIVIDE) {
    Token op = lexer.getNextToken();
    node = new BinaryOp(node, op, factor());
  }

  return node;
}

// expr : term ((PLUS | MINUS) term)*
interpreter::AST *interpreter::Parser::expr() {
  AST *node = term();

  while (lexer.peek().type == PLUS || lexer.peek().type == MINUS) {
    Token token = lexer.getNextToken();
    node = new BinaryOp(node, token, term());
  }
  return node;
}

interpreter::AST *interpreter::Parser::parse() {
  AST* node = program();
  if (lexer.getNextToken().type != ENDLINE) {
    throw std::invalid_argument("Invalid program");
  }
  return node;
}

interpreter::Parser::Parser(const std::vector<std::string> &lines) : lexer(lines) {

}
interpreter::AST *interpreter::Parser::program() {
  AST* node = compoundStatement();
  eat(DOT);
  return node;
}
interpreter::AST *interpreter::Parser::compoundStatement() {
  eat(BEGIN);
  std::vector<AST*> nodes = statementList();
  eat(END);

  auto* root = new Compound();

  for (AST* node : nodes) {
    root->append(node);
  }

  return root;
}
std::vector<interpreter::AST*> interpreter::Parser::statementList() {
  std::vector<AST*> nodes;
  AST* node = statement();
  nodes.emplace_back(node);

  while (lexer.peek().type == SEMI) {
    eat(SEMI);
    nodes.emplace_back(statement());
  }

  if (lexer.peek().type == ID) {
    throw std::invalid_argument("Invalid token in statementlist.");
  }

  return nodes;
}
interpreter::AST *interpreter::Parser::statement() {
  Token token = lexer.peek();
  if (token.type == BEGIN) {
    return compoundStatement();
  } else if (token.type == ID) {
    return assignmentStatement();
  } else {
    return empty();
  }
}
interpreter::AST *interpreter::Parser::assignmentStatement() {
  Var* var = variable();
  eat(ASSIGN);
  AST* value = expr();
  return new Assign(var, value);
}

interpreter::Var *interpreter::Parser::variable() {
  Token token = lexer.getNextToken();
  if (token.type != ID) {
    throw std::invalid_argument("Invalid assignment. Variable name is not valid.");
  }
  return new Var(token);
}

interpreter::AST *interpreter::Parser::empty() {
  return new NoOp();
}

void interpreter::BasicVisitor::visit(interpreter::Num &number) {
  vals.push_back(number.value);
}

void interpreter::BasicVisitor::visit(interpreter::BinaryOp &opNode) {
  opNode.left->accept(*this);
  opNode.right->accept(*this);
  int b = vals.at(vals.size() - 1);
  vals.pop_back();
  int a = vals.at(vals.size() - 1);
  vals.pop_back();
  if (opNode.op.type == MULTIPLY) {
    vals.emplace_back(a * b);
  } else if (opNode.op.type == DIVIDE) {
    vals.emplace_back(a / b);
  } else if (opNode.op.type == PLUS) {
    vals.emplace_back(a + b);
  } else if (opNode.op.type == MINUS) {
    vals.emplace_back(a - b);
  }
}

void interpreter::BasicVisitor::visit(interpreter::UnaryOp &opNode) {
  opNode.node->accept(*this);
  int a = vals.at(vals.size() - 1);
  if (opNode.op.type == MINUS) {
    vals.pop_back();
    vals.push_back(-a);
  }
}


int interpreter::BasicVisitor::getVal() {
  if (vals.size() != 1) {
    throw std::invalid_argument("Invalid number of items in visitor");
  }
  return vals.at(0);
}

std::string interpreter::PrintVisitor::getTabs() {
  std::string val;
  for (unsigned int i = 0; i < tabCount; i++) {
    val += "\t";
  }
  return val;
}


void interpreter::PrintVisitor::visit(interpreter::Num &number) {
  xml += getTabs() + "<integer>" + std::to_string(number.value) + "</integer>\n";
}

void interpreter::PrintVisitor::visit(interpreter::BinaryOp &opNode) {
  std::string type;
  if (opNode.op.type == MULTIPLY) {
    type = "*";
  } else if (opNode.op.type == DIVIDE) {
    type = "/";
  } else if (opNode.op.type == PLUS) {
    type = "+";
  } else if (opNode.op.type == MINUS) {
    type = "-";
  }
  xml += getTabs() + "<" + type + ">\n";
  tabCount++;
  opNode.left->accept(*this);
  opNode.right->accept(*this);
  tabCount--;
  xml += getTabs() + "</" + type + ">\n";
}

void interpreter::PrintVisitor::visit(interpreter::UnaryOp &opNode) {
  std::string type;
  if (opNode.op.type == PLUS) {
    type = "+";
  } else if (opNode.op.type == MINUS) {
    type = "-";
  }
  xml += getTabs() + "<" + type + ">\n";
  tabCount++;
  opNode.node->accept(*this);
  tabCount--;
  xml += getTabs() + "</" + type + ">\n";
}

std::string interpreter::PrintVisitor::getVal() {
  return xml;
}
void interpreter::PrintVisitor::visit(interpreter::Compound &node) {
  xml += getTabs() + "<compound>\n";
  tabCount++;
  for (AST* child : node.children) {
    child->accept(*this);
  }
  tabCount--;
  xml += getTabs() + "</compound>\n";
}
void interpreter::PrintVisitor::visit(interpreter::Assign &node) {
  xml += getTabs() + "<:=>\n";
  tabCount++;
  node.left->accept(*this);
  node.right->accept(*this);
  tabCount--;
  xml += getTabs() + "</:=>\n";
}
void interpreter::PrintVisitor::visit(interpreter::Var &node) {
  xml += getTabs() + "<var>" + node.value.name + "</var>\n";
}
void interpreter::PrintVisitor::visit(interpreter::NoOp &node) {
  xml += getTabs() + "<noop />\n";
}

interpreter::Assign::Assign(interpreter::Var *left, interpreter::AST *right) {
  this->left = left;
  this->right = right;
}

interpreter::Assign::~Assign() {
  delete left;
  delete right;
}

interpreter::Compound::Compound() = default;

void interpreter::Compound::append(interpreter::AST *node) {
  children.emplace_back(node);
}
interpreter::Compound::~Compound() {
  for (AST* node : children) {
    delete node;
  }
}
interpreter::Var::Var(interpreter::Token var) {
  this->value = var;
}



std::string interpreter::Interpreter::getTabs() {
  std::string val;
  for (unsigned int i = 0; i < tabCount; i++) {
    val += "\t";
  }
  return val;
}


void interpreter::Interpreter::visit(interpreter::Num &number) {
  vals.emplace_back(number.value);
  xml += getTabs() + "<integer>" + std::to_string(number.value) + "</integer>\n";
}

void interpreter::Interpreter::visit(interpreter::BinaryOp &opNode) {
  opNode.left->accept(*this);
  opNode.right->accept(*this);
  int b = vals[vals.size() - 1];
  vals.pop_back();
  int a = vals[vals.size() - 1];
  vals.pop_back();
  if (opNode.op.type == MULTIPLY) {
    vals.emplace_back(a * b);
  } else if (opNode.op.type == DIVIDE) {
    vals.emplace_back(a / b);
  } else if (opNode.op.type == PLUS) {
    vals.emplace_back(a + b);
  } else if (opNode.op.type == MINUS) {
    vals.emplace_back(a - b);
  }
}

void interpreter::Interpreter::visit(interpreter::UnaryOp &opNode) {
  opNode.node->accept(*this);
  if (opNode.op.type == MINUS) {
    int a = vals[vals.size() - 1];
    vals.pop_back();
    vals.emplace_back(-a);
  }
}

std::string interpreter::Interpreter::getVal() {
  for( auto const& [key, val] : symbolicTable )
  {
    std::cout << key         // string (key)
              << ':'
              << val        // string's value
              << std::endl ;
  }
  return "";
}
void interpreter::Interpreter::visit(interpreter::Compound &node) {
  xml += getTabs() + "<compound>\n";
  tabCount++;
  for (AST* child : node.children) {
    child->accept(*this);
  }
  tabCount--;
  xml += getTabs() + "</compound>\n";
}
void interpreter::Interpreter::visit(interpreter::Assign &node) {
//  xml += getTabs() + "<:=>\n";
//  tabCount++;
  node.left->accept(*this);
  std::string name = varName;
  needVal = true;
  node.right->accept(*this);
  needVal = false;

  symbolicTable[name] = vals[vals.size() - 1];
  vals.pop_back();
//  tabCount--;
//  xml += getTabs() + "</:=>\n";
}
void interpreter::Interpreter::visit(interpreter::Var &node) {
  varName = node.value.name;
  if (needVal) {
    if (symbolicTable.find(node.value.name) != symbolicTable.end()) {
      vals.emplace_back(symbolicTable[node.value.name]);
    } else {
      throw std::invalid_argument("No value for var.");
    }
  }
}
void interpreter::Interpreter::visit(interpreter::NoOp &node) {
//  xml += getTabs() + "<noop />\n";
}