del /a /f /s /q  "*.vs" "*.VC.db" "*.aps" "*.ilk" "*.exp" "*.log" "*.obj" "*.pch" "*.res" "*.idb" "*.cache" "*.iobj" "*.ipdb" "*.tlog" "*.DMP" "*.lastbuildstate"

rd /s /q ".\Windows Hello - VPR\.vs"
rd /s /q ".\Windows Hello - VPR\ipch"

rd /s /q ".\Windows Hello - VPR\Voiceprint Recognition\Debug"
rd /s /q ".\Windows Hello - VPR\Voiceprint Recognition\Release"

rd /s /q ".\Windows Hello - VPR\Windows Hello - VPR\Debug"
rd /s /q ".\Windows Hello - VPR\Windows Hello - VPR\Release"

rd /s /q ".\Windows Hello - VPR\Windows Hello Application\Debug"
rd /s /q ".\Windows Hello - VPR\Windows Hello Application\Release"