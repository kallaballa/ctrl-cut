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

cd ..
[ $CLEAR ] && try "Clear build directory" "rm -r $CC_BASE"
if [ -d $CC_BASE ]; then 
    try "Pull ctrl-cut"         "git --git-dir=$CC_BASE pull"
else
    try "Clone" "git clone $CC_GIT_URL"
fi

cd /tmp
try "Download pysvg"        "wget $CC_PYSVG_URL"
try "Unpack"                "unzip pysvg-*.zip"
cd pysvg-*
try "Install"               "python setup.py install"
try "Clean up"              "rm -r pysvg-*"

cd $CC_BASE
try "Generate Makefile"         "qmake -recursive"
try "Build"                     "make"
try "Installing printer"        "./cc install $CC_PRINTERNAME $CC_PPD_DIR/$CC_PRINTERPPD dump://"

