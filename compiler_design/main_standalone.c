/*
 * main_standalone.c
 * =================
 * Minimal entry point for the standalone compiler.
 * Works ONLY with lexer.l and parser_standalone.y
 * NO external helper files needed!
 */

#include <stdio.h>
#include <stdlib.h>

/* External declarations from parser_standalone.y */
extern int   yyparse(void);
extern FILE *yyin;
extern int   yylineno;
extern int   debug_tokens;
extern int   has_errors;
extern int   semantic_errors;

/* These functions are defined in parser_standalone.y */
extern void sym_init(void);
extern void sym_print(void);
extern void tac_init(void);
extern void tac_print(void);
extern void tac_print_optimised(void);
extern int  tac_count(void);

/* Parse tree functions from parser_standalone.y */
typedef struct Node Node;
extern Node *parse_tree_root;
extern void printTree(Node *node, int indent);
extern void saveTree(Node *node, const char *filename);
extern void freeTree(Node *node);

int main(int argc, char **argv)
{
    printf("=============================================================\n");
    printf("       MINI RUST COMPILER  (Flex + Bison STANDALONE)         \n");
    printf("       ALL CODE IN .l AND .y FILES ONLY!                     \n");
    printf("=============================================================\n\n");

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
        printf("Reading from file: %s\n\n", argv[1]);
    } else {
        printf("Reading from standard input:\n\n");
        yyin = stdin;
    }

    sym_init();
    tac_init();

    if (debug_tokens) {
        printf("=============================================================\n");
        printf("  PHASE 1: LEXICAL ANALYSIS  (Token Stream)                  \n");
        printf("=============================================================\n");
    }

    int result = yyparse();

    if (debug_tokens) {
        printf("\n=============================================================\n\n");
    }

    printf("=============================================================\n");
    printf("  PHASE 2: SYNTAX ANALYSIS  (Parsing Result)                 \n");
    printf("=============================================================\n");
    if (result == 0 && !has_errors) {
        printf("  >>> Parsing completed SUCCESSFULLY. No errors.\n");
    } else {
        printf("  >>> Parsing completed with ERRORS.\n");
    }
    if (semantic_errors > 0) {
        printf("  >>> %d semantic error(s) detected.\n", semantic_errors);
    }
    printf("=============================================================\n\n");

    printf("=============================================================\n");
    printf("  PHASE 3: PARSE TREE                                        \n");
    printf("=============================================================\n");
    if (parse_tree_root != NULL) {
        printTree(parse_tree_root, 1);
        saveTree(parse_tree_root, "parsetree.txt");
    } else {
        printf("  [No parse tree generated (parsing may have failed).]\n");
    }
    printf("=============================================================\n\n");

    printf("=============================================================\n");
    printf("  PHASE 4: SYMBOL TABLE  (Semantic Analysis)                 \n");
    printf("=============================================================\n");
    sym_print();

    if (tac_count() > 0) {
        tac_print();
        tac_print_optimised();
    } else {
        printf("[No intermediate code generated.]\n");
    }

    if (parse_tree_root != NULL) {
        freeTree(parse_tree_root);
        parse_tree_root = NULL;
    }

    if (yyin != stdin) {
        fclose(yyin);
    }

    return (result == 0 && !has_errors) ? 0 : 1;
}
