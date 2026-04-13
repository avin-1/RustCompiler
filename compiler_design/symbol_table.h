/*
 * symbol_table.h
 * ===============
 * Header file for the Symbol Table module.
 *
 * The symbol table stores information about every variable declared
 * in the source program:
 *   - name       : the identifier string
 *   - type       : INT_TYPE, FLOAT_TYPE, or UNKNOWN_TYPE
 *   - is_defined : whether the variable has been assigned a value
 *   - scope      : scope level (0 = global)
 *   - int_val / float_val : constant value (used for constant folding)
 *
 * A simple linear-search array is used (sufficient for a college lab).
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Maximum number of symbols we can store */
#define MAX_SYMBOLS 256

/* Variable types */
typedef enum {
    UNKNOWN_TYPE,
    INT_TYPE,
    FLOAT_TYPE
} VarType;

/* A single symbol-table entry */
typedef struct {
    char   name[64];      /* variable name                            */
    VarType type;         /* data type                                */
    int    is_defined;    /* 1 if the variable has been assigned      */
    int    scope;         /* scope level (0 = global)                 */
    int    int_val;       /* integer constant value (for folding)     */
    double float_val;     /* float   constant value (for folding)     */
    int    is_const;      /* 1 if the stored value is a known constant*/
} Symbol;

/* ---- public API ---- */

/* Initialise the symbol table (call once at start-up) */
void    sym_init(void);

/* Look up a symbol by name.  Returns its index, or -1 if not found. */
int     sym_lookup(const char *name);

/* Insert a new symbol.  Returns the index, or -1 on error
   (e.g. duplicate declaration). */
int     sym_insert(const char *name, VarType type);

/* Set the value of an existing symbol (marks it as defined). */
void    sym_set_int(int index, int value);
void    sym_set_float(int index, double value);

/* Retrieve type of a symbol by index. */
VarType sym_get_type(int index);

/* Print the entire symbol table (for debugging / output). */
void    sym_print(void);

/* Return current number of symbols. */
int     sym_count(void);

/* Get symbol by index (read-only pointer). */
const Symbol *sym_get(int index);

#endif /* SYMBOL_TABLE_H */
