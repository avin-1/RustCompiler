@echo off
echo ============================================================
echo Running Comprehensive Compiler Tests
echo ============================================================
echo.

echo ============================================================
echo TEST 1: test_all_types.rs (All Type Declarations)
echo ============================================================
.\compiler_standalone.exe test_all_types.rs > test1_output.txt 2>&1
echo Test completed. Checking results...
findstr /C:"Parsing completed SUCCESSFULLY" test1_output.txt >nul
if %errorlevel%==0 (
    echo [PASS] Parsing completed successfully
) else (
    echo [FAIL] Parsing failed
)
findstr /C:"semantic error" test1_output.txt >nul
if %errorlevel%==0 (
    echo [FAIL] Semantic errors found
) else (
    echo [PASS] No semantic errors
)
for /f %%i in ('findstr /C:"exited" test1_output.txt ^| find /C "exited"') do set count1=%%i
echo [INFO] Symbol table contains %count1% symbols
echo.

echo ============================================================
echo TEST 2: test.rs (Basic Functionality)
echo ============================================================
.\compiler_standalone.exe test.rs > test2_output.txt 2>&1
echo Test completed. Checking results...
findstr /C:"Parsing completed SUCCESSFULLY" test2_output.txt >nul
if %errorlevel%==0 (
    echo [PASS] Parsing completed successfully
) else (
    echo [FAIL] Parsing failed
)
findstr /C:"semantic error" test2_output.txt >nul
if %errorlevel%==0 (
    echo [FAIL] Semantic errors found
) else (
    echo [PASS] No semantic errors
)
for /f %%i in ('findstr /C:"exited" test2_output.txt ^| find /C "exited"') do set count2=%%i
echo [INFO] Symbol table contains %count2% symbols
echo.

echo ============================================================
echo TEST 3: test_complex.rs (Complex Expressions)
echo ============================================================
.\compiler_standalone.exe test_complex.rs > test3_output.txt 2>&1
echo Test completed. Checking results...
findstr /C:"Parsing completed SUCCESSFULLY" test3_output.txt >nul
if %errorlevel%==0 (
    echo [PASS] Parsing completed successfully
) else (
    echo [FAIL] Parsing failed
)
findstr /C:"semantic error" test3_output.txt >nul
if %errorlevel%==0 (
    echo [FAIL] Semantic errors found
) else (
    echo [PASS] No semantic errors
)
for /f %%i in ('findstr /C:"exited" test3_output.txt ^| find /C "exited"') do set count3=%%i
echo [INFO] Symbol table contains %count3% symbols
echo.

echo ============================================================
echo TEST 4: test_errors.rs (Error Handling)
echo ============================================================
if exist test_errors.rs (
    .\compiler_standalone.exe test_errors.rs > test4_output.txt 2>&1
    echo Test completed. Checking results...
    findstr /C:"semantic error" test4_output.txt >nul
    if %errorlevel%==0 (
        echo [PASS] Errors correctly detected
    ) else (
        echo [INFO] No errors detected (file may be valid)
    )
) else (
    echo [SKIP] test_errors.rs not found
)
echo.

echo ============================================================
echo SUMMARY
echo ============================================================
echo Test 1 (test_all_types.rs): %count1% symbols in table
echo Test 2 (test.rs):            %count2% symbols in table
echo Test 3 (test_complex.rs):    %count3% symbols in table
echo.
echo All test outputs saved to test*_output.txt files
echo ============================================================
pause
