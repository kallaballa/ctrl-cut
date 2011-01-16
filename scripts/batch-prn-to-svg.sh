#!/bin/bash

. $CC_FUNCTIONS

echo "Running tests"
srcdir=$CC_TEST_DATA/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  try "*$testcase...."  "$CC_PYTHON/rtltest.py $f"
done
