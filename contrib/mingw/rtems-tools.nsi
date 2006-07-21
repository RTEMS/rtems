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
!define PRODUCT_PUBLISHER "RTEM Project Team"
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
; Components page
; Details of what will happen.
Page custom RTEMSMessage
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
OutFile "${RTEMS_BINARY}/rtems${PRODUCT_VERSION}-${RTEMS_TARGET}-${RTEMS_VERSION}.exe"
InstallDir "C:\rtems"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "RTEMS ${RTEMS_TARGET} Tools v${PRODUCT_VERSION}"
AllowRootDirInstall false
AutoCloseWindow false
CRCCheck force

Section "RTEMS ${RTEMS_TARGET} Tools" SecTools
 !insertmacro RTEMS_INSTALL_FILES
SectionEnd

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
    CreateFont $R2 "Tahoma" "9" "550"
    SendMessage $R1 ${WM_SETFONT} $R2 0
	
    GetDlgItem $R1 $R0 1203 ;1200 + Field number - 1
    ;$R1 contains the HWND of the first field
    CreateFont $R2 "Tahoma" "9" "550"
    SendMessage $R1 ${WM_SETFONT} $R2 0
	
    GetDlgItem $R1 $R0 1204 ;1200 + Field number - 1
    ;$R1 contains the HWND of the first field
    CreateFont $R2 "Tahoma" "9" "550"
    SendMessage $R1 ${WM_SETFONT} $R2 0
	
    GetDlgItem $R1 $R0 1205 ;1200 + Field number - 1
    ;$R1 contains the HWND of the first field
    CreateFont $R2 "Tahoma" "9" "550"
    SendMessage $R1 ${WM_SETFONT} $R2 0
	
    InstallOptions::show
    Pop $R0

  Pop $R2
  Pop $R1
  Pop $R0

FunctionEnd

Section -Post
 WriteUninstaller "$INSTDIR\uninst.exe"
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
            "Are you sure you want to uninstall RTEMS Tools?" IDYES +2
 Abort
FunctionEnd

Section Uninstall
 Delete $INSTDIR\Uninst.exe
 DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
 DetailPrint "Delete the install directory"
 !insertmacro RTEMS_DELETE_FILES
 DetailPrint "All done."
 SetAutoClose true
SectionEnd
