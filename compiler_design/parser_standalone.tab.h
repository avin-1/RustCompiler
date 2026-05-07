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

#ifndef YY_YY_PARSER_STANDALONE_TAB_H_INCLUDED
# define YY_YY_PARSER_STANDALONE_TAB_H_INCLUDED
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
    TYPE_I8 = 265,                 /* TYPE_I8  */
    TYPE_I16 = 266,                /* TYPE_I16  */
    TYPE_I32 = 267,                /* TYPE_I32  */
    TYPE_I64 = 268,                /* TYPE_I64  */
    TYPE_I128 = 269,               /* TYPE_I128  */
    TYPE_U8 = 270,                 /* TYPE_U8  */
    TYPE_U16 = 271,                /* TYPE_U16  */
    TYPE_U32 = 272,                /* TYPE_U32  */
    TYPE_U64 = 273,                /* TYPE_U64  */
    TYPE_U128 = 274,               /* TYPE_U128  */
    TYPE_F32 = 275,                /* TYPE_F32  */
    TYPE_F64 = 276,                /* TYPE_F64  */
    TYPE_BOOL = 277,               /* TYPE_BOOL  */
    PRINTLN = 278,                 /* PRINTLN  */
    INT_LIT = 279,                 /* INT_LIT  */
    FLOAT_LIT = 280,               /* FLOAT_LIT  */
    STRING_LIT = 281,              /* STRING_LIT  */
    IDENTIFIER = 282,              /* IDENTIFIER  */
    TRUE_LIT = 283,                /* TRUE_LIT  */
    FALSE_LIT = 284,               /* FALSE_LIT  */
    EQ = 285,                      /* EQ  */
    NEQ = 286,                     /* NEQ  */
    LEQ = 287,                     /* LEQ  */
    GEQ = 288,                     /* GEQ  */
    ARROW = 289,                   /* ARROW  */
    UMINUS = 290                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 422 "parser_standalone.y"

    int    ival;
    double fval;
    char   sval[64];
    struct Node *node;
    struct {
        char   name[64];
        int    type;
        int    is_const;
        int    int_val;
        double float_val;
        struct Node *node;
    } expr;

#line 114 "parser_standalone.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_STANDALONE_TAB_H_INCLUDED  */
