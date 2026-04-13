/*
 * main.c
 * ======
 * Entry point for the Mini Rust Compiler.
 *
 * This file orchestrates all compiler phases in order:
 *
 *   Phase 1: LEXICAL ANALYSIS    — Flex tokenises the source file
 *   Phase 2: SYNTAX ANALYSIS     — Bison validates grammar (CFG)
 *   Phase 3: PARSE TREE          — Built during syntax analysis (SDT)
 *   Phase 4: SEMANTIC ANALYSIS   — Symbol table checks (during parsing)
 *   Phase 5: INTERMEDIATE CODE   — TAC generated during parsing (SDT)
 *   Phase 6: OPTIMISATION        — Constant folding on TAC
 *
 * OUTPUT PRODUCED:
 *   1. Token stream              (from lexer, if debug_tokens=1)
 *   2. Parsing result            (success or error summary)
 *   3. Parse Tree                (hierarchical tree display)
 *   4. Symbol Table              (all declared variables)
 *   5. Intermediate Code (TAC)   (three-address code)
 *   6. Optimised Code            (after constant folding)
 *
 * COMPILATION:
 *   win_flex lexer.l
 *   win_bison -d parser.y
 *   gcc lex.yy.c parser.tab.c symbol_table.c intermediate_code.c
 *       parse_tree.c main.c -o compiler.exe
 *
 * USAGE:
 *   compiler.exe test.rs          (read from file)
 *   compiler.exe                  (read from stdin, Ctrl-Z to end)
 */

#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "intermediate_code.h"
#include "parse_tree.h"

/* ---- External declarations from parser.y and lex.yy.c ---- */
extern int   yyparse(void);       /* Bison parser entry point      */
extern FILE *yyin;                /* Flex input file pointer        */
extern int   yylineno;            /* current line number (Flex)     */

/* ---- Global flags declared in parser.y ---- */
extern int   debug_tokens;        /* 1 = print token stream        */
extern int   has_errors;          /* 1 = any error was detected     */
extern int   semantic_errors;     /* count of semantic errors       */

/* ---- Parse tree root, set by parser.y after successful parse ---- */
extern Node *parse_tree_root;

/*
 * main
 * ----
 * Entry point.  Initialises all modules, runs the parser, and
 * prints all compiler outputs in the correct order.
 */
int main(int argc, char **argv)
{
    /* ============================================================ */
    /*  BANNER                                                      */
    /* ============================================================ */
    printf("=============================================================\n");
    printf("       MINI RUST COMPILER  (Flex + Bison)                    \n");
    printf("       Phases: Lexer → Parser → Parse Tree → Semantics → TAC\n");
    printf("=============================================================\n\n");

    /* ============================================================ */
    /*  INPUT                                                       */
    /* ============================================================ */
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
        printf("Reading from file: %s\n\n", argv[1]);
    } else {
        printf("Reading from standard input ");
        printf("(type Rust code, then Ctrl-Z + Enter on Windows):\n\n");
        yyin = stdin;
    }

    /* ============================================================ */
    /*  PHASE 1+2+3+4+5: INITIALISE & PARSE                        */
    /*                                                              */
    /*  All phases run simultaneously during a single yyparse()     */
    /*  call (Syntax-Directed Translation):                         */
    /*    - Flex performs lexical analysis (DFA-based tokenisation)  */
    /*    - Bison performs syntax analysis (LALR(1) CFG parsing)    */
    /*    - Semantic actions build the parse tree, check semantics, */
    /*      and emit TAC — all during grammar rule reductions.      */
    /* ============================================================ */
    sym_init();
    tac_init();

    /* Print token stream header */
    if (debug_tokens) {
        printf("=============================================================\n");
        printf("  PHASE 1: LEXICAL ANALYSIS  (Token Stream)                  \n");
        printf("=============================================================\n");
    }

    /* Run the parser (drives the lexer internally) */
    int result = yyparse();

    if (debug_tokens) {
        printf("\n=============================================================\n\n");
    }

    /* ============================================================ */
    /*  PHASE 2 RESULT: SYNTAX ANALYSIS SUMMARY                     */
    /* ============================================================ */
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

    /* ============================================================ */
    /*  PHASE 3: PARSE TREE  (printed hierarchically)               */
    /* ============================================================ */
    printf("=============================================================\n");
    printf("  PHASE 3: PARSE TREE                                        \n");
    printf("=============================================================\n");
    if (parse_tree_root != NULL) {
        printTree(parse_tree_root, 1);
        /* Also save to file */
        saveTree(parse_tree_root, "parsetree.txt");
    } else {
        printf("  [No parse tree generated (parsing may have failed).]\n");
    }
    printf("=============================================================\n\n");

    /* ============================================================ */
    /*  PHASE 4: SYMBOL TABLE  (semantic analysis results)          */
    /* ============================================================ */
    printf("=============================================================\n");
    printf("  PHASE 4: SYMBOL TABLE  (Semantic Analysis)                 \n");
    printf("=============================================================\n");
    sym_print();

    /* ============================================================ */
    /*  PHASE 5+6: INTERMEDIATE CODE + OPTIMISATION                 */
    /* ============================================================ */
    if (tac_count() > 0) {
        tac_print();
        tac_print_optimised();
    } else {
        printf("[No intermediate code generated.]\n");
    }

    /* ============================================================ */
    /*  CLEANUP                                                     */
    /* ============================================================ */
    if (parse_tree_root != NULL) {
        freeTree(parse_tree_root);
        parse_tree_root = NULL;
    }

    if (yyin != stdin) {
        fclose(yyin);
    }

    return (result == 0 && !has_errors) ? 0 : 1;
}
