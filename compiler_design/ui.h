#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

/* Structure to hold references to UI components */
typedef struct {
    GtkWidget *window;
    GtkWidget *editor_text_view;
    GtkWidget *terminal_text_view;
    
    // Notebook and Tabs
    GtkWidget *notebook;
    
    // Tab data stores
    GtkListStore *tokens_store;
    GtkTreeStore *ast_store;       /* TreeStore for hierarchical AST */
    GtkWidget    *ast_tree_view;   /* TreeView widget for AST tab   */
    GtkListStore *symbols_store;
    GtkListStore *quads_store;
    
    // File currently open
    char *current_filepath;
} AppUI;

/* Global instance of the UI struct */
extern AppUI app_ui;

/* UI construction */
void build_ui(GtkWidget *window);

/* Callbacks */
void on_open_file_clicked(GtkWidget *widget, gpointer data);
void on_save_file_clicked(GtkWidget *widget, gpointer data);
void on_compile_clicked(GtkWidget *widget, gpointer data);
void on_theme_toggled(GtkToggleButton *button, gpointer data);

/* Compiler interface */
void run_compiler_and_update_ui(const char *source_code);

/* Logging to terminal */
void append_to_terminal(const char *text);
void clear_terminal(void);

#endif // UI_H
