CLS
@echo off
rem This is a small helper script that programs a prebuilt binary for a device using DSLite
rem
:input
@echo MSP-EXP432P401R-LaunchPad Firmware Programmer

set rootDir=%~dp0
set DSLite="%rootDir%..\DSLite\DebugServer\bin\DSLite.exe"
set targetconfig="%rootDir%..\DSLite\MSP432P401R.ccxml"
set firmware="%rootDir%BOOSTXL-SENSORS_SensorGUI_MSP432P401R.out"

@echo Programing %firmware% into %device% ......
%DSLite% flash -l -v -c %targetconfig% -f %firmware%

pause