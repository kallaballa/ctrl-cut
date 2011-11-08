#!/bin/bash

function green { echo -ne "\033[32;1m$1\033[0m" 1>&2; tput sgr0 1>&2; }
function yellow { echo -ne "\033[33;1m$1\033[0m" 1>&2; tput sgr0 1>&2; }
function red { echo -ne "\033[31;1m$1\033[0m" 1>&2; tput sgr0 1>&2; }
function warn { red "$1\n"; }
function ok { green "ok\n"; }
function failed { red "failed\n"; }
function verbose { [ ! -z $VERBOSE ] && echo "$@ " 1>&2; }
function error { 
    msg=$1; 
    errcode=$2; 
    [ -z "$errcode" ] && errcode=1; 
    [ -z "$msg" ] && failed || red "$msg\n"; 
    exit $errcode; 
}

function try {
  fatal=true
  targetfd=2
  OPTIND=0
  while getopts 'no:' c
  do
    case $c in
      n)fatal="false";;
      o)targetfd="$2";;
      \?)echo "Invalid option: -$OPTARG" >&2;;
    esac
  done
  shift $((OPTIND-1));
  errcode=0; 
  echo -n "$1... " 1>&2; 
  shift;

  verbose $@;
  bash -c "$@ 1>&$targetfd"
  errcode=$?; 
  
  if [ $errcode != 0 ]; then 
    [ "$fatal" != "true" ] && failed || error;
  else 
    ok; 
  fi
  return $errcode; 
}

function check {
  try -n "$1" "$2"
}

function checkcat {
  try -n -o "1" "$1" "$2"
}

function trycat { 
  try -o "1" "$1" "$2"
}

function findcases {
  find "$1" -name ".cases" -exec dirname '{}' \;
}

export -f verbose green yellow red warn ok failed  error try trycat check checkcat findcases
