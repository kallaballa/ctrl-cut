#!/bin/bash

echo "Running tests"
srcdir=$EC_TEST_DATA/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  try "*$testcase...." "$EC_SCRIPTS/prn-to-pdf.sh $f"
done
