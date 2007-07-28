;
; $Id$
;
; RTEMS Tools Installer.
;
; Copyright Chris Johns (chrisj@rtems.org)
;

;!define EMPTY_INSTALLER

!define PRODUCT_NAME      "RTEMS Tools"
!define PRODUCT_VERSION   ${RTEMS_VERSION}
!define PRODUCT_PUBLISHER "RTEMS Project Team"
!define PRODUCT_WEB_SITE  "http://www.rtems.org/"

; MUI 1.66 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${RTEMS_LOGO}"

!define MUI_COMPONENTSPAGE_SMALLDESC

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Details of what will happen.
Page custom RTEMSMessage
; License page
;!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "${RTEMS_LICENSE_FILE}"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

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
!ifdef COMMON_FILES
OutFile "${RTEMS_BINARY}/rtems${PRODUCT_VERSION}-tools-${RTEMS_BUILD_VERSION}.exe"
!else
OutFile "${RTEMS_BINARY}/${RTEMS_OUTFILE}"
!endif
InstallDir "C:\opt\rtems-${PRODUCT_VERSION}"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "RTEMS ${RTEMS_TARGET} Tools v${PRODUCT_VERSION}"
AllowRootDirInstall false
AutoCloseWindow false
CRCCheck force

!include "${RTEMS_SOURCE}/msys-path.nsi"

Section -SecFiles
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

!macro FILE_WRITE_LINE Handle Text
  FileWrite     ${Handle} `${Text}`
  FileWriteByte ${Handle} "13"
  FileWriteByte ${Handle} "10"
!macroend

Function .onInit
  ;Extract InstallOptions INI files
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "rtems.ini"
FunctionEnd

Function RTEMSMessage

  !insertmacro MUI_HEADER_TEXT \
               "RTEMS Tools (${RTEMS_TARGET})" \
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

; Push $filenamestring (e.g. 'c:\this\and\that\filename.htm')
; Push '\\'
; Pop $R0
; Call StrSlash
; Pop $R0
; ;Now $R0 contains 'c:/this/and/that/filename.htm'
Function StrSlash
 Exch $R0
 Push $R1
 Push $R2
 StrCpy $R1 0
loop:
  IntOp $R1 $R1 - 1
  StrCpy $R2 $R0 1 $R1
  StrCmp $R2 "" done
 StrCmp $R2 "\" 0 loop  ; "
  StrCpy $R2 $R0 $R1
   Push $R1
  IntOp $R1 $R1 + 1
  StrCpy $R1 $R0 "" $R1
 StrCpy $R0 "$R2/$R1"
   Pop $R1
  IntOp $R1 $R1 - 1
 Goto loop
done:
 Pop $R2
 Pop $R1
 Exch $R0
FunctionEnd

!ifdef COMMON_FILES
Section -BatchFiles
 FileOpen $9 $INSTDIR\rtems.bat w
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

 FileOpen $9 $INSTDIR\rtems-cmd.bat w
 !insertmacro FILE_WRITE_LINE $9 "@echo off"
 !insertmacro FILE_WRITE_LINE $9 "rem RTEMS batch file: ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 "If $\"x%OS%x$\" == $\"xWindows_NTx$\" Goto WinNT"
 !insertmacro FILE_WRITE_LINE $9 "start command.com /e:4096 /k $INSTDIR\rtems.bat %1 %2 %3 %4"
 !insertmacro FILE_WRITE_LINE $9 "exit"
 !insertmacro FILE_WRITE_LINE $9 ":WinNT"
 !insertmacro FILE_WRITE_LINE $9 "start cmd.exe /k $INSTDIR\rtems.bat %1 %2 %3 %4"
 !insertmacro FILE_WRITE_LINE $9 "exit"
 FileClose $9

 FileOpen $9 $INSTDIR\sh-run.bat w
 !insertmacro FILE_WRITE_LINE $9 "@echo off"
 !insertmacro FILE_WRITE_LINE $9 "rem RTEMS batch file: ${RTEMS_VERSION}-${RTEMS_BUILD_VERSION}"
 !insertmacro FILE_WRITE_LINE $9 "rem We can only handle 9 parameters. More is too hard."
 !insertmacro FILE_WRITE_LINE $9 "call $INSTDIR\rtems.bat"
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

Section -MSYSLinks
 Call MSYSDetectSilent
 Pop $R0
 ifFileExists "$R0\etc\fstab" 0 Done
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
    MessageBox MB_OK "Cannot open $R0\etc\fstab. MSYS mount point no added."
    Goto Close
  FileSeek $9 0 SET
 ReadLoop:
  FileRead $9 $R2
  ifErrors Append
  StrCmp $R1 $R2 Close ReadLoop
 Append:
  FileSeek $9 0 END
  FileWrite $9 $R1
 Close:
  FileClose $9
 Done:
  ClearErrors
SectionEnd
!endif

Section -Post
!ifdef COMMON_FILES
 WriteUninstaller "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-uninst.exe"
!else
 WriteUninstaller "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-${RTEMS_TARGET}-uninst.exe"
!endif
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                  "DisplayName" "$(^Name)"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                  "UninstallString" "$INSTDIR\uninst.exe"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                  "DisplayVersion" "${PRODUCT_VERSION}"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                  "URLInfoAbout" "${PRODUCT_WEB_SITE}"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                  "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onInit
 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
            "Are you sure you want to uninstall RTEMS Tools (${RTEMS_TARGET})?" IDYES +2
 Abort
FunctionEnd

Section Uninstall
 SetDetailsView show
 DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
 DetailPrint "Delete the installed files"
 !insertmacro RTEMS_DELETE_FILES
!ifdef COMMON_FILES
 FindFirst $0 $1 "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-*-uninst.exe"
 Uninstall_Targets:
  StrCmp $1 "" Uninstall_Targets_Done
  DetailPrint "Uninstalling $1"
  ExecWait '"$INSTDIR\$1" /S'
  Delete $1
  BringToFront
  FindNext $0 $1
  Goto Uninstall_Targets
 Uninstall_Targets_Done:
 FindClose $0
 Delete "$INSTDIR\AUTHORS"
 Delete "$INSTDIR\COPYING"
 Delete "$INSTDIR\README"
 Delete "$INSTDIR\rtems.bat"
 Delete "$INSTDIR\rtems-cmd.bat"
 Delete "$INSTDIR\sh-run.bat"
 Delete "$INSTDIR\vs-make.sh"
 Delete "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-uninst.exe"
!else
 Delete "$INSTDIR\rtems${PRODUCT_VERSION}-${RTEMS_BUILD_VERSION}-tools-${RTEMS_TARGET}-uninst.exe"
 Delete "$INSTDIR\Packages"
!endif
 RMDir "$INSTDIR"
 DetailPrint "All done."
 SetAutoClose false
SectionEnd
