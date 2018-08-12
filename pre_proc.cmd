:: -----------------------------------------------------------------------------
:: Script to automatically creates a buildnumber
:: -----------------------------------------------------------------------------

setlocal EnableDelayedExpansion

:: get the mplabx project name
for %%i in ("%cd%") do set "project=%%~nxi"

:: check wether the file >buildnumber.txt< exists or not
if NOT exist build\buildnumber.txt echo 0; > build\buildnumber.txt

:: read the file
for /f "delims=;" %%i in (build\buildnumber.txt) do set bn=%%i

:: increase the buildnumber
set /a bn=%bn%+1

:: create the header file
echo /******************************************************** > include/build.h
echo * This is an automatically created file!
echo *******************************************************/ >> include/build.h
echo. >> include/build.h
echo #ifndef BUILD_H >> include/build.h
echo #define BUILD_H >> include/build.h
echo. >> include/build.h 
echo #define BUILD_NR %bn% >> include/build.h
echo. >> include/build.h
echo #endif >> include/build.h