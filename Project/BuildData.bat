@echo off

set METACOMP="..\Tools\Metadata Compiler\Bin\metacomp.exe"
set INPATH="..\Bin\Metadata"
set OUTPATH="..\Bin\Metadata"
set KEYFILE="..\Crypto\Keys\AES-256-Key.txt"

%METACOMP% %INPATH%\Fonts.mta %OUTPATH%\Fonts.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Maps.mta %OUTPATH%\Maps.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Sounds.mta %OUTPATH%\Sounds.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Sprites.mta %OUTPATH%\Sprites.emta %KEYFILE% > nul
%METACOMP% %INPATH%\Strings.mta %OUTPATH%\Strings.emta %KEYFILE% > nul