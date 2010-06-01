#!/bin/bash

echo "Running tests"
srcdir=test-data/corel
for f in $srcdir/*.ps; do
  testcase=`basename $f .ps`
  echo -n "*$testcase...."
  outfile=test-data/corel/$testcase.raw
  scripts/run-filter.sh $f > $outfile 2> $testcase.log
  diff -a $srcdir/$testcase.prn $outfile >> $testcase.log
  if [ $? ]; then
    echo Failed
  else
    echo OK
  fi
done
