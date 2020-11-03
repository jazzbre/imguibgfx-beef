@echo off
IF EXIST "..\SDL2" (
   echo SDL2 found!
) else (
   echo SDL2 path not found! Please clone or unpack to ..\SDL2
   pause
   exit 1
)


IF EXIST "..\bgfx-beef" (
   echo bgfx-beef found!
) else (
   echo bgfx-beef path not found! Please clone or unpack to ..\bgfx-beef
   pause
   exit 1
)

submodules\bx\tools\bin\windows\genie vs2019

echo Check Visual Studio version
IF EXIST "c:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE" (
echo Using Visual Studio 2019 Professional Path
set "VISUALSTUDIO19PATH=c:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE"
) else (
echo Using Visual Studio 2019 Community Path
set "VISUALSTUDIO19PATH=c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE"
)

IF EXIST "%VISUALSTUDIO19PATH%" (
echo Building ImGuiBgfx in Visual Studio 2019
"%VISUALSTUDIO19PATH%\devenv" ".build\projects\vs2019\imguibgfx.sln" /Build "Release|x64"
echo ErrorLevel:%ERRORLEVEL%
IF %ERRORLEVEL% EQU 0 (
   echo Build successful!
) else (
   echo Build failed!
)
) else (
echo Visual Studio 2019 not found! Open 'submodules\bgfx\.build\projects\vs2019\imguibgfx.sln' yourself and build it with your own version (NOTE you'll need to change vs2019 above to your installed version)
)

pause