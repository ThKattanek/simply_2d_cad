#!/bin/bash

# script by thorsten kattanek
# example: crossbuild-win-releases.sh ~/mxe

# check of variable $1
if [ -n "$1" ]; then
    declare mxe_path=$1
    # check of exist path from variable $1
    if [ ! -d "$mxe_path" ]; then
        echo "MXE path does not exist: $1"
        exit 1
    fi
else
    echo "Please specify the MXE path (example: crossbuild-win-releases.sh ~/mxe)"
    exit 1
fi

export PATH=$mxe_path/usr/bin:$PATH

# 1. CMake Konfiguration
x86_64-w64-mingw32.static-cmake -B build_w64

# 2. Build ausführen mit allen CPU-Kernen
x86_64-w64-mingw32.static-cmake --build build_w64 -j$(nproc)

# 3. Binärdatei verkleinern (Debug-Symbole entfernen)
if [ -f "build_w64/simply_2d_cad.exe" ]; then
    x86_64-w64-mingw32.static-strip build_w64/simply_2d_cad.exe
elif [ -f "build_w64/bin/simply_2d_cad.exe" ]; then
    x86_64-w64-mingw32.static-strip build_w64/bin/simply_2d_cad.exe
fi

# 4. Packaging (Erzeugt ZIP und NSIS Setup-Exe)
x86_64-w64-mingw32.static-cmake --build build_w64 --target package
