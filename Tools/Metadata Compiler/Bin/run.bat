@echo off

set INPATH=..\..\..\Bin\Metadata
set OUTPATH=..\..\..\Bin\Metadata
set KEYFILE=..\..\..\Crypto\Keys\AES-256-Key.txt

metacomp %INPATH%\Fonts.mta %OUTPATH%\Fonts.emta %KEYFILE%
metacomp %INPATH%\Interface.mta %OUTPATH%\Interface.emta %KEYFILE%
metacomp %INPATH%\Maps.mta %OUTPATH%\Maps.emta %KEYFILE%
metacomp %INPATH%\Sounds.mta %OUTPATH%\Sounds.emta %KEYFILE%
metacomp %INPATH%\Sprites.mta %OUTPATH%\Sprites.emta %KEYFILE%
metacomp %INPATH%\Strings.mta %OUTPATH%\Strings.emta %KEYFILE%

pause