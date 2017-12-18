cd /d "H:\CODIGO\PYV\EmuladorPic" &msbuild "C18Emulator.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform=Win32
exit %errorlevel% 