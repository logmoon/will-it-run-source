@echo off
setlocal

rem Set the paths for include and lib folders
set "includeFolder=include/"
set "libFolder=lib/windows/"

rem Check if include folder exists, if not, create it
if not exist "%includeFolder%" (
    mkdir "%includeFolder%"
    echo Created include folder.
)
rem Check if include folder doesn't have raylib.h, if not, copy the .h file
if not exist "%includeFolder%raylib.h" (
    copy "thirdparty\raylib-5.0\src\raylib.h" "%includeFolder%"
    echo Copied raylib.h.
)
if not exist "%includeFolder%raymath.h" (
    copy "thirdparty\raylib-5.0\src\raymath.h" "%includeFolder%"
    echo Copied raymath.h.
)
rem Check if include folder doesn't have cLDtk.h, if not, copy the .h file
if not exist "%includefolder%cldtk.h" (
    copy "thirdparty\cldtk\cldtk.h" "%includefolder%"
    echo copied cLDtk.h.
)
rem Check if include folder doesn't have parson.h, if not, copy the .h file
if not exist "%includefolder%parson.h" (
    copy "thirdparty\cldtk\parson.h" "%includefolder%"
    echo copied parson.h.
)

rem Check if lib folder exists, if not, create it
if not exist "%libFolder%" (
    mkdir "%libFolder%"
    echo Created lib folder.
)

rem Check if lib folder has libraylib, if not, compile raylib and copy the .a file
if not exist "%libFolder%libraylib.a" (
    cd thirdparty/raylib-5.0/src/
    make clean
    mingw32-make PLATFORM=PLATFORM_DESKTOP
    cd ../../..
    copy "thirdparty\raylib-5.0\src\libraylib.a" "%libFolder%"
    echo Compiled and copied libraylib.a.
)

rem Check if lib folder has libcLDtk, if not, compile cLDtk and copy the .a file
if not exist "%libFolder%libcLDtk.a" (
    cd thirdparty/cLDtk/
    gcc -c cLDtk.c parson.c -std=c99
    ar rvs libcLDtk.a cLDtk.o parson.o
    cd ../..
    copy "thirdparty\cLDtk\libcLDtk.a" "%libFolder%"
    echo Compiled and copied libcLDtk.a.
)

rem Copy resources folder
if not exist "build/windows/resources" (
    mkdir "build/windows/resources"
)
robocopy "resources" "build/windows/resources" /E /XO /FFT /Z /COPY:DAT /PURGE
echo Copied resources folder.


rem Build game
gcc src/global_context.c src/cutscene_state.c src/menu_state.c src/entity.c src/popup.c src/game_app.c src/string.c src/ide.c src/state_manager.c src/gameplay_state.c src/script.c src/main.c -o build/windows/game.exe -Wall -Wextra -I %includeFolder% -L %libFolder% %libFolder%libcLDtk.a -lraylib -lopengl32 -lgdi32 -lwinmm
echo Game built successfully.

rem If run command was passed, run the executable
if /i "%1"=="run" (
    cd build/windows
    .\game.exe
)
endlocal