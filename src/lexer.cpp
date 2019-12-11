#include <utility>
#include<vector>
#include<string>
#include <iostream>
#include "interpreter.h"

Lexer::Lexer(std::vector <std::string> lines) : lines(std::move(lines)) {}

bool Lexer::advance() {
  if (line < lines.size()) {
    if (pos + 1 < lines[line].size()) {
      pos++;
      return false;
    } else {
      line++;
      pos = 0;
    }
  }
  if (line > lines.size()) {
    throw "advance";
  }
  return true;
}

void Lexer::skipComment() {
  while (lines[line][pos] != '}') {
    advance();
  }
  advance();
}

void Lexer::skipWhitespace() {
  while(lines[line][pos] == ' ') {
    advance();
  }
}

bool Lexer::isEmpty() {
  return !(line < lines.size() && pos < lines[line].size());
}

void Lexer::print() {
  std::cout << "Line: " << line << " Pos: " << pos << std::endl;
}

Token Lexer::getNextToken() {
  while (!isEmpty()) {
    char currentChar = lines[line][pos];

    if (currentChar == ' ') {
      skipWhitespace();
      continue;
    } else if (currentChar == '{') {
      advance();
      skipComment();
      continue;
    }

    if (isDigit(currentChar)) {
        return number();
    } else if (std::isalpha(currentChar) != 0) {
      return _id();
    }

    if (currentChar == '+') {
      advance();
      Token token = Token(Token::Type::PLUS);
      return token;
    } else if (currentChar == '-') {
      advance();
      Token token = Token(Token::Type::MINUS);
      return token;
    } else if (currentChar == '*') {
      advance();
      Token token = Token(Token::Type::MULTPLY);
      return token;
    } else if (currentChar == '/') {
      advance();
      Token token = Token(Token::Type::DIVIDE);
      return token;
    } else if (currentChar == '(') {
      advance();
      Token token = Token(Token::Type::LEFT_PAREN);
      return token;
    } else if (currentChar == ')') {
      advance();
      Token token = Token(Token::Type::RIGHT_PAREN);
      return token;
    } else if (currentChar == ';') {
      advance();
      Token token = Token(Token::Type::SEMI);
      return token;
    } else if (currentChar == '.') {
      advance();
      Token token = Token(Token::Type::DOT);
      return token;
    } else if (currentChar == ',') {
      advance();
      Token token = Token(Token::Type::COMMA);
      return token;
    } else if (currentChar == '!') {
      advance();
      if (lines[line][pos] == '=') {
        advance();
        Token token = Token(Token::Type::NOT_EQUAL);
        return token;
      }
      Token token = Token(Token::Type::NOT);
      return token;
    } else if (currentChar == '=' && peekChar() == '=') {
      advance();
      advance();
      Token token = Token(Token::Type::EQUAL);
      return token;
    } else if (currentChar == ':') {
      advance();
      if (lines[line][pos] == '=') {
        advance();
        Token token = Token(Token::Type::ASSIGN);
        return token;
      }
      Token token = Token(Token::Type::COLON);
      return token;
    } else if (currentChar == '<') {
      advance();
      if (lines[line][pos] == '=') {
        advance();
        Token token = Token(Token::Type::LESS_EQUAL);
        return token;
      }
      Token token = Token(Token::Type::LESS);
      return token;
    } else if (currentChar == '>') {
      advance();
      if (lines[line][pos] == '=') {
        advance();
        Token token = Token(Token::Type::GREATER_EQUAL);
        return token;
      }
      Token token = Token(Token::Type::GREATER);
      return token;
    } else if (currentChar == '&' && this->peekChar() == '&') {
      advance();
      advance();
      return Token(Token::Type::AND);
    } else if (currentChar == '|' && this->peekChar() == '|') {
      advance();
      advance();
      return Token(Token::Type::OR);
    }

    throw std::invalid_argument("Invalid token.");
  }
  Token token = Token(Token::Type::END_OF_FILE);
  return token;
}

Token Lexer::peek(const int & count) {
  const int oldLine = line;
  const int oldPos = pos;
  Token token = getNextToken();

  for (int i = 1; i < count; ++i) {
    token = getNextToken();
  }

  line = oldLine;
  pos = oldPos;

  return token;
}

Token Lexer::number() {
  std::string num;
  while (!isEmpty() && isDigit(lines[line][pos])) {
    num += lines[line][pos];
    advance();
  }
  Token token (Token::Type::INTEGER_CONST, std::stoi(num));
  return token;
}

char Lexer::peekChar() {
  if (line < lines.size() && pos + 1 < lines[line].size()) {
    return lines[line][pos + 1];
  }
  throw std::invalid_argument("Can not peek character.");
}

Token Lexer::_id() {
  std::string result;
  while (!isEmpty() && std::isalnum(lines[line][pos]) != 0) {
    result += lines[line][pos];
    if (advance()) break;
  }
  // TODO: uppercase
  if (RESERVED_KEYWORDS.find(result) != RESERVED_KEYWORDS.end()) {
    return RESERVED_KEYWORDS.at(result);
  }
  return Token(result, Token::Type::ID, 0);
}
