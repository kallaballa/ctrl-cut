#!/bin/bash

function green { echo -e "\033[32;1m $1 \033[0m" 1>&2; tput sgr0 1>&2; }
function red { echo -e "\033[31;1m $1 \033[0m" 1>&2; tput sgr0 1>&2; }
function warn { red "$1"; }
function ok { green "ok"; }
function verbose { [ ! -z $VERBOSE ] && echo "$@ " 1>&2; }
function error { 
    msg=$1; 
    errcode=$2; 
    [ -z "$errcode" ] && errcode=1; 
    [ -z "$msg" ] && msg="failed"; 
    red "$msg"; exit $errcode; 
}


function try { 
    errcode=0; 
    echo -n "$1... " 1>&2; 
    shift;
    verbose $@; $@ 1>&2;
    errcode=$?; 
    if [ $errcode != 0 ]; then error; else ok; fi
    return $errcode; 
}

while getopts 'c' c
do
        case $c in
            c) CLEAR=-c; shift;;
            ?) printUsage; exit 1 ;;
        esac
done

BUILD_DIR=$1

[ "$CLEAR" == "-c" -a -d $BUILD_DIR ] && try "Clear build directory" "rm -r $BUILD_DIR"
if [ -d $BUILD_DIR ]; then 
    try "Pull" "git --git-dir=$BUILD_DIR pull"
else
    cd `dirname $BUILD_DIR`
    try "Clone" "git clone $CC_GIT_URL"
fi

cd /tmp
try "Download pysvg" "wget $CC_PYSVG_URL"
try "Unpack" "unzip pysvg-*.zip"
cd pysvg-*
try "Install" "python setup.py install"
try "Clean up" "rm -r /tmp/pysvg-*"

cd $BUILD_DIR
CC_BASE=$BUILD_DIR . ./cc
try "Generate Makefile" "qmake -recursive"
try "Build" "make"
try "Installing printer" "./cc install -p $CC_PRINTERNAME $CC_PPD_DIR/$CC_PRINTERPPD dump://"

