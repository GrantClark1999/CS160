#include "typecheck.hpp"

// Defines the function used to throw type errors. The possible
// type errors are defined as an enumeration in the header file.
void typeError(TypeErrorCode code) {
  switch (code) {
    case undefined_variable:
      std::cerr << "Undefined variable." << std::endl;
      break;
    case undefined_method:
      std::cerr << "Method does not exist." << std::endl;
      break;
    case undefined_class:
      std::cerr << "Class does not exist." << std::endl;
      break;
    case undefined_member:
      std::cerr << "Class member does not exist." << std::endl;
      break;
    case not_object:
      std::cerr << "Variable is not an object." << std::endl;
      break;
    case expression_type_mismatch:
      std::cerr << "Expression types do not match." << std::endl;
      break;
    case argument_number_mismatch:
      std::cerr << "Method called with incorrect number of arguments."
                << std::endl;
      break;
    case argument_type_mismatch:
      std::cerr << "Method called with argument of incorrect type."
                << std::endl;
      break;
    case while_predicate_type_mismatch:
      std::cerr << "Predicate of while loop is not boolean." << std::endl;
      break;
    case do_while_predicate_type_mismatch:
      std::cerr << "Predicate of do while loop is not boolean." << std::endl;
      break;
    case if_predicate_type_mismatch:
      std::cerr << "Predicate of if statement is not boolean." << std::endl;
      break;
    case assignment_type_mismatch:
      std::cerr << "Left and right hand sides of assignment types mismatch."
                << std::endl;
      break;
    case return_type_mismatch:
      std::cerr << "Return statement type does not match declared return type."
                << std::endl;
      break;
    case constructor_returns_type:
      std::cerr << "Class constructor returns a value." << std::endl;
      break;
    case no_main_class:
      std::cerr << "The \"Main\" class was not found." << std::endl;
      break;
    case main_class_members_present:
      std::cerr << "The \"Main\" class has members." << std::endl;
      break;
    case no_main_method:
      std::cerr << "The \"Main\" class does not have a \"main\" method."
                << std::endl;
      break;
    case main_method_incorrect_signature:
      std::cerr << "The \"main\" method of the \"Main\" class has an incorrect "
                   "signature."
                << std::endl;
      break;
  }
  exit(1);
}

// TypeCheck Visitor Functions: These are the functions you will
// complete to build the symbol table and type check the program.
// Not all functions must have code, many may be left empty.

void TypeCheck::visitProgramNode(ProgramNode* node) {
  classTable = new ClassTable();
  node->visit_children(this);
  if (!classTable->count("Main")) typeError(no_main_class);
}

void TypeCheck::visitClassNode(ClassNode* node) {
  IdentifierNode* superClass = node->identifier_2;
  std::string superClassName = superClass ? superClass->name : "";
  if (superClass && !classTable->count(superClassName))
    typeError(undefined_class);

  // Set currentLocalOffset to 0 initially so we know when declarations
  // are members or local variables. (see visitDeclarationNode)
  currentLocalOffset = 0;
  currentMemberOffset = 0;
  currentMethodTable = new MethodTable();
  currentVariableTable = new VariableTable();
  currentClassName = node->identifier_1->name;

  if (currentClassName == "Main") {
    if (node->declaration_list && node->declaration_list->size())
      typeError(main_class_members_present);
    bool found_main_method = false;
    if (node->method_list) {
      for (auto method : *node->method_list) {
        found_main_method |= method->identifier->name == "main";
      }
    }
    if (!found_main_method) typeError(no_main_method);
  }

  ClassInfo classInfo{superClassName, currentMethodTable, currentVariableTable,
                      currentMemberOffset};
  (*classTable)[currentClassName] = classInfo;

  node->visit_children(this);
}

void TypeCheck::visitMethodNode(MethodNode* node) {
  currentLocalOffset = -4;
  currentParameterOffset = 12;
  currentVariableTable = new VariableTable();

  node->visit_children(this);
  node->basetype = node->type->basetype;
  node->objectClassName = node->type->objectClassName;

  if (node->methodbody->basetype != node->basetype ||
      node->methodbody->objectClassName != node->objectClassName)
    typeError(return_type_mismatch);
  if (node->identifier->name == currentClassName && node->basetype != bt_none)
    typeError(constructor_returns_type);
  if (currentClassName == "Main" && node->identifier->name == "main" &&
      node->basetype != bt_none)
    typeError(main_method_incorrect_signature);

  auto parameters = new std::list<CompoundType>();
  if (node->parameter_list) {
    for (auto param : (*node->parameter_list)) {
      CompoundType type{param->type->basetype, param->type->objectClassName};
      parameters->push_back(type);
    }
  }
  int localsSize = -currentLocalOffset - 4;
  CompoundType returnType{node->basetype, node->objectClassName};
  MethodInfo methodInfo{returnType, currentVariableTable, parameters,
                        localsSize};

  (*currentMethodTable)[node->identifier->name] = methodInfo;
}

void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {
  node->visit_children(this);
  node->basetype =
      node->returnstatement ? node->returnstatement->basetype : bt_none;
  node->objectClassName =
      node->returnstatement ? node->returnstatement->objectClassName : "";
}

void TypeCheck::visitParameterNode(ParameterNode* node) {
  node->visit_children(this);
  node->basetype = node->type->basetype;
  node->objectClassName = node->type->objectClassName;

  CompoundType type{node->basetype, node->objectClassName};
  VariableInfo var{type, currentParameterOffset, 4};

  currentParameterOffset += 4;
  (*currentVariableTable)[node->identifier->name] = var;
}

void TypeCheck::visitDeclarationNode(DeclarationNode* node) {
  node->visit_children(this);
  node->basetype = node->type->basetype;
  node->objectClassName = node->type->objectClassName;

  if (node->basetype == bt_object && !classTable->count(node->objectClassName))
    typeError(undefined_class);

  CompoundType type{node->basetype, node->objectClassName};

  for (auto id : (*node->identifier_list)) {
    VariableInfo var;
    if (currentLocalOffset == 0) {
      var = {type, currentMemberOffset, 4};
      currentMemberOffset += 4;
    } else {
      var = {type, currentLocalOffset, 4};
      currentLocalOffset -= 4;
    }
    (*currentVariableTable)[id->name] = var;
  }
}

void TypeCheck::visitReturnStatementNode(ReturnStatementNode* node) {
  node->visit_children(this);
  node->basetype = node->expression->basetype;
  node->objectClassName = node->expression->objectClassName;
}

void TypeCheck::visitAssignmentNode(AssignmentNode* node) {
  node->visit_children(this);
  std::string varName = node->identifier_1->name;
  CompoundType type;

  // Local variable.
  if (currentVariableTable->count(varName))
    type = (*currentVariableTable)[varName].type;
  // Check current class members, then super classes members.
  else {
    std::string className = currentClassName;
    while (!(*classTable)[className].members->count(varName)) {
      className = (*classTable)[className].superClassName;
      if (!classTable->count(className)) typeError(undefined_variable);
    }
    type = (*(*classTable)[className].members)[varName].type;
  }

  // LHS takes the form: foo.bar()
  if (node->identifier_2) {
    if (type.baseType != bt_object) typeError(not_object);
    std::string className = type.objectClassName;
    std::string memberName = node->identifier_2->name;
    // Search members of current class and super classes.
    while (!(*classTable)[className].members->count(memberName)) {
      className = (*classTable)[className].superClassName;
      if (!classTable->count(className)) typeError(undefined_member);
    }

    type = (*(*classTable)[className].members)[memberName].type;
  }

  if (type.baseType != node->expression->basetype ||
      type.objectClassName != node->expression->objectClassName) {
    typeError(assignment_type_mismatch);
  }
}

void TypeCheck::visitCallNode(CallNode* node) {
  node->visit_children(this);
  node->basetype = node->methodcall->basetype;
  node->objectClassName = node->methodcall->objectClassName;
}

void TypeCheck::visitIfElseNode(IfElseNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(if_predicate_type_mismatch);
}

void TypeCheck::visitWhileNode(WhileNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(while_predicate_type_mismatch);
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(do_while_predicate_type_mismatch);
}

void TypeCheck::visitPrintNode(PrintNode* node) { node->visit_children(this); }

void TypeCheck::visitPlusNode(PlusNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer ||
      node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitMinusNode(MinusNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer ||
      node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitTimesNode(TimesNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer ||
      node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitDivideNode(DivideNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer ||
      node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitGreaterNode(GreaterNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer ||
      node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitGreaterEqualNode(GreaterEqualNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer ||
      node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitEqualNode(EqualNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != node->expression_2->basetype ||
      (node->expression_1->basetype != bt_integer &&
       node->expression_1->basetype != bt_boolean))
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitAndNode(AndNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_boolean ||
      node->expression_2->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitOrNode(OrNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_boolean ||
      node->expression_2->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitNotNode(NotNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitNegationNode(NegationNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void checkArguments(std::list<ExpressionNode*>* actual,
                    std::list<CompoundType>* expected) {
  if (expected->size() != actual->size()) typeError(argument_number_mismatch);
  auto expected_iter = expected->begin();
  auto actual_iter = actual->begin();
  for (; expected_iter != expected->end(); ++expected_iter, ++actual_iter) {
    if (expected_iter->baseType != (*actual_iter)->basetype ||
        expected_iter->objectClassName != (*actual_iter)->objectClassName)
      typeError(argument_type_mismatch);
  }
}

void TypeCheck::visitMethodCallNode(MethodCallNode* node) {
  node->visit_children(this);

  std::string methodName =
      node->identifier_2 ? node->identifier_2->name : node->identifier_1->name;
  MethodInfo methodInfo;

  // Pattern: foo.bar()
  if (node->identifier_2) {
    VariableNode varNode(node->identifier_1);
    varNode.accept(this);
    if (varNode.basetype != bt_object) typeError(not_object);
    std::string className = varNode.objectClassName;

    // Search methods of current class and super classes.
    while (!(*classTable)[className].methods->count(methodName)) {
      className = (*classTable)[className].superClassName;
      if (!classTable->count(className)) typeError(undefined_method);
    }

    methodInfo = (*(*classTable)[className].methods)[methodName];
  }
  // Pattern: foo()
  else {
    if (!currentMethodTable->count(methodName)) typeError(undefined_method);
    methodInfo = (*currentMethodTable)[methodName];
  }

  checkArguments(node->expression_list, methodInfo.parameters);

  node->basetype = methodInfo.returnType.baseType;
  node->objectClassName = methodInfo.returnType.objectClassName;
}

void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {
  node->visit_children(this);
  std::string memberName = node->identifier_2->name;

  VariableNode varNode(node->identifier_1);
  varNode.accept(this);

  if (varNode.basetype != bt_object) typeError(not_object);
  std::string className = varNode.objectClassName;

  // Search members of current class and super classes.
  while (!(*classTable)[className].members->count(memberName)) {
    className = (*classTable)[className].superClassName;
    if (!classTable->count(className)) typeError(undefined_member);
  }

  // Overwrite type to be the member variable's type
  CompoundType type = (*(*classTable)[className].members)[memberName].type;
  node->basetype = type.baseType;
  node->objectClassName = type.objectClassName;
}

void TypeCheck::visitVariableNode(VariableNode* node) {
  node->visit_children(this);
  std::string varName = node->identifier->name;
  CompoundType type;

  // Local variable.
  if (currentVariableTable->count(varName))
    type = (*currentVariableTable)[varName].type;
  // Check current class members, then super classes members.
  else {
    std::string className = currentClassName;
    while (!(*classTable)[className].members->count(varName)) {
      className = (*classTable)[className].superClassName;
      if (!classTable->count(className)) typeError(undefined_variable);
    }
    type = (*(*classTable)[className].members)[varName].type;
  }

  node->basetype = type.baseType;
  node->objectClassName = type.objectClassName;
}

void TypeCheck::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  node->basetype = bt_integer;
}

void TypeCheck::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  node->basetype = bt_boolean;
}

void TypeCheck::visitNewNode(NewNode* node) {
  std::string className = node->identifier->name;

  node->visit_children(this);

  if (!classTable->count(className)) typeError(undefined_class);
  MethodTable* methodTable = (*classTable)[className].methods;
  if (!methodTable->count(className)) typeError(undefined_method);

  MethodInfo methodInfo = (*methodTable)[className];

  checkArguments(node->expression_list, methodInfo.parameters);

  node->basetype = bt_object;
  node->objectClassName = className;
}

void TypeCheck::visitIntegerTypeNode(IntegerTypeNode* node) {
  node->basetype = bt_integer;
}

void TypeCheck::visitBooleanTypeNode(BooleanTypeNode* node) {
  node->basetype = bt_boolean;
}

void TypeCheck::visitObjectTypeNode(ObjectTypeNode* node) {
  node->basetype = bt_object;
  node->objectClassName = node->identifier->name;
}

void TypeCheck::visitNoneNode(NoneNode* node) { node->basetype = bt_none; }

void TypeCheck::visitIdentifierNode(IdentifierNode* node) {}

void TypeCheck::visitIntegerNode(IntegerNode* node) {}

// The following functions are used to print the Symbol Table.
// They do not need to be modified at all.

std::string genIndent(int indent) {
  std::string string = std::string("");
  for (int i = 0; i < indent; i++) string += std::string(" ");
  return string;
}

std::string string(CompoundType type) {
  switch (type.baseType) {
    case bt_integer:
      return std::string("Integer");
    case bt_boolean:
      return std::string("Boolean");
    case bt_none:
      return std::string("None");
    case bt_object:
      return std::string("Object(") + type.objectClassName + std::string(")");
    default:
      return std::string("");
  }
}

void print(VariableTable variableTable, int indent) {
  std::cout << genIndent(indent) << "VariableTable {";
  if (variableTable.size() == 0) {
    std::cout << "}";
    return;
  }
  std::cout << std::endl;
  for (VariableTable::iterator it = variableTable.begin();
       it != variableTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {"
              << string(it->second.type);
    std::cout << ", " << it->second.offset << ", " << it->second.size << "}";
    if (it != --variableTable.end()) std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}";
}

void print(MethodTable methodTable, int indent) {
  std::cout << genIndent(indent) << "MethodTable {";
  if (methodTable.size() == 0) {
    std::cout << "}";
    return;
  }
  std::cout << std::endl;
  for (MethodTable::iterator it = methodTable.begin(); it != methodTable.end();
       it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
    std::cout << genIndent(indent + 4) << string(it->second.returnType) << ","
              << std::endl;
    std::cout << genIndent(indent + 4) << it->second.localsSize << ","
              << std::endl;
    print(*it->second.variables, indent + 4);
    std::cout << std::endl;
    std::cout << genIndent(indent + 2) << "}";
    if (it != --methodTable.end()) std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}";
}

void print(ClassTable classTable, int indent) {
  std::cout << genIndent(indent) << "ClassTable {" << std::endl;
  for (ClassTable::iterator it = classTable.begin(); it != classTable.end();
       it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
    if (it->second.superClassName != "")
      std::cout << genIndent(indent + 4) << it->second.superClassName << ","
                << std::endl;
    print(*it->second.members, indent + 4);
    std::cout << "," << std::endl;
    print(*it->second.methods, indent + 4);
    std::cout << std::endl;
    std::cout << genIndent(indent + 2) << "}";
    if (it != --classTable.end()) std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}" << std::endl;
}

void print(ClassTable classTable) { print(classTable, 0); }
