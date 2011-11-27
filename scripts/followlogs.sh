#!/bin/bash

cd $CC_BASE

function printUsage() {
  echo "find and live print changes to test case log files"
  echo
  echo "followlogs <searchpath>"
}

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"

function tailcases {
  findcases $dir | while read casedir; do
    casename="`basename $casedir`"
    casedir="`dirname $casedir`/$casename"
    casepath="$casedir/$casename.log"
    mkdir -p "$casedir"
    touch "$casepath"
    echo "$casepath"
  done | xargs tail -n0 -F | egrep -v "^(==> .* <==)?$"
}

tailcases
