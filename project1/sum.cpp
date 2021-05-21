#include "sum.hpp"

void SumVisitor::visitNode(Node* node) {
  sum += node->value;
  node->visit_children(this);
}
