:: Batch file for generating and updating Net_Security.c contents
@echo off
setlocal EnableDelayedExpansion

:: Pack Path (when toolchain root folder is not specified)
set RTEPATH=C:\Keil\ARM\PACK

:: Pack Versions (overrides automatic selection of latest pack)
REM set  MDK_MW_VER=7.1.0
REM set mbedTLS_VER=1.1.0

:: Convert @K Path (replaces '/' with '\')
set KPATH=%1
set KPATH=!KPATH:/=\!

:: Pack Path (derived from specified toolchain root folder)
if not .%1==. (
  for /F "tokens=2 delims==" %%a in ('findstr /B /C:"RTEPATH" %KPATH%TOOLS.INI') do set RTEPATH=%%~a
)

:: Pack Versions (latest packs if not already defined)
if not defined MDK_MW_VER (
  for /F "delims=" %%a in ('dir /B /O:N "%RTEPATH%\Keil\MDK-Middleware"') do set MDK_MW_VER=%%a
)
if not defined mbedTLS_VER (
  for /F "delims=" %%a in ('dir /B /O:N "%RTEPATH%\ARM\mbedTLS"') do set mbedTLS_VER=%%a
)

:: Tool path
set  MW_NET_TOOL_PATH=%RTEPATH%\Keil\MDK-Middleware\%MDK_MW_VER%\Network\Tool
set MBED_TLS_PRG_PATH=%RTEPATH%\ARM\mbedTLS\%mbedTLS_VER%\MDK\programs

:: Check if tools exist
if not exist "%MW_NET_TOOL_PATH%\pem2mw.exe" (
  echo Error: pem2mw.exe not found in %MW_NET_TOOL_PATH%
  pause
  exit 1
)
if not exist "%MBED_TLS_PRG_PATH%\gen_key.exe" (
  echo Error: gen_key.exe not found in %MBED_TLS_PRG_PATH%
  pause
  exit 1
)
if not exist "%MBED_TLS_PRG_PATH%\cert_write.exe" (
  echo Error: cert_write.exe not found in %MBED_TLS_PRG_PATH%
  pause
  exit 1
)

echo on

:: Generate random CA and Server Private Keys
echo | "%MBED_TLS_PRG_PATH%\gen_key" type=rsa rsa_keysize=2048 format=pem filename=ca.key
echo | "%MBED_TLS_PRG_PATH%\gen_key" type=rsa rsa_keysize=2048 format=pem filename=server.key

:: Generate Self-signed CA Certificate
echo | "%MBED_TLS_PRG_PATH%\cert_write" ^
  selfsign=1 ^
  issuer_key=ca.key ^
  issuer_name="CN=Test CA,O=Unknown,C=US" ^
  not_before=20200101000000 ^
  not_after=20301231235959 ^
  is_ca=1 ^
  output_file=ca.crt

:: Generate Server Certificate
echo | "%MBED_TLS_PRG_PATH%\cert_write" ^
  subject_key=server.key ^
  subject_name="CN=localhost,O=MyOrganization,C=US" ^
  issuer_key=ca.key ^
  issuer_crt=ca.crt ^
  serial=1 ^
  not_before=20200101000000 ^
  not_after=20301231235959 ^
  output_file=server.crt

:: Update Net_Security.c with generated Certificates and Server Key
echo | "%MW_NET_TOOL_PATH%\pem2mw" --r=ca.crt --c=server.crt --p=server.key --o=Net_Security.c

@pause
