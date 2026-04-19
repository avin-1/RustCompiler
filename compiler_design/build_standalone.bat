@echo off
REM Build script for STANDALONE compiler (only .l and .y files)
REM No external .c/.h helper files needed!

echo ============================================================
echo Building STANDALONE Mini Rust Compiler
echo Only lexer.l and parser_standalone.y are used!
echo ============================================================
echo.

echo Step 1: Running Flex on lexer.l...
win_flex lexer.l
if errorlevel 1 (
    echo ERROR: Flex failed!
    pause
    exit /b 1
)
echo Flex completed successfully.
echo.

echo Step 2: Running Bison on parser_standalone.y...
win_bison -d parser_standalone.y
if errorlevel 1 (
    echo ERROR: Bison failed!
    pause
    exit /b 1
)
echo Bison completed successfully.
echo.

echo Step 3: Compiling with GCC...
gcc -o compiler_standalone.exe lex.yy.c parser_standalone.tab.c main_standalone.c -lm
if errorlevel 1 (
    echo ERROR: GCC compilation failed!
    pause
    exit /b 1
)
echo.

echo ============================================================
echo Build successful!
echo Executable: compiler_standalone.exe
echo.
echo Usage:
echo   compiler_standalone.exe test.rs
echo   compiler_standalone.exe ^< input.rs
echo ============================================================
pause
