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
    echo "`dirname $casedir`/out/$casename/$casename.log"
  done | xargs tail -n0 -q --retry --follow=name 
}

tailcases 2> /dev/null


