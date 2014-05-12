; VolViewerInstaller.nsi
;
; Nullsoft Scriptable Install System
;
; Custom script for the VolViewer Application
; author: Jerome Avondo
;--------------------------------

; The name of the installer
Name "VolViewer"

; The file to write
OutFile "VolViewerInstaller.exe"

; The default installation directory
InstallDir $PROGRAMFILES\VolViewer

; The text to prompt the user to enter a directory
DirText "This will install the VolViewer application on your computer. Please choose a directory"

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

; Set output path to the installation directory.
; Put file there
SetOutPath $INSTDIR
File VolViewer\VolViewer.exe
File VolViewer\Qt3Support4.dll
File VolViewer\QtCore4.dll
File VolViewer\QtGui4.dll
File VolViewer\QtNetwork4.dll
File VolViewer\QtOpenGL4.dll
File VolViewer\QtScript4.dll
File VolViewer\QtScriptTools4.dll
File VolViewer\QtSql4.dll
File VolViewer\QtSvg4.dll
File VolViewer\QtXml4.dll
File VolViewer\QtXmlPatterns4.dll
File VolViewer\settings.ini
SetOutPath $INSTDIR\imageformats
File VolViewer\imageformats\*.*
SetOutPath $INSTDIR\Icons
File VolViewer\Icons\*.*
SetOutPath $INSTDIR\Shaders
File VolViewer\Shaders\*.*
SetOutPath $INSTDIR

; Now create shortcuts
CreateDirectory "$SMPROGRAMS\VolViewer"
CreateShortCut "$SMPROGRAMS\VolViewer\VolViewer.lnk" "$INSTDIR\VolViewer.exe"
CreateShortCut "$SMPROGRAMS\VolViewer\Edit Settings.lnk" "$INSTDIR\Settings.ini"
CreateShortCut "$SMPROGRAMS\VolViewer\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
CreateShortCut "$DESKTOP\VolViewer.lnk" "$INSTDIR\VolViewer.exe"

; Tell the compiler to write an uninstaller and to look for a "Uninstall" section
WriteUninstaller $INSTDIR\Uninstall.exe

SectionEnd ; end the section

; The uninstall section
Section "Uninstall"

; Delete the files and folders
Delete $INSTDIR\*.*
Delete $INSTDIR\Icons\*.*
Delete $INSTDIR\Shaders\*.*
Delete $INSTDIR\imageformats\*.*
RMDir $INSTDIR\Icons
RMDir $INSTDIR\Shaders
RMDir $INSTDIR\imageformats
RMDir $INSTDIR

;Now remove shortcuts too
Delete "$SMPROGRAMS\VolViewer\Uninstall.lnk"
Delete "$SMPROGRAMS\VolViewer\Edit Settings.lnk"
Delete "$SMPROGRAMS\VolViewer\VolViewer.lnk"
RMDir "$SMPROGRAMS\VolViewer"
Delete "$DESKTOP\VolViewer.lnk" 

SectionEnd 