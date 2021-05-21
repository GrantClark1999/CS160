#pragma once

#include <climits>
#include <sstream>
#include <stack>

#include "helpers.hpp"

// SCANNER DEFINITION

// You can freely add member fields and functions to this class.
class Scanner {
  int line;
  int value;

 public:
  // You really need to implement these four methods for the scanner to work.
  Token nextToken();
  void eatToken(Token);
  int lineNumber();
  int getNumberValue();

  Scanner();
};

// PARSER DEFINITION

// You can freely add member fields and functions to this class.
class Parser {
  Scanner scanner;

  void start();
  void exprList();
  void _exprList();
  void expr();
  void _expr();
  void term();
  void _term();
  void factor();

  // Evaluation
  bool evaluate;
  std::stack<Token> ops;
  std::stack<int> values;
  std::ostringstream output;

  void eatToken(Token t);
  void eval();

 public:
  void parse();

  Parser(bool);
};
