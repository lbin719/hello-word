@REM 先把STM32Cube IDE的安装路径添加到�??变量,如把 C:\ST\STM32CubeIDE_1.12.1\STM32CubeIDE 添加到环境变�?
@REM set IDEDIR=C:\ST\STM32CubeIDE_1.12.1\STM32CubeIDE

@echo off

@REM build bootloader
set PROJECTNAME=bootloader
set BASEDIR=%~dp0
set PROJCETSDIR=Projects\bootloader\SW4STM32\bootloader

@REM import project
stm32cubeidec.exe --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\ -import %BASEDIR%\%PROJCETSDIR%

@REM clean and build
stm32cubeidec.exe --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\ -cleanBuild %PROJECTNAME%

mkdir binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.elf .\binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.map .\binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.bin .\binary

pause