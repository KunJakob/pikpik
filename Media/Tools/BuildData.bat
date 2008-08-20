@echo off

cd "%CD%"
echo %CD%

set METACOMP="..\..\Tools\Metadata Compiler\Bin\metacomp.exe"
set INPATH="..\Metadata"
set OUTPATH="..\Metadata"
set KEYFILE="..\..\Crypto\Keys\AES-256-Key.txt"

%METACOMP% %INPATH%\Font.mta %OUTPATH%\Font.emta %KEYFILE%
%METACOMP% %INPATH%\Interface.mta %OUTPATH%\Interface.emta %KEYFILE%
%METACOMP% %INPATH%\Maps.mta %OUTPATH%\Maps.emta %KEYFILE%
%METACOMP% %INPATH%\Sprites.mta %OUTPATH%\Sprites.emta %KEYFILE%
%METACOMP% %INPATH%\Strings.mta %OUTPATH%\Strings.emta %KEYFILE%

pause