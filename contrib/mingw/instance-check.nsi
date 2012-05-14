; RTEMS Tools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

Function CheckInstance
  Push $R0
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "RTEMSTools") i .r1 ?e'
  Pop $R0
!ifdef COMMON_FILES
  StrCmp $R0 0 +3
   MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
   Abort
!else
  StrCmp $R0 0 0 +3
   MessageBox MB_OK|MB_ICONEXCLAMATION "This installer should not be run directly. Please run the rtems${PRODUCT_VERSION}-tools-${RTEMS_BUILD_VERSION}.exe installer."
   Abort
!endif
  Pop $R0
FunctionEnd
