#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "ui.h"
#include "symbol_table.h"
#include "parse_tree.h"

/* Flex/Bison hooks */
extern void yyrestart(FILE *input_file);
extern int yyparse(void);
extern void sym_init(void);
extern void tac_init(void);
extern void sym_print(void);
extern void tac_print(void);
extern void tac_print_optimised(void);

extern FILE *yyin;
extern int debug_tokens;
extern int has_errors;
extern int semantic_errors;
extern int yylineno;
extern Node *parse_tree_root;

/* ================================================================ */
/*  TOKEN PARSING — populate Tokens tab                             */
/* ================================================================ */

static void parse_tokens(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        char *p_start = line;
        while ((p_start = strchr(p_start, '[')) != NULL) {
            char *p_end = strchr(p_start, ']');
            if (p_end) {
                *p_end = '\0';
                GtkTreeIter iter;
                gtk_list_store_append(app_ui.tokens_store, &iter);
                
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "[%s]", p_start + 1);
                gtk_list_store_set(app_ui.tokens_store, &iter, 0, buffer, -1);
                
                *p_end = ']';
                p_start = p_end + 1;
            } else {
                p_start++;
            }
        }
    }
    fclose(f);
}

/* ================================================================ */
/*  TAC PARSING — populate Quadruples tab                           */
/* ================================================================ */

static char* trim_space(char* str) {
    char *end;
    while (*str == ' ') str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;
    end[1] = '\0';
    return str;
}

static void parse_quadruples(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[1024];
    int in_tac = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "INTERMEDIATE CODE") || strstr(line, "OPTIMISED CODE")) {
            in_tac = 1;
            
            GtkTreeIter iter;
            gtk_list_store_append(app_ui.quads_store, &iter);
            char header[128];
            snprintf(header, sizeof(header), "--- %s ---", 
                strstr(line, "OPTIMISED") ? "Optimised Code" : "TAC");
            gtk_list_store_set(app_ui.quads_store, &iter,
                0, header, 1, "", 2, "", 3, "", -1);
            continue;
        }
        
        if (in_tac) {
            if (strstr(line, "========")) {
                continue;
            }
            
            line[strcspn(line, "\r\n")] = 0;
            char *p = line;
            while (*p == ' ') p++;
            
            if (strlen(p) > 0 && strstr(p, "=") != NULL) {
                char res[64] = "", op[16] = "", arg1[64] = "", arg2[64] = "";
                
                char *eq = strchr(p, '=');
                if (eq) {
                    *eq = '\0';
                    strcpy(res, trim_space(p));
                    
                    char *expr = eq + 1;
                    expr = trim_space(expr);
                    
                    /* Look for arithmetic operators */
                    char *op_ptr = strpbrk(expr, "+-*/");
                    if (op_ptr) {
                        strncpy(op, op_ptr, 1);
                        op[1] = '\0';
                        *op_ptr = '\0';
                        
                        strcpy(arg1, trim_space(expr));
                        strcpy(arg2, trim_space(op_ptr + 1));
                    } else {
                        /* Simple assignment: t1 = 5 */
                        strcpy(arg1, expr);
                        strcpy(op, "=");
                    }
                }
                
                GtkTreeIter iter;
                gtk_list_store_append(app_ui.quads_store, &iter);
                gtk_list_store_set(app_ui.quads_store, &iter, 
                    0, op,
                    1, arg1,
                    2, arg2,
                    3, res,
                    -1);
            }
        }
    }
    append_to_terminal("TAC parsed successfully\n");
    fclose(f);
}

/* ================================================================ */
/*  AST POPULATION — recursive tree builder                         */
/* ================================================================ */

static void add_node(GtkTreeStore *store, GtkTreeIter *parent, Node *node) {
    if (!node) return;
    
    GtkTreeIter iter;
    gtk_tree_store_append(store, &iter, parent);
    gtk_tree_store_set(store, &iter, 0, node->label, -1);
    
    for (int i = 0; i < node->child_count; i++) {
        add_node(store, &iter, node->children[i]);
    }
}

static void populate_ast(void) {
    if (parse_tree_root != NULL) {
        gtk_tree_store_clear(app_ui.ast_store);
        add_node(app_ui.ast_store, NULL, parse_tree_root);
        
        /* Expand all nodes so the full tree is visible */
        gtk_tree_view_expand_all(GTK_TREE_VIEW(app_ui.ast_tree_view));
        
        append_to_terminal("AST built successfully\n");
    } else {
        append_to_terminal("Warning: parse_tree_root is NULL — AST not built.\n");
    }
}

/* ================================================================ */
/*  SYMBOL TABLE — populate Symbols tab                             */
/* ================================================================ */

static void populate_symbols(void) {
    int count = sym_count();
    for (int i = 0; i < count; i++) {
        const Symbol *s = sym_get(i);
        if (!s) continue;
        
        const char *t_str = "unknown";
        if (s->type == INT_TYPE) t_str = "i32";
        else if (s->type == FLOAT_TYPE) t_str = "f64";
        
        char scope_buf[32];
        snprintf(scope_buf, sizeof(scope_buf), "%d", s->scope);
        
        char val_buf[64];
        if (s->is_defined) {
            if (s->type == INT_TYPE) snprintf(val_buf, sizeof(val_buf), "%d", s->int_val);
            else if (s->type == FLOAT_TYPE) snprintf(val_buf, sizeof(val_buf), "%g", s->float_val);
            else snprintf(val_buf, sizeof(val_buf), "defined");
        } else {
            snprintf(val_buf, sizeof(val_buf), "undefined");
        }
        
        GtkTreeIter iter;
        gtk_list_store_append(app_ui.symbols_store, &iter);
        gtk_list_store_set(app_ui.symbols_store, &iter, 
            0, s->name,
            1, t_str,
            2, scope_buf,
            3, val_buf,
            -1);
    }
}

/* ================================================================ */
/*  TERMINAL DUMP                                                   */
/* ================================================================ */

static void dump_output_to_terminal(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, f)) > 0) {
        buffer[bytes] = '\0';
        append_to_terminal(buffer);
    }
    fclose(f);
}

/* ================================================================ */
/*  COMPILE ENTRY POINT                                             */
/* ================================================================ */

void run_compiler_and_update_ui(const char *source_code) {
    FILE *f_in = fopen("temp_input.rs", "w");
    if (!f_in) {
        append_to_terminal("Error: Could not create temp_input.rs\n");
        return;
    }
    fputs(source_code, f_in);
    fclose(f_in);
    
    /* Redirect stdout/stderr to capture compiler output */
    FILE *out = freopen("temp_output.txt", "w", stdout);
    if (!out) {
        append_to_terminal("Failed to redirect stdout!\n");
        return;
    }
    freopen("temp_output.txt", "a", stderr);
    
    yyin = fopen("temp_input.rs", "r");
    if (yyin) {
        /* Reset globals */
        yylineno = 1;
        has_errors = 0;
        semantic_errors = 0;
        debug_tokens = 1;

        /* Ensure AST always reflects ONLY the current compile. */
        if (parse_tree_root != NULL) {
            freeTree(parse_tree_root);
            parse_tree_root = NULL;
        }
        
        sym_init();
        tac_init();
        
        yyrestart(yyin);
        
        printf("=============================================================\n");
        printf("       MINI RUST COMPILER  (Flex + Bison) GTK FRONTEND       \n");
        printf("=============================================================\n\n");
        
        /* Execute parser */
        int result = yyparse();

        /* If parsing failed or any errors were detected, ensure we do not
           keep a stale tree and still provide a non-NULL root for the AST tab. */
        if (result != 0 || has_errors) {
            if (parse_tree_root != NULL) {
                freeTree(parse_tree_root);
                parse_tree_root = NULL;
            }
            parse_tree_root = createNode("ERROR", NULL, NULL);
        } else if (parse_tree_root == NULL) {
            /* Successful parse must always yield a root; guard against NULL. */
            parse_tree_root = createNode("EMPTY_PROGRAM", NULL, NULL);
        }
        
        if (result == 0 && !has_errors) {
            printf("\n>>> Parsing completed SUCCESSFULLY. No errors.\n");
        } else {
            printf("\n>>> Parsing completed with ERRORS.\n");
        }
        if (semantic_errors > 0) {
            printf(">>> %d semantic error(s) detected.\n", semantic_errors);
        }
        
        sym_print();
        tac_print();
        tac_print_optimised();
        
        fclose(yyin);
    } else {
        printf("Error: Could not open temp_input.rs for reading.\n");
    }
    
    fflush(stdout);
    fflush(stderr);
    
    /* Propagate parsed data back into GUI models */
    parse_tokens("temp_output.txt");
    parse_quadruples("temp_output.txt");
    populate_symbols();
    populate_ast();
    
    dump_output_to_terminal("temp_output.txt");
    append_to_terminal("\n>>> GUI Update finished.\n");
}
