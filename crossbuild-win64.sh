#!/bin/bash

# script by thorsten kattanek
# excample: crossbuild-win-releases.sh ~/mxe

# check of variable $1
if [ $1 ]; then
    declare mxe_path=$1
    # check of exist path from variable $1
    if [ ! -d $mxe_path ]; then
        echo "Not exist the MXE path: " $1
        exit
    fi
else
    echo "Please specify the MXE path (excample: crossbuild-win-releases.sh ~/mxe)"
    exit
fi    

export PATH=$mxe_path/usr/bin:$PATH
x86_64-w64-mingw32.static-cmake -B build_w64
x86_64-w64-mingw32.static-cmake --build build_w64 -j${nproc}
strip build_w64/simply_2d_cad.exe
