#ifndef _MAX_HPP
#define _MAX_HPP

#include <climits>

#include "tree.hpp"

class MaxVisitor : public Visitor {
 public:
  int max;
  MaxVisitor() : max(INT_MIN) {}

  void visitNode(Node* node);
};

#endif
