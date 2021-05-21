%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>

    #include "ast.hpp"
    
    #define YYDEBUG 1
    #define YYINITDEPTH 10000
    int yylex(void);
    void yyerror(const char *);
    
    extern ASTNode* astRoot;
%}

%define parse.error verbose

/* Tokens */
%token T_EXTENDS T_OPENCURLY T_CLOSEDCURLY T_COMMA T_ARROW T_RETURN T_NEW T_DOT
%token T_IF T_ELSE T_DO T_WHILE T_PRINT T_ASSIGN T_SEMICOLON
%token T_TRUE T_FALSE T_OR T_AND T_NOT T_GTR T_GEQ T_EQUALS
%token T_BOOLEAN T_INTEGER T_NONE
%token T_PLUS T_MINUS T_MULT T_DIV T_OPENPAREN T_CLOSEDPAREN
%token T_ID T_NUMBER

/* Precedence */
%left T_OR
%left T_AND
%left T_GTR T_GEQ T_EQUALS
%left T_PLUS T_MINUS
%left T_MULT T_DIV
%precedence T_NOT

/* Non-terminal types for AST */
%type <program_ptr>           Start
%type <class_list_ptr>        Classes
%type <class_ptr>             Class ClassBody
%type <method_list_ptr>       Methods
%type <method_ptr>            Method
%type <parameter_list_ptr>    Parameters
%type <parameter_ptr>         Parameter
%type <methodbody_ptr>        MethodBody
%type <declaration_list_ptr>  Declarations
%type <declaration_ptr>       Declaration
%type <type_ptr>              Type
%type <identifier_list_ptr>   Ids
%type <statement_list_ptr>    Statements Block
%type <statement_ptr>         Statement
%type <returnstatement_ptr>   Return
%type <assignment_ptr>        Assignment
%type <methodcall_ptr>        MethodCall
%type <ifelse_ptr>            IfElse
%type <while_ptr>             While
%type <dowhile_ptr>           DoWhile
%type <print_ptr>             Print
%type <expression_list_ptr>   Arguments Arguments_
%type <expression_ptr>        Expression

/* Terminal types for AST */
%type <base_int>              T_NUMBER T_TRUE T_FALSE
%type <base_char_ptr>         T_ID


/* Bison Grammar Specification */
%%

Start         : Classes { $$ = new ProgramNode($1); astRoot = $$; }
              ;

Classes       : Classes Class { $$ = $1; $$->push_back($2); }
              | Class         { $$ = new std::list<ClassNode*>(); $$->push_back($1); }
              ;

Class         : T_ID T_OPENCURLY ClassBody T_CLOSEDCURLY                { $$ = new ClassNode(new IdentifierNode($1), NULL, $3->declaration_list, $3->method_list); }
              | T_ID T_EXTENDS T_ID T_OPENCURLY ClassBody T_CLOSEDCURLY { $$ = new ClassNode(new IdentifierNode($1), new IdentifierNode($3), $5->declaration_list, $5->method_list); }
              ;

ClassBody     : Declarations Methods  { $$ = new ClassNode(NULL, NULL, $1, $2); }
              | Declarations          { $$ = new ClassNode(NULL, NULL, $1, NULL); }
              | Methods               { $$ = new ClassNode(NULL, NULL, NULL, $1); }
              | %empty                { $$ = new ClassNode(NULL, NULL, NULL, NULL); }
              ;

Methods       : Methods Method  { $$ = $1; $$->push_back($2); }
              | Method          { $$ = new std::list<MethodNode*>(); $$->push_back($1); }
              ;

Method        : T_ID T_OPENPAREN Parameters T_CLOSEDPAREN T_ARROW Type T_OPENCURLY MethodBody T_CLOSEDCURLY { $$ = new MethodNode(new IdentifierNode($1), $3, $6, $8); }
              ;

Parameters    : Parameters Parameter  { $$ = $1; $$->push_back($2); }
              | %empty                { $$ = new std::list<ParameterNode*>(); }
              ;

Parameter     : Type T_ID         { $$ = new ParameterNode($1, new IdentifierNode($2)); }
              | Type T_ID T_COMMA { $$ = new ParameterNode($1, new IdentifierNode($2)); }
              ;

MethodBody    : Declarations Statements Return  { $$ = new MethodBodyNode($1, $2, $3); }
              | Declarations Statements         { $$ = new MethodBodyNode($1, $2, NULL); }
              | Declarations Return             { $$ = new MethodBodyNode($1, new std::list<StatementNode*>(), $2); }
              | Declarations                    { $$ = new MethodBodyNode($1, new std::list<StatementNode*>(), NULL); }
              | Statements Return               { $$ = new MethodBodyNode(new std::list<DeclarationNode*>(), $1, $2); }
              | Statements                      { $$ = new MethodBodyNode(new std::list<DeclarationNode*>(), $1, NULL); }
              | Return                          { $$ = new MethodBodyNode(new std::list<DeclarationNode*>(), new std::list<StatementNode*>(), $1); }
              | %empty                          { $$ = new MethodBodyNode(new std::list<DeclarationNode*>(), new std::list<StatementNode*>(), NULL); }
              ;

Declarations  : Declarations Declaration  { $$ = $1; $$->push_back($2); }
              | Declaration               { $$ = new std::list<DeclarationNode*>(); $$->push_back($1); }
              ;

Declaration   : Type Ids T_SEMICOLON {$$ = new DeclarationNode($1, $2); }
              ;

Type          : T_BOOLEAN { $$ = new BooleanTypeNode(); }
              | T_INTEGER { $$ = new IntegerTypeNode(); }
              | T_NONE    { $$ = new NoneNode(); }
              | T_ID      { $$ = new ObjectTypeNode(new IdentifierNode($1)); }
              ;

Ids           : Ids T_COMMA T_ID  { $$ = $1; $$->push_back(new IdentifierNode($3)); }
              | T_ID              { $$ = new std::list<IdentifierNode*>(); $$->push_back(new IdentifierNode($1)); }
              ;

Statements    : Statements Statement  { $$ = $1; $$->push_back($2); }
              | Statement             { $$ = new std::list<StatementNode*>(); $$->push_back($1); }
              ;

Statement     : Assignment              { $$ = $1; }
              | MethodCall T_SEMICOLON  { $$ = new CallNode($1); }
              | IfElse                  { $$ = $1; }
              | While                   { $$ = $1; }
              | DoWhile                 { $$ = $1; }
              | Print                   { $$ = $1; }
              ;

Return        : T_RETURN Expression T_SEMICOLON { $$ = new ReturnStatementNode($2); }
              ;

Assignment    : T_ID T_ASSIGN Expression T_SEMICOLON            { $$ = new AssignmentNode(new IdentifierNode($1), NULL, $3); }
              | T_ID T_DOT T_ID T_ASSIGN Expression T_SEMICOLON { $$ = new AssignmentNode(new IdentifierNode($1), new IdentifierNode($3), $5); }
              ;

MethodCall    : T_ID T_OPENPAREN Arguments T_CLOSEDPAREN            { $$ = new MethodCallNode(new IdentifierNode($1), NULL, $3); }
              | T_ID T_DOT T_ID T_OPENPAREN Arguments T_CLOSEDPAREN { $$ = new MethodCallNode(new IdentifierNode($1), new IdentifierNode($3), $5); }
              ;

IfElse        : T_IF Expression Block               { $$ = new IfElseNode($2, $3, NULL); }
              | T_IF Expression Block T_ELSE Block  { $$ = new IfElseNode($2, $3, $5); }
              ;

While         : T_WHILE Expression Block  { $$ = new WhileNode($2, $3); }
              ;

DoWhile       : T_DO Block T_WHILE T_OPENPAREN Expression T_CLOSEDPAREN T_SEMICOLON { $$ = new DoWhileNode($2, $5); }
              ;

Block         : T_OPENCURLY Statements T_CLOSEDCURLY  { $$ = $2; }
              ;

Print         : T_PRINT Expression T_SEMICOLON  { $$ = new PrintNode($2); }
              ;

Arguments     : Arguments_  { $$ = $1; }
              | %empty      { $$ = new std::list<ExpressionNode*>(); }                                 
              ;

Arguments_    : Arguments T_COMMA Expression  { $$->push_back($3); }        
              | Expression                    { $$ = new std::list<ExpressionNode*>(); $$->push_back($1); }                   
              ;

Expression    : Expression T_PLUS Expression                    { $$ = new PlusNode($1, $3); }
              | Expression T_MINUS Expression                   { $$ = new MinusNode($1, $3); }
              | Expression T_MULT Expression                    { $$ = new TimesNode($1, $3); }
              | Expression T_DIV Expression                     { $$ = new DivideNode($1, $3); }
              | Expression T_GTR Expression                     { $$ = new GreaterNode($1, $3); }
              | Expression T_GEQ Expression                     { $$ = new GreaterEqualNode($1, $3); }
              | Expression T_EQUALS Expression                  { $$ = new EqualNode($1, $3); }
              | Expression T_AND Expression                     { $$ = new AndNode($1, $3); }
              | Expression T_OR Expression                      { $$ = new OrNode($1, $3); }
              | T_NOT Expression                                { $$ = new NotNode($2); }
              | T_MINUS Expression %prec T_NOT                  { $$ = new NegationNode($2); }
              | T_ID                                            { $$ = new VariableNode(new IdentifierNode($1)); }
              | T_ID T_DOT T_ID                                 { $$ = new MemberAccessNode(new IdentifierNode($1), new IdentifierNode($3)); }
              | MethodCall                                      { $$ = $1; }
              | T_OPENPAREN Expression T_CLOSEDPAREN            { $$ = $2; }
              | T_NUMBER                                        { $$ = new IntegerLiteralNode(new IntegerNode($1)); }
              | T_TRUE                                          { $$ = new BooleanLiteralNode(new IntegerNode($1)); }
              | T_FALSE                                         { $$ = new BooleanLiteralNode(new IntegerNode($1)); }
              | T_NEW T_ID                                      { $$ = new NewNode(new IdentifierNode($2), new std::list<ExpressionNode*>()); }
              | T_NEW T_ID T_OPENPAREN Arguments T_CLOSEDPAREN  { $$ = new NewNode(new IdentifierNode($2), $4); }
              ;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(1);
}
