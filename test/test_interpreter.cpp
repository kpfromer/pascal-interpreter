#include "catch2.h"
#include "interpreter.h"
#include <vector>
#include <string>

TEST_CASE("Lexer", "[interpreter]" ) {
  std::vector<std::string> lines;
  lines.emplace_back("6+2   - 3      * 10 / 5");
  interpreter::Lexer lexer(lines);

  SECTION("It gets an INTEGER token") {
    interpreter::Token token = lexer.getNextToken();
    REQUIRE(token.type == interpreter::TokenType::INTEGER);
    REQUIRE(token.value == 6);
  }

  SECTION("It gets a token") {
    SECTION("PLUS token") {
      lexer.getNextToken();
      interpreter::Token token = lexer.getNextToken();
      REQUIRE(token.type == interpreter::TokenType::PLUS);
    }
    SECTION("MINUS token") {
      lexer.getNextToken(); // 6
      lexer.getNextToken(); // +
      lexer.getNextToken(); // 2
      interpreter::Token token = lexer.getNextToken(); // -
      REQUIRE(token.type == interpreter::TokenType::MINUS);
    }
    SECTION("MULTIPLY token") {
      lexer.getNextToken(); // 6
      lexer.getNextToken(); // +
      lexer.getNextToken(); // 2
      lexer.getNextToken(); // -
      lexer.getNextToken(); // 3
      interpreter::Token token = lexer.getNextToken(); // *
      REQUIRE(token.type == interpreter::TokenType::MULTIPLY);
    }
    SECTION("DIVIDE token") {
      lexer.getNextToken(); // 6
      lexer.getNextToken(); // +
      lexer.getNextToken(); // 2
      lexer.getNextToken(); // -
      lexer.getNextToken(); // 3
      lexer.getNextToken(); // *
      lexer.getNextToken(); // 10
      interpreter::Token token = lexer.getNextToken(); // /
      REQUIRE(token.type == interpreter::TokenType::DIVIDE);
    }
    // TODO: BEGIN DOT SEMI END
  }

  SECTION("It can peek for tokens") {
    interpreter::Token peeked = lexer.peek(2);
    interpreter::Token current = lexer.getNextToken();
    REQUIRE(peeked.type == interpreter::TokenType::PLUS);
    REQUIRE(current.type == interpreter::TokenType::INTEGER);
  }

  SECTION("It can peek characters") {
    REQUIRE(lexer.peekCharacter(2) == '2');
  }
}