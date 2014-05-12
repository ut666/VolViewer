; VolViewerInstaller.nsi
;
; Nullsoft Scriptable Install System
;
; Custom script for the VolViewer Application
; author: Jerome Avondo
;--------------------------------

; The name of the installer
Name "VolViewer - 32bit"

; The file to write
OutFile "VolViewerInstaller_x86.exe"

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
File 32bitInstall\VolViewer.exe
File 32bitInstall\*.dll
File 32bitInstall\settings.ini
SetOutPath $INSTDIR\imageformats
File 32bitInstall\imageformats\*.*
SetOutPath $INSTDIR\Icons
File 32bitInstall\Icons\*.*
SetOutPath $INSTDIR\Shaders
File 32bitInstall\Shaders\*.*
SetOutPath $INSTDIR\Kernels
File 32bitInstall\Kernels\*.*
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
Delete $INSTDIR\Kernels\*.*
Delete $INSTDIR\imageformats\*.*
RMDir $INSTDIR\Icons
RMDir $INSTDIR\Shaders
RMDir $INSTDIR\Kernels
RMDir $INSTDIR\imageformats
RMDir $INSTDIR

;Now remove shortcuts too
Delete "$SMPROGRAMS\VolViewer\Uninstall.lnk"
Delete "$SMPROGRAMS\VolViewer\Edit Settings.lnk"
Delete "$SMPROGRAMS\VolViewer\VolViewer.lnk"
RMDir "$SMPROGRAMS\VolViewer"
Delete "$DESKTOP\VolViewer.lnk" 

SectionEnd 