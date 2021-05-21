%option yylineno
%pointer

%{
    #include <errno.h>
    #include <limits.h>
    #include <stdlib.h>
    #include <string.h>

    #include "ast.hpp"
    #include "parser.hpp"
    void yyerror(const char *);
%}

%x COMMENT

%%

"/*"                    { BEGIN(COMMENT); }
<COMMENT>[^*\n]*        ;
<COMMENT>"*"+[^*/\n]*   ;
<COMMENT>\n             ;
<COMMENT>"*"+"/"        { BEGIN(INITIAL); }
<COMMENT><<EOF>>        { yyerror("Danging comment"); }

[ \n\t]+                ;

"extends"               { return T_EXTENDS; }
"{"                     { return T_OPENCURLY; }
"}"                     { return T_CLOSEDCURLY; }
","                     { return T_COMMA; }
"->"                    { return T_ARROW; }
"return"                { return T_RETURN; }
"new"                   { return T_NEW; }
"."                     { return T_DOT; }

"if"                    { return T_IF; }
"else"                  { return T_ELSE; }
"do"                    { return T_DO; }
"while"                 { return T_WHILE; }
"print"                 { return T_PRINT; }
"="                     { return T_ASSIGN; }
";"                     { return T_SEMICOLON; }

"true"                  { yylval.base_int = 1; return T_TRUE; }
"false"                 { yylval.base_int = 0; return T_FALSE; }
"or"                    { return T_OR; }
"and"                   { return T_AND; }
"not"                   { return T_NOT; }
">"                     { return T_GTR; }
">="                    { return T_GEQ; }
"equals"                { return T_EQUALS; }

"boolean"               { return T_BOOLEAN; }
"integer"               { return T_INTEGER; }
"none"                  { return T_NONE; }

"+"                     { return T_PLUS; }
"-"                     { return T_MINUS; }
"*"                     { return T_MULT; }
"/"                     { return T_DIV; }
"("                     { return T_OPENPAREN; }
")"                     { return T_CLOSEDPAREN; }

[[:alpha:]][[:alnum:]]* { yylval.base_char_ptr = strdup(yytext); return T_ID; }
0|[1-9][[:digit:]]*     { yylval.base_int = atoi(yytext); return T_NUMBER; }

.                       { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
