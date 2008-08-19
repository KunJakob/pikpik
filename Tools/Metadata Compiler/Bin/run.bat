@echo off

set INPATH=..\..\..\Media\Metadata
set OUTPATH=..\..\..\Media\Metadata
set KEYFILE=..\..\..\Crypto\Keys\AES-256-Key.txt

metacomp %INPATH%\Font.mta %OUTPATH%\Font.emta %KEYFILE%
metacomp %INPATH%\Interface.mta %OUTPATH%\Interface.emta %KEYFILE%
metacomp %INPATH%\Maps.mta %OUTPATH%\Maps.emta %KEYFILE%
metacomp %INPATH%\Sprites.mta %OUTPATH%\Sprites.emta %KEYFILE%
metacomp %INPATH%\Strings.mta %OUTPATH%\Strings.emta %KEYFILE%

pause