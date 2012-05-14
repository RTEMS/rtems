; RTEMS Tools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

!include "${RTEMS_SOURCE}/strslash.nsi"

Function MsysPath
  Push $R0
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MSYS-1.0_is1" "Inno Setup: App Path"
  Exch $R0
FunctionEnd

Function MsysDTKPath
  Push $R0
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MSYS-DTK_is1" "Inno Setup: App Path"
  Exch $R0
FunctionEnd

Function MSYSCheckExes
  Exch $R0
  DetailPrint "Checking for MSYS $R0\bin\sh.exe."
  IfFileExists "$R0\bin\sh.exe" +5 0
   DetailPrint "Could not find $R0\bin\sh.exe."
   StrCpy $R0 "$R0\bin\sh.exe"
   Exch $R0
   Return
  DetailPrint "Checking for MSYS $R0\bin\m4.exe."
  IfFileExists "$R0\bin\m4.exe" +5 0
   DetailPrint "Could not find $R0\bin\m4.exe."
   StrCpy $R0 "$R0\bin\m4.exe"
   Exch $R0
   Return
  DetailPrint "Checking for MSYS $R0\bin\perl.exe."
  IfFileExists "$R0\bin\perl.exe" +5 0
   DetailPrint "Could not find $R0\bin\perl.exe."
   StrCpy $R0 "$R0\bin\perl.exe"
   Exch $R0
   Return
  StrCpy $R0 "ok"
  Exch $R0
FunctionEnd

Function MSYSDetect
  Push $R0
  Push $R1
  StrCpy $R1 "c:\msys\1.0"
  DetailPrint "Checking MSYS default install path: $R1"
  Push $R1
  Call MSYSCheckExes
  Pop $R0
  StrCmp $R0 "ok" Found
  DetailPrint "Checking Registry for MSYS Installed Path"
  Call MsysPath
  Pop $R0
  ifErrors 0 +3
   MessageBox MB_OK "MSYS Not Installed. Please install MSYS from http://www.mingw.org/"
   Abort
  DetailPrint "Checking Registry for MSYS DTK Installed Path"
  Call MsysDTKPath
  Pop $R1
  ifErrors 0 +3
   MessageBox MB_OK "MSYS DTK Not Installed. Please install MSYS DTK from http://www.mingw.org/"
   Abort
  StrCmp $R0 $R1 +3 0
   MessageBox MB_OK "MSYS and MSYS DTK installed to different paths. This is not supported."
   Abort
  Push $R0
  Call MSYSCheckExes
  Pop $R0
  StrCmp $R0 "ok" Found
   MessageBox MB_OK "MSYS Executable '$R0' not found. Please install from http://www.mingw.org/"
   Abort
 Found:
  StrCpy $R0 $R1
  DetailPrint "MSYS Found: $R0"
  Pop $R1
  Exch $R0
FunctionEnd

Function MSYSDetectSilent
  Push $R0
  Push $R1
  StrCpy $R1 "c:\msys\1.0"
  DetailPrint "Checking MSYS default install path: $R1"
  Push $R1
  Call MSYSCheckExes
  Pop $R0
  StrCmp $R0 "ok" Done
  DetailPrint "Checking Registry for MSYS Installed Path"
  Call MsysPath
  Pop $R0
  ifErrors 0 +3
   StrCpy $R1 "not-found"
   Goto Done
  DetailPrint "Checking Registry for MSYS DTK Installed Path"
  Call MsysDTKPath
  Pop $R1
  ifErrors 0 +3
   StrCpy $R1 "not-found"
   Goto Done
  StrCmp $R0 $R1 +3 0
   StrCpy $R1 "not-found"
   Goto Done
  Push $R0
  Call MSYSCheckExes
  Pop $R0
  StrCmp $R0 "ok" Done
   StrCpy $R1 "not-found"
   Goto Done
 Done:
  StrCpy $R0 $R1
  DetailPrint "MSYS Result: $R0"
  Pop $R1
  Exch $R0
FunctionEnd

Function MSYSFstabUpdate
 Call MSYSDetectSilent
 Pop $R0
 Push $R0
 Push $INSTDIR
 Push '\\'
 Pop $R0
 Call StrSlash
 Pop $R1
 Pop $R0
 DetailPrint "Setting MSYS fstab: $R1 -> /opt/rtems-${PRODUCT_VERSION}"
 StrCpy $R1 "$R1 /opt/rtems-${PRODUCT_VERSION}$\n"
 FileOpen $9 "$R0\etc\fstab" a
 ifErrors 0 +3
   MessageBox MB_OK "Cannot open $R0\etc\fstab. MSYS mount point not added."
   Goto Close
 FileSeek $9 0 SET
ReadLoop:
 FileRead $9 $R2
 ifErrors Append
 StrCmp $R1 $R2 Close ReadLoop
Append:
 FileSeek $9 0 END
 StrCpy $R2 $R2 1 -1
 StrCmp $R2 "$\n" +2 0
 FileWrite $9 "$\n"
 FileWrite $9 $R1
Close:
 FileClose $9
 ClearErrors
FunctionEnd
