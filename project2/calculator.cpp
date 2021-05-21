
#include "calculator.hpp"

#include <cmath>
#include <iostream>
#include <string>

// Scanner implementation

Scanner::Scanner() : line(1), value(0) {}

Token Scanner::nextToken() {
  char next = std::cin.peek();

  bool isNum = std::isdigit(next);
  if (isNum) {
    std::string digits = "";
    while (std::isdigit(next)) {
      int next_digit = std::cin.get();
      digits += next_digit;
      next = std::cin.peek();
    }
    for (auto it = digits.crbegin(); it != digits.crend(); ++it)
      std::cin.putback(*it);
    value = std::stoi(digits);
    return T_NUMBER;
  }

  switch (next) {
    case '+':
      return T_PLUS;
    case '-':
      return T_MINUS;
    case '*':
      return T_MULTIPLY;
    case '/':
      return T_DIVIDE;
    case 'm': {
      char c1 = std::cin.get();
      char c2 = std::cin.get();
      if (c2 != 'o') scanError(line, c2);
      char c3 = std::cin.peek();
      if (c3 != 'd') scanError(line, c3);
      std::cin.putback(c2);
      std::cin.putback(c1);
      return T_MODULO;
    }
    case '(':
      return T_OPENPAREN;
    case ')':
      return T_CLOSEPAREN;
    case ';':
      return T_SEMICOLON;
    case ' ':
      std::cin.get();
      return nextToken();
    case '\n':
      return T_NEWLN;
    case EOF:
      return T_EOF;
    default:
      scanError(line, next);
  }
  return T_EOF;  // SHOULD NEVER REACH HERE!
}

void Scanner::eatToken(Token toConsume) {
  Token nextToken = this->nextToken();
  if (nextToken != toConsume) mismatchError(line, toConsume, nextToken);

  // Number of characters to consume
  int numChars = 1;
  if (toConsume == T_NUMBER) numChars = std::to_string(value).length();
  if (toConsume == T_MODULO) numChars = 3;
  if (toConsume == T_NEWLN) ++line;
  for (int i = 0; i < numChars; ++i) std::cin.get();
}

int Scanner::lineNumber() { return this->line; }

int Scanner::getNumberValue() { return this->value; }

// Parser implementation

Parser::Parser(bool eval) : evaluate(eval) {}

void Parser::parse() { start(); }

/*
  Grammar
  -------
  start     (S)   ->  L
  exprList  (L)   ->  EL'
  _exprList (L')  ->  ;EL' | epsilon
  expr      (E)   ->  TE'
  _expr     (E')  ->  +TE' | -TE' | epsilon
  term      (T)   ->  FT'
  _term     (T')  ->  *FT' | /FT' | modFT' | epsilon
  factor    (F)   ->  (E) | num
*/

void Parser::start() { exprList(); }

void Parser::exprList() {
  expr();
  _exprList();
  if (scanner.nextToken() == T_EOF) eatToken(T_EOF);
}

void Parser::_exprList() {
  if (scanner.nextToken() == T_SEMICOLON) {
    eatToken(T_SEMICOLON);
    if (scanner.nextToken() == T_NEWLN) eatToken(T_NEWLN);
    expr();
    _exprList();
  }
}

void Parser::expr() {
  term();
  _expr();
}

void Parser::_expr() {
  Token next = scanner.nextToken();
  switch (next) {
    case T_PLUS:
    case T_MINUS:
      eatToken(next);
      term();
      _expr();
    default:
      return;
  }
}

void Parser::term() {
  factor();
  _term();
}

void Parser::_term() {
  Token next = scanner.nextToken();
  switch (next) {
    case T_MULTIPLY:
      eatToken(next);
      factor();
      _term();
      break;
    case T_DIVIDE:
    case T_MODULO:
      eatToken(next);
      factor();
      _term();
      break;
    default:
      return;
  }
}

void Parser::factor() {
  Token next = scanner.nextToken();
  switch (next) {
    case T_NUMBER:
      if (evaluate && scanner.getNumberValue() > INT_MAX)
        outOfBoundsError(scanner.lineNumber(), scanner.getNumberValue());
      return eatToken(next);
    case T_OPENPAREN:
      eatToken(next);
      expr();
      return eatToken(T_CLOSEPAREN);
    case T_EOF:
      return eatToken(next);
    default:
      parseError(scanner.lineNumber(), next);
  }
}

// Evaluation Functions
// Implements the Shunting-Yard Algorithm
// Reference used: https://en.wikipedia.org/wiki/Shunting-yard_algorithm

int precedence(Token t) {
  switch (t) {
    case T_MULTIPLY:
    case T_DIVIDE:
    case T_MODULO:
      return 2;
    case T_PLUS:
    case T_MINUS:
      return 1;
    default:
      return 0;
  }
}

void Parser::eatToken(Token t) {
  scanner.eatToken(t);
  if (evaluate) {
    switch (t) {
      case T_NEWLN:
        break;
      case T_SEMICOLON:
      case T_EOF:
        while (!ops.empty()) eval();
        output << std::to_string(values.top());
        if (t == T_SEMICOLON) output << '\n';
        values.pop();
        if (t == T_EOF) std::cout << output.str();
        break;
      case T_NUMBER:
        values.push(scanner.getNumberValue());
        break;
      case T_OPENPAREN:
        ops.push(t);
        break;
      case T_CLOSEPAREN:
        while (ops.top() != T_OPENPAREN) eval();
        if (ops.top() == T_OPENPAREN) ops.pop();
        break;
      default:  // default = arithmetic operator
        while (!ops.empty() && precedence(ops.top()) >= precedence(t) &&
               ops.top() != T_OPENPAREN) {
          eval();
        }
        ops.push(t);
        break;
    }
  }
}

void Parser::eval() {
  int v1 = values.top();
  values.pop();
  int v2 = values.top();
  values.pop();
  switch (ops.top()) {
    case T_PLUS:
      values.push(v2 + v1);
      break;
    case T_MINUS:
      values.push(v2 - v1);
      break;
    case T_MULTIPLY: {
      long result = (long)v2 * v1;
      if (result > INT_MAX || result < INT_MIN)
        outOfBoundsError(scanner.lineNumber(), result);
      values.push(v2 * v1);
      break;
    }
    case T_DIVIDE:
      if (v1 == 0) divideByZeroError(scanner.lineNumber(), v2);
      values.push(v2 / v1);
      break;
    case T_MODULO:
      if (v1 == 0) divideByZeroError(scanner.lineNumber(), v2);
      values.push(v2 % v1);
      break;
    default:
      break;
  }
  ops.pop();
}