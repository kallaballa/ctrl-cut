#!/bin/bash

#set -x 

function runtest {
  testcase=`basename $1 .ps`
  echo -n "*$testcase...."
  outfile=test-data/corel/$testcase.raw
  scripts/run-filter.sh $1 > $outfile 2> $testcase.log
  diff -a $srcdir/$testcase.prn $outfile >> $testcase.log
  if [ $? == 0 ]; then
    echo OK
  else
    echo -n "binaries differ, comparing vectors..."
    scripts/prn-to-pbm.sh $srcdir/$testcase.prn
    scripts/prn-to-pbm.sh $outfile
    errorstr=`scripts/compare-bitmaps.sh $srcdir/$testcase.prn.pbm $outfile.pbm`
    if [ $? == 0 ]; then
      echo OK
    else
      echo "Failed ($errorstr)"
    fi
  fi
}

if [ $# -gt 1 ]; then
  echo "Usage: $0 [ps-file]" 
  exit 1
fi

if [ $# == 1 ]; then
  srcdir=`dirname $1`
  runtest $1
else
  echo "Running tests"
  srcdir=test-data/corel
  for f in $srcdir/*.ps; do
    runtest $f
  done
fi
