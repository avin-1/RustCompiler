/*
 * parser_standalone.y
 * ===================
 * STANDALONE Bison grammar with ALL helper code embedded.
 * No external .c or .h files needed (except lex.yy.c from lexer.l)
 *
 * This version contains:
 *   - Symbol Table implementation
 *   - Intermediate Code (TAC) implementation  
 *   - Parse Tree implementation
 *   - All compiler logic
 */

%{
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
%}

/* ================================================================ */
/*  YYLVAL  union                                                   */
/* ================================================================ */
%union {
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
}

/* ================================================================ */
/*  TOKEN DECLARATIONS                                              */
/* ================================================================ */

%token LET MUT FN IF ELSE WHILE RETURN
%token TYPE_I32 TYPE_F64 TYPE_BOOL
%token PRINTLN

%token <ival> INT_LIT
%token <fval> FLOAT_LIT
%token <sval> STRING_LIT
%token <sval> IDENTIFIER

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
/*  OPERATOR PRECEDENCE                                             */
/* ================================================================ */
%left  '<' '>' EQ NEQ LEQ GEQ
%left  '+' '-'
%left  '*' '/'
%right UMINUS

%%

/* ================================================================ */
/*  GRAMMAR RULES                                                   */
/* ================================================================ */

program
    : item_list
        {
            $$ = $1;
            parse_tree_root = $$;
            printf("[Parser] program → item_list reached; parse_tree_root set.\n");
        }
    | /* empty */
        {
            $$ = createNode("EMPTY_PROGRAM", NULL, NULL);
            parse_tree_root = $$;
            printf("[Parser] program → ε reached; parse_tree_root set to EMPTY_PROGRAM.\n");
        }
    ;

item_list
    : item
        {
            $$ = $1;
        }
    | item_list item
        {
            $$ = createNode("SEQ", $1, $2);
        }
    ;

item
    : statement   { $$ = $1; }
    | function_def { $$ = $1; }
    ;

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

block
    : '{' stmt_list '}'
        {
            $$ = createNode("BLOCK", $2, NULL);
        }
    ;

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
            yyerrok;
            $$ = createNode("ERROR", NULL, NULL);
        }
    ;

declaration
    : LET IDENTIFIER '=' expression ';'
        {
            int idx = sym_insert($2, (VarType)$4.type);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, $2);
            } else {
                if ($4.is_const) {
                    if ($4.type == INT_TYPE)
                        sym_set_int(idx, $4.int_val);
                    else
                        sym_set_float(idx, $4.float_val);
                }
                tac_emit($2, $4.name, NULL, NULL);
                printf("[Parser] Declared '%s' (inferred type).\n", $2);
            }
            Node *id = createLeaf($2);
            $$ = createNode("LET", id, $4.node);
        }
    | LET IDENTIFIER ':' type_spec '=' expression ';'
        {
            int idx = sym_insert($2, (VarType)$4);
            if (idx < 0) {
                sem_error("line %d: variable '%s' already declared", yylineno, $2);
            } else {
                if ($4 != $6.type && $6.type != UNKNOWN_TYPE) {
                    sem_error("line %d: type mismatch in declaration of '%s'", yylineno, $2);
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
                sem_error("line %d: variable '%s' already declared", yylineno, $3);
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
                sem_error("line %d: variable '%s' already declared", yylineno, $3);
            } else {
                if ($5 != $7.type && $7.type != UNKNOWN_TYPE) {
                    sem_error("line %d: type mismatch in declaration of '%s'", yylineno, $3);
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

type_spec
    : TYPE_I32   { $$ = INT_TYPE;   }
    | TYPE_F64   { $$ = FLOAT_TYPE; }
    | TYPE_BOOL  { $$ = INT_TYPE;   }
    ;

assignment
    : IDENTIFIER '=' expression ';'
        {
            int idx = sym_lookup($1);
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'", yylineno, $1);
            } else {
                VarType declared = sym_get_type(idx);
                if (declared != UNKNOWN_TYPE && $3.type != UNKNOWN_TYPE && declared != (VarType)$3.type) {
                    sem_error("line %d: type mismatch in assignment to '%s'", yylineno, $1);
                }
                tac_emit($1, $3.name, NULL, NULL);
                printf("[Parser] Assignment to '%s'.\n", $1);
            }
            Node *id = createLeaf($1);
            $$ = createNode("ASSIGN", id, $3.node);
        }
    ;

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

while_stmt
    : WHILE expression block
        {
            $$ = createNode("WHILE", $2.node, $3);
            printf("[Parser] While loop.\n");
        }
    ;

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

expr_stmt
    : expression ';'
        {
            $$ = createNode("EXPR_STMT", $1.node, NULL);
        }
    ;

expression
    : expression '+' term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const && $1.type == INT_TYPE && $3.type == INT_TYPE) {
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
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
                tac_emit($$.name, $1.name, "+", $3.name);
            }
            $$.node = createNode("+", $1.node, $3.node);
        }
    | expression '-' term
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const && $1.type == INT_TYPE && $3.type == INT_TYPE) {
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
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
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
            if ($1.is_const && $3.is_const && $1.type == INT_TYPE && $3.type == INT_TYPE) {
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
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
                tac_emit($$.name, $1.name, "*", $3.name);
            }
            $$.node = createNode("*", $1.node, $3.node);
        }
    | term '/' factor
        {
            char *t = tac_new_temp();
            strcpy($$.name, t);
            if ($1.is_const && $3.is_const && $1.type == INT_TYPE && $3.type == INT_TYPE) {
                $$.is_const  = 1;
                $$.int_val   = ($3.int_val != 0) ? $1.int_val / $3.int_val : 0;
                $$.float_val = (double)$$.int_val;
                $$.type      = INT_TYPE;
                char a[32], b[32];
                sprintf(a, "%d", $1.int_val);
                sprintf(b, "%d", $3.int_val);
                tac_emit($$.name, a, "/", b);
            } else {
                $$.is_const = 0;
                $$.type = ($1.type == FLOAT_TYPE || $3.type == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
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
            int idx = sym_lookup($1);
            if (idx < 0) {
                sem_error("line %d: undeclared variable '%s'", yylineno, $1);
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
            $$.node = createNode("NEG", $2.node, NULL);
        }
    ;

%%

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
