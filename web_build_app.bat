@echo off
setlocal

rem Set the paths for include and lib folders
set "includeFolder=include/"
set "libFolder=lib/web/"


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
    make PLATFORM=PLATFORM_WEB -B
    cd ../../..
    copy "thirdparty\raylib-5.0\src\libraylib.a" "%libFolder%"
    echo Compiled and copied libraylib.a.
)
rem Check if lib folder has libcLDtk, if not, compile cLDtk and copy the .a file
if not exist "%libFolder%libcLDtk.a" (
    cd thirdparty/cLDtk/
    emcc -c cLDtk.c parson.c -std=c99
    emar rvs libcLDtk.a cLDtk.o parson.o
    cd ../..
    copy "thirdparty\cLDtk\libcLDtk.a" "%libFolder%"
    echo Compiled and copied libcLDtk.a.
)

rem Build app
emcc -o build/web/index.html src/global_context.c src/cutscene_state.c src/menu_state.c src/entity.c src/popup.c src/game_app.c src/string.c src/ide.c src/state_manager.c src/gameplay_state.c src/script.c src/main.c -Os -Wall %libFolder%libcLDtk.a %libFolder%libraylib.a -I%includeFolder% -L%libFolder% -s USE_GLFW=3 --shell-file thirdparty/raylib-5.0/src/minshell.html --preload-file resources -DPLATFORM_WEB
endlocal