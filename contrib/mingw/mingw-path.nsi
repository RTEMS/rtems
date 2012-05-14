; RTEMS Tools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

Function MinGWPath
  Push $R0
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MinGW" "InstallLocation"
  Exch $R0
FunctionEnd

Function MinGWCheckFile
  Exch $R0
  DetailPrint "Checking for MinGW $R0."
  IfFileExists "$R0" +5 0
   DetailPrint "Could not find $R0."
   Exch $R0
   Return
  StrCpy $R0 "ok"
  Exch $R0
FunctionEnd

Function MinGWCheckExes
  Exch $R0
  Push $R1
  StrCpy $R1 "$R0\bin\gcc.exe"
  Push $R1
  Call MinGWCheckFile
  Pop $R1
  StrCmp "ok" 0 Done
  StrCpy $R1 "$R0\bin\as.exe"
  Push $R1
  Call MinGWCheckFile
  Pop $R1
  StrCmp "ok" 0 Done
  StrCpy $R1 "$R0\bin\ld.exe"
  Push $R1
  Call MinGWCheckFile
  Pop $R1
Done:
  StrCpy $R0 $R1
  Pop $R1
  Exch $R0
FunctionEnd

Function MinGWDetect
  Push $R0
  Push $R1
  StrCpy $R1 "c:\mingw"
  DetailPrint "Checking MinGW default install path: $R1"
  Push $R1
  Call MinGWCheckExes
  Pop $R0
  StrCmp $R0 "ok" Found
  DetailPrint "Checking Registry for MinGW Installed Path"
  Call MinGWPath
  Pop $R0
  ifErrors 0 +3
   MessageBox MB_OK "MinGW Not Installed. Please install MinGW from http://www.mingw.org/"
   Abort
 Found:
  StrCpy $R0 $R1
  DetailPrint "MinGW Found: $R0"
  Pop $R1
  Exch $R0
FunctionEnd
