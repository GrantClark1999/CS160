/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_EXTENDS = 258,
    T_OPENCURLY = 259,
    T_CLOSEDCURLY = 260,
    T_COMMA = 261,
    T_ARROW = 262,
    T_RETURN = 263,
    T_NEW = 264,
    T_DOT = 265,
    T_IF = 266,
    T_ELSE = 267,
    T_DO = 268,
    T_WHILE = 269,
    T_PRINT = 270,
    T_ASSIGN = 271,
    T_SEMICOLON = 272,
    T_TRUE = 273,
    T_FALSE = 274,
    T_OR = 275,
    T_AND = 276,
    T_NOT = 277,
    T_GTR = 278,
    T_GEQ = 279,
    T_EQUALS = 280,
    T_BOOLEAN = 281,
    T_INTEGER = 282,
    T_NONE = 283,
    T_PLUS = 284,
    T_MINUS = 285,
    T_MULT = 286,
    T_DIV = 287,
    T_OPENPAREN = 288,
    T_CLOSEDPAREN = 289,
    T_ID = 290,
    T_NUMBER = 291
  };
#endif

/* Value type.  */


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_HPP_INCLUDED  */
