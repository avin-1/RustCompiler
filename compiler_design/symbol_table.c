/*
 * symbol_table.c
 * ===============
 * Implementation of the Symbol Table module.
 *
 * Uses a simple fixed-size array of Symbol structs.
 * Linear search by name — perfectly adequate for small programs.
 */

#include "symbol_table.h"

/* ---- private state ---- */
static Symbol table[MAX_SYMBOLS];   /* the table itself          */
static int    num_symbols = 0;      /* how many entries so far   */

/*
 * sym_init
 * --------
 * Clear the symbol table.  Call once before compilation begins.
 */
void sym_init(void)
{
    num_symbols = 0;
    memset(table, 0, sizeof(table));
}

/*
 * sym_lookup
 * ----------
 * Search for `name` in the table.
 * Returns the index (>= 0) if found, or -1 if not found.
 */
int sym_lookup(const char *name)
{
    for (int i = 0; i < num_symbols; i++) {
        if (strcmp(table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;   /* not found */
}

/*
 * sym_insert
 * ----------
 * Add a new symbol with the given name and type.
 * Returns its index on success, or -1 if the name already exists
 * (a "multiple declaration" semantic error).
 */
int sym_insert(const char *name, VarType type)
{
    /* Check for duplicate */
    if (sym_lookup(name) >= 0) {
        return -1;   /* already declared */
    }

    if (num_symbols >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: symbol table overflow (max %d)\n",
                MAX_SYMBOLS);
        return -1;
    }

    /* Copy name, set type, mark as un-defined */
    strncpy(table[num_symbols].name, name, 63);
    table[num_symbols].name[63] = '\0';
    table[num_symbols].type       = type;
    table[num_symbols].is_defined = 0;
    table[num_symbols].scope      = 0;      /* global scope */
    table[num_symbols].is_const   = 0;

    return num_symbols++;
}

/*
 * sym_set_int
 * -----------
 * Record an integer constant value for the symbol at `index`.
 */
void sym_set_int(int index, int value)
{
    if (index < 0 || index >= num_symbols) return;
    table[index].int_val    = value;
    table[index].float_val  = (double)value;
    table[index].is_defined = 1;
    table[index].is_const   = 1;
}

/*
 * sym_set_float
 * -------------
 * Record a floating-point constant value for the symbol at `index`.
 */
void sym_set_float(int index, double value)
{
    if (index < 0 || index >= num_symbols) return;
    table[index].float_val  = value;
    table[index].int_val    = (int)value;
    table[index].is_defined = 1;
    table[index].is_const   = 1;
}

/*
 * sym_get_type
 * ------------
 * Return the declared type of the symbol at `index`.
 */
VarType sym_get_type(int index)
{
    if (index < 0 || index >= num_symbols) return UNKNOWN_TYPE;
    return table[index].type;
}

/*
 * sym_print
 * ---------
 * Pretty-print the entire symbol table to stdout.
 */
void sym_print(void)
{
    printf("\n");
    printf("=============================================================\n");
    printf("                      SYMBOL  TABLE                          \n");
    printf("=============================================================\n");
    printf("%-4s %-16s %-10s %-10s %-10s %-10s\n",
           "Idx", "Name", "Type", "Defined?", "Scope", "Value");
    printf("-------------------------------------------------------------\n");

    for (int i = 0; i < num_symbols; i++) {
        const char *type_str;
        switch (table[i].type) {
            case INT_TYPE:   type_str = "int";     break;
            case FLOAT_TYPE: type_str = "float";   break;
            default:         type_str = "unknown"; break;
        }

        printf("%-4d %-16s %-10s %-10s %-10d ",
               i,
               table[i].name,
               type_str,
               table[i].is_defined ? "yes" : "no",
               table[i].scope);

        if (table[i].is_defined) {
            if (table[i].type == INT_TYPE)
                printf("%d", table[i].int_val);
            else if (table[i].type == FLOAT_TYPE)
                printf("%.2f", table[i].float_val);
        } else {
            printf("-");
        }
        printf("\n");
    }

    printf("=============================================================\n\n");
}

/*
 * sym_count / sym_get
 * -------------------
 * Accessors for iterating over the table externally.
 */
int sym_count(void)
{
    return num_symbols;
}

const Symbol *sym_get(int index)
{
    if (index < 0 || index >= num_symbols) return NULL;
    return &table[index];
}
