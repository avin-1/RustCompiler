/*
 * intermediate_code.c  (renamed from codegen.c)
 * ====================
 * Implementation of the Intermediate Code Generation module.
 *
 * Generates Three-Address Code (TAC) and provides a simple
 * constant-folding optimisation pass.
 *
 * COMPILER DESIGN THEORY:
 * -----------------------
 * TAC is a linear sequence of instructions where each instruction
 * performs at most one operation.  It bridges the gap between the
 * high-level source language and low-level machine code.
 *
 * OPTIMISATION — CONSTANT FOLDING:
 * If both operands of an arithmetic instruction are known constants
 * (integer literals), we compute the result at compile time:
 *     t1 = 3 + 4   →   t1 = 7   (folded)
 */

#include "intermediate_code.h"

/* ---- private state ---- */
static TACInstr instructions[MAX_TAC];  /* instruction buffer      */
static int      instr_count = 0;        /* number of instructions  */
static int      temp_counter = 0;       /* for generating t0,t1,…  */

/* Static buffer for tac_new_temp() */
static char temp_buf[64];

/*
 * tac_init
 * --------
 * Reset the code generator.  Call once before compilation begins.
 */
void tac_init(void)
{
    instr_count  = 0;
    temp_counter = 0;
    memset(instructions, 0, sizeof(instructions));
}

/*
 * tac_new_temp
 * ------------
 * Return the next temporary variable name: t0, t1, t2, …
 *
 * WARNING: the returned pointer points to a static buffer and will
 * be overwritten by the next call.  Copy it immediately if you
 * need to keep the value.
 *
 * THEORY: Temporary variables act as virtual registers in the
 * intermediate representation, holding sub-expression results.
 */
char *tac_new_temp(void)
{
    sprintf(temp_buf, "t%d", temp_counter++);
    return temp_buf;
}

/*
 * tac_emit
 * --------
 * Append a new TAC instruction to the instruction buffer.
 *
 *   result = arg1  op  arg2      (if op is non-empty)
 *   result = arg1                (simple copy / assignment)
 *
 * This function is called from Bison semantic actions during
 * Syntax-Directed Translation (SDT).
 */
void tac_emit(const char *result,
              const char *arg1,
              const char *op,
              const char *arg2)
{
    if (instr_count >= MAX_TAC) {
        fprintf(stderr, "Error: TAC buffer overflow (max %d)\n", MAX_TAC);
        return;
    }

    TACInstr *ins = &instructions[instr_count++];

    strncpy(ins->result, result ? result : "", 63);
    strncpy(ins->arg1,   arg1   ? arg1   : "", 63);
    strncpy(ins->op,     op     ? op     : "", 3);
    strncpy(ins->arg2,   arg2   ? arg2   : "", 63);
}

/*
 * tac_print
 * ---------
 * Print all TAC instructions in human-readable form.
 */
void tac_print(void)
{
    printf("\n");
    printf("=============================================================\n");
    printf("          INTERMEDIATE CODE  (Three-Address Code)            \n");
    printf("=============================================================\n");

    for (int i = 0; i < instr_count; i++) {
        TACInstr *ins = &instructions[i];
        if (ins->op[0] != '\0') {
            /* result = arg1 op arg2 */
            printf("  %s = %s %s %s\n",
                   ins->result, ins->arg1, ins->op, ins->arg2);
        } else {
            /* result = arg1  (simple assignment) */
            printf("  %s = %s\n", ins->result, ins->arg1);
        }
    }

    printf("=============================================================\n\n");
}

/* ------------------------------------------------------------------ */
/*          CONSTANT  FOLDING  OPTIMISATION  PASS                     */
/* ------------------------------------------------------------------ */

/*
 * is_integer_literal
 * ------------------
 * Return 1 if `s` looks like an integer literal (optional leading '-'
 * followed by digits).
 */
static int is_integer_literal(const char *s)
{
    if (!s || !*s) return 0;
    if (*s == '-') s++;
    if (!*s) return 0;
    while (*s) {
        if (*s < '0' || *s > '9') return 0;
        s++;
    }
    return 1;
}

/*
 * tac_print_optimised
 * -------------------
 * Perform a single-pass constant-folding optimisation and print
 * the resulting TAC.
 *
 * THEORY: Constant folding is a compile-time optimisation where
 * expressions involving only constants are evaluated during
 * compilation rather than at runtime.
 *
 * Strategy: if both arg1 and arg2 are integer literals, compute
 * the result and replace the instruction with a simple assignment.
 */
void tac_print_optimised(void)
{
    printf("\n");
    printf("=============================================================\n");
    printf("     OPTIMISED CODE  (after constant folding)                \n");
    printf("=============================================================\n");

    for (int i = 0; i < instr_count; i++) {
        TACInstr *ins = &instructions[i];

        /* Attempt constant folding */
        if (ins->op[0] != '\0' &&
            is_integer_literal(ins->arg1) &&
            is_integer_literal(ins->arg2))
        {
            int a = atoi(ins->arg1);
            int b = atoi(ins->arg2);
            int result = 0;
            int valid  = 1;

            switch (ins->op[0]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/':
                    if (b != 0) result = a / b;
                    else        valid = 0;      /* division by zero */
                    break;
                default:
                    valid = 0;
                    break;
            }

            if (valid) {
                printf("  %s = %d    ; folded from: %s %s %s\n",
                       ins->result, result,
                       ins->arg1, ins->op, ins->arg2);
                continue;
            }
        }

        /* Not foldable — print as-is */
        if (ins->op[0] != '\0') {
            printf("  %s = %s %s %s\n",
                   ins->result, ins->arg1, ins->op, ins->arg2);
        } else {
            printf("  %s = %s\n", ins->result, ins->arg1);
        }
    }

    printf("=============================================================\n\n");
}

/*
 * tac_count
 * ---------
 * Return the number of instructions emitted so far.
 */
int tac_count(void)
{
    return instr_count;
}
