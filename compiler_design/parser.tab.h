/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LET = 258,                     /* LET  */
    MUT = 259,                     /* MUT  */
    FN = 260,                      /* FN  */
    IF = 261,                      /* IF  */
    ELSE = 262,                    /* ELSE  */
    WHILE = 263,                   /* WHILE  */
    RETURN = 264,                  /* RETURN  */
    TYPE_I32 = 265,                /* TYPE_I32  */
    TYPE_F64 = 266,                /* TYPE_F64  */
    TYPE_BOOL = 267,               /* TYPE_BOOL  */
    PRINTLN = 268,                 /* PRINTLN  */
    INT_LIT = 269,                 /* INT_LIT  */
    FLOAT_LIT = 270,               /* FLOAT_LIT  */
    STRING_LIT = 271,              /* STRING_LIT  */
    IDENTIFIER = 272,              /* IDENTIFIER  */
    EQ = 273,                      /* EQ  */
    NEQ = 274,                     /* NEQ  */
    LEQ = 275,                     /* LEQ  */
    GEQ = 276,                     /* GEQ  */
    ARROW = 277,                   /* ARROW  */
    UMINUS = 278                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 77 "parser.y"

    int    ival;           /* integer literal value                */
    double fval;           /* float literal value                  */
    char   sval[64];       /* identifier / string                  */
    struct Node *node;     /* parse tree node (for statements etc) */
    struct {
        char   name[64];   /* TAC temporary or variable name       */
        int    type;       /* VarType: 0=unknown, 1=int, 2=float   */
        int    is_const;   /* 1 if value is a known constant       */
        int    int_val;    /* integer constant value                */
        double float_val;  /* float constant value                  */
        struct Node *node; /* parse tree node for this expression   */
    } expr;                /* expression attributes                 */

#line 102 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
