; RTEMS Tools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

;!define EMPTY_INSTALLER

!define PRODUCT_NAME      "RTEMS Tools"
!define PRODUCT_VERSION   ${RTEMS_VERSION}
!define PRODUCT_PUBLISHER "RTEMS Project Team"
!define PRODUCT_WEB_SITE  "http://www.rtems.org/"
!ifdef COMMON_FILES
!define PRODUCT_TITLE     "${PRODUCT_NAME} ${PRODUCT_VERSION} (Build ${RTEMS_BUILD_VERSION})"
!else
!define PRODUCT_TITLE     "${PRODUCT_NAME} ${PRODUCT_VERSION} (${RTEMS_TARGET})"
!endif
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_TITLE}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; MUI 1.66 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_FUNCTION_GUIINIT
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${RTEMS_LOGO}"

!define MUI_COMPONENTSPAGE_SMALLDESC

!ifdef COMMON_FILES
; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Details of what will happen.
Page custom RTEMSMessage
; License page
!insertmacro MUI_PAGE_LICENSE "${RTEMS_LICENSE_FILE}"
; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!define MUI_PAGE_HEADER_SUBTEXT "Choose the folder in which to install the RTEMS Tools."
!define MUI_DIRECTORYPAGE_TEXT_TOP "${PRODUCT_NAME} will install RTEMS Tools in the following directory. To install in a different folder click Browse and select another folder. Click Next to continue."
!insertmacro MUI_PAGE_DIRECTORY
!endif

; Start menu page
!ifndef COMMON_FILES
!include "${RTEMS_SOURCE}/sm-dummy.nsi"
!endif
var smfolder
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "RTEMS ${PRODUCT_VERSION}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!ifdef COMMON_FILES
!insertmacro MUI_PAGE_STARTMENU Application $smfolder
!else
!insertmacro MUI_PAGE_STARTMENU_DUMMY Application $smfolder
!endif

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

!ifdef COMMON_FILES
; Finish page
!insertmacro MUI_PAGE_FINISH
!endif

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

ReserveFile "rtems.ini"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

!ifdef COMMON_FILES
Name "${PRODUCT_NAME} ${PRODUCT_VERSION} (Build ${RTEMS_BUILD_VERSION})"
OutFile "${RTEMS_BINARY}/rtems${PRODUCT_VERSION}-tools-${RTEMS_BUILD_VERSION}.exe"
BrandingText "RTEMS Tools v${PRODUCT_VERSION}"
AutoCloseWindow false
!else
Name "${PRODUCT_NAME} ${PRODUCT_VERSION} (${RTEMS_TARGET})"
OutFile "${RTEMS_BINARY}/${RTEMS_OUTFILE}"
BrandingText "RTEMS ${RTEMS_TARGET} Tools v${PRODUCT_VERSION}"
AutoCloseWindow true
!endif
InstallDir "C:\opt\rtems-${PRODUCT_VERSION}"
ShowInstDetails show
ShowUnInstDetails show
AllowRootDirInstall false
CRCCheck force

!include "${RTEMS_SOURCE}/instance-check.nsi"
!include "${RTEMS_SOURCE}/msys-path.nsi"
!include "${RTEMS_SOURCE}/options.nsi"
!include "${RTEMS_SOURCE}/filewrite.nsi"

Section -SecFiles
 AddSize ${RTEMS_TOOLS_SIZE}
 !insertmacro RTEMS_INSTALL_FILES
SectionEnd

!ifdef COMMON_FILES
Section -SecCommon
 SetOutPath "$INSTDIR"
 File "${RTEMS_SOURCE}/AUTHORS"
 File "${RTEMS_SOURCE}/COPYING"
 File "${RTEMS_SOURCE}/README"
SectionEnd
!endif

Function .onInit
  ;Check if we are the correct instance for our mode.
  Call CheckInstance
  ;Extract InstallOptions INI files
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "rtems.ini"
  ;Handle the Command line options
  Call CheckSilent
  Call CheckDebug
  Call MSYSFstabUpdate
FunctionEnd

Function RTEMSMessage
  !insertmacro MUI_HEADER_TEXT \
               "RTEMS Tools (Build ${RTEMS_BUILD_VERSION})" \
               "A tool set for the RTEMS operating system."

  ;Display the Install Options dialog

  Push $R0
  Push $R1
  Push $R2

  InstallOptions::initDialog /NOUNLOAD "$PLUGINSDIR\rtems.ini"
  Pop $R0

  GetDlgItem $R1 $R0 1200 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "8" "300"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  GetDlgItem $R1 $R0 1201 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "12" "700"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  GetDlgItem $R1 $R0 1202 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "8" "300"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  GetDlgItem $R1 $R0 1203 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "8" "300"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  GetDlgItem $R1 $R0 1204 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "8" "300"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  GetDlgItem $R1 $R0 1205 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "8" "300"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  GetDlgItem $R1 $R0 1206 ;1200 + Field number - 1
  ;$R1 contains the HWND of the first field
  CreateFont $R2 "Tahoma" "8" "300"
  SendMessage $R1 ${WM_SETFONT} $R2 0
      
  InstallOptions::show
  Pop $R0
  Pop $R2
  Pop $R1
  Pop $R0
FunctionEnd

!ifdef COMMON_FILES
Section -BatchFiles
 FileOpen $9 $INSTDIR\rtems-env.bat w
 !insertmacro FILE_WRITE_LINE $9 "@echo off"
 !insertmacro FILE_WRITE_LINE $9 "rem RTEMS batch file: ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 "set PATH=$INSTDIR\bin;c:\mingw\bin;c:\msys\1.0\bin;%PATH%"
 !insertmacro FILE_WRITE_LINE $9 "set PROMPT=RTEMS $$P$$G"
 !insertmacro FILE_WRITE_LINE $9 "If $\"x%OS%x$\" == $\"xWindows_NTx$\" Goto WinNT_Title"
 !insertmacro FILE_WRITE_LINE $9 "doskey > Nul"
 !insertmacro FILE_WRITE_LINE $9 "goto Finished"
 !insertmacro FILE_WRITE_LINE $9 ":WinNT_Title"
 !insertmacro FILE_WRITE_LINE $9 "Title RTEMS ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 ":Finished"
 FileClose $9

 FileOpen $9 $INSTDIR\rtems.bat w
 !insertmacro FILE_WRITE_LINE $9 "@echo off"
 !insertmacro FILE_WRITE_LINE $9 "rem RTEMS batch file: ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 "If $\"x%OS%x$\" == $\"xWindows_NTx$\" Goto WinNT"
 !insertmacro FILE_WRITE_LINE $9 "start command.com /e:4096 /k $INSTDIR\rtems-env.bat %1 %2 %3 %4"
 !insertmacro FILE_WRITE_LINE $9 "exit"
 !insertmacro FILE_WRITE_LINE $9 ":WinNT"
 !insertmacro FILE_WRITE_LINE $9 "start cmd.exe /k $INSTDIR\rtems-env.bat %1 %2 %3 %4"
 !insertmacro FILE_WRITE_LINE $9 "exit"
 FileClose $9

 FileOpen $9 $INSTDIR\sh-run.bat w
 !insertmacro FILE_WRITE_LINE $9 "@echo off"
 !insertmacro FILE_WRITE_LINE $9 "rem RTEMS batch file: ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 "rem We can only handle 9 parameters. More is too hard."
 !insertmacro FILE_WRITE_LINE $9 "call $INSTDIR\rtems-env.bat"
 !insertmacro FILE_WRITE_LINE $9 "%1 %2 %3 %4 %5 %6 %7 %8 %9"
 FileClose $9

 FileOpen $9 $INSTDIR\vs-make.sh w
 !insertmacro FILE_WRITE_LINE $9 "#! /bin/sh"
 !insertmacro FILE_WRITE_LINE $9 "# RTEMS script: ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 "if [ ! -d $$1 ]; then"
 !insertmacro FILE_WRITE_LINE $9 " echo $\"error: no build directory found$\""
 !insertmacro FILE_WRITE_LINE $9 " exit 1"
 !insertmacro FILE_WRITE_LINE $9 "fi"
 !insertmacro FILE_WRITE_LINE $9 "cd $$1"
 !insertmacro FILE_WRITE_LINE $9 "shift"
 !insertmacro FILE_WRITE_LINE $9 "make $$* 2>&1 | sed -e 's/c:\([0-9]*\):/c(\1):/' \\"
 !insertmacro FILE_WRITE_LINE $9 "                   -e 's/h:\([0-9]*\):/h(\1):/' \\"
 !insertmacro FILE_WRITE_LINE $9 "                   -e 's/S:\([0-9]*\):/S(\1):/' \\"
 !insertmacro FILE_WRITE_LINE $9 "                   -e 's/s:\([0-9]*\):/s(\1):/'"
 FileClose $9
SectionEnd
!endif

Section -Post
!ifdef COMMON_FILES
 StrCpy $R0 "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-uninst.exe"
 StrCpy $R1 "RTEMS Tools.lnk"
!else
 StrCpy $R0 "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-${RTEMS_TARGET}-uninst.exe"
 StrCpy $R1 "RTEMS ${RTEMS_TARGET} Tools.lnk"
 !insertmacro MUI_STARTMENU_GETFOLDER "Application" $smfolder
!endif
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
 !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
 CreateDirectory "$SMPROGRAMS\$smfolder"
!ifdef COMMON_FILES
 SetOutPath $INSTDIR
 WriteIniStr "$INSTDIR\RTEMS.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
 WriteIniStr "$INSTDIR\RTEMS-Wiki.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}wiki"
 WriteIniStr "$INSTDIR\RTEMS-Documentation.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}onlinedocs/releases/rtemsdocs-4.7.99.2/share/rtems/html/"
 WriteIniStr "$INSTDIR\RTEMS-Support.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}support.html"
 CreateShortCut "$SMPROGRAMS\$smfolder\RTEMS Tools.lnk" "$INSTDIR\rtems.bat"
 CreateShortCut "$SMPROGRAMS\$smfolder\RTEMS Website.lnk" "$INSTDIR\RTEMS.url"
 CreateShortCut "$SMPROGRAMS\$smfolder\RTEMS Wiki.lnk" "$INSTDIR\RTEMS-Wiki.url"
 CreateShortCut "$SMPROGRAMS\$smfolder\RTEMS Documentation.lnk" "$INSTDIR\RTEMS-Documentation.url"
 CreateShortCut "$SMPROGRAMS\$smfolder\RTEMS Support.lnk" "$INSTDIR\RTEMS-Support.url"
!endif
 CreateDirectory "$SMPROGRAMS\$smfolder\Uninstall"
 CreateShortCut "$SMPROGRAMS\$smfolder\Uninstall\$R1" "$R0"
 !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Function un.onInit
 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
            "Are you sure you want to uninstall ${PRODUCT_TITLE} ?" IDYES +2
 Abort
FunctionEnd

Section Uninstall
 SetDetailsView show
 StrCpy $2 0
!ifdef COMMON_FILES
 FindFirst $0 $1 "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-*-uninst.exe"
 Uninstall_Targets:
  StrCmp $1 "" Uninstall_Targets_Done
  IntCmp $2 0 0 +3
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
             "You have tools installed. Are you sure you want to uninstall all RTEMS Tools ?" IDYES +2
    Abort
  StrCpy $2 1
  DetailPrint "Uninstalling $1"
  ExecWait '"$INSTDIR\$1" $SilentOption $DebugOption'
  Delete $1
  BringToFront
  FindNext $0 $1
  Goto Uninstall_Targets
 Uninstall_Targets_Done:
 FindClose $0
!endif
 DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
 DetailPrint "Delete the installed files"
 !insertmacro RTEMS_DELETE_FILES
 !insertmacro MUI_STARTMENU_GETFOLDER "Application" $smfolder
!ifdef COMMON_FILES
 Delete "$INSTDIR\AUTHORS"
 Delete "$INSTDIR\COPYING"
 Delete "$INSTDIR\README"
 Delete "$INSTDIR\rtems.bat"
 Delete "$INSTDIR\rtems-env.bat"
 Delete "$INSTDIR\sh-run.bat"
 Delete "$INSTDIR\vs-make.sh"
 Delete "$INSTDIR\RTEMS.url"
 Delete "$INSTDIR\RTEMS-Wiki.url"
 Delete "$INSTDIR\RTEMS-Documentation.url"
 Delete "$INSTDIR\RTEMS-Support.url"
 RMDir /r "$SMPROGRAMS\$smfolder"
 Delete "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-uninst.exe"
!else
 Delete "$SMPROGRAMS\$smfolder\Uninstall\RTEMS ${RTEMS_TARGET} Tools.lnk"
 Delete "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-${RTEMS_TARGET}-uninst.exe"
!endif
 RMDir "$INSTDIR"
 DetailPrint "All done."
!ifdef COMMON_FILES
 SetAutoClose false
!else
 SetAutoClose true
!endif
SectionEnd
