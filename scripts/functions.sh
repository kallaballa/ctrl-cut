#!/bin/bash

function green { echo -ne "\033[32;1m$1\033[0m" 1>&2; tput sgr0 1>&2; }
function yellow { echo -ne "\033[33;1m$1\033[0m" 1>&2; tput sgr0 1>&2; }
function red { echo -ne "\033[31;1m$1\033[0m" 1>&2; tput sgr0 1>&2; }
function warn { red "$1\n"; }
function ok { green "ok\n"; }
function verbose { [ ! -z $VERBOSE ] && echo "$@ " 1>&2; }
function error { 
    msg=$1; 
    errcode=$2; 
    [ -z "$errcode" ] && errcode=1; 
    [ -z "$msg" ] && msg="failed"; 
    red "$msg\n"; exit $errcode; 
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

export -f green yellow red warn ok verbose error
