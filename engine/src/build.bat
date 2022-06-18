@ECHO OFF

SET compilerFLags= -Od -nologo -Gm- -GR- -Oi -WX -W3 -wd4530 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -Zi
SET linkerFlags= -incremental:no User32.lib Gdi32.lib Winmm.lib opengl32.lib
SET defines= -D_DEBUG
SET includeGLADPath=..\libs\glad\include
SET includeGLPath=..\libs\GL\include
SET includeGLMPath=..\libs\glm

PUSHD ..\build
cl %compilerFLags% ..\src\win32_platform.cpp ..\libs\glad\src\glad.c -Fewin32_engine /I%includeGLMPath% /I%includeGLADPath% /I%includeGLPath% %defines% /link %linkerFlags% /subsystem:console
POPD
