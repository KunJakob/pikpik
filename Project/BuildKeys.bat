@echo off

set METACOMP="..\Tools\Metadata Compiler\Bin\metacomp.exe"
set INPATH="PikPik.rc"
set OUTPATH="PikPik.erc"
set KEYFILE="..\Crypto\Keys\AES-256-Key.txt"
set KEYOUTFILE="..\Crypto\Keys\AES-Hex-256-Key.txt"

%METACOMP% %INPATH% %OUTPATH% %KEYFILE% %KEYOUTFILE%