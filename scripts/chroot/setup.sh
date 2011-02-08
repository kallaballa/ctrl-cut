#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

while getopts 'c' c
do
        case $c in
            c) CLEAR=-c; shift;;
            ?) printUsage; exit 1 ;;
        esac
done

BUILD_DIR=$1

[ $CLEAR ] && try "Clear build directory" "rm -r $BUILD_DIR"
if [ -d $BUILD_DIR ]; then 
    try "Pull" "git --git-dir=$BUILD_DIR pull"
else
    try "Clone" "git --git-dir=$BUILD_DIR clone $CC_GIT_URL"
fi

cd /tmp
try "Download pysvg" "wget $CC_PYSVG_URL"
try "Unpack" "unzip pysvg-*.zip"
cd pysvg-*
try "Install" "python setup.py install"
try "Clean up" "rm -r pysvg-*"

cd $BUILD_DIR
try "Generate Makefile" "qmake -recursive"
try "Build" "make"
try "Installing printer" "./cc install $CC_PRINTERNAME $CC_PPD_DIR/$CC_PRINTERPPD dump://"

