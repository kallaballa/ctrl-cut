#!/bin/bash

echo "Running tests"
srcdir=$CC_TEST_DATA/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  try "*$testcase...." "$CC_SCRIPTS/prn-to-pdf.sh $f"
done
