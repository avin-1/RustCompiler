/*
 * intermediate_code.h  (renamed from codegen.h)
 * ====================
 * Header file for the Intermediate Code Generation module.
 *
 * COMPILER DESIGN THEORY:
 * -----------------------
 * Intermediate Code Generation is the phase that translates the
 * validated parse tree / semantic representation into a machine-
 * independent intermediate form.  We use Three-Address Code (TAC),
 * where each instruction has at most three operands:
 *
 *     result = arg1  op  arg2
 *
 * Example:
 *     t1 = a + b
 *     c  = t1
 *
 * This module also includes a constant-folding optimisation pass
 * that evaluates operations on known constants at compile time.
 *
 * THEORETICAL MAPPING:
 *   - Syntax-Directed Translation (SDT) drives the emission of TAC
 *     instructions as grammar rules are reduced.
 *   - Temporary variables (t0, t1, t2, …) act as virtual registers.
 */

#ifndef INTERMEDIATE_CODE_H
#define INTERMEDIATE_CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Maximum number of TAC instructions */
#define MAX_TAC 1024

/* A single Three-Address Code instruction */
typedef struct {
    char result[64];   /* destination (e.g. "t1" or "x")  */
    char arg1[64];     /* first operand                    */
    char op[4];        /* operator: +  -  *  /  or ""      */
    char arg2[64];     /* second operand (may be empty)     */
} TACInstr;

/* ---- public API ---- */

/* Initialise the code generator (call once). */
void        tac_init(void);

/* Create a new temporary variable name  (t0, t1, t2, …).
   The returned pointer is to a static buffer — copy it if needed. */
char       *tac_new_temp(void);

/* Emit a TAC instruction:  result = arg1 op arg2
   If `op` is NULL/empty, it is a simple assignment: result = arg1 */
void        tac_emit(const char *result,
                     const char *arg1,
                     const char *op,
                     const char *arg2);

/* Print all emitted TAC instructions. */
void        tac_print(void);

/* Print the optimised (constant-folded) TAC. */
void        tac_print_optimised(void);

/* Return the number of TAC instructions emitted so far. */
int         tac_count(void);

#endif /* INTERMEDIATE_CODE_H */
