@echo off
REM ============================================================
REM  build.bat  —  Build script for the Mini Rust Compiler
REM ============================================================
REM  Usage:
REM    build.bat           — compile the project
REM    build.bat run       — compile and run with test.rs
REM    build.bat errors    — compile and run with test_errors.rs
REM    build.bat clean     — delete generated files
REM ============================================================

if "%1"=="clean" goto clean

echo.
echo ============================================================
echo   Building Mini Rust Compiler...
echo ============================================================
echo.

REM Step 1: Lexer — Flex (generates DFA-based scanner)
echo [1/3] Running win_flex on lexer.l ...
win_flex lexer.l
if errorlevel 1 (
    echo ERROR: win_flex failed. Make sure win_flex is on your PATH.
    exit /b 1
)

REM Step 2: Parser — Bison (generates LALR(1) parser)
echo [2/3] Running win_bison on parser.y ...
win_bison -d parser.y
if errorlevel 1 (
    echo ERROR: win_bison failed. Make sure win_bison is on your PATH.
    exit /b 1
)

REM Step 3: Compile all sources — GCC
echo [3/3] Compiling with gcc ...
gcc -g -o compiler.exe lex.yy.c parser.tab.c symbol_table.c intermediate_code.c parse_tree.c main.c
if errorlevel 1 (
    echo ERROR: gcc compilation failed.
    exit /b 1
)

echo.
echo BUILD SUCCESSFUL — compiler.exe created.
echo.

if "%1"=="run" (
    echo Running with test.rs ...
    echo ============================================================
    compiler.exe test.rs
)

if "%1"=="errors" (
    echo Running with test_errors.rs ...
    echo ============================================================
    compiler.exe test_errors.rs
)

goto end

:clean
echo Cleaning generated files...
del /Q lex.yy.c parser.tab.c parser.tab.h compiler.exe parsetree.txt 2>nul
echo Done.

:end
