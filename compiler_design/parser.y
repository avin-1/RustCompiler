/*
 * parser.y
 * ========
 * Bison grammar specification for the Mini Rust Compiler.
 *
 * COMPILER DESIGN THEORY — SYNTAX ANALYSIS:
 * ------------------------------------------
 * This file defines a Context-Free Grammar (CFG) for a subset of Rust.
 * Bison uses an LALR(1) parsing algorithm to validate the token stream
 * produced by the lexer (Flex) against this grammar.
 *
 * SYNTAX-DIRECTED TRANSLATION (SDT):
 * Each grammar rule has a semantic action (in { ... }) that:
 *   1. Builds a PARSE TREE node   (using createNode / createLeaf)
 *   2. Performs SEMANTIC ANALYSIS  (symbol table checks)
 *   3. Emits INTERMEDIATE CODE    (Three-Address Code)
 *
 * This is a single-pass compiler — all three are done simultaneously
 * during bottom-up parsing.
 *
 * Supported language constructs (subset of Rust):
 *   1. Variable declarations       :  let x = 10;   let mut y: i32 = 5;
 *   2. Assignment statements       :  x = x + 1;
 *   3. Arithmetic expressions      :  a + b * c - d / e
 *   4. If / if-else statements     :  if x > 0 { ... } else { ... }
 *   5. While loops                 :  while x < 10 { ... }
 *   6. println! macro (simple)     :  println!("hello");
 *   7. Function definitions        :  fn main() { ... }
 *
 * PRECEDENCE AND ASSOCIATIVITY:
 *   Lowest  →  Highest:
 *     < > == != <= >=     (left-associative)
 *     + -                 (left-associative)
 *     * /                 (left-associative)
 *     unary minus         (right-associative)
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "intermediate_code.h"
#include "parse_tree.h"

/* ---- Flex / Bison glue ---- */
extern int  yylex(void);
extern int  yylineno;
extern FILE *yyin;
extern char *yytext;

void yyerror(const char *msg);

/* ---- Flags ---- */
int debug_tokens    = 1;   /* set to 1 to see token stream     */
int has_errors      = 0;   /* set to 1 on any semantic error    */
int semantic_errors = 0;   /* count of semantic errors          */

/* ---- Parse Tree global root ---- */
Node *parse_tree_root = NULL;   /* set after successful parse   */

/* ---- Semantic helpers (defined at the bottom of this file) ---- */
static void sem_error(const char *fmt, ...);
%}

/* ================================================================ */
/*  YYLVAL  union                                                   */
/* ================================================================ */
/*
 * The %union defines the semantic value that each token and
 * non-terminal can carry through the parse.
 *
 * IMPORTANT CHANGE: Added `struct Node *node` for parse tree
 * construction, and embedded a `node` pointer inside the `expr`
 * struct so expressions carry BOTH TAC attributes AND tree nodes.
 */
%union {
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
}

/* ================================================================ */
/*  TOKEN DECLARATIONS                                              */
/* ================================================================ */

/* Keywords */
%token LET MUT FN IF ELSE WHILE RETURN
%token TYPE_I32 TYPE_F64 TYPE_BOOL
%token PRINTLN

/* Literals */
%token <ival> INT_LIT
%token <fval> FLOAT_LIT
%token <sval> STRING_LIT

/* Identifiers */
%token <sval> IDENTIFIER

/* Multi-character operators */
%token EQ NEQ LEQ GEQ ARROW

/* ================================================================ */
/*  NON-TERMINAL TYPES                                              */
/* ================================================================ */
%type <expr>  expression term factor
%type <ival>  type_spec
%type <node>  program item_list item function_def block stmt_list
%type <node>  statement declaration assignment if_stmt while_stmt
%type <node>  println_stmt return_stmt expr_stmt

/* ================================================================ */
/*  OPERATOR PRECEDENCE  (lowest → highest)                         */
/*  Declares associativity for resolving shift-reduce conflicts.    */
/* ================================================================ */
%left  '<' '>' EQ NEQ LEQ GEQ
%left  '+' '-'
%left  '*' '/'
%right UMINUS

%%

/* ================================================================ */
/*  GRAMMAR RULES + PARSE TREE CONSTRUCTION + SEMANTIC ACTIONS      */
/* ================================================================ */

/*
 * program → item_list
 *
 * The root of the entire parse tree.  After parsing, the global
 * variable parse_tree_root is set so main.c can print/save it.
 */
program
    : item_list
        {
            $$ = $1;
            parse_tree_root = $$;   /* store root for main.c */
            printf("[Parser] program → item_list reached; parse_tree_root set.\n");
        }
    | /* empty */
        {
            $$ = createNode("EMPTY_PROGRAM", NULL, NULL);
            parse_tree_root = $$;
            printf("[Parser] program → ε reached; parse_tree_root set to EMPTY_PROGRAM.\n");
        }
    ;

/*
 * item_list → item  |  item_list item
 *
 * A sequence of top-level items.  We chain them using a SEQ node
 * (left-linear list: SEQ(previous_items, new_item)).
 *
 * CFG:  item_list → item
 *       item_list → item_list  item
 */
item_list
    : item
        {
            $$ = $1;
        }
    | item_list item
        {
            /* Chain statements into a sequence node */
            $$ = createNode("SEQ", $1, $2);
        }
    ;

/*
 * item → statement  |  function_def
 */
item
    : statement   { $$ = $1; }
    | function_def { $$ = $1; }
    ;

/*
 * function_def → fn IDENTIFIER ( ) block
 *              | fn IDENTIFIER ( ) -> type_spec block
 *
 * Creates a FUNC node with the function name as the left child
 * and the body (block) as the right child.
 */
function_def
    : FN IDENTIFIER '(' ')' block
        {
            Node *nameNode = createLeaf($2);
            $$ = createNode("FUNC", nameNode, $5);
            printf("[Parser] Function '%s' defined.\n", $2);
        }
    | FN IDENTIFIER '(' ')' ARROW type_spec block
        {
            Node *nameNode = createLeaf($2);
            $$ = createNode("FUNC", nameNode, $7);
            printf("[Parser] Function '%s' defined (returns type).\n", $2);
        }
    ;

/*
 * block → { stmt_list }
 *
 * A block is a brace-delimited sequence of statements.
 * We wrap it in a BLOCK node.
 */
block
    : '{' stmt_list '}'
        {
            $$ = createNode("BLOCK", $2, NULL);
        }
    ;

/*
 * stmt_list → ε  |  stmt_list statement
 *
 * Empty produces NULL.  Each new statement is chained with SEQ.
 */
stmt_list
    : /* empty */
        {
            $$ = NULL;
        }
    | stmt_list statement
        {
            if ($1 == NULL) {
                $$ = $2;
            } else {
                $$ = createNode("SEQ", $1, $2);
            }
        }
    ;

/* ================================================================ */
/*  STATEMENTS                                                      */
/* ================================================================ */

statement
    : declaration    { $$ = $1; }
    | assignment     { $$ = $1; }
    | if_stmt        { $$ = $1; }
    | while_stmt     { $$ = $1; }
    | println_stmt   { $$ = $1; }
    | return_stmt    { $$ = $1; }
    | expr_stmt      { $$ = $1; }
    | error ';'
        {
            /* Minimal error recovery: skip to next statement boundary. */
            yyerrok;
            $$ = createNode("ERROR", NULL, NULL);
        }
    ;

/*
 * declaration (4 forms)
 * ---------------------
 * All create a LET node:  LET(identifier_leaf, expression_subtree)
 *
 * Semantic actions:
 *   - Insert into symbol table (detect duplicates)
 *   - Type-mismatch check (if explicit type given)
 *   - Emit TAC: identifier = expression_temp
 *   - Build parse tree node
 */
declaration
    : LET IDENTIFIER '=' expression ';'
        {
            /* --- Semantic Analysis --- */
            int idx = sym_insert($2, (VarType)$4.type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared",
                          yylineno, $2);
            } else {
                if ($4.is_const) {
                    if ($4.type == INT_TYPE)
                        sym_set_int(idx, $4.int_val);
                    else
                        sym_set_float(idx, $4.float_val);
                }
                /* --- Intermediate Code --- */
                tac_emit($2, $4.name, NULL, NULL);
                printf("[Parser] Declared '%s' (inferred type).\n", $2);
            }
            /* --- Parse Tree --- */
            Node *id = createLeaf($2);
            $$ = createNode("LET", id, $4.node);
        }
    | LET IDENTIFIER ':' type_spec '=' expression ';'
        {
            int idx = sym_insert($2, (VarType)$4);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared",
                          yylineno, $2);
            } else {
                /* Type-mismatch check */
                if ($4 != $6.type && $6.type != UNKNOWN_TYPE) {
                    sem_error("line %d: type mismatch in declaration of '%s' "
                              "(expected %s, got %s)",
                              yylineno, $2,
                              $4 == INT_TYPE ? "i32" : "f64",
                              $6.type == INT_TYPE ? "i32" : "f64");
                }
                if ($6.is_const) {
                    if ($4 == INT_TYPE)
                        sym_set_int(idx, $6.int_val);
                    else
                        sym_set_float(idx, $6.float_val);
                }
                tac_emit($2, $6.name, NULL, NULL);
                printf("[Parser] Declared '%s' with explicit type.\n", $2);
            }
            Node *id = createLeaf($2);
            $$ = createNode("LET", id, $6.node);
        }
    | LET MUT IDENTIFIER '=' expression ';'
        {
            int idx = sym_insert($3, (VarType)$5.type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared",
                          yylineno, $3);
            } else {
                if ($5.is_const) {
                    if ($5.type == INT_TYPE)
                        sym_set_int(idx, $5.int_val);
                    else
                        sym_set_float(idx, $5.float_val);
                }
                tac_emit($3, $5.name, NULL, NULL);
                printf("[Parser] Declared mutable '%s'.\n", $3);
            }
            Node *id = createLeaf($3);
            $$ = createNode("LET_MUT", id, $5.node);
        }
    | LET MUT IDENTIFIER ':' type_spec '=' expression ';'
        {
            int idx = sym_insert($3, (VarType)$5);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared",
                          yylineno, $3);
            } else {
                if ($5 != $7.type && $7.type != UNKNOWN_TYPE) {
                    sem_error("line %d: type mismatch in declaration of '%s'",
                              yylineno, $3);
                }
                if ($7.is_const) {
                    if ($5 == INT_TYPE)
                        sym_set_int(idx, $7.int_val);
                    else
                        sym_set_float(idx, $7.float_val);
                }
                tac_emit($3, $7.name, NULL, NULL);
                printf("[Parser] Declared mutable '%s' with type.\n", $3);
            }
            Node *id = createLeaf($3);
            $$ = createNode("LET_MUT", id, $7.node);
        }
    ;

/*
 * type_spec → i32 | f64 | bool
 *
 * Returns VarType as an integer.  No tree node needed here
 * (type info is stored in the symbol table, not the parse tree).
 */
type_spec
    : TYPE_I32   { $$ = INT_TYPE;   }
    | TYPE_F64   { $$ = FLOAT_TYPE; }
    | TYPE_BOOL  { $$ = INT_TYPE;   }   /* booleans stored as int */
    ;

/*
 * assignment → IDENTIFIER = expression ;
 *
 * Creates an ASSIGN node: ASSIGN(id_leaf, expr_subtree)
 */
assignment
    : IDENTIFIER '=' expression ';'
        {
            /* --- Semantic Analysis --- */
            int idx = sym_lookup($1);
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'",
                          yylineno, $1);
            } else {
                VarType declared = sym_get_type(idx);
                if (declared != UNKNOWN_TYPE &&
                    $3.type  != UNKNOWN_TYPE &&
                    declared != (VarType)$3.type)
                {
                    sem_error("line %d: type mismatch in assignment to '%s' "
                              "(expected %s, got %s)",
                              yylineno, $1,
                              declared == INT_TYPE ? "i32" : "f64",
                              $3.type  == INT_TYPE ? "i32" : "f64");
                }
                tac_emit($1, $3.name, NULL, NULL);
                printf("[Parser] Assignment to '%s'.\n", $1);
            }
            /* --- Parse Tree --- */
            Node *id = createLeaf($1);
            $$ = createNode("ASSIGN", id, $3.node);
        }
    ;

/*
 * if_stmt → IF expression block
 *         | IF expression block ELSE block
 *
 * IF node:      IF(condition, then_block)
 * IF-ELSE node: IF_ELSE(condition, BRANCHES(then_block, else_block))
 */
if_stmt
    : IF expression block
        {
            $$ = createNode("IF", $2.node, $3);
            printf("[Parser] If statement.\n");
        }
    | IF expression block ELSE block
        {
            Node *branches = createNode("BRANCHES", $3, $5);
            $$ = createNode("IF_ELSE", $2.node, branches);
            printf("[Parser] If-else statement.\n");
        }
    ;

/*
 * while_stmt → WHILE expression block
 *
 * WHILE node: WHILE(condition, body_block)
 */
while_stmt
    : WHILE expression block
        {
            $$ = createNode("WHILE", $2.node, $3);
            printf("[Parser] While loop.\n");
        }
    ;

/*
 * println_stmt → PRINTLN ( STRING_LIT ) ;
 *              | PRINTLN ( STRING_LIT , expression ) ;
 */
println_stmt
    : PRINTLN '(' STRING_LIT ')' ';'
        {
            Node *str = createLeaf($3);
            $$ = createNode("PRINTLN", str, NULL);
            printf("[Parser] println! statement.\n");
        }
    | PRINTLN '(' STRING_LIT ',' expression ')' ';'
        {
            Node *str = createLeaf($3);
            $$ = createNode("PRINTLN", str, $5.node);
            printf("[Parser] println! statement with expression.\n");
        }
    ;

/*
 * return_stmt → RETURN expression ;  |  RETURN ;
 */
return_stmt
    : RETURN expression ';'
        {
            $$ = createNode("RETURN", $2.node, NULL);
            printf("[Parser] Return statement.\n");
        }
    | RETURN ';'
        {
            $$ = createNode("RETURN", NULL, NULL);
            printf("[Parser] Return (void).\n");
        }
    ;

/*
 * expr_stmt → expression ;
 *
 * A bare expression used as a statement.
 */
expr_stmt
    : expression ';'
        {
            $$ = createNode("EXPR_STMT", $1.node, NULL);
        }
    ;

/* ================================================================ */
/*  EXPRESSIONS                                                     */
/* ================================================================ */
/*
 * CFG (Context-Free Grammar) for expressions:
 *
 *   expression → expression  (+|-|<|>|==|!=|<=|>=)  term
 *              | term
 *
 *   term       → term  (*|/)  factor
 *              | factor
 *
 *   factor     → INT_LIT | FLOAT_LIT | IDENTIFIER
 *              | ( expression )
 *              | - factor          (unary minus)
 *
 * This grammar enforces operator precedence:
 *   * and /  bind tighter than  + and -
 *   + and -  bind tighter than  relational operators
 *
 * Each rule builds a parse tree AND emits TAC.
 */

expression
    : expression '+' term
        {
            /* --- TAC Generation (with constant folding) --- */
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const &&
                $1.type == INT_TYPE && $3.type == INT_TYPE)
            {
                $$.is_const  = 1;
                $$.int_val   = $1.int_val + $3.int_val;
                $$.float_val = (double)$$.int_val;
                $$.type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", $1.int_val);
                sprintf(b, "%d", $3.int_val);
                tac_emit($$.name, a, "+", b);
            } else {
                $$.is_const = 0;
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE)
                          ? FLOAT_TYPE : INT_TYPE;
                tac_emit($$.name, $1.name, "+", $3.name);
            }
            /* --- Parse Tree --- */
            $$.node = createNode("+", $1.node, $3.node);
        }
    | expression '-' term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const &&
                $1.type == INT_TYPE && $3.type == INT_TYPE)
            {
                $$.is_const  = 1;
                $$.int_val   = $1.int_val - $3.int_val;
                $$.float_val = (double)$$.int_val;
                $$.type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", $1.int_val);
                sprintf(b, "%d", $3.int_val);
                tac_emit($$.name, a, "-", b);
            } else {
                $$.is_const = 0;
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE)
                          ? FLOAT_TYPE : INT_TYPE;
                tac_emit($$.name, $1.name, "-", $3.name);
            }
            $$.node = createNode("-", $1.node, $3.node);
        }
    | expression '<' term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            $$.type = INT_TYPE;
            $$.is_const = 0;
            tac_emit($$.name, $1.name, "<", $3.name);
            $$.node = createNode("<", $1.node, $3.node);
        }
    | expression '>' term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            $$.type = INT_TYPE;
            $$.is_const = 0;
            tac_emit($$.name, $1.name, ">", $3.name);
            $$.node = createNode(">", $1.node, $3.node);
        }
    | expression EQ term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            $$.type = INT_TYPE;
            $$.is_const = 0;
            tac_emit($$.name, $1.name, "==", $3.name);
            $$.node = createNode("==", $1.node, $3.node);
        }
    | expression NEQ term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            $$.type = INT_TYPE;
            $$.is_const = 0;
            tac_emit($$.name, $1.name, "!=", $3.name);
            $$.node = createNode("!=", $1.node, $3.node);
        }
    | expression LEQ term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            $$.type = INT_TYPE;
            $$.is_const = 0;
            tac_emit($$.name, $1.name, "<=", $3.name);
            $$.node = createNode("<=", $1.node, $3.node);
        }
    | expression GEQ term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            $$.type = INT_TYPE;
            $$.is_const = 0;
            tac_emit($$.name, $1.name, ">=", $3.name);
            $$.node = createNode(">=", $1.node, $3.node);
        }
    | term
        {
            $$ = $1;
        }
    ;

term
    : term '*' factor
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const &&
                $1.type == INT_TYPE && $3.type == INT_TYPE)
            {
                $$.is_const  = 1;
                $$.int_val   = $1.int_val * $3.int_val;
                $$.float_val = (double)$$.int_val;
                $$.type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", $1.int_val);
                sprintf(b, "%d", $3.int_val);
                tac_emit($$.name, a, "*", b);
            } else {
                $$.is_const = 0;
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE)
                          ? FLOAT_TYPE : INT_TYPE;
                tac_emit($$.name, $1.name, "*", $3.name);
            }
            $$.node = createNode("*", $1.node, $3.node);
        }
    | term '/' factor
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const &&
                $1.type == INT_TYPE && $3.type == INT_TYPE)
            {
                $$.is_const  = 1;
                $$.int_val   = ($3.int_val != 0)
                               ? $1.int_val / $3.int_val : 0;
                $$.float_val = (double)$$.int_val;
                $$.type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", $1.int_val);
                sprintf(b, "%d", $3.int_val);
                tac_emit($$.name, a, "/", b);
            } else {
                $$.is_const = 0;
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE)
                          ? FLOAT_TYPE : INT_TYPE;
                tac_emit($$.name, $1.name, "/", $3.name);
            }
            $$.node = createNode("/", $1.node, $3.node);
        }
    | factor
        {
            $$ = $1;
        }
    ;

factor
    : INT_LIT
        {
            sprintf($$.name, "%d", $1);
            $$.type      = INT_TYPE;
            $$.is_const  = 1;
            $$.int_val   = $1;
            $$.float_val = (double)$1;
            /* Parse tree: leaf with the literal value */
            char buf[32];
            sprintf(buf, "%d", $1);
            $$.node = createLeaf(buf);
        }
    | FLOAT_LIT
        {
            sprintf($$.name, "%.4f", $1);
            $$.type      = FLOAT_TYPE;
            $$.is_const  = 1;
            $$.float_val = $1;
            $$.int_val   = (int)$1;
            char buf[32];
            sprintf(buf, "%.4f", $1);
            $$.node = createLeaf(buf);
        }
    | IDENTIFIER
        {
            /* --- Semantic Analysis: check variable is declared --- */
            int idx = sym_lookup($1);
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'",
                          yylineno, $1);
                strcpy($$.name, $1);
                $$.type     = UNKNOWN_TYPE;
                $$.is_const = 0;
            } else {
                strcpy($$.name, $1);
                $$.type = (int)sym_get_type(idx);
                const Symbol *s = sym_get(idx);
                if (s && s->is_const) {
                    $$.is_const  = 1;
                    $$.int_val   = s->int_val;
                    $$.float_val = s->float_val;
                } else {
                    $$.is_const = 0;
                }
            }
            /* Parse tree: leaf with the identifier name */
            $$.node = createLeaf($1);
        }
    | '(' expression ')'
        {
            $$ = $2;
        }
    | '-' factor  %prec UMINUS
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($2.is_const && $2.type == INT_TYPE) {
                $$.is_const  = 1;
                $$.int_val   = -$2.int_val;
                $$.float_val = (double)$$.int_val;
                $$.type      = INT_TYPE;
                char arg[32];
                sprintf(arg, "%d", $2.int_val);
                tac_emit($$.name, "0", "-", arg);
            } else {
                $$.is_const = 0;
                $$.type = $2.type;
                tac_emit($$.name, "0", "-", $2.name);
            }
            /* Parse tree: unary negate node */
            $$.node = createNode("NEG", $2.node, NULL);
        }
    ;

%%

/* ================================================================ */
/*  C  CODE  SECTION  (error handling only — main() is in main.c)   */
/* ================================================================ */

#include <stdarg.h>

/*
 * sem_error
 * ---------
 * Report a semantic error.  Uses printf-style formatting.
 *
 * THEORY: Semantic analysis detects errors that cannot be caught
 * by the grammar alone, such as:
 *   - Using a variable before declaring it
 *   - Declaring the same variable twice
 *   - Assigning a value of the wrong type
 */
static void sem_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Semantic Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    has_errors = 1;
    semantic_errors++;
}

/*
 * yyerror
 * -------
 * Called by Bison on a syntax error.
 * Reports the line number where the error was detected.
 *
 * THEORY: Bison's error recovery uses the `error` token to
 * attempt recovery from syntax errors.  We report the line
 * number using yylineno (tracked by Flex via %option yylineno).
 */
void yyerror(const char *msg)
{
    fprintf(stderr, "Syntax Error at line %d: %s\n", yylineno, msg);
    has_errors = 1;
}
