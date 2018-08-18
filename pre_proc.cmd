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
echo %bn%; > build\buildnumber.txt
echo The new buildnumber is %bn%

:: create the header file
echo /******************************************************************************* > include/build.h
echo * This is an automatically created file! >> include/build.h
echo *******************************************************************************/ >> include/build.h
echo. >> include/build.h
echo #ifndef BUILD_H >> include/build.h
echo #define BUILD_H >> include/build.h
echo. >> include/build.h 
echo //.............................................................................. >> include/build.h
echo. >> include/build.h 
echo #define BUILD_NR       %bn% >> include/build.h
echo #define BUILD_DATE     "%date:~6,4%-%date:~3,2%-%date:~0,2%-%time:~0,2%-%time:~3,2%-%time:~6,2%%"^
  >> include/build.h
echo. >> include/build.h
echo //.............................................................................. >> include/build.h
echo. >> include/build.h 
echo #endif >> include/build.h

:: -----------------------------------------------------------------------------
