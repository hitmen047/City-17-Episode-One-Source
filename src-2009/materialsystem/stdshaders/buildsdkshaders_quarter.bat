@echo off
setlocal

rem Use dynamic shaders to build .inc files only
rem set dynamic_shaders=1
rem == Setup path to nmake.exe, from vc 2005 common tools directory ==
call "%VS100COMNTOOLS%vsvars32.bat"

rem ================================
rem ==== MOD PATH CONFIGURATIONS ===

rem == Set the absolute path to your mod's game directory here ==
rem == Note that this path needs does not support long file/directory names ==
rem == So instead of a path such as "C:\Program Files\Steam\steamapps\mymod" ==
rem == you need to find the 8.3 abbreviation for the directory name using 'dir /x' ==
rem == and set the directory to something like C:\PROGRA~2\Steam\steamapps\sourcemods\mymod ==
set GAMEDIR=C:\PROGRA~2\Steam\steamapps\sourcemods\city17

rem == Set the relative path to SourceSDK\bin\orangebox\bin ==
rem == As above, this path does not support long directory names or spaces ==
rem == e.g. ..\..\..\..\..\PROGRA~2\Steam\steamapps\<USER NAME>\sourcesdk\bin\orangebox\bin ==
set SDKBINDIR=..\..\..\..\..\..\PROGRA~2\Steam\steamapps\kyle777777\sourcesdk\bin\orangebox\bin

rem ==  Set the Path to your mods root source code ==
rem this should already be correct, accepts relative paths only!
set SOURCEDIR=..\..

rem ==== MOD PATH CONFIGURATIONS END ===
rem ====================================





set TTEXE=..\..\devtools\bin\timeprecise.exe
if not exist %TTEXE% goto no_ttexe
goto no_ttexe_end

:no_ttexe
set TTEXE=time /t
:no_ttexe_end


rem echo.
rem echo ~~~~~~ buildsdkshaders %* ~~~~~~
%TTEXE% -cur-Q
set tt_all_start=%ERRORLEVEL%
set tt_all_chkpt=%tt_start%

set BUILD_SHADER=call buildshaders.bat
set ARG_EXTRA=

%BUILD_SHADER% quartershader			-game %GAMEDIR% -source %SOURCEDIR%
%BUILD_SHADER% quartershader_30			-game %GAMEDIR% -source %SOURCEDIR% -dx9_30	-force30

rem echo.
if not "%dynamic_shaders%" == "1" (
  rem echo Finished full buildallshaders %*
) else (
  rem echo Finished dynamic buildallshaders %*
)

rem %TTEXE% -diff %tt_all_start% -cur
rem echo.
