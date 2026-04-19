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
    INT_TYPE,
    FLOAT_TYPE
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

void sym_init(void) {
    sym_count_var = 0;
    memset(sym_table, 0, sizeof(sym_table));
}

int sym_lookup(const char *name) {
    for (int i = 0; i < sym_count_var; i++) {
        if (strcmp(sym_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int sym_insert(const char *name, VarType type) {
    if (sym_lookup(name) >= 0) return -1;
    if (sym_count_var >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: symbol table overflow\n");
        return -1;
    }
    strncpy(sym_table[sym_count_var].name, name, 63);
    sym_table[sym_count_var].name[63] = '\0';
    sym_table[sym_count_var].type = type;
    sym_table[sym_count_var].is_defined = 0;
    sym_table[sym_count_var].scope = 0;
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
        const char *type_str = (sym_table[i].type == INT_TYPE) ? "int" :
                               (sym_table[i].type == FLOAT_TYPE) ? "float" : "unknown";
        printf("%-4d %-16s %-10s %-10s %-10d ",
               i, sym_table[i].name, type_str,
               sym_table[i].is_defined ? "yes" : "no",
               sym_table[i].scope);
        if (sym_table[i].is_defined) {
            if (sym_table[i].type == INT_TYPE)
                printf("%d", sym_table[i].int_val);
            else if (sym_table[i].type == FLOAT_TYPE)
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

#line 381 "parser_standalone.tab.c"

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
  YYSYMBOL_TYPE_I32 = 10,                  /* TYPE_I32  */
  YYSYMBOL_TYPE_F64 = 11,                  /* TYPE_F64  */
  YYSYMBOL_TYPE_BOOL = 12,                 /* TYPE_BOOL  */
  YYSYMBOL_PRINTLN = 13,                   /* PRINTLN  */
  YYSYMBOL_INT_LIT = 14,                   /* INT_LIT  */
  YYSYMBOL_FLOAT_LIT = 15,                 /* FLOAT_LIT  */
  YYSYMBOL_STRING_LIT = 16,                /* STRING_LIT  */
  YYSYMBOL_IDENTIFIER = 17,                /* IDENTIFIER  */
  YYSYMBOL_EQ = 18,                        /* EQ  */
  YYSYMBOL_NEQ = 19,                       /* NEQ  */
  YYSYMBOL_LEQ = 20,                       /* LEQ  */
  YYSYMBOL_GEQ = 21,                       /* GEQ  */
  YYSYMBOL_ARROW = 22,                     /* ARROW  */
  YYSYMBOL_23_ = 23,                       /* '<'  */
  YYSYMBOL_24_ = 24,                       /* '>'  */
  YYSYMBOL_25_ = 25,                       /* '+'  */
  YYSYMBOL_26_ = 26,                       /* '-'  */
  YYSYMBOL_27_ = 27,                       /* '*'  */
  YYSYMBOL_28_ = 28,                       /* '/'  */
  YYSYMBOL_UMINUS = 29,                    /* UMINUS  */
  YYSYMBOL_30_ = 30,                       /* '('  */
  YYSYMBOL_31_ = 31,                       /* ')'  */
  YYSYMBOL_32_ = 32,                       /* '{'  */
  YYSYMBOL_33_ = 33,                       /* '}'  */
  YYSYMBOL_34_ = 34,                       /* ';'  */
  YYSYMBOL_35_ = 35,                       /* '='  */
  YYSYMBOL_36_ = 36,                       /* ':'  */
  YYSYMBOL_37_ = 37,                       /* ','  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_program = 39,                   /* program  */
  YYSYMBOL_item_list = 40,                 /* item_list  */
  YYSYMBOL_item = 41,                      /* item  */
  YYSYMBOL_function_def = 42,              /* function_def  */
  YYSYMBOL_block = 43,                     /* block  */
  YYSYMBOL_stmt_list = 44,                 /* stmt_list  */
  YYSYMBOL_statement = 45,                 /* statement  */
  YYSYMBOL_declaration = 46,               /* declaration  */
  YYSYMBOL_type_spec = 47,                 /* type_spec  */
  YYSYMBOL_assignment = 48,                /* assignment  */
  YYSYMBOL_if_stmt = 49,                   /* if_stmt  */
  YYSYMBOL_while_stmt = 50,                /* while_stmt  */
  YYSYMBOL_println_stmt = 51,              /* println_stmt  */
  YYSYMBOL_return_stmt = 52,               /* return_stmt  */
  YYSYMBOL_expr_stmt = 53,                 /* expr_stmt  */
  YYSYMBOL_expression = 54,                /* expression  */
  YYSYMBOL_term = 55,                      /* term  */
  YYSYMBOL_factor = 56                     /* factor  */
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
typedef yytype_int8 yy_state_t;

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
#define YYFINAL  41
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   253

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  53
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  109

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   278


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
      30,    31,    27,    25,    37,    26,     2,    28,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,    34,
      23,    35,    24,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    29
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   381,   381,   388,   396,   400,   407,   408,   412,   418,
     427,   435,   438,   449,   450,   451,   452,   453,   454,   455,
     456,   464,   482,   503,   521,   545,   546,   547,   551,   570,
     575,   584,   592,   598,   607,   612,   620,   627,   647,   667,
     676,   685,   694,   703,   712,   721,   728,   748,   768,   775,
     786,   797,   819,   823
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
  "IF", "ELSE", "WHILE", "RETURN", "TYPE_I32", "TYPE_F64", "TYPE_BOOL",
  "PRINTLN", "INT_LIT", "FLOAT_LIT", "STRING_LIT", "IDENTIFIER", "EQ",
  "NEQ", "LEQ", "GEQ", "ARROW", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "UMINUS", "'('", "')'", "'{'", "'}'", "';'", "'='", "':'", "','",
  "$accept", "program", "item_list", "item", "function_def", "block",
  "stmt_list", "statement", "declaration", "type_spec", "assignment",
  "if_stmt", "while_stmt", "println_stmt", "return_stmt", "expr_stmt",
  "expression", "term", "factor", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-71)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-4)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      60,   -23,     1,     0,   190,   190,    12,   -17,   -71,   -71,
     -11,   190,   190,    28,    92,   -71,   -71,   -71,   -71,   -71,
     -71,   -71,   -71,   -71,   -71,   105,    13,   -71,   -71,    16,
      -5,     7,   -71,   203,   203,   -71,   117,    27,   190,   -71,
     213,   -71,   -71,   190,   190,   190,   190,   190,   190,   190,
     190,   -71,   190,   190,    20,   190,    37,    19,   -71,    46,
     -71,   -71,   -21,   129,   -71,    13,    13,    13,    13,    13,
      13,    13,    13,   -71,   -71,   190,    37,   141,   -71,   -71,
     -71,    22,     3,     6,    26,    25,   190,   -71,   153,    29,
     -71,   190,    37,   -71,   -71,   -71,   -71,   -71,   222,   -71,
     190,   165,    26,    33,   177,   -71,   -71,   -71,   -71
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,    49,    50,
      51,     0,     0,     0,     0,     4,     7,     6,    13,    14,
      15,    16,    17,    18,    19,     0,    45,    48,    20,     0,
       0,     0,    51,     0,     0,    35,     0,     0,     0,    53,
       0,     1,     5,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,     0,     0,     0,     0,    11,    29,
      31,    34,     0,     0,    52,    41,    42,    43,    44,    39,
      40,    37,    38,    46,    47,     0,     0,     0,    25,    26,
      27,     0,     0,     0,     0,     0,     0,    28,     0,     0,
      21,     0,     0,     8,    10,    12,    30,    32,     0,    23,
       0,     0,     0,     0,     0,    22,     9,    33,    24
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -71,   -71,   -71,    48,   -71,   -30,   -71,   -13,   -71,   -70,
     -71,   -71,   -71,   -71,   -71,   -71,    -4,    67,    -8
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    13,    14,    15,    16,    59,    83,    17,    18,    81,
      19,    20,    21,    22,    23,    24,    25,    26,    27
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      33,    34,    36,    39,    60,    29,    89,     1,    40,     2,
      85,    28,     4,    37,     5,     6,    86,    31,    30,     7,
       8,     9,   102,    10,    38,    92,     8,     9,    41,    32,
      55,    56,    11,    54,    63,    58,    12,    57,    11,    94,
      52,    53,    12,    62,    73,    74,    35,    78,    79,    80,
      82,    77,    93,    84,    96,    75,    76,    91,    58,    97,
      -3,     1,    42,     2,   100,     3,     4,   107,     5,     6,
      95,    88,   106,     7,     8,     9,     0,    10,     0,     0,
       0,     0,    98,     0,     0,     0,    11,   101,     0,     0,
      12,     0,    -2,     1,     0,     2,   104,     3,     4,     0,
       5,     6,     0,     0,     0,     7,     8,     9,     0,    10,
      65,    66,    67,    68,    69,    70,    71,    72,    11,     0,
       0,     0,    12,    43,    44,    45,    46,     0,    47,    48,
      49,    50,     0,     0,     0,    43,    44,    45,    46,    51,
      47,    48,    49,    50,     0,     0,     0,    43,    44,    45,
      46,    61,    47,    48,    49,    50,     0,     0,     0,    43,
      44,    45,    46,    87,    47,    48,    49,    50,     0,     0,
       0,    43,    44,    45,    46,    90,    47,    48,    49,    50,
       0,     0,     0,    43,    44,    45,    46,    99,    47,    48,
      49,    50,     0,     0,     0,    43,    44,    45,    46,   105,
      47,    48,    49,    50,     8,     9,     0,    32,     0,     0,
       0,   108,     0,     0,     0,     0,    11,     0,     0,     0,
      12,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,    43,    44,    45,    46,    58,    47,    48,    49,    50,
      43,    44,    45,    46,    64,    47,    48,    49,    50,     0,
       0,     0,     0,   103
};

static const yytype_int8 yycheck[] =
{
       4,     5,     6,    11,    34,     4,    76,     1,    12,     3,
      31,    34,     6,    30,     8,     9,    37,    17,    17,    13,
      14,    15,    92,    17,    35,    22,    14,    15,     0,    17,
      35,    36,    26,    17,    38,    32,    30,    30,    26,    33,
      27,    28,    30,    16,    52,    53,    34,    10,    11,    12,
      31,    55,    82,     7,    84,    35,    36,    35,    32,    34,
       0,     1,    14,     3,    35,     5,     6,    34,     8,     9,
      83,    75,   102,    13,    14,    15,    -1,    17,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    -1,    26,    91,    -1,    -1,
      30,    -1,     0,     1,    -1,     3,   100,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    13,    14,    15,    -1,    17,
      43,    44,    45,    46,    47,    48,    49,    50,    26,    -1,
      -1,    -1,    30,    18,    19,    20,    21,    -1,    23,    24,
      25,    26,    -1,    -1,    -1,    18,    19,    20,    21,    34,
      23,    24,    25,    26,    -1,    -1,    -1,    18,    19,    20,
      21,    34,    23,    24,    25,    26,    -1,    -1,    -1,    18,
      19,    20,    21,    34,    23,    24,    25,    26,    -1,    -1,
      -1,    18,    19,    20,    21,    34,    23,    24,    25,    26,
      -1,    -1,    -1,    18,    19,    20,    21,    34,    23,    24,
      25,    26,    -1,    -1,    -1,    18,    19,    20,    21,    34,
      23,    24,    25,    26,    14,    15,    -1,    17,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,
      30,    18,    19,    20,    21,    -1,    23,    24,    25,    26,
      -1,    18,    19,    20,    21,    32,    23,    24,    25,    26,
      18,    19,    20,    21,    31,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    31
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     5,     6,     8,     9,    13,    14,    15,
      17,    26,    30,    39,    40,    41,    42,    45,    46,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    34,     4,
      17,    17,    17,    54,    54,    34,    54,    30,    35,    56,
      54,     0,    41,    18,    19,    20,    21,    23,    24,    25,
      26,    34,    27,    28,    17,    35,    36,    30,    32,    43,
      43,    34,    16,    54,    31,    55,    55,    55,    55,    55,
      55,    55,    55,    56,    56,    35,    36,    54,    10,    11,
      12,    47,    31,    44,     7,    31,    37,    34,    54,    47,
      34,    35,    22,    43,    33,    45,    43,    34,    54,    34,
      35,    54,    47,    31,    54,    34,    43,    34,    34
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    39,    40,    40,    41,    41,    42,    42,
      43,    44,    44,    45,    45,    45,    45,    45,    45,    45,
      45,    46,    46,    46,    46,    47,    47,    47,    48,    49,
      49,    50,    51,    51,    52,    52,    53,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    55,    55,    55,    56,
      56,    56,    56,    56
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     1,     2,     1,     1,     5,     7,
       3,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     5,     7,     6,     8,     1,     1,     1,     4,     3,
       5,     3,     5,     7,     3,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     1,     1,
       1,     1,     3,     2
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
#line 382 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].node);
            parse_tree_root = (yyval.node);
            printf("[Parser] program → item_list reached; parse_tree_root set.\n");
        }
#line 1520 "parser_standalone.tab.c"
    break;

  case 3: /* program: %empty  */
#line 388 "parser_standalone.y"
        {
            (yyval.node) = createNode("EMPTY_PROGRAM", NULL, NULL);
            parse_tree_root = (yyval.node);
            printf("[Parser] program → ε reached; parse_tree_root set to EMPTY_PROGRAM.\n");
        }
#line 1530 "parser_standalone.tab.c"
    break;

  case 4: /* item_list: item  */
#line 397 "parser_standalone.y"
        {
            (yyval.node) = (yyvsp[0].node);
        }
#line 1538 "parser_standalone.tab.c"
    break;

  case 5: /* item_list: item_list item  */
#line 401 "parser_standalone.y"
        {
            (yyval.node) = createNode("SEQ", (yyvsp[-1].node), (yyvsp[0].node));
        }
#line 1546 "parser_standalone.tab.c"
    break;

  case 6: /* item: statement  */
#line 407 "parser_standalone.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1552 "parser_standalone.tab.c"
    break;

  case 7: /* item: function_def  */
#line 408 "parser_standalone.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1558 "parser_standalone.tab.c"
    break;

  case 8: /* function_def: FN IDENTIFIER '(' ')' block  */
#line 413 "parser_standalone.y"
        {
            Node *nameNode = createLeaf((yyvsp[-3].sval));
            (yyval.node) = createNode("FUNC", nameNode, (yyvsp[0].node));
            printf("[Parser] Function '%s' defined.\n", (yyvsp[-3].sval));
        }
#line 1568 "parser_standalone.tab.c"
    break;

  case 9: /* function_def: FN IDENTIFIER '(' ')' ARROW type_spec block  */
#line 419 "parser_standalone.y"
        {
            Node *nameNode = createLeaf((yyvsp[-5].sval));
            (yyval.node) = createNode("FUNC", nameNode, (yyvsp[0].node));
            printf("[Parser] Function '%s' defined (returns type).\n", (yyvsp[-5].sval));
        }
#line 1578 "parser_standalone.tab.c"
    break;

  case 10: /* block: '{' stmt_list '}'  */
#line 428 "parser_standalone.y"
        {
            (yyval.node) = createNode("BLOCK", (yyvsp[-1].node), NULL);
        }
#line 1586 "parser_standalone.tab.c"
    break;

  case 11: /* stmt_list: %empty  */
#line 435 "parser_standalone.y"
        {
            (yyval.node) = NULL;
        }
#line 1594 "parser_standalone.tab.c"
    break;

  case 12: /* stmt_list: stmt_list statement  */
#line 439 "parser_standalone.y"
        {
            if ((yyvsp[-1].node) == NULL) {
                (yyval.node) = (yyvsp[0].node);
            } else {
                (yyval.node) = createNode("SEQ", (yyvsp[-1].node), (yyvsp[0].node));
            }
        }
#line 1606 "parser_standalone.tab.c"
    break;

  case 13: /* statement: declaration  */
#line 449 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1612 "parser_standalone.tab.c"
    break;

  case 14: /* statement: assignment  */
#line 450 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1618 "parser_standalone.tab.c"
    break;

  case 15: /* statement: if_stmt  */
#line 451 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1624 "parser_standalone.tab.c"
    break;

  case 16: /* statement: while_stmt  */
#line 452 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1630 "parser_standalone.tab.c"
    break;

  case 17: /* statement: println_stmt  */
#line 453 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1636 "parser_standalone.tab.c"
    break;

  case 18: /* statement: return_stmt  */
#line 454 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1642 "parser_standalone.tab.c"
    break;

  case 19: /* statement: expr_stmt  */
#line 455 "parser_standalone.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1648 "parser_standalone.tab.c"
    break;

  case 20: /* statement: error ';'  */
#line 457 "parser_standalone.y"
        {
            yyerrok;
            (yyval.node) = createNode("ERROR", NULL, NULL);
        }
#line 1657 "parser_standalone.tab.c"
    break;

  case 21: /* declaration: LET IDENTIFIER '=' expression ';'  */
#line 465 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-3].sval), (VarType)(yyvsp[-1].expr).type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-3].sval));
            } else {
                if ((yyvsp[-1].expr).is_const) {
                    if ((yyvsp[-1].expr).type == INT_TYPE)
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
#line 1679 "parser_standalone.tab.c"
    break;

  case 22: /* declaration: LET IDENTIFIER ':' type_spec '=' expression ';'  */
#line 483 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-5].sval), (VarType)(yyvsp[-3].ival));
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-5].sval));
            } else {
                if ((yyvsp[-3].ival) != (yyvsp[-1].expr).type && (yyvsp[-1].expr).type != UNKNOWN_TYPE) {
                    sem_error("line %d: type mismatch in declaration of '%s'", yylineno, (yyvsp[-5].sval));
                }
                if ((yyvsp[-1].expr).is_const) {
                    if ((yyvsp[-3].ival) == INT_TYPE)
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
#line 1704 "parser_standalone.tab.c"
    break;

  case 23: /* declaration: LET MUT IDENTIFIER '=' expression ';'  */
#line 504 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-3].sval), (VarType)(yyvsp[-1].expr).type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-3].sval));
            } else {
                if ((yyvsp[-1].expr).is_const) {
                    if ((yyvsp[-1].expr).type == INT_TYPE)
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
#line 1726 "parser_standalone.tab.c"
    break;

  case 24: /* declaration: LET MUT IDENTIFIER ':' type_spec '=' expression ';'  */
#line 522 "parser_standalone.y"
        {
            int idx = sym_insert((yyvsp[-5].sval), (VarType)(yyvsp[-3].ival));
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, (yyvsp[-5].sval));
            } else {
                if ((yyvsp[-3].ival) != (yyvsp[-1].expr).type && (yyvsp[-1].expr).type != UNKNOWN_TYPE) {
                    sem_error("line %d: type mismatch in declaration of '%s'", yylineno, (yyvsp[-5].sval));
                }
                if ((yyvsp[-1].expr).is_const) {
                    if ((yyvsp[-3].ival) == INT_TYPE)
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
#line 1751 "parser_standalone.tab.c"
    break;

  case 25: /* type_spec: TYPE_I32  */
#line 545 "parser_standalone.y"
                 { (yyval.ival) = INT_TYPE;   }
#line 1757 "parser_standalone.tab.c"
    break;

  case 26: /* type_spec: TYPE_F64  */
#line 546 "parser_standalone.y"
                 { (yyval.ival) = FLOAT_TYPE; }
#line 1763 "parser_standalone.tab.c"
    break;

  case 27: /* type_spec: TYPE_BOOL  */
#line 547 "parser_standalone.y"
                 { (yyval.ival) = INT_TYPE;   }
#line 1769 "parser_standalone.tab.c"
    break;

  case 28: /* assignment: IDENTIFIER '=' expression ';'  */
#line 552 "parser_standalone.y"
        {
            int idx = sym_lookup((yyvsp[-3].sval));
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'", yylineno, (yyvsp[-3].sval));
            } else {
                VarType declared = sym_get_type(idx);
                if (declared != UNKNOWN_TYPE && (yyvsp[-1].expr).type != UNKNOWN_TYPE && declared != (VarType)(yyvsp[-1].expr).type) {
                    sem_error("line %d: type mismatch in assignment to '%s'", yylineno, (yyvsp[-3].sval));
                }
                tac_emit((yyvsp[-3].sval), (yyvsp[-1].expr).name, NULL, NULL);
                printf("[Parser] Assignment to '%s'.\n", (yyvsp[-3].sval));
            }
            Node *id = createLeaf((yyvsp[-3].sval));
            (yyval.node) = createNode("ASSIGN", id, (yyvsp[-1].expr).node);
        }
#line 1789 "parser_standalone.tab.c"
    break;

  case 29: /* if_stmt: IF expression block  */
#line 571 "parser_standalone.y"
        {
            (yyval.node) = createNode("IF", (yyvsp[-1].expr).node, (yyvsp[0].node));
            printf("[Parser] If statement.\n");
        }
#line 1798 "parser_standalone.tab.c"
    break;

  case 30: /* if_stmt: IF expression block ELSE block  */
#line 576 "parser_standalone.y"
        {
            Node *branches = createNode("BRANCHES", (yyvsp[-2].node), (yyvsp[0].node));
            (yyval.node) = createNode("IF_ELSE", (yyvsp[-3].expr).node, branches);
            printf("[Parser] If-else statement.\n");
        }
#line 1808 "parser_standalone.tab.c"
    break;

  case 31: /* while_stmt: WHILE expression block  */
#line 585 "parser_standalone.y"
        {
            (yyval.node) = createNode("WHILE", (yyvsp[-1].expr).node, (yyvsp[0].node));
            printf("[Parser] While loop.\n");
        }
#line 1817 "parser_standalone.tab.c"
    break;

  case 32: /* println_stmt: PRINTLN '(' STRING_LIT ')' ';'  */
#line 593 "parser_standalone.y"
        {
            Node *str = createLeaf((yyvsp[-2].sval));
            (yyval.node) = createNode("PRINTLN", str, NULL);
            printf("[Parser] println! statement.\n");
        }
#line 1827 "parser_standalone.tab.c"
    break;

  case 33: /* println_stmt: PRINTLN '(' STRING_LIT ',' expression ')' ';'  */
#line 599 "parser_standalone.y"
        {
            Node *str = createLeaf((yyvsp[-4].sval));
            (yyval.node) = createNode("PRINTLN", str, (yyvsp[-2].expr).node);
            printf("[Parser] println! statement with expression.\n");
        }
#line 1837 "parser_standalone.tab.c"
    break;

  case 34: /* return_stmt: RETURN expression ';'  */
#line 608 "parser_standalone.y"
        {
            (yyval.node) = createNode("RETURN", (yyvsp[-1].expr).node, NULL);
            printf("[Parser] Return statement.\n");
        }
#line 1846 "parser_standalone.tab.c"
    break;

  case 35: /* return_stmt: RETURN ';'  */
#line 613 "parser_standalone.y"
        {
            (yyval.node) = createNode("RETURN", NULL, NULL);
            printf("[Parser] Return (void).\n");
        }
#line 1855 "parser_standalone.tab.c"
    break;

  case 36: /* expr_stmt: expression ';'  */
#line 621 "parser_standalone.y"
        {
            (yyval.node) = createNode("EXPR_STMT", (yyvsp[-1].expr).node, NULL);
        }
#line 1863 "parser_standalone.tab.c"
    break;

  case 37: /* expression: expression '+' term  */
#line 628 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && (yyvsp[-2].expr).type == INT_TYPE && (yyvsp[0].expr).type == INT_TYPE) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = (yyvsp[-2].expr).int_val + (yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "+", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = ((yyvsp[-2].expr).type == FLOAT_TYPE || (yyvsp[0].expr).type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "+", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("+", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1887 "parser_standalone.tab.c"
    break;

  case 38: /* expression: expression '-' term  */
#line 648 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && (yyvsp[-2].expr).type == INT_TYPE && (yyvsp[0].expr).type == INT_TYPE) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = (yyvsp[-2].expr).int_val - (yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "-", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = ((yyvsp[-2].expr).type == FLOAT_TYPE || (yyvsp[0].expr).type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "-", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("-", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1911 "parser_standalone.tab.c"
    break;

  case 39: /* expression: expression '<' term  */
#line 668 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = INT_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "<", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("<", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1924 "parser_standalone.tab.c"
    break;

  case 40: /* expression: expression '>' term  */
#line 677 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = INT_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, ">", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode(">", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1937 "parser_standalone.tab.c"
    break;

  case 41: /* expression: expression EQ term  */
#line 686 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = INT_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "==", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("==", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1950 "parser_standalone.tab.c"
    break;

  case 42: /* expression: expression NEQ term  */
#line 695 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = INT_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "!=", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("!=", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1963 "parser_standalone.tab.c"
    break;

  case 43: /* expression: expression LEQ term  */
#line 704 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = INT_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "<=", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode("<=", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1976 "parser_standalone.tab.c"
    break;

  case 44: /* expression: expression GEQ term  */
#line 713 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            (yyval.expr).type = INT_TYPE;
            (yyval.expr).is_const = 0;
            tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, ">=", (yyvsp[0].expr).name);
            (yyval.expr).node = createNode(">=", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 1989 "parser_standalone.tab.c"
    break;

  case 45: /* expression: term  */
#line 722 "parser_standalone.y"
        {
            (yyval.expr) = (yyvsp[0].expr);
        }
#line 1997 "parser_standalone.tab.c"
    break;

  case 46: /* term: term '*' factor  */
#line 729 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && (yyvsp[-2].expr).type == INT_TYPE && (yyvsp[0].expr).type == INT_TYPE) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = (yyvsp[-2].expr).int_val * (yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "*", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = ((yyvsp[-2].expr).type == FLOAT_TYPE || (yyvsp[0].expr).type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "*", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("*", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2021 "parser_standalone.tab.c"
    break;

  case 47: /* term: term '/' factor  */
#line 749 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[-2].expr).is_const && (yyvsp[0].expr).is_const && (yyvsp[-2].expr).type == INT_TYPE && (yyvsp[0].expr).type == INT_TYPE) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = ((yyvsp[0].expr).int_val != 0) ? (yyvsp[-2].expr).int_val / (yyvsp[0].expr).int_val : 0;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", (yyvsp[-2].expr).int_val);
                sprintf(b, "%d", (yyvsp[0].expr).int_val);
                tac_emit((yyval.expr).name, a, "/", b);
            } else {
                (yyval.expr).is_const = 0;
                (yyval.expr).type = ((yyvsp[-2].expr).type == FLOAT_TYPE || (yyvsp[0].expr).type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
                tac_emit((yyval.expr).name, (yyvsp[-2].expr).name, "/", (yyvsp[0].expr).name);
            }
            (yyval.expr).node = createNode("/", (yyvsp[-2].expr).node, (yyvsp[0].expr).node);
        }
#line 2045 "parser_standalone.tab.c"
    break;

  case 48: /* term: factor  */
#line 769 "parser_standalone.y"
        {
            (yyval.expr) = (yyvsp[0].expr);
        }
#line 2053 "parser_standalone.tab.c"
    break;

  case 49: /* factor: INT_LIT  */
#line 776 "parser_standalone.y"
        {
            sprintf((yyval.expr).name, "%d", (yyvsp[0].ival));
            (yyval.expr).type      = INT_TYPE;
            (yyval.expr).is_const  = 1;
            (yyval.expr).int_val   = (yyvsp[0].ival);
            (yyval.expr).float_val = (double)(yyvsp[0].ival);
            char buf[32];
            sprintf(buf, "%d", (yyvsp[0].ival));
            (yyval.expr).node = createLeaf(buf);
        }
#line 2068 "parser_standalone.tab.c"
    break;

  case 50: /* factor: FLOAT_LIT  */
#line 787 "parser_standalone.y"
        {
            sprintf((yyval.expr).name, "%.4f", (yyvsp[0].fval));
            (yyval.expr).type      = FLOAT_TYPE;
            (yyval.expr).is_const  = 1;
            (yyval.expr).float_val = (yyvsp[0].fval);
            (yyval.expr).int_val   = (int)(yyvsp[0].fval);
            char buf[32];
            sprintf(buf, "%.4f", (yyvsp[0].fval));
            (yyval.expr).node = createLeaf(buf);
        }
#line 2083 "parser_standalone.tab.c"
    break;

  case 51: /* factor: IDENTIFIER  */
#line 798 "parser_standalone.y"
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
#line 2109 "parser_standalone.tab.c"
    break;

  case 52: /* factor: '(' expression ')'  */
#line 820 "parser_standalone.y"
        {
            (yyval.expr) = (yyvsp[-1].expr);
        }
#line 2117 "parser_standalone.tab.c"
    break;

  case 53: /* factor: '-' factor  */
#line 824 "parser_standalone.y"
        {
            char *t = tac_new_temp();
            strcpy((yyval.expr).name, t);
            if ((yyvsp[0].expr).is_const && (yyvsp[0].expr).type == INT_TYPE) {
                (yyval.expr).is_const  = 1;
                (yyval.expr).int_val   = -(yyvsp[0].expr).int_val;
                (yyval.expr).float_val = (double)(yyval.expr).int_val;
                (yyval.expr).type      = INT_TYPE;
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
#line 2140 "parser_standalone.tab.c"
    break;


#line 2144 "parser_standalone.tab.c"

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

#line 844 "parser_standalone.y"


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
