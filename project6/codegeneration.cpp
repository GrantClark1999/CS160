#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

void CodeGenerator::visitProgramNode(ProgramNode* node) {
  std::cout << "  .data" << std::endl;
  std::cout << "  printstr: .asciz \"%d\\n\"" << std::endl;
  std::cout << "  .text" << std::endl;
  std::cout << "  .globl Main_main" << std::endl;

  node->visit_children(this);
}

void CodeGenerator::visitClassNode(ClassNode* node) {
  currentClassName = node->identifier_1->name;
  currentClassInfo = classTable->at(currentClassName);
  node->visit_children(this);
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
  currentMethodName = node->identifier->name;
  currentMethodInfo = currentClassInfo.methods->at(currentMethodName);
  std::cout << currentClassName << '_' << currentMethodName << ':' << std::endl;
  node->visit_children(this);
}

// CHECK - B
void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
  std::cout << "# METHOD BODY" << std::endl;
  std::cout << "  push %ebp" << std::endl;
  std::cout << "  mov %esp, %ebp" << std::endl;
  std::cout << "  sub $" << currentMethodInfo.localsSize << ", %esp"
            << std::endl;

  node->visit_children(this);
  std::cout << "  add $" << currentMethodInfo.localsSize <<", %esp" << std::endl;
	std::cout << "  pop %ebp" << std::endl;
  std::cout << "  ret" << std::endl;
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
  node->visit_children(this);
  std::cout << "# RETURN" << std::endl;
  std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
  node->visit_children(this);
  std::cout << "# ASSIGNMENT TO: "
            << node->identifier_1->name
            << (node->identifier_2 ? "." + node->identifier_2->name : "")
            << std::endl;

  int offset = 0;
  ClassInfo id2Class;

  // identifier_1 is a local variable
  if (currentMethodInfo.variables->count(node->identifier_1->name)) {
    VariableInfo var = currentMethodInfo.variables->at(node->identifier_1->name);
    offset = var.offset;
    if (node->identifier_2) {
      std::cout << "  mov "<< offset <<"(%ebp), %ecx" << std::endl;
			id2Class = classTable->at(var.type.objectClassName);
    } else {
      std::cout << "  pop %eax" << std::endl;
			std::cout << "  mov %eax, "<< offset << "(%ebp)" << std::endl;
    }
  }

  // identifier_1 is a member variable
  else {
    // Find class for identifier_1
    ClassInfo classInfo = classTable->at(currentClassName);
    while (!classInfo.members->count(node->identifier_1->name)) {
      classInfo = classTable->at(classInfo.superClassName);
    }
    // Calculate offset based on superclasses
    VariableInfo var = classInfo.members->at(node->identifier_1->name);
    offset = var.offset;
		while (!classInfo.superClassName.empty()) {
      classInfo = classTable->at(classInfo.superClassName);
			offset += classInfo.membersSize;
		}
    if (node->identifier_2) {
      id2Class = classTable->at(var.type.objectClassName);
			std::cout << "  mov 8(%ebp), %ebx" << std::endl;
			std::cout << "  mov " << offset << "(%ebx), %ecx" << std::endl;
		}
		else {
			std::cout << "  pop %eax" << std::endl;
			std::cout << "  mov 8(%ebp), %ebx" << std::endl;
			std::cout << "  mov %eax, " << offset << "(%ebx)" << std::endl;
		}
  }

  if (node->identifier_2) {
    while (!id2Class.members->count(node->identifier_2->name)) {
      id2Class = classTable->at(id2Class.superClassName);
    }
    VariableInfo var = id2Class.members->at(node->identifier_2->name);
    offset = var.offset;

    // Calculate offset based on superclasses
		while (!id2Class.superClassName.empty()) {
      id2Class = classTable->at(id2Class.superClassName);
			offset += id2Class.membersSize;
		}

    std::cout << "  pop %eax" << std::endl;
		std::cout << "  mov %eax, "<< offset << "(%ecx)" << std::endl;
  }
}

void CodeGenerator::visitCallNode(CallNode* node) {
  node->visit_children(this);
  std::cout << "# CALL NODE" << std::endl;
  std::cout << "  add $4, %esp" << std::endl;
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
  node->expression->accept(this);
  std::string elseLabel = "label_" + std::to_string(nextLabel());
  std::string endLabel = "label_" + std::to_string(nextLabel());

  std::cout << "# IF ELSE" << std::endl;

  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp $0, %eax" << std::endl;
  std::cout << "  je " << elseLabel << std::endl;

  if (node->statement_list_1)
    for (auto stmt : *(node->statement_list_1)) stmt->accept(this);

  std::cout << "  jmp " << endLabel << std::endl;
  std::cout << elseLabel << ":" << std::endl;

  if (node->statement_list_2)
    for (auto stmt : *(node->statement_list_2)) stmt->accept(this);

  std::cout << endLabel << ":" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
  std::string startLabel = "label_" + std::to_string(nextLabel());
  std::string exitLabel = "label_" + std::to_string(nextLabel());

  std::cout << "# WHILE" << std::endl;
  std::cout << startLabel << ":" << std::endl;
  node->expression->accept(this);
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp $0, %eax" << std::endl;
  std::cout << "  je " << exitLabel << std::endl;

  for (auto stmt : *(node->statement_list)) stmt->accept(this);

  std::cout << "  jmp " << startLabel << std::endl;
  std::cout << exitLabel << ":" << std::endl;
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
  node->visit_children(this);

  std::cout << "# PRINT" << std::endl;

  std::cout << "  push $printstr" << std::endl;
  std::cout << "  call printf" << std::endl;
  std::cout << "  add $8, %esp" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
  std::string startLabel = "label_" + std::to_string(nextLabel());
  std::string exitLabel = "label_" + std::to_string(nextLabel());

  std::cout << "# DO WHILE" << std::endl;

  std::cout << startLabel << ":" << std::endl;

  for (auto stmt : *(node->statement_list)) stmt->accept(this);
  node->expression->accept(this);

  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp $0, %eax" << std::endl;
  std::cout << "  jne " << startLabel << std::endl;
  std::cout << exitLabel << ":" << std::endl;
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
  node->visit_children(this);
  std::cout << "# PLUS" << std::endl;
  std::cout << "  pop %edx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  add %edx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
  node->visit_children(this);
  std::cout << "# MINUS" << std::endl;
  std::cout << "  pop %edx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  sub %edx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
  node->visit_children(this);
  std::cout << "# TIMES" << std::endl;
  std::cout << "  pop %edx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  imul %edx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
  node->visit_children(this);
  std::cout << "# DIVIDE" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cdq" << std::endl;
  std::cout << "  idiv %ebx" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
  node->visit_children(this);
  std::string tLabel = "label_" + std::to_string(nextLabel());
  std::string eLabel = "label_" + std::to_string(nextLabel());

  std::cout << "# GREATER" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp %ebx, %eax" << std::endl;
  std::cout << "  jg " << tLabel << std::endl;
  std::cout << "  push $0" << std::endl;
  std::cout << "  jmp " << eLabel << std::endl;
  std::cout << tLabel << ":" << std::endl;
  std::cout << "  push $1" << std::endl;
  std::cout << eLabel << ":" << std::endl;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
  node->visit_children(this);
  std::string tLabel = "label_" + std::to_string(nextLabel());
  std::string eLabel = "label_" + std::to_string(nextLabel());

  std::cout << "# GREATER EQUAL" << std::endl;

  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp %ebx, %eax" << std::endl;
  std::cout << "  jge " << tLabel << std::endl;
  std::cout << "  push $0" << std::endl;
  std::cout << "  jmp " << eLabel << std::endl;
  std::cout << tLabel << ":" << std::endl;
  std::cout << "  push $1" << std::endl;
  std::cout << eLabel << ":" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
  node->visit_children(this);
  std::string tLabel = "label_" + std::to_string(nextLabel());
  std::string eLabel = "label_" + std::to_string(nextLabel());

  std::cout << "# EQUAL" << std::endl;

  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp %ebx, %eax" << std::endl;
  std::cout << "  je " << tLabel << std::endl;
  std::cout << "  push $0" << std::endl;
  std::cout << "  jmp " << eLabel << std::endl;
  std::cout << tLabel << ":" << std::endl;
  std::cout << "  push $1" << std::endl;
  std::cout << eLabel << ":" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {
  node->visit_children(this);

  std::cout << "# AND" << std::endl;

  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  and %ebx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {
  node->visit_children(this);

  std::cout << "# OR" << std::endl;

  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  or %ebx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {
  node->visit_children(this);

  std::cout << "# NOT" << std::endl;

  std::cout << "  pop %eax" << std::endl;
  std::cout << "  xor $1, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
  node->visit_children(this);

  std::cout << "# NEGATION" << std::endl;

  std::cout << "  pop %eax" << std::endl;
  std::cout << "  neg %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
  node->visit_children(this);

  std::cout << "# CALLING METHOD "
            << node->identifier_1->name
            << (node->identifier_2 ? "." + node->identifier_2->name : "")
            << std::endl;

  std::string className = currentClassName;
  ClassInfo classInfo = classTable->at(className);
  std::string methodName = node->identifier_1->name;
  int offset;

  // Pattern: foo.bar()
  if (node->identifier_2) {
    VariableInfo var;
    if (currentMethodInfo.variables->count(node->identifier_1->name)) {
      var = currentMethodInfo.variables->at(node->identifier_1->name);
      offset = var.offset;
      std::cout << "  mov " << offset << "(%ebp), %eax" << std::endl;
    } else {
      while (!classInfo.members->count(node->identifier_1->name)) {
        className = classInfo.superClassName;
        classInfo = classTable->at(className);
      }
      var = classInfo.members->at(node->identifier_1->name);
      offset = var.offset;
      ClassInfo id1Class = classInfo;
      while (!id1Class.superClassName.empty()) {
        id1Class = classTable->at(classInfo.superClassName);
			  offset += id1Class.membersSize;
		  }
      std::cout << "  mov 8(%ebp), %ebx" << std::endl;
			std::cout << "  mov " << offset	<< "(%ebx), %eax" << std::endl;
    }
    className = var.type.objectClassName;
    classInfo = classTable->at(className);
    methodName = node->identifier_2->name;
  } 
  // Pattern: foo()
  else {
    std::cout << "  mov 8(%ebp), %eax" << std::endl;
  }

  // Search class and superclasses for method.
  while (!classInfo.methods->count(methodName)) {
    className = classInfo.superClassName;
    classInfo = classTable->at(className);
  }

  std::cout << "  push %eax" << std::endl;
  std::cout << "  call " << className << "_" << methodName << std::endl;
  std::cout << "  add $" << 4 * (node->expression_list->size() + 1 ) << ", %esp"
            << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
  std::cout << "  # ACCESSING MEMBER: " << node->identifier_1->name << "." << node->identifier_2->name << std::endl;

  int offset = 0;
  ClassInfo id2Class;

  // identifier_1 is a local variable
  if (currentMethodInfo.variables->count(node->identifier_1->name)) {
    VariableInfo var = currentMethodInfo.variables->at(node->identifier_1->name);
    offset = var.offset;
    std::cout << "  mov "<< offset <<"(%ebp), %ecx" << std::endl;
		id2Class = classTable->at(var.type.objectClassName);
  }

  // identifier_1 is a member variable
  else {
    // Find class for identifier_1
    ClassInfo classInfo = classTable->at(currentClassName);
    while (!classInfo.members->count(node->identifier_1->name)) {
      classInfo = classTable->at(classInfo.superClassName);
    }

    // Calculate offset based on superclasses
    VariableInfo var = classInfo.members->at(node->identifier_1->name);
    offset = var.offset;
		while (!classInfo.superClassName.empty()) {
      classInfo = classTable->at(classInfo.superClassName);
			offset += classInfo.membersSize;
		}

    id2Class = classTable->at(var.type.objectClassName);
		std::cout << "  mov 8(%ebp), %ebx" << std::endl;
		std::cout << "  mov " << offset << "(%ebx), %ecx" << std::endl;
  }

  while (!id2Class.members->count(node->identifier_2->name)) {
    id2Class = classTable->at(id2Class.superClassName);
  }
  VariableInfo var = id2Class.members->at(node->identifier_2->name);
  offset = var.offset;

  // Calculate offset based on superclasses
	while (!id2Class.superClassName.empty()) {
    id2Class = classTable->at(id2Class.superClassName);
		offset += id2Class.membersSize;
	}

	std::cout << "  mov "<< offset << "(%ecx), %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

// CHECK - A
void CodeGenerator::visitVariableNode(VariableNode* node) {
  std::cout << "# LOAD VARIABLE " << node->identifier->name << std::endl;

  // Local Variable
  if (currentMethodInfo.variables->count(node->identifier->name)) {
    int offset = currentMethodInfo.variables->at(node->identifier->name).offset;
    std::cout << "  mov " << offset << "(%ebp), %eax" << std::endl;
  }

  // Member Variable
  else {
    ClassInfo classInfo = currentClassInfo;
    // Find member in class heirarchy
    while (!classInfo.members->count(node->identifier->name)) {
      classInfo = classTable->at(classInfo.superClassName);
    }

    // Offset within member class
    int offset = classInfo.members->at(node->identifier->name).offset;
    // Offset of other super classes
    while (!classInfo.superClassName.empty()) {
      classInfo = classTable->at(classInfo.superClassName);
      offset += classInfo.membersSize;
    }

    std::cout << "  mov " << "8(%ebp), %eax" << std::endl;
    std::cout << "  mov " << offset << "(%eax), %eax" << std::endl;
  }

  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  std::cout << "# INTEGER" << std::endl;
  std::cout << "  push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  std::cout << "# BOOLEAN" << std::endl;
  std::cout << "  push $" << node->integer->value << std::endl;
}

// CHECK - A
void CodeGenerator::visitNewNode(NewNode* node) {
  int stackOffset =
      node->expression_list ? 4 * (node->expression_list->size() + 1) : 4;

  ClassInfo classInfo = classTable->at(node->identifier->name);
  bool hasConstructor = classInfo.methods->count(node->identifier->name);
  int size = classInfo.membersSize;
	while (!classInfo.superClassName.empty()) {
    classInfo = classTable->at(classInfo.superClassName);
		size += classInfo.membersSize;
	}

  std::cout << "# NEW" << std::endl;

  std::cout << "  push $" << size << std::endl;
  std::cout << "  call malloc" << std::endl;
  std::cout << "  add $4, %esp" << std::endl;
  std::cout << "  push %eax" << std::endl;

  if (hasConstructor) {
    if (node->expression_list) node->expression_list->reverse();
    node->visit_children(this);

    std::cout << "  mov "
              << (node->expression_list
                      ? std::to_string(node->expression_list->size() * 4)
                      : "")
              << "(%esp), %eax" << std::endl;

    std::cout << "  push %eax" << std::endl;
    std::cout << "  call " << node->identifier->name << "_"
              << node->identifier->name << std::endl;
    std::cout << "  add $" << stackOffset << ", %esp" << std::endl;
  }
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {}

void CodeGenerator::visitNoneNode(NoneNode* node) {}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {}