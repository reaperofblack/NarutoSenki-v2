@echo off

echo./*
echo. * Check VC++ environment...
echo. */
echo.

set VSTOOLS="%VS142COMNTOOLS%"
set VC_VER=142

set VSTOOLS=%VSTOOLS:"=%
set "VSTOOLS=%VSTOOLS:\=/%"

set VSVARS="%VSTOOLS%vsvars32.bat"

if not defined VSVARS (
    echo Can't find VC2019 installed!
    goto ERROR
)

echo./*
echo. * Building cocos2d-x library binary, please wait a while...
echo. */
echo.

call %VSVARS%

# build without clean
# msbuild cocos2d-win32.sln /t:Clean
msbuild cocos2d-win32.sln /p:Configuration="Debug" /m
# msbuild cocos2d-win32.sln /p:Configuration="Release" /m

echo./*
echo. * Check the cocos2d-win32 application "NarutoSenki.exe" ...
echo. */
echo.

pushd ".\Release.win32\"

set CC_BIN=NarutoSenki.exe
set CC_RES=..\projects\NarutoSenki\Resources

echo./*
echo. * Copy resources
echo. */
echo.
xcopy  /E /Y /Q "%CC_RES%" .

if not exist "%CC_BIN%" (
    echo Can't find the binary "NarutoSenki.exe", is there build error?
    goto ERROR
)

popd
goto EOF

:ERROR
pause

:EOF
