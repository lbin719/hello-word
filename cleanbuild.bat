@REM 鍏堟妸STM32Cube IDE鐨勫畨瑁呰矾寰勬坊鍔犲埌鐜??鍙橀噺,濡傛妸 C:\ST\STM32CubeIDE_1.12.1\STM32CubeIDE 娣诲姞鍒扮幆澧冨彉閲?
@REM set IDEDIR=C:\ST\STM32CubeIDE_1.12.1\STM32CubeIDE

@echo off

@REM build bootloader
@REM set PROJECTNAME=bootloader
@REM set BASEDIR=%~dp0
@REM set PROJCETSDIR=v2\Projects\STM32405\Applications\%PROJECTNAME%\SW4STM32\STM324xG_BOOT

@REM import project
@REM stm32cubeidec.exe --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\ -import %BASEDIR%\%PROJCETSDIR%

@REM clean and build
@REM stm32cubeidec.exe --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\ -cleanBuild %PROJECTNAME%

@REM mkdir binary
@REM copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.elf .\binary
@REM copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.map .\binary
@REM copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.bin .\binary


@REM build smartscale
set PROJECTNAME=smartscale
set BASEDIR=%~dp0
set PROJCETSDIR=Projects\smartscale\SW4STM32\smartscale

@REM import project
@REM stm32cubeidec.exe --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\ -import %BASEDIR%\%PROJCETSDIR%

@REM clean and build
stm32cubeidec.exe --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\ -cleanBuild %PROJECTNAME%

mkdir binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.elf .\binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.map .\binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%.bin .\binary
copy .\%PROJCETSDIR%\Debug\%PROJECTNAME%_raw.bin .\binary

pause