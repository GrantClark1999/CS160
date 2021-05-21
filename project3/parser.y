%{
    #include <cstdlib>
    #include <cstdio>
    #include <iostream>

    #define YYDEBUG 1

    int yylex(void);
    void yyerror(const char *);
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

/* Bison Grammar Specification */
%%

Start         : Class Classes
              ;

Classes       : Class Classes
              | %empty
              ;

Class         : T_ID T_OPENCURLY Members T_CLOSEDCURLY
              | T_ID T_EXTENDS T_ID T_OPENCURLY Members T_CLOSEDCURLY
              ;

Members       : Type T_ID T_SEMICOLON Members
              | Methods
              ;

Methods       : T_ID T_OPENPAREN Params T_CLOSEDPAREN T_ARROW Type T_OPENCURLY Decls Return T_CLOSEDCURLY Methods
              | %empty
              ;

Params        : Type T_ID Params_
              | %empty
              ;

Params_       : T_COMMA Type T_ID Params_
              | %empty
              ;

Decls         : Type T_ID Ids T_SEMICOLON Decls
              | Stmts
              ;

Type          : T_BOOLEAN
              | T_INTEGER
              | T_NONE
              | T_ID
              ;

Ids           : T_COMMA T_ID Ids
              | %empty
              ;

Block         : T_OPENCURLY Stmt Stmts T_CLOSEDCURLY

Stmts         : Stmt Stmts
              | %empty
              ;

Stmt          : T_ID MemberRef T_ASSIGN Expr T_SEMICOLON
              | T_ID MemberRef MethodArgs
              | T_IF Expr Block Else
              | T_WHILE Expr Block
              | T_DO Block T_WHILE T_OPENPAREN Expr T_CLOSEDPAREN T_SEMICOLON
              | T_PRINT EndExpr
              ;

Return        : T_RETURN EndExpr
              | %empty
              ;

Else          : T_ELSE Block
              | %empty
              ;

EndExpr       : Expr T_SEMICOLON
              ;

Expr          : Expr T_PLUS Expr
              | Expr T_MINUS Expr
              | Expr T_MULT Expr
              | Expr T_DIV Expr
              | Expr T_GTR Expr
              | Expr T_GEQ Expr
              | Expr T_EQUALS Expr
              | Expr T_AND Expr
              | Expr T_OR Expr
              | T_NOT Expr
              | T_MINUS Expr %prec T_NOT
              | T_ID MemberRef OptnlArgs
              | T_OPENPAREN Expr T_CLOSEDPAREN
              | T_NUMBER
              | T_TRUE
              | T_FALSE
              | T_NEW T_ID OptnlArgs
              ;

MemberRef     : T_DOT T_ID
              | %empty
              ;

OptnlArgs     : MethodArgs
              | %empty
              ;

MethodArgs    : T_OPENPAREN Args T_CLOSEDPAREN
              ;

Args          : Expr Args_
              | %empty
              ;

Args_         : T_COMMA Expr Args_
              | %empty
              ;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(1);
}
