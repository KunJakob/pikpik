@echo off

set METACOMP="..\Tools\Metadata Compiler\Bin\metacomp.exe"
set INPATH="..\Media\Metadata"
set OUTPATH="..\Media\Metadata"
set KEYFILE="..\Crypto\Keys\AES-256-Key.txt"

%METACOMP% %INPATH%\Font.mta %OUTPATH%\Font.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Interface.mta %OUTPATH%\Interface.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Maps.mta %OUTPATH%\Maps.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Sprites.mta %OUTPATH%\Sprites.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Strings.mta %OUTPATH%\Strings.emta %KEYFILE% > nul