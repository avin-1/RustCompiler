@echo off
echo ============================================================
echo   Building Mini Rust Compiler GUI...
echo ============================================================

echo [1/3] Running flex on lexer.l ...
flex lexer.l 2>nul || win_flex lexer.l
if errorlevel 1 (
    echo ERROR: flex/win_flex failed. Make sure flex is installed.
    echo For MSYS2: pacman -S flex bison
    exit /b 1
)

echo [2/3] Running bison on parser_standalone.y ...
bison -d parser_standalone.y 2>nul || win_bison -d parser_standalone.y
if errorlevel 1 (
    echo ERROR: bison/win_bison failed. Make sure bison is installed.
    echo For MSYS2: pacman -S flex bison
    exit /b 1
)

echo [3/3] Compiling with gcc (GTK3) ...

REM Try pkg-config first
for /f "delims=" %%i in ('pkg-config --cflags --libs gtk+-3.0 2^>nul') do set GTK_FLAGS=%%i

REM If pkg-config failed, try direct compilation (assumes GTK in PATH)
if "%GTK_FLAGS%"=="" (
    echo pkg-config not found, trying direct GTK compilation...
    gcc ui.c callbacks.c compiler_interface.c lex.yy.c parser_standalone.tab.c -g -o compiler_gui.exe -mwindows -lgtk-3 -lgdk-3 -lgdi32 -limm32 -lshell32 -lole32 -Wl,-luuid -lpangowin32-1.0 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl
) else (
    gcc ui.c callbacks.c compiler_interface.c lex.yy.c parser_standalone.tab.c -g -o compiler_gui.exe %GTK_FLAGS%
)
if errorlevel 1 (
    echo ERROR: gcc compilation failed.
    echo Make sure MSYS2/MinGW GTK3 development libraries are installed.
    exit /b 1
)

echo.
echo ============================================================
echo BUILD SUCCESSFUL — compiler_gui.exe created.
echo ============================================================
echo.
echo To run the GUI compiler, execute: compiler_gui.exe
echo.
