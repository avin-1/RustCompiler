#include <gtk/gtk.h>
#include "ui.h"

AppUI app_ui;

/* ================================================================ */
/*  CSS THEME — injected globally for modern IDE aesthetics         */
/* ================================================================ */
static const char *css_style =
    "/* --- Global Font Defaults --- */\n"
    "* { font-family: 'Segoe UI', 'Cantarell', sans-serif; }\n"
    "\n"
    "/* --- Editor text area --- */\n"
    "#editor_view {\n"
    "    font-family: 'Consolas', 'Fira Code', 'Source Code Pro', monospace;\n"
    "    font-size: 12px;\n"
    "    padding: 8px;\n"
    "}\n"
    "\n"
    "/* --- Terminal (bottom pane) --- */\n"
    "#terminal_view {\n"
    "    font-family: 'Consolas', 'Fira Code', monospace;\n"
    "    font-size: 11px;\n"
    "    background-color: #1e1e2e;\n"
    "    color: #cdd6f4;\n"
    "    padding: 6px;\n"
    "}\n"
    "\n"
    "/* --- Toolbar --- */\n"
    "toolbar {\n"
    "    padding: 2px 6px;\n"
    "    border-bottom: 1px solid @borders;\n"
    "}\n"
    "\n"
    "/* --- Notebook tabs --- */\n"
    "notebook tab {\n"
    "    padding: 4px 12px;\n"
    "    font-weight: bold;\n"
    "}\n"
    "\n"
    "/* --- Tree view rows --- */\n"
    "treeview {\n"
    "    font-family: 'Consolas', monospace;\n"
    "    font-size: 11px;\n"
    "}\n"
    "treeview:selected {\n"
    "    background-color: alpha(@theme_selected_bg_color, 0.4);\n"
    "}\n"
    "\n"
    "/* --- Paned handle --- */\n"
    "paned > separator {\n"
    "    min-width: 4px;\n"
    "    min-height: 4px;\n"
    "}\n";

/* ================================================================ */
/*  TERMINAL HELPERS                                                */
/* ================================================================ */

void append_to_terminal(const char *text) {
    if (!app_ui.terminal_text_view) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_ui.terminal_text_view));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, text, -1);
    
    /* Auto-scroll to bottom */
    GtkTextMark *mark = gtk_text_buffer_create_mark(buffer, NULL, &end, FALSE);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(app_ui.terminal_text_view), mark);
    gtk_text_buffer_delete_mark(buffer, mark);
}

void clear_terminal(void) {
    if (!app_ui.terminal_text_view) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_ui.terminal_text_view));
    gtk_text_buffer_set_text(buffer, "", -1);
}

/* ================================================================ */
/*  TAB FACTORY FUNCTIONS                                           */
/* ================================================================ */

static GtkWidget* create_tokens_tab(void) {
    app_ui.tokens_store = gtk_list_store_new(1, G_TYPE_STRING);
    
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(app_ui.tokens_store));
    g_object_unref(app_ui.tokens_store);
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "family", "Consolas, monospace", NULL);
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Token Output", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_expand(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), tree);
    return scroll;
}

static GtkWidget* create_ast_tab(void) {
    /* --- TreeStore with a single string column for node labels --- */
    app_ui.ast_store = gtk_tree_store_new(1, G_TYPE_STRING);
    
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(app_ui.ast_store));
    g_object_unref(app_ui.ast_store);
    app_ui.ast_tree_view = tree;

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "family", "Consolas, monospace", NULL);
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Abstract Syntax Tree", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_expand(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(tree), TRUE);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), tree);
    return scroll;
}

static GtkWidget* create_symbols_tab(void) {
    /* Model: Name, Type, Scope, Value/Defined */
    app_ui.symbols_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,
                                                  G_TYPE_STRING, G_TYPE_STRING);
    
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(app_ui.symbols_store));
    g_object_unref(app_ui.symbols_store);
    
    const char *titles[] = {"Name", "Type", "Scope", "Value / Defined"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        g_object_set(renderer, "family", "Consolas, monospace", NULL);
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_expand(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    }

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), tree);
    return scroll;
}

static GtkWidget* create_quads_tab(void) {
    /* Model: op, arg1, arg2, result */
    app_ui.quads_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,
                                                G_TYPE_STRING, G_TYPE_STRING);
    
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(app_ui.quads_store));
    g_object_unref(app_ui.quads_store);
    
    const char *titles[] = {"Op", "Arg1", "Arg2", "Result"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        g_object_set(renderer, "family", "Consolas, monospace", NULL);
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 80);
        gtk_tree_view_column_set_expand(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    }

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree), GTK_TREE_VIEW_GRID_LINES_BOTH);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), tree);
    return scroll;
}

/* ================================================================ */
/*  MAIN UI BUILDER                                                 */
/* ================================================================ */

void build_ui(GtkWidget *window) {
    app_ui.window = window;

    /* ---------- Apply CSS Theme ---------- */
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, css_style, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);

    /* ---------- Root VBox ---------- */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* ===== TOOLBAR ===== */
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH_HORIZ);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    /* Open */
    GtkToolItem *btn_open = gtk_tool_button_new(
        gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_SMALL_TOOLBAR), "Open");
    gtk_tool_item_set_tooltip_text(btn_open, "Open a source file");
    g_signal_connect(btn_open, "clicked", G_CALLBACK(on_open_file_clicked), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btn_open, -1);

    /* Save */
    GtkToolItem *btn_save = gtk_tool_button_new(
        gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_SMALL_TOOLBAR), "Save");
    gtk_tool_item_set_tooltip_text(btn_save, "Save current file");
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_file_clicked), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btn_save, -1);

    /* Separator */
    GtkToolItem *sep = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep, -1);

    /* Compile */
    GtkToolItem *btn_compile = gtk_tool_button_new(
        gtk_image_new_from_icon_name("system-run", GTK_ICON_SIZE_SMALL_TOOLBAR), "Compile");
    gtk_tool_item_set_tooltip_text(btn_compile, "Compile the source code");
    g_signal_connect(btn_compile, "clicked", G_CALLBACK(on_compile_clicked), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btn_compile, -1);

    /* Separator */
    GtkToolItem *sep2 = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep2, -1);

    /* Dark Mode Toggle */
    GtkWidget *toggle_btn = gtk_toggle_button_new_with_label("  Dark Mode  ");
    g_signal_connect(toggle_btn, "toggled", G_CALLBACK(on_theme_toggled), NULL);
    GtkToolItem *tool_item_toggle = gtk_tool_item_new();
    gtk_container_add(GTK_CONTAINER(tool_item_toggle), toggle_btn);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_toggle, -1);

    /* ===== VERTICAL PANE: Top (editor + tabs) / Bottom (terminal) ===== */
    GtkWidget *vpaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(vbox), vpaned, TRUE, TRUE, 0);

    /* ===== HORIZONTAL PANE: Left (editor) / Right (notebook) ===== */
    GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_pack1(GTK_PANED(vpaned), hpaned, TRUE, FALSE);

    /* ----- Left: Code Editor ----- */
    GtkWidget *editor_frame = gtk_frame_new(NULL);
    GtkWidget *editor_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(editor_scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    app_ui.editor_text_view = gtk_text_view_new();
    gtk_widget_set_name(app_ui.editor_text_view, "editor_view");

    /* Monospace font */
    PangoFontDescription *font_desc = pango_font_description_from_string("Consolas, Monospace 11");
    gtk_widget_override_font(app_ui.editor_text_view, font_desc);
    pango_font_description_free(font_desc);

    /* Editor margins for readability */
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(app_ui.editor_text_view), 12);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(app_ui.editor_text_view), 12);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(app_ui.editor_text_view), 10);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(app_ui.editor_text_view), 10);
    gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(app_ui.editor_text_view), 2);
    gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(app_ui.editor_text_view), 2);

    gtk_container_add(GTK_CONTAINER(editor_scroll), app_ui.editor_text_view);
    gtk_container_add(GTK_CONTAINER(editor_frame), editor_scroll);
    gtk_paned_pack1(GTK_PANED(hpaned), editor_frame, TRUE, FALSE);
    gtk_widget_set_size_request(editor_frame, 420, 400);

    /* ----- Right: Notebook (Tabs) ----- */
    app_ui.notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(app_ui.notebook), GTK_POS_TOP);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(app_ui.notebook), TRUE);

    gtk_notebook_append_page(GTK_NOTEBOOK(app_ui.notebook),
                             create_tokens_tab(), gtk_label_new("  Tokens  "));
    gtk_notebook_append_page(GTK_NOTEBOOK(app_ui.notebook),
                             create_ast_tab(), gtk_label_new("  AST  "));
    gtk_notebook_append_page(GTK_NOTEBOOK(app_ui.notebook),
                             create_symbols_tab(), gtk_label_new("  Symbols  "));
    gtk_notebook_append_page(GTK_NOTEBOOK(app_ui.notebook),
                             create_quads_tab(), gtk_label_new("  Quadruples  "));

    gtk_paned_pack2(GTK_PANED(hpaned), app_ui.notebook, TRUE, FALSE);
    gtk_widget_set_size_request(app_ui.notebook, 420, 400);

    /* ----- Bottom: Terminal / Console ----- */
    GtkWidget *terminal_frame = gtk_frame_new("Console Output");
    GtkWidget *terminal_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(terminal_scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    app_ui.terminal_text_view = gtk_text_view_new();
    gtk_widget_set_name(app_ui.terminal_text_view, "terminal_view");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app_ui.terminal_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app_ui.terminal_text_view), FALSE);

    /* Terminal font */
    font_desc = pango_font_description_from_string("Consolas, Monospace 10");
    gtk_widget_override_font(app_ui.terminal_text_view, font_desc);
    pango_font_description_free(font_desc);

    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(app_ui.terminal_text_view), 8);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(app_ui.terminal_text_view), 6);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(app_ui.terminal_text_view), 6);
    gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(app_ui.terminal_text_view), 1);

    gtk_container_add(GTK_CONTAINER(terminal_scroll), app_ui.terminal_text_view);
    gtk_container_add(GTK_CONTAINER(terminal_frame), terminal_scroll);
    gtk_paned_pack2(GTK_PANED(vpaned), terminal_frame, FALSE, FALSE);
    gtk_widget_set_size_request(terminal_frame, -1, 160);

    /* Give initial focus to the editor */
    gtk_widget_grab_focus(app_ui.editor_text_view);
}

/* ================================================================ */
/*  MAIN ENTRY POINT                                                */
/* ================================================================ */

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Mini Rust Compiler IDE");
    gtk_window_set_default_size(GTK_WINDOW(window), 1100, 720);
    gtk_container_set_border_width(GTK_CONTAINER(window), 2);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    build_ui(window);

    gtk_widget_show_all(window);

    /* Welcome banner */
    append_to_terminal("╔══════════════════════════════════════════════════╗\n");
    append_to_terminal("║       Mini Rust Compiler IDE — Ready            ║\n");
    append_to_terminal("╠══════════════════════════════════════════════════╣\n");
    append_to_terminal("║  Type your code or Open a file, then Compile.   ║\n");
    append_to_terminal("╚══════════════════════════════════════════════════╝\n");

    gtk_main();
    return 0;
}
