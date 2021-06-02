#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.


void CodeGenerator::visitProgramNode(ProgramNode* node) {
    std::cout << ".data" << std::endl;
    std::cout << "printstr: .asciz \"%d\\n\"" << std::endl;
    std::cout << ".text" << std::endl;
    std::cout << ".globl Main_main" << std::endl;

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

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
    std::cout << "    # MethodBody" << std::endl;

    std::cout << "    push %ebp" << std::endl;
    std::cout << "    mov %esp, %ebp" << std::endl;
    std::cout << "    sub $" << currentMethodInfo.localsSize << ", %esp" << std::endl;

    node->visit_children(this);

    std::cout << "    leave" << std::endl;
    std::cout << "    ret" << std::endl; 
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    node->visit_children(this);

    std::cout << "    # ReturnStatement" << std::endl;

    std::cout << "    pop %eax" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
    node->visit_children(this);

}

void CodeGenerator::visitCallNode(CallNode* node) {
    node->visit_children(this);

    std::cout << "    # CallNode" << std::endl;

    std::cout << "    add $4, %esp" << std::endl;
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    node->expression->accept(this);
    auto elseLabel = nextLabel();
    auto endLabel = nextLabel();

    std::cout << "    # IfElse" << std::endl;

    std::cout << "    pop %eax" << std::endl;
    std::cout << "    mov $0, %ebx" << std::endl;
    std::cout << "    cmp %eax, %ebx" << std::endl;
    std::cout << "    je " << elseLabel << std::endl;

    for (auto i = node->statement_list_1->begin(); i != node->statement_list_1->end(); i++)
        (*i)->accept(this);

    std::cout << "    jmp " << endLabel << std::endl;
    std::cout << "    " << elseLabel << ":" << std::endl;

    for (auto i = node->statement_list_2->begin(); i != node->statement_list_2->end(); i++)
        (*i)->accept(this);

    std::cout << "    " << endLabel << ":" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
    node->expression->accept(this);
    auto startLabel = nextLabel();
    auto exitLabel = nextLabel();

    std::cout << "    # While" << std::endl;

    std::cout << "    pop %eax" << std::endl;
    std::cout << "    mov $0, %ebx" << std::endl;
    std::cout << "    cmp %eax, %ebx" << std::endl;
    std::cout << "    je " << exitLabel << std::endl;
    std::cout << "    " << startLabel << ":" << std::endl;

    for (auto i = node->statement_list->begin(); i != node->statement_list->end(); i++)
        (*i)->accept(this);

    std::cout << "    jmp " << startLabel << std::endl;
    std::cout << "    " << exitLabel << ":" << std::endl;
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    node->visit_children(this);

    std::cout << "    # Print" << std::endl;

    std::cout << "    push $printstr" << std::endl;
    std::cout << "    call printf" << std::endl;
    std::cout << "    add $8, %esp" << std::endl;
}

// CHECK
void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
    node->expression->accept(this);
    auto startLabel = nextLabel();
    auto exitLabel = nextLabel();

    std::cout << "    # DoWhile" << std::endl;

    std::cout << "    " << startLabel << ":" << std::endl;

    for (auto i = node->statement_list->begin(); i != node->statement_list->end(); i++)
        (*i)->accept(this);

    std::cout << "    pop %eax" << std::endl;
    std::cout << "    mov $0, %ebx" << std::endl;
    std::cout << "    cmp %eax, %ebx" << std::endl;
    std::cout << "    jne " << startLabel << std::endl;
    std::cout << "    " << exitLabel << ":" << std::endl;
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
    node->visit_children(this);

    std::cout << "    # Plus" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    add %ebx, %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
    node->visit_children(this);

    std::cout << "    # Minus" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    sub %ebx, %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
    node->visit_children(this);

    std::cout << "    # Times" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    imul %ebx, %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
    node->visit_children(this);

    std::cout << "    # Divide" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    cdq" << std::endl;
    std::cout << "    idiv %ebx" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
    node->visit_children(this);
    auto tLabel = nextLabel();
    auto eLabel = nextLabel();

    std::cout << "    # Greater" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    cmp %ebx, %eax" << std::endl;
    std::cout << "    jg " << tLabel << std::endl;
    std::cout << "    push $0" << std::endl;
    std::cout << "    jmp " << eLabel << std::endl;
    std::cout << "    " << tLabel << ":" << std::endl;
    std::cout << "    push $1" << std::endl;
    std::cout << "    " << eLabel << ":" << std::endl;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
    node->visit_children(this);
    auto tLabel = nextLabel();
    auto eLabel = nextLabel();

    std::cout << "    # GreaterEqual" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    cmp %ebx, %eax" << std::endl;
    std::cout << "    jge " << tLabel << std::endl;
    std::cout << "    push $0" << std::endl;
    std::cout << "    jmp " << eLabel << std::endl;
    std::cout << "    " << tLabel << ":" << std::endl;
    std::cout << "    push $1" << std::endl;
    std::cout << "    " << eLabel << ":" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
    node->visit_children(this);
    auto tLabel = nextLabel();
    auto eLabel = nextLabel();

    std::cout << "    # Equal" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    cmp %ebx, %eax" << std::endl;
    std::cout << "    je " << tLabel << std::endl;
    std::cout << "    push $0" << std::endl;
    std::cout << "    jmp " << eLabel << std::endl;
    std::cout << "    " << tLabel << ":" << std::endl;
    std::cout << "    push $1" << std::endl;
    std::cout << "    " << eLabel << ":" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {
    node->visit_children(this);

    std::cout << "    # And" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    and %ebx, %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {
    node->visit_children(this);

    std::cout << "    # Or" << std::endl;

    std::cout << "    pop %ebx" << std::endl;
    std::cout << "    pop %eax" << std::endl;
    std::cout << "    or %ebx, %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {
    node->visit_children(this);

    std::cout << "    # Not" << std::endl;

    std::cout << "    pop %eax" << std::endl;
    std::cout << "    xor $1, %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    node->visit_children(this);

    std::cout << "    # Negation" << std::endl;

    std::cout << "    pop %eax" << std::endl;
    std::cout << "    neg %eax" << std::endl;
    std::cout << "    push %eax" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {

}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
    node->visit_children(this);

}

void CodeGenerator::visitVariableNode(VariableNode* node) {

}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    std::cout << "    push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    std::cout << "    push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitNewNode(NewNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
    // WRITEME: Replace with code if necessary
}
