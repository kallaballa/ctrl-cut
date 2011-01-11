#!/bin/bash

. $EC_FUNCTIONS

echo "Running tests"
srcdir=$EC_TEST_DATA/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  try "*$testcase...."  "$EC_PYTHON/rtltest.py $f"
done
