#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

while getopts 'c' c
do
        case $c in
            p) CLEAR=-c; shift;;
            ?) printUsage; exit 1 ;;
        esac
done

if [ $CLEAR ]; then
    cd $CC_BASE/..

    try "Clear ctrl-cut"        "rm -r $CC_BASE"
    try "Clone"                 "git clone $CC_GIT_URL"
    cd /tmp
    try "Download pysvg"        "wget $CC_PYSVG_URL"
    try "Unpack"                "unzip pysvg-*.zip"
    cd pysvg-*
    try "Install"               "python setup.py install"

    cd $CC_BASE
else
    try "Pull ctrl-cut"         "git pull"
fi

try "Generate Makefile"         "qmake -recursive"
try "Build"                     "make"
try "Installing printer"        "./cc install $CC_PRINTERNAME $CC_PPD_DIR/$CC_PRINTERPPD dump://"

