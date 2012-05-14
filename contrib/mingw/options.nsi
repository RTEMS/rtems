; RTEMS Tools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

!ifndef OPTIONS_DEFINED
!define OPTIONS_DEFINED

!include "${RTEMS_SOURCE}/strstr.nsi"
!include "${RTEMS_SOURCE}/getparameters.nsi"

Var "SilentOption"
Var "DebugOption"
Var "DebugScriptOption"

Function CheckCmdLineOption
  Exch $R0
  Call GetParameters
  Push $R0
  Call StrStr
  Exch $R1
  StrCmp $R1 "" 0 +2
  StrCpy $R0 "not-found"
  Pop $R1
  Exch $R0
FunctionEnd

Function CheckSilent
  StrCpy $SilentOption ""
  Push $R0
  Push "/SILENT"
  Call CheckCmdLineOption
  Pop $R0
  StrCmp $R0 "not-found" +2 0
  StrCpy $SilentOption "/S"
  Pop $R0
FunctionEnd

Function CheckDebug
  StrCpy $DebugOption ""
  Push $R0
  Push "/DEBUG"
  Call CheckCmdLineOption
  Pop $R0
  StrCmp $R0 "not-found" +3 0
  StrCpy $DebugScriptOption "-d"
  StrCpy $DebugOption "/DEBUG"
  Pop $R0
FunctionEnd

!endif
