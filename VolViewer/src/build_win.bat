@echo ==============================================================-
@echo (NB) Remember to edit the paths/variables by editing the file!
@echo ==============================================================-

Set QMAKESPEC=C:\Qt\Qt4.7.4_x86\mkspecs\win32-msvc2010
Set QMAKEPATH=C:\Qt\Qt4.7.4_x86\bin
Set NMAKEPATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin
Set ICE_HOME=C:\Ice-3.3.1-VC90\
Set OMERO_HOME=E:\SVN\-=OMERO_DEV=-\OMEROLIB 4.2.2\

Set PATH=%QMAKEPATH%;%NMAKEPATH%;%PATH%

qmake src.pro
nmake
pause