#!/bin/bash
# build_gui.sh
# Build script for MSYS2/MinGW to compile the GTK3 GUI version.

echo "============================================================"
echo "  Building Mini Rust Compiler GUI..."
echo "============================================================"

echo "[1/3] Running flex on lexer.l ..."
flex lexer.l || win_flex lexer.l || cmd.exe //c "win_flex lexer.l"
if [ $? -ne 0 ]; then
    echo "ERROR: flex/win_flex failed."
    exit 1
fi

echo "[2/3] Running bison on parser.y ..."
bison -d parser.y || win_bison -d parser.y || cmd.exe //c "win_bison -d parser.y"
if [ $? -ne 0 ]; then
    echo "ERROR: bison/win_bison failed."
    exit 1
fi

echo "[3/3] Compiling with gcc (GTK3) ..."
gcc ui.c callbacks.c compiler_interface.c lex.yy.c parser.tab.c symbol_table.c intermediate_code.c parse_tree.c -g -o app.exe $(pkg-config --cflags --libs gtk+-3.0)
if [ $? -ne 0 ]; then
    echo "ERROR: gcc compilation failed. Make sure GTK3 is installed in your MSYS2 environment."
    exit 1
fi

echo ""
echo "BUILD SUCCESSFUL — app.exe created."
echo "Run it by typing: ./app.exe"
