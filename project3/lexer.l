%option yylineno
%pointer

%{
    #include <cstdlib>
    #include <cerrno>
    #include <climits>
    #include <limits>
    #include "parser.hpp"
    
	void yyerror(const char *);
%}
  /* Definitions */
ID          [[:alpha:]][[:alnum:]]*
NUMBER      0|[1-9][[:digit:]]*
WHITESPACE  [ \n\t]

  /* Start Conditions */
%x COMMENT

  /* Flex Token Rules */
%%
  /* Ignored Tokens */
"/*"                    { BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT><<EOF>>        { yyerror("Danging comment"); }
<COMMENT>\n             ;
<COMMENT>.              ;
{WHITESPACE}+           ; /* ignore whitespace */

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

"true"                  { return T_TRUE; }
"false"                 { return T_FALSE; }
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

{ID}                    { return T_ID; }
{NUMBER}                { return T_NUMBER; }

  /* If character doesn't match any of the above, throw an error. */
.                       { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
