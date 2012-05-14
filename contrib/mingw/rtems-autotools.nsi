; RTEMS Autotools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

;!define EMPTY_INSTALLER

!define PRODUCT_NAME      "RTEMS Autotools"
!define PRODUCT_VERSION   ${RTEMS_VERSION}
!define PRODUCT_PUBLISHER "RTEM Project Team"
!define PRODUCT_WEB_SITE  "http://www.rtems.org/"
!define PRODUCT_TITLE     "${PRODUCT_NAME} ${PRODUCT_VERSION} (${RTEMS_TARGET})"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; MUI 1.66 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${RTEMS_LOGO}"

!define MUI_COMPONENTSPAGE_SMALLDESC

; Start Menu Support
!include "${RTEMS_SOURCE}/sm-dummy.nsi"
var smfolder
!define MUI_STARTMENUPAGE_DISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "RTEMS ${PRODUCT_VERSION}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU_DUMMY Application $smfolder

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

ReserveFile "rtems.ini"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

Name "${PRODUCT_NAME} ${PRODUCT_VERSION} (${RTEMS_TARGET})"
OutFile "${RTEMS_BINARY}/rtems${PRODUCT_VERSION}-tools-${RTEMS_TARGET}-${RTEMS_BUILD_VERSION}.exe"
InstallDir "C:\opt\rtems-${PRODUCT_VERSION}"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "RTEMS ${RTEMS_TARGET} Tools v${PRODUCT_VERSION}"
AllowRootDirInstall false
AutoCloseWindow true
CRCCheck force

!include "${RTEMS_SOURCE}/instance-check.nsi"
!include "${RTEMS_SOURCE}/mingw-path.nsi"
!include "${RTEMS_SOURCE}/msys-path.nsi"
!include "${RTEMS_SOURCE}/options.nsi"
!include "${RTEMS_SOURCE}/filewrite.nsi"

Section "RTEMS ${RTEMS_TARGET} Tools" SecTools
 SetDetailsView show
 AddSize ${RTEMS_TOOLS_SIZE}

 Call MSYSFstabUpdate

 /*
  * Detect if MSYS is installed.
  */
 Call MSYSDetect
 Pop $9
 StrCmp $9 "not-found" 0 +3
  MessageBox MB_OK "Could not find a valid MSYS. Please install from http://www.mingw.org/"
  Abort
 SetOutPath "$INSTDIR\Uninstall"
 SetOutPath "$INSTDIR\Packages\Build"
 SetOutPath "$INSTDIR\Packages\Build"
 SetOutPath "$INSTDIR\Packages\Source"
 File "${RTEMS_SOURCE}/build-autotools.sh"
 File "${RTEMS_SOURCE}/ba-wrap.sh"
 File "${RTEMS_PACKAGE_SOURCE}/${RTEMS_AUTOCONF_SOURCE}"
 File "${RTEMS_SOURCE}/autoconf.def"
 File "${RTEMS_SOURCE}/automake.def"
 !insertmacro RTEMS_AUTOCONF_PATCHES
 File "${RTEMS_PACKAGE_SOURCE}/${RTEMS_AUTOMAKE_SOURCE}"
 !insertmacro RTEMS_AUTOMAKE_PATCHES

 Delete "$INSTDIR\Packages\Source\at-log.txt"

 /*
  * Need to fix the slashes when using a Unix type shell.
  */
 Push "$INSTDIR\Packages\Source\ba-wrap.sh"
 Push '\\'
 Pop $R0
 Call StrSlash
 Pop $R1
 Push "$INSTDIR\Packages\Build"
 Push '\\'
 Pop $R0
 Call StrSlash
 Pop $R2

 /*
  * Build Autoconf.
  */
 Push "$INSTDIR\Packages\Source\autoconf.def"
 Push '\\'
 Pop $R0
 Call StrSlash
 Pop $R0
 DetailPrint "Building automake. Command window closes automatically. Debug:$DebugScriptOption"
 ExecWait '"$9\bin\sh" --login -c "$R1 $DebugScriptOption -p /opt/rtems-${PRODUCT_VERSION} -b $R2 -c $R0"' $0
 BringToFront
 IntCmp $0 0 +3
  MessageBox MB_OK "Autoconf build failed. See $INSTDIR\Packages\Source\at-log.txt"
  Abort

 BringToFront

 Push "$INSTDIR\Uninstall\${RTEMS_AUTOCONF}-files"
 call InstallFilesFromFile

 /*
  * Build Automake.
  */
 Push "$INSTDIR\Packages\Source\automake.def"
 Push '\\'
 Pop $R0
 Call StrSlash
 Pop $R0
 DetailPrint "Building automake. Command window closes automatically. Debug:$DebugScriptOption"
 ExecWait '"$9\bin\sh" --login -c "$R1 $DebugScriptOption -p /opt/rtems-${PRODUCT_VERSION} -b $R2 -c $R0"' $0
 BringToFront
 IntCmp $0 0 +3
  MessageBox MB_OK "Automake build failed. See $INSTDIR\Packages\Source\at-log.txt"
  Abort

 BringToFront

 Push "$INSTDIR\Uninstall\${RTEMS_AUTOMAKE}-files"
 call InstallFilesFromFile

 DetailPrint "Removing Build directory"
 RMDir /r "$INSTDIR\Packages\Build"
SectionEnd

Function .onInit
  ;Check if we are the correct instance for our mode.
  Call CheckInstance
  ;Handle the Command line options
  Call CheckSilent
  Call CheckDebug
  ;See if MinGW and MSYS are installed
  Call MinGWDetect
  Call MSYSDetect
FunctionEnd

Var FileList

Function InstallFilesFromFile
  Pop $R6
  Push $R7
  Push $R8
  Push $R9
  DetailPrint "Installing file list: $R6"
  SetFileAttributes "$R6" NORMAL
  FileOpen $FileList "$R6" r
  IfErrors 0 +3
   MessageBox MB_OK "Internal error reading file list (0:$R6)."
   Abort
 ReadLoop:
  FileRead $FileList $R7
  IfErrors Done
  StrCpy $R7 $R7 -1 # has a \n only at the end
  StrCpy $R8 $R7
  StrCpy $R7 $R7 1  # first character on the line
  StrCpy $R8 $R8 256 2
  StrCmpS $R7 "D" ReadLoop
  StrCmpS $R7 "b" 0 +3
   StrCpy $R9 $R8
   Goto ReadLoop
  StrCmpS $R7 "d" 0 +3
   CreateDirectory "$INSTDIR\$R8"
   Goto ReadLoop
  StrCmpS $R7 "f" 0 +3
   CopyFiles /SILENT "$INSTDIR\Packages\Build\$R9\$R8" "$INSTDIR\$R8" 
   Goto ReadLoop
  MessageBox MB_OK "Internal error installing package (2:$R6)."
  Abort
 Done:
  FileClose $FileList
  ClearErrors
  Pop $R9
  Pop $R8
  Pop $R7
FunctionEnd

Function un.RemoveFilesFromFile
  ; Two passes, one to delete files then the directories
  ; This is due to the order in the *-files list. Plus
  ; RMDir seems to stuff things up (maybe a NSIS bug!)
  Pop $R6
  Push $R7
  Push $R8
  Push $R9
  DetailPrint "Delete file list: $R6"
  SetFileAttributes "$R6" NORMAL
  FileOpen $FileList "$R6" r
  IfErrors 0 +3
   MessageBox MB_OK "Internal error reading file list (0:$R6)."
   Abort
 ReadLoop:
  FileRead $FileList $R7
  IfErrors Pass2
  StrCpy $R7 $R7 -1 # has a \n only at the end
  StrCpy $R8 $R7
  StrCpy $R7 $R7 1  # first character on the line
  StrCpy $R8 $R8 256 2
  StrCmp $R7 "b" ReadLoop
  StrCmp $R7 "d" ReadLoop
  StrCmp $R7 "D" ReadLoop
  StrCmp $R7 "f" 0 +3
   Delete "$INSTDIR\$R8"
   Goto ReadLoop
  MessageBox MB_OK "Internal error removing package (2:$R6)."
  Abort
 Pass2:
  ClearErrors
  FileSeek $FileList 0 SET
 ReadLoop2:
  FileRead $FileList $R7
  IfErrors Done
  StrCpy $R7 $R7 -1 # has a \n only at the end
  StrCpy $R8 $R7
  StrCpy $R7 $R7 1  # first character on the line
  StrCpy $R8 $R8 256 2
  StrCmpS $R7 "b" ReadLoop2
  StrCmpS $R7 "d" ReadLoop2
  StrCmpS $R7 "f" ReadLoop2
  StrCmpS $R7 "D" 0 Fail
   ifFileExists "$INSTDIR\$R8\*.*" 0 ReadLoop2
    RMDir "$INSTDIR\$R8"  #is dir
   Delete "$INSTDIR\$R8"
   ClearErrors
   Goto ReadLoop2
 Fail:
  MessageBox MB_OK "Internal error removing package (2:$R6)."
  Abort
 Done:
  FileClose $FileList
  ClearErrors
  Delete $R6
  Pop $R9
  Pop $R8
  Pop $R7
FunctionEnd

Section -Post
 StrCpy $R0 "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-${RTEMS_TARGET}-uninst.exe"
 StrCpy $R1 "RTEMS ${RTEMS_TARGET} Tools.lnk"
 WriteUninstaller "$R0"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}" \
                  "DisplayName" "$(^Name)"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}" \
                  "UninstallString" "$R0"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}" \
                  "DisplayVersion" "${PRODUCT_VERSION} Build-${RTEMS_BUILD_VERSION}"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}" \
                  "URLInfoAbout" "${PRODUCT_WEB_SITE}"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}" \
                  "Publisher" "${PRODUCT_PUBLISHER}"
 !insertmacro MUI_STARTMENU_GETFOLDER "Application" $R2
 !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
 CreateDirectory "$SMPROGRAMS\$R2"
 CreateDirectory "$SMPROGRAMS\$R2\Uninstall"
 CreateShortCut "$SMPROGRAMS\$R2\Uninstall\$R1" "$R0"
 !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section Uninstall
 SetDetailsView show
 DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}"
 DetailPrint "Delete the installed files"
 Push "$INSTDIR\Uninstall\${RTEMS_AUTOCONF}-files"
 call un.RemoveFilesFromFile 
 Push "$INSTDIR\Uninstall\${RTEMS_AUTOMAKE}-files"
 call un.RemoveFilesFromFile
 RMDir "$INSTDIR\Uninstall"
 Delete "$INSTDIR\Packages\Source\build-autotools.sh"
 Delete "$INSTDIR\Packages\Source\ba-wrap.sh"
 Delete "$INSTDIR\Packages\Source\${RTEMS_AUTOCONF_SOURCE}"
 Delete "$INSTDIR\Packages\Source\${RTEMS_AUTOMAKE_SOURCE}"
 Delete "$INSTDIR\Packages\Source\autoconf.def"
 Delete "$INSTDIR\Packages\Source\automake.def"
 Delete "$INSTDIR\Packages\Source\at-log.txt"
 !insertmacro RTEMS_DELETE_AUTOCONF_PATCHES
 !insertmacro RTEMS_DELETE_AUTOMAKE_PATCHES
 RMDir "$INSTDIR\Packages\Source"
 RMDir "$INSTDIR\Packages\Build"
 !insertmacro MUI_STARTMENU_GETFOLDER "Application" $smfolder
 Delete "$SMPROGRAMS\$smfolder\Uninstall\RTEMS ${RTEMS_TARGET} Tools.lnk"
 Delete $INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-${RTEMS_TARGET}-uninst.exe
 RMDir "$INSTDIR"
 DetailPrint "All done."
 SetAutoClose true
SectionEnd
