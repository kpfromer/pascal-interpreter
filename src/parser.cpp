#include "interpreter.h"
#include <memory>
#include <utility>
#include <vector>

Parser::Parser(std::vector<std::string> lines): lexer(std::move(lines)) {}

Token Parser::eat(Token::Type type) {
  Token token = lexer.getNextToken();
  if (token.type != type) {
    throw std::invalid_argument("Invalid token type for eatting.");
  }
  return token;
}

std::shared_ptr<AST> Parser::number() {
  Token num = eat(Token::Type::INTEGER_CONST);
//  std::cout << "Number: " << num.value << std::endl;
  return std::shared_ptr<AST>(new Num(num.value));
}

std::shared_ptr<AST> Parser::boolean() {
  Token boolean = lexer.getNextToken();
  if (boolean.type != Token::Type::TRUE && boolean.type != Token::Type::FALSE) {
    throw std::invalid_argument("Invalid Boolean.");
  }
  return std::shared_ptr<AST>(new Boolean(boolean.type == Token::Type::TRUE));
}
std::shared_ptr<AST> Parser::variable() {
  Token var = eat(Token::Type::ID);
  return std::shared_ptr<AST>(new Var(var.name));
}
// factor: PLUS factor
//       | MINUS factor
//       | NOT factor
//       | INTEGER_CONST
//       | TODO: REAL_CONST
//       | TRUE
//       | FALSE
//       | LEFT_PAREN expr RIGHT_PAREN
//       | variable
//       | callStatement
std::shared_ptr<AST> Parser::factor() {
  auto peeked = lexer.peek();
//  std::cout << "fact" << std::endl;

  if (peeked.type == Token::Type::PLUS || peeked.type == Token::Type::MINUS || peeked.type == Token::Type::NOT) {
    return std::shared_ptr<AST>(new UnaryOp(lexer.getNextToken(), factor()));
  } else if (peeked.type == Token::Type::INTEGER_CONST) {
    return number();
  } else if (peeked.type == Token::Type::LEFT_PAREN) {
    lexer.getNextToken();
    auto expr = expression();
    eat(Token::Type::RIGHT_PAREN);
    return expr;
  } else if (peeked.type == Token::Type::TRUE || peeked.type == Token::Type::FALSE) {
    return boolean();
  } else if (peeked.type == Token::Type::ID) {
    if (lexer.peek(2).type == Token::Type::LEFT_PAREN) {
      return callStatement();
    }
    return variable();
  } else {
    std::cout << (peeked.type == Token::Type::END_OF_FILE) << std::endl;
    lexer.print();
    throw std::invalid_argument("Invalid Factor.");
  }
}

// multDivFactor: factor ((MULTIPLY | DIVIDE) factor)*
std::shared_ptr<AST> Parser::multDivFactor() {
  auto node = factor();
  auto peeked = lexer.peek();
  while (peeked.type == Token::Type::MULTPLY || peeked.type == Token::Type::DIVIDE) {
    auto op = lexer.getNextToken();
    node = std::shared_ptr<AST>(new BinaryOp(node, op, factor()));
    peeked = lexer.peek();
  }
  return node;
}

// addSubFactor: multDivFactor ((PLUS | MINUS) multDivFactor)*
std::shared_ptr<AST> Parser::addSubFactor() {
  auto node = multDivFactor();
  auto peeked = lexer.peek();
  while (peeked.type == Token::Type::PLUS || peeked.type == Token::Type::MINUS) {
    auto op = lexer.getNextToken();
    node = std::shared_ptr<AST>(new BinaryOp(node, op, multDivFactor()));
    peeked = lexer.peek();
  }
  return node;
}

// relationalFactor: addSubFactor ((LESS_EQUAL | GREATER_EQUAL | LESS | GREATER) addSubFactor)*
std::shared_ptr<AST> Parser::relationalFactor() {
  auto node = addSubFactor();
  auto peeked = lexer.peek();
  while (
    peeked.type == Token::Type::LESS || peeked.type == Token::Type::LESS_EQUAL ||
    peeked.type == Token::Type::GREATER || peeked.type == Token::Type::GREATER_EQUAL
  ) {
    auto op = lexer.getNextToken();
    node = std::shared_ptr<AST>(new BinaryOp(node, op, addSubFactor()));
    peeked = lexer.peek();
  }
  return node;
}

// andFactor: relationalFactor (AND relationalFactor)*
std::shared_ptr<AST> Parser::andFactor() {
  auto node = relationalFactor();
  auto peeked = lexer.peek();
  while (peeked.type == Token::Type::AND) {
    auto op = lexer.getNextToken();
    node = std::shared_ptr<AST>(new BinaryOp(node, op, relationalFactor()));
    peeked = lexer.peek();
  }
  return node;
}

// orFactor: andFactor (OR andFactor)*
std::shared_ptr<AST> Parser::orFactor() {
  auto node = andFactor();
  auto peeked = lexer.peek();
  while (peeked.type == Token::Type::OR) {
    auto op = lexer.getNextToken();
    node = std::shared_ptr<AST>(new BinaryOp(node, op, andFactor()));
    peeked = lexer.peek();
  }
  return node;
}

// comparisonFactor: orFactor ((EQUAL | NOT_EQUAL) orFactor)*
std::shared_ptr<AST> Parser::comparisonFactor() {
  auto node = orFactor();
  auto peeked = lexer.peek();
  while (peeked.type == Token::Type::EQUAL || peeked.type == Token::Type::NOT_EQUAL) {
    auto op = lexer.getNextToken();
    node = std::shared_ptr<AST>(new BinaryOp(node, op, orFactor()));
    peeked = lexer.peek();
  }
  return node;
}

// expression: comparisonFactor
std::shared_ptr<AST> Parser::expression() {
  return comparisonFactor();
}

// program: PROGRAM variable SEMI block DOT
std::shared_ptr<AST> Parser::program() {
  eat(Token::Type::PROGRAM);
  auto varNode = variable();
  eat(Token::Type::SEMI);
  auto blockNode = block();
  eat(Token::Type::DOT);
  return std::shared_ptr<AST>(new Program(varNode, blockNode));
}

// compoundStatement: BEGIN statementList END
std::shared_ptr<AST> Parser::compoundStatement() {
  eat(Token::Type::BEGIN);
  auto nodes = statementList();
  eat(Token::Type::END);
  auto root = std::make_shared<Compound>();
  for (const auto& node : nodes) {
    root->children.push_back(node);
  }
  return root;
}

// block: declarations compoundStatement
std::shared_ptr<AST> Parser::block() {
  std::vector<std::shared_ptr<AST>> decls = declarations();
  // TODO: decls
  std::shared_ptr<AST> compoundStatementNode = compoundStatement();
  return std::shared_ptr<AST>(new Block(decls, compoundStatementNode));
}

// statementList : statement (statement)*
std::vector<std::shared_ptr<AST>> Parser::statementList() {
  std::vector<std::shared_ptr<AST>> nodes;
  nodes.push_back(statement());
  while (lexer.peek().type == Token::Type::SEMI) {
    lexer.getNextToken();
    nodes.push_back(statement());
  }
  if (lexer.peek().type == Token::Type::ID) {
    std::cout << lexer.peek().name << std::endl;
    throw std::invalid_argument("AHH");
  }
  return nodes;
}

// statement: compoundStatement | ifStatement | whileStatement | callStatement | assignmentStatement | empty
std::shared_ptr<AST> Parser::statement() {
  auto type = lexer.peek().type;
  if (type == Token::Type::BEGIN) {
    return compoundStatement();
  } else if (type == Token::Type::ID) {
    if (lexer.peek(2).type == Token::LEFT_PAREN) {
      return callStatement();
    }
    return assignmentStatement();
  }
  return empty();
}

std::shared_ptr<AST> Parser::empty() {
  return std::shared_ptr<AST>(new NoOp);
}

// assignmentStatement: variable ASSIGN expr
std::shared_ptr<AST> Parser::assignmentStatement() {
  auto var = variable();
  eat(Token::Type::ASSIGN);
  auto expr = expression();
  return std::shared_ptr<AST>(new Assign(var, expr));
}

// callStatement: ID LEFT_PAREN (expr (COMMA expr)*)? RIGHT_PAREN
std::shared_ptr<AST> Parser::callStatement() {
  auto name = variable();
  eat(Token::Type::LEFT_PAREN);
  std::vector<std::shared_ptr<AST>> actualParams;
  if (lexer.peek().type != Token::Type::RIGHT_PAREN) {
    actualParams.push_back(expression());
    while (lexer.peek().type == Token::Type::COMMA) {
      lexer.getNextToken();
      actualParams.push_back(expression());
    }
  }
  eat(Token::Type::RIGHT_PAREN);
  return std::shared_ptr<AST>(new Call(name, actualParams));
}

// declarations: (VAR (variableDeclaration SEMI)+)* (PROCEDURE ID (LEFT_PAREN formalParameterList RIGHT_PAREN)? SEMI block SEMI)* (FUNCTION ID (LEFT_PAREN formalParameterList RIGHT_PAREN)? SEMI block SEMI)* | empty
std::vector<std::shared_ptr<AST>> Parser::declarations() {
  std::vector<std::shared_ptr<AST>> declarationsNodes;

  if (lexer.peek().type == Token::Type::VAR) {
    lexer.getNextToken();
    while (lexer.peek().type == Token::Type::ID) {
      auto varDecs = variableDeclaration();
      // append varDecs to declarationNodes
      declarationsNodes.insert(declarationsNodes.end(), varDecs.begin(), varDecs.end());
      eat(Token::Type::SEMI);
    }
  }

  while(lexer.peek().type == Token::Type::FUNCTION) {
    lexer.getNextToken();
    auto name = variable();
    std::vector<std::shared_ptr<AST>> args;
    if (lexer.peek().type == Token::Type::LEFT_PAREN) {
      lexer.getNextToken();
      args = formalParameterList();
      eat(Token::Type::RIGHT_PAREN);
    }
    eat(Token::Type::SEMI);
    auto blockNode = block();
    declarationsNodes.push_back(std::shared_ptr<AST>(new FunctionDecl(name, args, blockNode)));
    eat(Token::Type::SEMI);
  }

  return declarationsNodes;
}

// variableDeclaration: ID (COMMA ID)* COLON typeSpec
std::vector<std::shared_ptr<AST>> Parser::variableDeclaration() {
  std::vector<std::shared_ptr<AST>> varNodes;
  varNodes.push_back(this->variable());

  while (lexer.peek().type == Token::Type::COMMA) {
    lexer.getNextToken();
    varNodes.push_back(this->variable());
  }
  eat(Token::Type::COLON);
  auto type = typeSpec();

  std::vector<std::shared_ptr<AST>> withTypes;
  withTypes.reserve(varNodes.size());
  for(const auto& varNode : varNodes) {
    withTypes.push_back(std::shared_ptr<AST>(new VarDecl(varNode, type)));
  }
  return withTypes;
}

// typeSpec: INTEGER | REAL | BOOLEAN
std::shared_ptr<AST> Parser::typeSpec() {
  auto type = lexer.getNextToken().type;
  if (type == Token::Type::INTEGER) {
    return std::shared_ptr<AST>(new Type("INTEGER"));
  } else if (type == Token::Type::BOOLEAN) {
    return std::shared_ptr<AST>(new Type("BOOLEAN"));
  } else {
    throw std::invalid_argument("ERROR");
  }
}

// formalParameterList: formalParameter (SEMI formalParameter)*
std::vector<std::shared_ptr<AST>> Parser::formalParameterList() {
  std::vector<std::shared_ptr<AST>> params = formalParameter();

  while (lexer.peek().type == Token::Type::SEMI) {
    std::vector<std::shared_ptr<AST>> newParams = formalParameter();
    params.insert(params.end(), newParams.begin(), newParams.end());
  }
  return params;
}

// formalParameter: ID (COMMA ID)* COLON typeSpec
std::vector<std::shared_ptr<AST>> Parser::formalParameter() {
  std::vector<std::shared_ptr<AST>> vars;
  vars.push_back(variable());
  while (lexer.peek().type == Token::Type::COMMA) {
    vars.push_back(variable());
  }
  eat(Token::Type::COLON);
  auto type = typeSpec();

  std::vector<std::shared_ptr<AST>> params;
  params.reserve(vars.size());
  for (const auto& var : vars) {
    params.push_back(std::shared_ptr<AST>(new Param(var, type)));
  }

  return params;
}