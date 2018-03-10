cd %~dp0
mkdir "C:\Program Files\Windows Hello VPR"
mkdir "C:\Program Files\Windows Hello VPR\voiceLib"
xcopy /e /y "..\msvcp100d.dll" "C:\Program Files\Windows Hello VPR"
xcopy /e /y "..\msvcr100.dll" "C:\Program Files\Windows Hello VPR"
xcopy /e /y "..\msvcr100d.dll" "C:\Program Files\Windows Hello VPR"
xcopy /e /y "..\Windows Hello Application.exe" "C:\Program Files\Windows Hello VPR"
xcopy /e /y "..\voiceLib\me.txt" "C:\Program Files\Windows Hello VPR\voiceLib"
regedit /s "setup.reg"
pause