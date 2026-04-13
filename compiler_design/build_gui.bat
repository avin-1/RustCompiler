@echo off
echo ============================================================
echo   Building Mini Rust Compiler GUI...
echo ============================================================

echo [1/3] Running win_flex on lexer.l ...
win_flex lexer.l
if errorlevel 1 exit /b 1

echo [2/3] Running win_bison on parser.y ...
win_bison -d parser.y
if errorlevel 1 exit /b 1

echo [3/3] Compiling with gcc (GTK3) ...
REM Use a temporary file to store pkg-config output to pass to gcc
pkg-config --cflags --libs gtk+-3.0 > pkg_config_opts.txt
set /p GTK_OPTS=<pkg_config_opts.txt
del pkg_config_opts.txt

gcc ui.c callbacks.c compiler_interface.c lex.yy.c parser.tab.c symbol_table.c intermediate_code.c parse_tree.c -g -o app.exe %GTK_OPTS%
if errorlevel 1 (
    echo ERROR: gcc compilation failed. Ensure MSYS2/MinGW GTK3 is installed.
    exit /b 1
)

echo BUILD SUCCESSFUL — app.exe created.
