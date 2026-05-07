/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 14 "parser_standalone.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* ================================================================ */
/*  SYMBOL TABLE - embedded implementation                          */
/* ================================================================ */

#define MAX_SYMBOLS 256

typedef enum {
    UNKNOWN_TYPE,
    I8_TYPE,
    I16_TYPE,
    I32_TYPE,
    I64_TYPE,
    I128_TYPE,
    U8_TYPE,
    U16_TYPE,
    U32_TYPE,
    U64_TYPE,
    U128_TYPE,
    F32_TYPE,
    F64_TYPE,
    BOOL_TYPE
} VarType;

typedef struct {
    char   name[64];
    VarType type;
    int    is_defined;
    int    scope;
    int    int_val;
    double float_val;
    int    is_const;
} Symbol;

static Symbol sym_table[MAX_SYMBOLS];
static int    sym_count_var = 0;
static int    current_scope = 0;

static int is_integer_type(VarType type) {
    return (type >= I8_TYPE && type <= U128_TYPE) || type == BOOL_TYPE;
}

static int is_float_type(VarType type) {
    return type == F32_TYPE || type == F64_TYPE;
}

static int is_signed_int_type(VarType type) {
    return (type >= I8_TYPE && type <= I128_TYPE);
}

static int is_unsigned_int_type(VarType type) {
    return (type >= U8_TYPE && type <= U128_TYPE);
}

/* Check if expr_type can be assigned to declared_type */
static int types_compatible(VarType declared_type, VarType expr_type) {
    if (declared_type == expr_type) return 1;
    if (expr_type == UNKNOWN_TYPE) return 1;
    
    /* Allow I32_TYPE literals to be assigned to any integer type */
    if (expr_type == I32_TYPE && is_integer_type(declared_type)) return 1;
    
    /* Allow F64_TYPE literals to be assigned to any float type */
    if (expr_type == F64_TYPE && is_float_type(declared_type)) return 1;
    
    return 0;
}

void sym_init(void) {
    sym_count_var = 0;
    current_scope = 0;
    memset(sym_table, 0, sizeof(sym_table));
}

void sym_enter_scope(void) {
    current_scope++;
}

void sym_exit_scope(void) {
    /* Mark symbols from current scope as out of scope instead of removing them */
    /* This preserves the symbol table for analysis after parsing */
    for (int i = 0; i < sym_count_var; i++) {
        if (sym_table[i].scope == current_scope) {
            sym_table[i].scope = -1;  /* Mark as out of scope */
        }
    }
    current_scope--;
}

int sym_lookup(const char *name) {
    /* Search from most recent (highest index) to oldest */
    for (int i = sym_count_var - 1; i >= 0; i--) {
        if (strcmp(sym_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int sym_lookup_current_scope(const char *name) {
    /* Only search in current scope */
    for (int i = sym_count_var - 1; i >= 0; i--) {
        if (sym_table[i].scope != current_scope) break;
        if (strcmp(sym_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int sym_insert(const char *name, VarType type) {
    if (sym_lookup_current_scope(name) >= 0) return -1;
    if (sym_count_var >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: symbol table overflow\n");
        return -1;
    }
    strncpy(sym_table[sym_count_var].name, name, 63);
    sym_table[sym_count_var].name[63] = '\0';
    sym_table[sym_count_var].type = type;
    sym_table[sym_count_var].is_defined = 0;
    sym_table[sym_count_var].scope = current_scope;
    sym_table[sym_count_var].is_const = 0;
    return sym_count_var++;
}

void sym_set_int(int index, int value) {
    if (index < 0 || index >= sym_count_var) return;
    sym_table[index].int_val = value;
    sym_table[index].float_val = (double)value;
    sym_table[index].is_defined = 1;
    sym_table[index].is_const = 1;
}

void sym_set_float(int index, double value) {
    if (index < 0 || index >= sym_count_var) return;
    sym_table[index].float_val = value;
    sym_table[index].int_val = (int)value;
    sym_table[index].is_defined = 1;
    sym_table[index].is_const = 1;
}

VarType sym_get_type(int index) {
    if (index < 0 || index >= sym_count_var) return UNKNOWN_TYPE;
    return sym_table[index].type;
}

void sym_print(void) {
    printf("\n=============================================================\n");
    printf("                      SYMBOL  TABLE                          \n");
    printf("=============================================================\n");
    printf("%-4s %-16s %-10s %-10s %-10s %-10s\n",
           "Idx", "Name", "Type", "Defined?", "Scope", "Value");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < sym_count_var; i++) {
        const char *type_str = "unknown";
        switch (sym_table[i].type) {
            case I8_TYPE:    type_str = "i8";    break;
            case I16_TYPE:   type_str = "i16";   break;
            case I32_TYPE:   type_str = "i32";   break;
            case I64_TYPE:   type_str = "i64";   break;
            case I128_TYPE:  type_str = "i128";  break;
            case U8_TYPE:    type_str = "u8";    break;
            case U16_TYPE:   type_str = "u16";   break;
            case U32_TYPE:   type_str = "u32";   break;
            case U64_TYPE:   type_str = "u64";   break;
            case U128_TYPE:  type_str = "u128";  break;
            case F32_TYPE:   type_str = "f32";   break;
            case F64_TYPE:   type_str = "f64";   break;
            case BOOL_TYPE:  type_str = "bool";  break;
            default:         type_str = "unknown"; break;
        }
        
        const char *scope_str;
        char scope_buf[16];
        if (sym_table[i].scope == -1) {
            scope_str = "exited";
        } else {
            sprintf(scope_buf, "%d", sym_table[i].scope);
            scope_str = scope_buf;
        }
        
        printf("%-4d %-16s %-10s %-10s %-10s ",
               i, sym_table[i].name, type_str,
               sym_table[i].is_defined ? "yes" : "no",
               scope_str);
        if (sym_table[i].is_defined) {
            if (sym_table[i].type >= I8_TYPE && sym_table[i].type <= BOOL_TYPE)
                printf("%d", sym_table[i].int_val);
            else if (sym_table[i].type == F32_TYPE || sym_table[i].type == F64_TYPE)
                printf("%.2f", sym_table[i].float_val);
        } else {
            printf("-");
        }
        printf("\n");
    }
    printf("=============================================================\n\n");
}

int sym_count(void) {
    return sym_count_var;
}

const Symbol *sym_get(int index) {
    if (index < 0 || index >= sym_count_var) return NULL;
    return &sym_table[index];
}

/* ================================================================ */
/*  INTERMEDIATE CODE (TAC) - embedded implementation               */
/* ================================================================ */

#define MAX_TAC 1024

typedef struct {
    char result[64];
    char arg1[64];
    char op[4];
    char arg2[64];
} TACInstr;

static TACInstr tac_instructions[MAX_TAC];
static int      tac_instr_count = 0;
static int      tac_temp_counter = 0;
static char     tac_temp_buf[64];

void tac_init(void) {
    tac_instr_count = 0;
    tac_temp_counter = 0;
    memset(tac_instructions, 0, sizeof(tac_instructions));
}

char *tac_new_temp(void) {
    sprintf(tac_temp_buf, "t%d", tac_temp_counter++);
    return tac_temp_buf;
}

void tac_emit(const char *result, const char *arg1, const char *op, const char *arg2) {
    if (tac_instr_count >= MAX_TAC) {
        fprintf(stderr, "Error: TAC buffer overflow\n");
        return;
    }
    TACInstr *ins = &tac_instructions[tac_instr_count++];
    strncpy(ins->result, result ? result : "", 63);
    strncpy(ins->arg1,   arg1   ? arg1   : "", 63);
    strncpy(ins->op,     op     ? op     : "", 3);
    strncpy(ins->arg2,   arg2   ? arg2   : "", 63);
}

void tac_print(void) {
    printf("\n=============================================================\n");
    printf("          INTERMEDIATE CODE  (Three-Address Code)            \n");
    printf("=============================================================\n");
    for (int i = 0; i < tac_instr_count; i++) {
        TACInstr *ins = &tac_instructions[i];
        if (ins->op[0] != '\0') {
            printf("  %s = %s %s %s\n", ins->result, ins->arg1, ins->op, ins->arg2);
        } else {
            printf("  %s = %s\n", ins->result, ins->arg1);
        }
    }
    printf("=============================================================\n\n");
}

static int is_integer_literal(const char *s) {
    if (!s || !*s) return 0;
    if (*s == '-') s++;
    if (!*s) return 0;
    while (*s) {
        if (*s < '0' || *s > '9') return 0;
        s++;
    }
    return 1;
}

void tac_print_optimised(void) {
    printf("\n=============================================================\n");
    printf("     OPTIMISED CODE  (after constant folding)                \n");
    printf("=============================================================\n");
    for (int i = 0; i < tac_instr_count; i++) {
        TACInstr *ins = &tac_instructions[i];
        if (ins->op[0] != '\0' && is_integer_literal(ins->arg1) && is_integer_literal(ins->arg2)) {
            int a = atoi(ins->arg1);
            int b = atoi(ins->arg2);
            int result = 0;
            int valid = 1;
            switch (ins->op[0]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': if (b != 0) result = a / b; else valid = 0; break;
                default: valid = 0; break;
            }
            if (valid) {
                printf("  %s = %d    ; folded from: %s %s %s\n",
                       ins->result, result, ins->arg1, ins->op, ins->arg2);
                continue;
            }
        }
        if (ins->op[0] != '\0') {
            printf("  %s = %s %s %s\n", ins->result, ins->arg1, ins->op, ins->arg2);
        } else {
            printf("  %s = %s\n", ins->result, ins->arg1);
        }
    }
    printf("=============================================================\n\n");
}

int tac_count(void) {
    return tac_instr_count;
}

/* ================================================================ */
/*  PARSE TREE - embedded implementation                            */
/* ================================================================ */

typedef struct Node {
    char         label[50];
    int          id;
    struct Node *children[10];
    int          child_count;
} Node;

static int global_node_id = 1;

Node *createNode(const char *name, Node *left, Node *right) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Error: out of memory in createNode\n");
        exit(1);
    }
    strncpy(n->label, name, 49);
    n->label[49] = '\0';
    n->id = global_node_id++;
    n->child_count = 0;
    if (left)  n->children[n->child_count++] = left;
    if (right) n->children[n->child_count++] = right;
    return n;
}

Node *createLeaf(const char *name) {
    return createNode(name, NULL, NULL);
}

void printTree(Node *node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s\n", node->label);
    for (int i = 0; i < node->child_count; i++) {
        printTree(node->children[i], indent + 1);
    }
}

static void saveTreeHelper(Node *node, int indent, FILE *fp) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) fprintf(fp, "  ");
    fprintf(fp, "%s\n", node->label);
    for (int i = 0; i < node->child_count; i++) {
        saveTreeHelper(node->children[i], indent + 1, fp);
    }
}

void saveTree(Node *node, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    fprintf(fp, "=============================================================\n");
    fprintf(fp, "                    PARSE TREE OUTPUT                        \n");
    fprintf(fp, "=============================================================\n");
    saveTreeHelper(node, 0, fp);
    fprintf(fp, "=============================================================\n");
    fclose(fp);
}

void freeTree(Node *node) {
    if (node == NULL) return;
    for (int i = 0; i < node->child_count; i++) {
        freeTree(node->children[i]);
    }
    free(node);
}

/* ================================================================ */
/*  BISON DECLARATIONS                                              */
/* ================================================================ */

extern int  yylex(void);
extern int  yylineno;
extern FILE *yyin;
extern char *yytext;

void yyerror(const char *msg);

int debug_tokens    = 1;
int has_errors      = 0;
int semantic_errors = 0;

Node *parse_tree_root = NULL;

static void sem_error(const char *fmt, ...);

#line 476 "parser_standalone.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser_standalone.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_LET = 3,                        /* LET  */
  YYSYMBOL_MUT = 4,                        /* MUT  */
  YYSYMBOL_FN = 5,                         /* FN  */
  YYSYMBOL_IF = 6,                         /* IF  */
  YYSYMBOL_ELSE = 7,                       /* ELSE  */
  YYSYMBOL_WHILE = 8,                      /* WHILE  */
  YYSYMBOL_RETURN = 9,                     /* RETURN  */
  YYSYMBOL_TYPE_I8 = 10,                   /* TYPE_I8  */
  YYSYMBOL_TYPE_I16 = 11,                  /* TYPE_I16  */
  YYSYMBOL_TYPE_I32 = 12,                  /* TYPE_I32  */
  YYSYMBOL_TYPE_I64 = 13,                  /* TYPE_I64  */
  YYSYMBOL_TYPE_I128 = 14,                 /* TYPE_I128  */
  YYSYMBOL_TYPE_U8 = 15,                   /* TYPE_U8  */
  YYSYMBOL_TYPE_U16 = 16,                  /* TYPE_U16  */
  YYSYMBOL_TYPE_U32 = 17,                  /* TYPE_U32  */
  YYSYMBOL_TYPE_U64 = 18,                  /* TYPE_U64  */
  YYSYMBOL_TYPE_U128 = 19,                 /* TYPE_U128  */
  YYSYMBOL_TYPE_F32 = 20,                  /* TYPE_F32  */
  YYSYMBOL_TYPE_F64 = 21,                  /* TYPE_F64  */
  YYSYMBOL_TYPE_BOOL = 22,                 /* TYPE_BOOL  */
  YYSYMBOL_PRINTLN = 23,                   /* PRINTLN  */
  YYSYMBOL_INT_LIT = 24,                   /* INT_LIT  */
  YYSYMBOL_FLOAT_LIT = 25,                 /* FLOAT_LIT  */
  YYSYMBOL_STRING_LIT = 26,                /* STRING_LIT  */
  YYSYMBOL_IDENTIFIER = 27,                /* IDENTIFIER  */
  YYSYMBOL_TRUE_LIT = 28,                  /* TRUE_LIT  */
  YYSYMBOL_FALSE_LIT = 29,                 /* FALSE_LIT  */
  YYSYMBOL_EQ = 30,                        /* EQ  */
  YYSYMBOL_NEQ = 31,                       /* NEQ  */
  YYSYMBOL_LEQ = 32,                       /* LEQ  */
  YYSYMBOL_GEQ = 33,                       /* GEQ  */
  YYSYMBOL_ARROW = 34,                     /* ARROW  */
  YYSYMBOL_35_ = 35,                       /* '<'  */
  YYSYMBOL_36_ = 36,                       /* '>'  */
  YYSYMBOL_37_ = 37,                       /* '+'  */
  YYSYMBOL_38_ = 38,                       /* '-'  */
  YYSYMBOL_39_ = 39,                       /* '*'  */
  YYSYMBOL_40_ = 40,                       /* '/'  */
  YYSYMBOL_UMINUS = 41,                    /* UMINUS  */
  YYSYMBOL_42_ = 42,                       /* '('  */
  YYSYMBOL_43_ = 43,                       /* ')'  */
  YYSYMBOL_44_ = 44,                       /* ','  */
  YYSYMBOL_45_ = 45,                       /* ':'  */
  YYSYMBOL_46_ = 46,                       /* '{'  */
  YYSYMBOL_47_ = 47,                       /* '}'  */
  YYSYMBOL_48_ = 48,                       /* ';'  */
  YYSYMBOL_49_ = 49,                       /* '='  */
  YYSYMBOL_YYACCEPT = 50,                  /* $accept  */
  YYSYMBOL_program = 51,                   /* program  */
  YYSYMBOL_item_list = 52,                 /* item_list  */
  YYSYMBOL_item = 53,                      /* item  */
  YYSYMBOL_function_def = 54,              /* function_def  */
  YYSYMBOL_param_list = 55,                /* param_list  */
  YYSYMBOL_56_1 = 56,                      /* $@1  */
  YYSYMBOL_param_list_items = 57,          /* param_list_items  */
  YYSYMBOL_param = 58,                     /* param  */
  YYSYMBOL_block = 59,                     /* block  */
  YYSYMBOL_60_2 = 60,                      /* $@2  */
  YYSYMBOL_61_3 = 61,                      /* $@3  */
  YYSYMBOL_stmt_list = 62,                 /* stmt_list  */
  YYSYMBOL_statement = 63,                 /* statement  */
  YYSYMBOL_declaration = 64,               /* declaration  */
  YYSYMBOL_type_spec = 65,                 /* type_spec  */
  YYSYMBOL_assignment = 66,                /* assignment  */
  YYSYMBOL_if_stmt = 67,                   /* if_stmt  */
  YYSYMBOL_while_stmt = 68,                /* while_stmt  */
  YYSYMBOL_println_stmt = 69,              /* println_stmt  */
  YYSYMBOL_return_stmt = 70,               /* return_stmt  */
  YYSYMBOL_expr_stmt = 71,                 /* expr_stmt  */
  YYSYMBOL_expression = 72,                /* expression  */
  YYSYMBOL_term = 73,                      /* term  */
  YYSYMBOL_factor = 74,                    /* factor  */
  YYSYMBOL_arg_list = 75                   /* arg_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  44
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   292

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  78
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  144

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   290


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      42,    43,    39,    37,    44,    38,     2,    40,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    45,    48,
      35,    49,    36,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,     2,    47,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      41
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   479,   479,   486,   494,   498,   505,   506,   510,   516,
     522,   530,   541,   541,   548,   552,   559,   572,   572,   572,
     580,   583,   594,   595,   596,   597,   598,   599,   600,   601,
     609,   627,   648,   666,   690,   691,   692,   693,   694,   695,
     696,   697,   698,   699,   700,   701,   702,   706,   725,   730,
     739,   747,   753,   762,   767,   775,   782,   802,   822,   831,
     840,   849,   858,   867,   876,   883,   903,   923,   930,   941,
     952,   961,   970,   992,  1005,  1018,  1022,  1044,  1048
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "LET", "MUT", "FN",
  "IF", "ELSE", "WHILE", "RETURN", "TYPE_I8", "TYPE_I16", "TYPE_I32",
  "TYPE_I64", "TYPE_I128", "TYPE_U8", "TYPE_U16", "TYPE_U32", "TYPE_U64",
  "TYPE_U128", "TYPE_F32", "TYPE_F64", "TYPE_BOOL", "PRINTLN", "INT_LIT",
  "FLOAT_LIT", "STRING_LIT", "IDENTIFIER", "TRUE_LIT", "FALSE_LIT", "EQ",
  "NEQ", "LEQ", "GEQ", "ARROW", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "UMINUS", "'('", "')'", "','", "':'", "'{'", "'}'", "';'", "'='",
  "$accept", "program", "item_list", "item", "function_def", "param_list",
  "$@1", "param_list_items", "param", "block", "$@2", "$@3", "stmt_list",
  "statement", "declaration", "type_spec", "assignment", "if_stmt",
  "while_stmt", "println_stmt", "return_stmt", "expr_stmt", "expression",
  "term", "factor", "arg_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-78)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-4)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      67,   -34,     2,   -10,    59,    59,   -16,   -22,   -78,   -78,
       7,   -78,   -78,    59,    59,    25,   107,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   120,     6,   -78,
     -78,     1,   -15,   -11,    -9,   198,   198,   -78,   129,    35,
      99,    59,   -78,   210,   -78,   -78,    59,    59,    59,    59,
      59,    59,    59,    59,   -78,    59,    59,     9,   253,    59,
      -2,   -78,    44,   -78,   -78,    16,   -78,   246,    20,   143,
     -78,     6,     6,     6,     6,     6,     6,     6,     6,   -78,
     -78,   253,    59,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,    17,   152,   -27,    28,
      47,   -78,    31,    32,    59,   -78,    59,   -78,    30,   166,
      59,   -78,   253,   -78,   -19,    37,    41,   -78,    15,   -78,
     -78,   219,   246,    59,   -78,   175,    31,   253,   -78,   253,
      47,   -78,   -78,    50,   189,   -78,   -78,    31,   -78,   -78,
     -78,   -78,   -78,   -78
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,    68,    69,
      72,    70,    71,     0,     0,     0,     0,     4,     7,     6,
      22,    23,    24,    25,    26,    27,    28,     0,    64,    67,
      29,     0,     0,     0,    72,     0,     0,    54,     0,     0,
       0,     0,    76,     0,     1,     5,     0,     0,     0,     0,
       0,     0,     0,     0,    55,     0,     0,     0,     0,     0,
      12,    17,    48,    50,    53,     0,    73,    77,     0,     0,
      75,    60,    61,    62,    63,    58,    59,    56,    57,    65,
      66,     0,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,     0,     0,     0,     0,
       0,    20,     0,     0,     0,    74,     0,    47,     0,     0,
       0,    30,     0,     8,     0,     0,    13,    14,     0,    49,
      51,     0,    78,     0,    32,     0,     0,     0,    11,     0,
       0,    18,    21,     0,     0,    31,     9,     0,    16,    15,
      19,    52,    33,    10
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -78,   -78,   -78,    73,   -78,   -78,   -78,   -78,   -31,   -33,
     -78,   -78,   -78,    -7,   -78,   -77,   -78,   -78,   -78,   -78,
     -78,   -78,    -4,   239,    -8,   -78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    15,    16,    17,    18,    99,   100,   116,   117,    62,
     101,   140,   118,    19,    20,    96,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      35,    36,    38,    63,   108,    42,    31,   112,     8,     9,
      43,    34,    11,    12,    30,   127,     1,    33,     2,    61,
      39,     4,    13,     5,     6,    44,    14,    61,    57,    32,
      58,    60,    37,    40,    59,   126,    67,    69,     7,     8,
       9,    98,    10,    11,    12,    55,    56,    79,    80,    40,
     137,   102,   138,    13,    81,    97,    41,    14,    82,   103,
     104,    65,   131,   105,   106,   113,   110,    -3,     1,   119,
       2,   114,     3,     4,   115,     5,     6,    61,   109,   123,
     120,   128,   129,     8,     9,   130,    34,    11,    12,    45,
       7,     8,     9,   136,    10,    11,    12,    13,   141,   139,
     121,    14,   122,     0,   143,    13,   125,    -2,     1,    14,
       2,   132,     3,     4,     0,     5,     6,     0,     0,   134,
       0,     0,     0,     8,     9,     0,    34,    11,    12,     0,
       7,     8,     9,     0,    10,    11,    12,    13,     0,     0,
       0,    14,    66,     0,     0,    13,     0,     0,     0,    14,
      46,    47,    48,    49,     0,    50,    51,    52,    53,    46,
      47,    48,    49,     0,    50,    51,    52,    53,    54,     0,
       0,     0,     0,    46,    47,    48,    49,    64,    50,    51,
      52,    53,    46,    47,    48,    49,     0,    50,    51,    52,
      53,   107,     0,     0,     0,     0,    46,    47,    48,    49,
     111,    50,    51,    52,    53,    46,    47,    48,    49,     0,
      50,    51,    52,    53,   124,     0,     0,     0,     0,    46,
      47,    48,    49,   135,    50,    51,    52,    53,    46,    47,
      48,    49,     0,    50,    51,    52,    53,   142,     0,     0,
      46,    47,    48,    49,    61,    50,    51,    52,    53,    46,
      47,    48,    49,    70,    50,    51,    52,    53,     0,     0,
       0,     0,   133,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    46,    47,    48,    49,
       0,    50,    51,    52,    53,    71,    72,    73,    74,    75,
      76,    77,    78
};

static const yytype_int16 yycheck[] =
{
       4,     5,     6,    36,    81,    13,     4,    34,    24,    25,
      14,    27,    28,    29,    48,    34,     1,    27,     3,    46,
      42,     6,    38,     8,     9,     0,    42,    46,    27,    27,
      45,    42,    48,    42,    49,   112,    40,    41,    23,    24,
      25,    43,    27,    28,    29,    39,    40,    55,    56,    42,
     127,     7,   129,    38,    45,    59,    49,    42,    49,    43,
      44,    26,    47,    43,    44,    98,    49,     0,     1,   102,
       3,    43,     5,     6,    27,     8,     9,    46,    82,    49,
      48,   114,    45,    24,    25,    44,    27,    28,    29,    16,
      23,    24,    25,   126,    27,    28,    29,    38,    48,   130,
     104,    42,   106,    -1,   137,    38,   110,     0,     1,    42,
       3,   118,     5,     6,    -1,     8,     9,    -1,    -1,   123,
      -1,    -1,    -1,    24,    25,    -1,    27,    28,    29,    -1,
      23,    24,    25,    -1,    27,    28,    29,    38,    -1,    -1,
      -1,    42,    43,    -1,    -1,    38,    -1,    -1,    -1,    42,
      30,    31,    32,    33,    -1,    35,    36,    37,    38,    30,
      31,    32,    33,    -1,    35,    36,    37,    38,    48,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    48,    35,    36,
      37,    38,    30,    31,    32,    33,    -1,    35,    36,    37,
      38,    48,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      48,    35,    36,    37,    38,    30,    31,    32,    33,    -1,
      35,    36,    37,    38,    48,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    48,    35,    36,    37,    38,    30,    31,
      32,    33,    -1,    35,    36,    37,    38,    48,    -1,    -1,
      30,    31,    32,    33,    46,    35,    36,    37,    38,    30,
      31,    32,    33,    43,    35,    36,    37,    38,    -1,    -1,
      -1,    -1,    43,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    30,    31,    32,    33,
      -1,    35,    36,    37,    38,    46,    47,    48,    49,    50,
      51,    52,    53
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     5,     6,     8,     9,    23,    24,    25,
      27,    28,    29,    38,    42,    51,    52,    53,    54,    63,
      64,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      48,     4,    27,    27,    27,    72,    72,    48,    72,    42,
      42,    49,    74,    72,     0,    53,    30,    31,    32,    33,
      35,    36,    37,    38,    48,    39,    40,    27,    45,    49,
      42,    46,    59,    59,    48,    26,    43,    72,    75,    72,
      43,    73,    73,    73,    73,    73,    73,    73,    73,    74,
      74,    45,    49,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    65,    72,    43,    55,
      56,    60,     7,    43,    44,    43,    44,    48,    65,    72,
      49,    48,    34,    59,    43,    27,    57,    58,    62,    59,
      48,    72,    72,    49,    48,    72,    65,    34,    59,    45,
      44,    47,    63,    43,    72,    48,    59,    65,    65,    58,
      61,    48,    48,    59
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    50,    51,    51,    52,    52,    53,    53,    54,    54,
      54,    54,    56,    55,    57,    57,    58,    60,    61,    59,
      62,    62,    63,    63,    63,    63,    63,    63,    63,    63,
      64,    64,    64,    64,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    66,    67,    67,
      68,    69,    69,    70,    70,    71,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    73,    73,    73,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    75,    75
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     1,     2,     1,     1,     5,     7,
       8,     6,     0,     2,     1,     3,     3,     0,     0,     5,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       5,     7,     6,     8,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     3,     5,
       3,     5,     7,     3,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     3,     4,     3,     2,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: item_list  */
#line 480 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].node);
            parse_tree_root = (yyval.node);
            printf("[Parser] program → item_list reached; parse_tree_root set.\n");
        }
#line 1667 "parser_standalone.tab.c"
    break;

  case 3: /* program: %empty  */
#line 486 "parser_standalone.y"
        {
            (yyval.node) = createNode("EMPTY_PROGRAM", NULL, NULL);
            parse_tree_root = (yyval.node);
            printf("[Parser] program → ε reached; parse_tree_root set to EMPTY_PROGRAM.\n");
        }
#line 1677 "parser_standalone.tab.c"
    break;

  case 4: /* item_list: item  */
#line 495 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].node);
        }
#line 1685 "parser_standalone.tab.c"
    break;

  case 5: /* item_list: item_list item  */
#line 499 "parser_standalone.y"
        {
            (yyval.node) = createNode("SEQ", (yyvsp[-1].node), (yyvsp[0].node));
        }
#line 1693 "parser_standalone.tab.c"
    break;

  case 6: /* item: statement  */
#line 505 "parser_standalone.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1699 "parser_standalone.tab.c"
    break;

  case 7: /* item: function_def  */
#line 506 "parser_standalone.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1705 "parser_standalone.tab.c"
    break;

  case 8: /* function_def: FN IDENTIFIER '(' ')' block  */
#line 511 "parser_standalone.y"
        {
            Node *nameNode = createLeaf((yyvsp[-3].sval));
            (yyval.node) = createNode("FUNC", nameNode, (yyvsp[0].node));
            printf("[Parser] Function '%s' defined.\n", (yyvsp[-3].sval));
        }
#line 1715 "parser_standalone.tab.c"
    break;

  case 9: /* function_def: FN IDENTIFIER '(' ')' ARROW type_spec block  */
#line 517 "parser_standalone.y"
        {
            Node *nameNode = createLeaf((yyvsp[-5].sval));
            (yyval.node) = createNode("FUNC", nameNode, (yyvsp[0].node));
            printf("[Parser] Function '%s' defined (returns type).\n", (yyvsp[-5].sval));
        }
#line 1725 "parser_standalone.tab.c"
    break;

  case 10: /* function_def: FN IDENTIFIER '(' param_list ')' ARROW type_spec block  */
#line 523 "parser_standalone.y"
        {
            sym_exit_scope();
            Node *nameNode = createLeaf((yyvsp[-6].sval));
            Node *funcNode = createNode("FUNC_PARAMS", nameNode, (yyvsp[-4].node));
            (yyval.node) = createNode("FUNC", funcNode, (yyvsp[0].node));
            printf("[Parser] Function '%s' defined with parameters.\n", (yyvsp[-6].sval));
        }
#line 1737 "parser_standalone.tab.c"
    break;

  case 11: /* function_def: FN IDENTIFIER '(' param_list ')' block  */
#line 531 "parser_standalone.y"
        {
            sym_exit_scope();
            Node *nameNode = createLeaf((yyvsp[-4].sval));
            Node *funcNode = createNode("FUNC_PARAMS", nameNode, (yyvsp[-2].node));
            (yyval.node) = createNode("FUNC", funcNode, (yyvsp[0].node));
            printf("[Parser] Function '%s' defined with parameters.\n", (yyvsp[-4].sval));
        }
#line 1749 "parser_standalone.tab.c"
    break;

  case 12: /* $@1: %empty  */
#line 541 "parser_standalone.y"
      { sym_enter_scope(); }
#line 1755 "parser_standalone.tab.c"
    break;

  case 13: /* param_list: $@1 param_list_items  */
#line 542 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].node);
        }
#line 1763 "parser_standalone.tab.c"
    break;

  case 14: /* param_list_items: param  */
#line 549 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].node);
        }
#line 1771 "parser_standalone.tab.c"
    break;

  case 15: /* param_list_items: param_list_items ',' param  */
#line 553 "parser_standalone.y"
        {
            (yyval.node) = createNode("PARAM_SEQ", (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1779 "parser_standalone.tab.c"
    break;

  case 16: /* param: IDENTIFIER ':' type_spec  */
#line 560 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-2].sval), (VarType)(yyvsp[0].ival));
            if (idx < 0) {
                sem_error("line %d: parameter '%s' already declared", yylineno, (yyvsp[-2].sval));
            }
            Node *id = createLeaf((yyvsp[-2].sval));
            (yyval.node) = createNode("PARAM", id, NULL);
            printf("[Parser] Parameter '%s' declared.\n", (yyvsp[-2].sval));
        }
#line 1793 "parser_standalone.tab.c"
    break;

  case 17: /* $@2: %empty  */
#line 572 "parser_standalone.y"
          { sym_enter_scope(); }
#line 1799 "parser_standalone.tab.c"
    break;

  case 18: /* $@3: %empty  */
#line 572 "parser_standalone.y"
                                               { sym_exit_scope(); }
#line 1805 "parser_standalone.tab.c"
    break;

  case 19: /* block: '{' $@2 stmt_list '}' $@3  */
#line 573 "parser_standalone.y"
        {
            (yyval.node) = createNode("BLOCK", (yyvsp[-2].node), NULL);
        }
#line 1813 "parser_standalone.tab.c"
    break;

  case 20: /* stmt_list: %empty  */
#line 580 "parser_standalone.y"
        {
            (yyval.node) = NULL;
        }
#line 1821 "parser_standalone.tab.c"
    break;

  case 21: /* stmt_list: stmt_list statement  */
#line 584 "parser_standalone.y"
        {
            if ((yyvsp[-1].node) == NULL) {
                (yyval.node) = (yyvsp[0].node);
            } else {
                (yyval.node) = createNode("SEQ", (yyvsp[-1].node), (yyvsp[0].node));
            }
        }
#line 1833 "parser_standalone.tab.c"
    break;

  case 22: /* statement: declaration  */
#line 594 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1839 "parser_standalone.tab.c"
    break;

  case 23: /* statement: assignment  */
#line 595 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1845 "parser_standalone.tab.c"
    break;

  case 24: /* statement: if_stmt  */
#line 596 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1851 "parser_standalone.tab.c"
    break;

  case 25: /* statement: while_stmt  */
#line 597 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1857 "parser_standalone.tab.c"
    break;

  case 26: /* statement: println_stmt  */
#line 598 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1863 "parser_standalone.tab.c"
    break;

  case 27: /* statement: return_stmt  */
#line 599 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1869 "parser_standalone.tab.c"
    break;

  case 28: /* statement: expr_stmt  */
#line 600 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1875 "parser_standalone.tab.c"
    break;

  case 29: /* statement: error ';'  */
#line 602 "parser_standalone.y"
        {
            yyerrok;
            (yyval.node) = createNode("ERROR", NULL, NULL);
        }
#line 1884 "parser_standalone.tab.c"
    break;

  case 30: /* declaration: LET IDENTIFIER '=' expression ';'  */
#line 610 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-3].sval), (VarType)(yyvsp[-1].expr).type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-3].sval));
            } else {
                if ((yyvsp[-1].expr).is_const) {
                    if (is_integer_type((yyvsp[-1].expr).type))
                        sym_set_int(idx, (yyvsp[-1].expr).int_val);
                    else
                        sym_set_float(idx, (yyvsp[-1].expr).float_val);
                }
                tac_emit((yyvsp[-3].sval), (yyvsp[-1].expr).name, NULL, NULL);
                printf("[Parser] Declared '%s' (inferred type).\n", (yyvsp[-3].sval));
            }
            Node *id = createLeaf((yyvsp[-3].sval));
            (yyval.node) = createNode("LET", id, (yyvsp[-1].expr).node);
        }
#line 1906 "parser_standalone.tab.c"
    break;

  case 31: /* declaration: LET IDENTIFIER ':' type_spec '=' expression ';'  */
#line 628 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-5].sval), (VarType)(yyvsp[-3].ival));
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-5].sval));
            } else {
                if (!types_compatible((VarType)(yyvsp[-3].ival), (VarType)(yyvsp[-1].expr).type)) {
                    sem_error("line %d: type mismatch in declaration of '%s'", yylineno, (yyvsp[-5].sval));
                }
                if ((yyvsp[-1].expr).is_const) {
                    if (is_integer_type((yyvsp[-3].ival)))
                        sym_set_int(idx, (yyvsp[-1].expr).int_val);
                    else
                        sym_set_float(idx, (yyvsp[-1].expr).float_val);
                }
                tac_emit((yyvsp[-5].sval), (yyvsp[-1].expr).name, NULL, NULL);
                printf("[Parser] Declared '%s' with explicit type.\n", (yyvsp[-5].sval));
            }
            Node *id = createLeaf((yyvsp[-5].sval));
            (yyval.node) = createNode("LET", id, (yyvsp[-1].expr).node);
        }
#line 1931 "parser_standalone.tab.c"
    break;

  case 32: /* declaration: LET MUT IDENTIFIER '=' expression ';'  */
#line 649 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-3].sval), (VarType)(yyvsp[-1].expr).type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-3].sval));
            } else {
                if ((yyvsp[-1].expr).is_const) {
                    if (is_integer_type((yyvsp[-1].expr).type))
                        sym_set_int(idx, (yyvsp[-1].expr).int_val);
                    else
                        sym_set_float(idx, (yyvsp[-1].expr).float_val);
                }
                tac_emit((yyvsp[-3].sval), (yyvsp[-1].expr).name, NULL, NULL);
                printf("[Parser] Declared mutable '%s'.\n", (yyvsp[-3].sval));
            }
            Node *id = createLeaf((yyvsp[-3].sval));
            (yyval.node) = createNode("LET_MUT", id, (yyvsp[-1].expr).node);
        }
#line 1953 "parser_standalone.tab.c"
    break;

  case 33: /* declaration: LET MUT IDENTIFIER ':' type_spec '=' expression ';'  */
#line 667 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-5].sval), (VarType)(yyvsp[-3].ival));
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-5].sval));
            } else {
                if (!types_compatible((VarType)(yyvsp[-3].ival), (VarType)(yyvsp[-1].expr).type)) {
                    sem_error("line %d: type mismatch in declaration of '%s'", yylineno, (yyvsp[-5].sval));
                }
                if ((yyvsp[-1].expr).is_const) {
                    if (is_integer_type((yyvsp[-3].ival)))
                        sym_set_int(idx, (yyvsp[-1].expr).int_val);
                    else
                        sym_set_float(idx, (yyvsp[-1].expr).float_val);
                }
                tac_emit((yyvsp[-5].sval), (yyvsp[-1].expr).name, NULL, NULL);
                printf("[Parser] Declared mutable '%s' with type.\n", (yyvsp[-5].sval));
            }
            Node *id = createLeaf((yyvsp[-5].sval));
            (yyval.node) = createNode("LET_MUT", id, (yyvsp[-1].expr).node);
        }
#line 1978 "parser_standalone.tab.c"
    break;

  case 34: /* type_spec: TYPE_I8  */
#line 690 "parser_standalone.y"
                 { (yyval.ival) = I8_TYPE;    }
#line 1984 "parser_standalone.tab.c"
    break;

  case 35: /* type_spec: TYPE_I16  */
#line 691 "parser_standalone.y"
                 { (yyval.ival) = I16_TYPE;   }
#line 1990 "parser_standalone.tab.c"
    break;

  case 36: /* type_spec: TYPE_I32  */
#line 692 "parser_standalone.y"
                 { (yyval.ival) = I32_TYPE;   }
#line 1996 "parser_standalone.tab.c"
    break;

  case 37: /* type_spec: TYPE_I64  */
#line 693 "parser_standalone.y"
                 { (yyval.ival) = I64_TYPE;   }
#line 2002 "parser_standalone.tab.c"
    break;

  case 38: /* type_spec: TYPE_I128  */
#line 694 "parser_standalone.y"
                 { (yyval.ival) = I128_TYPE;  }
#line 2008 "parser_standalone.tab.c"
    break;

  case 39: /* type_spec: TYPE_U8  */
#line 695 "parser_standalone.y"
                 { (yyval.ival) = U8_TYPE;    }
#line 2014 "parser_standalone.tab.c"
    break;

  case 40: /* type_spec: TYPE_U16  */
#line 696 "parser_standalone.y"
                 { (yyval.ival) = U16_TYPE;   }
#line 2020 "parser_standalone.tab.c"
    break;

  case 41: /* type_spec: TYPE_U32  */
#line 697 "parser_standalone.y"
                 { (yyval.ival) = U32_TYPE;   }
#line 2026 "parser_standalone.tab.c"
    break;

  case 42: /* type_spec: TYPE_U64  */
#line 698 "parser_standalone.y"
                 { (yyval.ival) = U64_TYPE;   }
#line 2032 "parser_standalone.tab.c"
    break;

  case 43: /* type_spec: TYPE_U128  */
#line 699 "parser_standalone.y"
                 { (yyval.ival) = U128_TYPE;  }
#line 2038 "parser_standalone.tab.c"
    break;

  case 44: /* type_spec: TYPE_F32  */
#line 700 "parser_standalone.y"
                 { (yyval.ival) = F32_TYPE;   }
#line 2044 "parser_standalone.tab.c"
    break;

  case 45: /* type_spec: TYPE_F64  */
#line 701 "parser_standalone.y"
                 { (yyval.ival) = F64_TYPE;   }
#line 2050 "parser_standalone.tab.c"
    break;

  case 46: /* type_spec: TYPE_BOOL  */
#line 702 "parser_standalone.y"
                 { (yyval.ival) = BOOL_TYPE;  }
#line 2056 "parser_standalone.tab.c"
    break;

  case 47: /* assignment: IDENTIFIER '=' expression ';'  */
#line 707 "parser_standalone.y"
        {
            int idx = sym_lookup((yyvsp[-3].sval));
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'", yylineno, (yyvsp[-3].sval));
            } else {
                VarType declared = sym_get_type(idx);
                if (!types_compatible(declared, (VarType)(yyvsp[-1].expr).type)) {
                    sem_error("line %d: type mismatch in assignment to '%s'", yylineno, (yyvsp[-3].sval));
                }
                tac_emit((yyvsp[-3].sval), (yyvsp[-1].expr).name, NULL, NULL);
                printf("[Parser] Assignment to '%s'.\n", (yyvsp[-3].sval));
            }
            Node *id = createLeaf((yyvsp[-3].sval));
            (yyval.node) = createNode("ASSIGN", id, (yyvsp[-1].expr).node);
        }
#line 2076 "parser_standalone.tab.c"
    break;

  case 48: /* if_stmt: IF expression block  */
#line 726 "parser_standalone.y"
        {
            (yyval.node) = createNode("IF", (yyvsp[-1].expr).node, (yyvsp[0].node));
            printf("[Parser] If statement.\n");
        }
#line 2085 "parser_standalone.tab.c"
    break;

  case 49: /* if_stmt: IF expression block ELSE block  */
#line 731 "parser_standalone.y"
        {
            Node *branches = createNode("BRANCHES", (yyvsp[-2].node), (yyvsp[0].node));
            (yyval.node) = createNode("IF_ELSE", (yyvsp[-3].expr).node, branches);
            printf("[Parser] If-else statement.\n");
        }
#line 2095 "parser_standalone.tab.c"
    break;

  case 50: /* while_stmt: WHILE expression block  */
#line 740 "parser_standalone.y"
        {
            (yyval.node) = createNode("WHILE", (yyvsp[-1].expr).node, (yyvsp[0].node));
            printf("[Parser] While loop.\n");
        }
#line 2104 "parser_standalone.tab.c"
    break;

  case 51: /* println_stmt: PRINTLN '(' STRING_LIT ')' ';'  */
#line 748 "parser_standalone.y"
        {
            Node *str = createLeaf((yyvsp[-2].sval));
            (yyval.node) = createNode("PRINTLN", str, NULL);
            printf("[Parser] println! statement.\n");
        }
#line 2114 "parser_standalone.tab.c"
    break;

  case 52: /* println_stmt: PRINTLN '(' STRING_LIT ',' expression ')' ';'  */
#line 754 "parser_standalone.y"
        {
            Node *str = createLeaf((yyvsp[-4].sval));
            (yyval.node) = createNode("PRINTLN", str, (yyvsp[-2].expr).node);
            printf("[Parser] println! statement with expression.\n");
        }
#line 2124 "parser_standalone.tab.c"
    break;

  case 53: /* return_stmt: RETURN expression ';'  */
#line 763 "parser_standalone.y"
        {
            (yyval.node) = createNode("RETURN", (yyvsp[-1].expr).node, NULL);
            printf("[Parser] Return statement.\n");
        }
#line 2133 "parser_standalone.tab.c"
    break;

  case 54: /* return_stmt: RETURN ';'  */
#line 768 "parser_standalone.y"
        {
            (yyval.node) = createNode("RETURN", NULL, NULL);
            printf("[Parser] Return (void).\n");
        }
#line 2142 "parser_standalone.tab.c"
    break;

  case 55: /* expr_stmt: expression ';'  */
#line 776 "parser_standalone.y"
        {
            (yyval.node) = createNode("EXPR_STMT", (yyvsp[-1].expr).node, NULL);
        }
#line 2150 "parser_standalone.tab.c"
    break;

  case 56: /* expression: expression '+' term  */
#line 783 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && is_integer_type((yyvsp[-2].expr).type) && is_integer_type((yyvsp[0].expr).type)) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = (yyvsp[-2].expr).int_val + (yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = I32_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "+", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = (is_float_type((yyvsp[-2].expr).type) || is_float_type((yyvsp[0].expr).type)) ? F64_TYPE : I32_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "+", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("+", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2174 "parser_standalone.tab.c"
    break;

  case 57: /* expression: expression '-' term  */
#line 803 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && is_integer_type((yyvsp[-2].expr).type) && is_integer_type((yyvsp[0].expr).type)) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = (yyvsp[-2].expr).int_val - (yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = I32_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "-", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = (is_float_type((yyvsp[-2].expr).type) || is_float_type((yyvsp[0].expr).type)) ? F64_TYPE : I32_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "-", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("-", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2198 "parser_standalone.tab.c"
    break;

  case 58: /* expression: expression '<' term  */
#line 823 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "<", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("<", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2211 "parser_standalone.tab.c"
    break;

  case 59: /* expression: expression '>' term  */
#line 832 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, ">", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode(">", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2224 "parser_standalone.tab.c"
    break;

  case 60: /* expression: expression EQ term  */
#line 841 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "==", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("==", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2237 "parser_standalone.tab.c"
    break;

  case 61: /* expression: expression NEQ term  */
#line 850 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "!=", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("!=", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2250 "parser_standalone.tab.c"
    break;

  case 62: /* expression: expression LEQ term  */
#line 859 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "<=", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("<=", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2263 "parser_standalone.tab.c"
    break;

  case 63: /* expression: expression GEQ term  */
#line 868 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, ">=", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode(">=", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2276 "parser_standalone.tab.c"
    break;

  case 64: /* expression: term  */
#line 877 "parser_standalone.y"
        {
            (yyval.expr) = (yyvsp[0].expr);
        }
#line 2284 "parser_standalone.tab.c"
    break;

  case 65: /* term: term '*' factor  */
#line 884 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && is_integer_type((yyvsp[-2].expr).type) && is_integer_type((yyvsp[0].expr).type)) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = (yyvsp[-2].expr).int_val * (yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = I32_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "*", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = (is_float_type((yyvsp[-2].expr).type) || is_float_type((yyvsp[0].expr).type)) ? F64_TYPE : I32_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "*", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("*", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2308 "parser_standalone.tab.c"
    break;

  case 66: /* term: term '/' factor  */
#line 904 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && is_integer_type((yyvsp[-2].expr).type) && is_integer_type((yyvsp[0].expr).type)) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = ((yyvsp[0].expr).int_val != 0) ? (yyvsp[-2].expr).int_val / (yyvsp[0].expr).int_val : 0;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = I32_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "/", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = (is_float_type((yyvsp[-2].expr).type) || is_float_type((yyvsp[0].expr).type)) ? F64_TYPE : I32_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "/", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("/", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2332 "parser_standalone.tab.c"
    break;

  case 67: /* term: factor  */
#line 924 "parser_standalone.y"
        {
            (yyval.expr) = (yyvsp[0].expr);
        }
#line 2340 "parser_standalone.tab.c"
    break;

  case 68: /* factor: INT_LIT  */
#line 931 "parser_standalone.y"
        {
            sprintf((yyval.expr).name, "%d", (yyvsp[0].ival));
            (yyval.expr).type      = I32_TYPE;
            (yyval.expr).is_const  = 1;
            (yyval.expr).int_val   = (yyvsp[0].ival);
            (yyval.expr).float_val = (double)(yyvsp[0].ival);
            char buf[32];
            sprintf(buf, "%d", (yyvsp[0].ival));
            (yyval.expr).node = createLeaf(buf);
        }
#line 2355 "parser_standalone.tab.c"
    break;

  case 69: /* factor: FLOAT_LIT  */
#line 942 "parser_standalone.y"
        {
            sprintf((yyval.expr).name, "%.4f", (yyvsp[0].fval));
            (yyval.expr).type      = F64_TYPE;
            (yyval.expr).is_const  = 1;
            (yyval.expr).float_val = (yyvsp[0].fval);
            (yyval.expr).int_val   = (int)(yyvsp[0].fval);
            char buf[32];
            sprintf(buf, "%.4f", (yyvsp[0].fval));
            (yyval.expr).node = createLeaf(buf);
        }
#line 2370 "parser_standalone.tab.c"
    break;

  case 70: /* factor: TRUE_LIT  */
#line 953 "parser_standalone.y"
        {
            sprintf((yyval.expr).name, "1");
            (yyval.expr).type      = BOOL_TYPE;
            (yyval.expr).is_const  = 1;
            (yyval.expr).int_val   = 1;
            (yyval.expr).float_val = 1.0;
            (yyval.expr).node = createLeaf("true");
        }
#line 2383 "parser_standalone.tab.c"
    break;

  case 71: /* factor: FALSE_LIT  */
#line 962 "parser_standalone.y"
        {
            sprintf((yyval.expr).name, "0");
            (yyval.expr).type      = BOOL_TYPE;
            (yyval.expr).is_const  = 1;
            (yyval.expr).int_val   = 0;
            (yyval.expr).float_val = 0.0;
            (yyval.expr).node = createLeaf("false");
        }
#line 2396 "parser_standalone.tab.c"
    break;

  case 72: /* factor: IDENTIFIER  */
#line 971 "parser_standalone.y"
        {
            int idx = sym_lookup((yyvsp[0].sval));
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'", yylineno, (yyvsp[0].sval));
                strcpy((yyval.expr).name, (yyvsp[0].sval));
                (yyval.expr).type     = UNKNOWN_TYPE;
                (yyval.expr).is_const = 0;
            } else {
                strcpy((yyval.expr).name, (yyvsp[0].sval));
                (yyval.expr).type = (int)sym_get_type(idx);
                const Symbol *s = sym_get(idx);
                if (s && s->is_const) {
                    (yyval.expr).is_const  = 1;
                    (yyval.expr).int_val   = s->int_val;
                    (yyval.expr).float_val = s->float_val;
                } else {
                    (yyval.expr).is_const = 0;
                }
            }
            (yyval.expr).node = createLeaf((yyvsp[0].sval));
        }
#line 2422 "parser_standalone.tab.c"
    break;

  case 73: /* factor: IDENTIFIER '(' ')'  */
#line 993 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            char call_str[128];
            sprintf(call_str, "call %s", (yyvsp[-2].sval));
            tac_emit((yyval.expr).name, call_str, NULL, NULL);
            Node *funcName = createLeaf((yyvsp[-2].sval));
            (yyval.expr).node = createNode("CALL", funcName, NULL);
            printf("[Parser] Function call '%s()'\n", (yyvsp[-2].sval));
        }
#line 2439 "parser_standalone.tab.c"
    break;

  case 74: /* factor: IDENTIFIER '(' arg_list ')'  */
#line 1006 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = I32_TYPE;
            (yyval.expr).is_const = 0;
            char call_str[128];
            sprintf(call_str, "call %s", (yyvsp[-3].sval));
            tac_emit((yyval.expr).name, call_str, NULL, NULL);
            Node *funcName = createLeaf((yyvsp[-3].sval));
            (yyval.expr).node = createNode("CALL", funcName, (yyvsp[-1].node));
            printf("[Parser] Function call '%s(...)'\n", (yyvsp[-3].sval));
        }
#line 2456 "parser_standalone.tab.c"
    break;

  case 75: /* factor: '(' expression ')'  */
#line 1019 "parser_standalone.y"
        {
            (yyval.expr) = (yyvsp[-1].expr);
        }
#line 2464 "parser_standalone.tab.c"
    break;

  case 76: /* factor: '-' factor  */
#line 1023 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[0].expr).is_const && is_integer_type((yyvsp[0].expr).type)) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = -(yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = I32_TYPE;
                char arg[32];
                sprintf(arg, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, "0", "-", arg);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = (yyvsp[0].expr).type;
                tac_emit((yyval.expr).name, "0", "-", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("NEG", (yyvsp[0].expr).node, NULL);
        }
#line 2487 "parser_standalone.tab.c"
    break;

  case 77: /* arg_list: expression  */
#line 1045 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].expr).node;
        }
#line 2495 "parser_standalone.tab.c"
    break;

  case 78: /* arg_list: arg_list ',' expression  */
#line 1049 "parser_standalone.y"
        {
            (yyval.node) = createNode("ARG_SEQ", (yyvsp[-2].node), (yyvsp[0].expr).node);
        }
#line 2503 "parser_standalone.tab.c"
    break;


#line 2507 "parser_standalone.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1054 "parser_standalone.y"


/* ================================================================ */
/*  C  CODE  SECTION                                                */
/* ================================================================ */

static void sem_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Semantic Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    has_errors = 1;
    semantic_errors++;
}

void yyerror(const char *msg) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", yylineno, msg);
    has_errors = 1;
}
