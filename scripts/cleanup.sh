#!/bin/bash

cd $CC_BASE

function printUsage() {
  echo "wipes test results"
  echo
  echo "cleanup <searchpath>"
  exit 1
}

[ $# -ne 1 ] && printUsage;

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"

findtests $dir | xargs  -I'[cc_test_dir]' bash -c "rm -rf [cc_test_dir]/*.png [cc_test_dir]/*.raw [cc_test_dir]/*.log [cc_test_dir]/*.info"

