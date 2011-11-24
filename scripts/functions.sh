#!/bin/bash
function black { echo -ne "\033[0;30m$1\033[0m"; tput sgr0; }
function blue { echo -ne "\033[0;34m$1\033[0m"; tput sgr0; }
function green { echo -ne "\033[0;32m$1\033[0m"; tput sgr0; }
function cyan { echo -ne "\033[0;36m$1\033[0m"; tput sgr0; }
function red { echo -ne "\033[1;31m$1\033[0m"; tput sgr0; }
function purple { echo -ne "\033[0;35m$1\033[0m"; tput sgr0; }
function brown { echo -ne "\033[0;33m$1\033[0m"; tput sgr0; }
function lightgray { echo -ne "\033[0;37m$1\033[0m"; tput sgr0; }
function lightgreen { echo -ne "\033[1;32m$1\033[0m"; tput sgr0; }
function lightcyan { echo -ne "\033[1;36m$1\033[0m"; tput sgr0; }
function lightred { echo -ne "\033[1;31m$1\033[0m"; tput sgr0; }
function lightpurple { echo -ne "\033[1;35m$1\033[0m"; tput sgr0; }
function yellow { echo -ne "\033[1;33m$1\033[0m"; tput sgr0; }
function white { echo -ne "\033[1;37m$1\033[0m"; tput sgr0; }
function warn { red "$1\n"; }
function ok { green "ok\n"; }
function failed { red "failed\n"; }
function verbose { [ ! -z $CC_VERBOSE ] && echo "$@ " 1>&2; }
function verboseexec { verbose $@; bash -c "$@"; }
function error { 
    msg=$1; 
    errcode=$2; 
    [ -z "$errcode" ] && errcode=1; 
    [ -z "$msg" ] && failed || red "$msg\n"; 
    exit $errcode; 
}

function dotimeout {
  timeout=30
  OPTIND=0
  while getopts 't:' c
  do
    case $c in
      t)timeout="$OPTARG";;
      \?)echo "Invalid option: -$OPTARG" >&2;;
    esac
  done
  shift $((OPTIND-1));

  # $CC_SCRIPTS/timeout.sh -t$timeout "$1" "$2" "$3"
   "$1" "$2" "$3"
}

function try {
  fatal=true
  targetfd=2
  OPTIND=0
  while getopts 'no:' c
  do
    case $c in
      n)fatal="false";;
      o)targetfd="$OPTARG";;
      \?)echo "Invalid option: -$OPTARG" >&2;;
    esac
  done
  shift $((OPTIND-1));
  errcode=0; 
  msg="$1"
  yellow "$msg... " 1>&2; 
  shift;

  if echo "$CC_IGNORE_TEST" | grep "$msg"; then
      yellow "ignored\n" 1>&2;
      return 0
  fi

  verbose $@;

  bash -c "$@ 1>&$targetfd"
  errcode=$?; 
  if [ $errcode != 0 ]; then 
    [ "$fatal" != "true" ] && failed 1>&2 || error 1>&2;
  else 
    ok 1>&2; 
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

readCases() {
  casesfile="$1"
  testlevel="$2"
  grep "^$testlevel " $casesfile | cut -d" " -f2-
}

readIgnores() {
  [ -f "$1" ] && export CC_IGNORE_TEST="`cat \"$1\"`" || export CC_IGNORE_TEST="";
}

function findtests {
  find "$1" -name ".cases" -exec dirname '{}' \;
}

function findcases {
  searchpath="$1"
  level="$2"
  [ -z $level ] && level="all"

  findtests "$1" |  while read line; do
    for case in `readCases "$line/.cases" "$level"`; do echo $line/$case; done
  done | sed '/^$/d'
}

export -f verbose verboseexec green yellow red warn ok failed  error try trycat check checkcat findtests findcases readCases dotimeout
