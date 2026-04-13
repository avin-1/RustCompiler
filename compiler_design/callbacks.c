#include <gtk/gtk.h>
#include "ui.h"
#include <string.h>
#include "parse_tree.h"

extern Node *parse_tree_root;

void on_open_file_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(app_ui.window),
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        gchar *contents = NULL;
        if (g_file_get_contents(filename, &contents, NULL, NULL)) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_ui.editor_text_view));
            gtk_text_buffer_set_text(buffer, contents, -1);
            g_free(contents);
            
            append_to_terminal("Opened file: ");
            append_to_terminal(filename);
            append_to_terminal("\n");
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_save_file_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Save File",
                                         GTK_WINDOW(app_ui.window),
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);
    
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_ui.editor_text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        
        g_file_set_contents(filename, text, -1, NULL);
        
        append_to_terminal("Saved file: ");
        append_to_terminal(filename);
        append_to_terminal("\n");
        
        g_free(text);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_compile_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_ui.editor_text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    if (strlen(text) == 0) {
        append_to_terminal("Editor is empty. Nothing to compile.\n");
        g_free(text);
        return;
    }
    
    clear_terminal();
    append_to_terminal("Starting compilation...\n");
    
    /* Clear all data stores */
    gtk_list_store_clear(app_ui.tokens_store);
    gtk_tree_store_clear(app_ui.ast_store);
    gtk_list_store_clear(app_ui.symbols_store);
    gtk_list_store_clear(app_ui.quads_store);
    
    run_compiler_and_update_ui(text);
    
    g_free(text);
}

void on_theme_toggled(GtkToggleButton *button, gpointer data) {
    gboolean is_dark = gtk_toggle_button_get_active(button);
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-application-prefer-dark-theme", is_dark, NULL);
}
