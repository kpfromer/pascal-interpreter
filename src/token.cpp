#include <utility>

#include "interpreter.h"

Token::Token(Token::Type type): type(type) {}

Token::Token(Token::Type type, int value): type(type), value(value) {}

Token::Token(std::string name, Token::Type type, int value): name(std::move(name)), type(type), value(value) {}