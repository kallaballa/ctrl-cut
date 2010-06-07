#!/bin/bash

#set -x 

function pad {
  echo -n $1
  s=$(printf "%$(($2-${#1}))s"); echo -n "${s// /.}"
}

function runtest {
  testcase=`basename $1 .ps`
  corelfile=$srcdir/$testcase.prn
  pad "*$testcase" 22
  outfile=test-data/corel/$testcase.raw
  scripts/run-filter.sh $1 > $outfile 2> $testcase.log
  diff -a $srcdir/$testcase.prn $outfile >> $testcase.log
  if [ $? == 0 ]; then
    echo -n OK
  else
    pad "no" 5
    scripts/prn-to-pbm.sh $corelfile
    scripts/prn-to-pbm.sh $outfile
    errorstr=`scripts/compare-bitmaps.sh $srcdir/$testcase.prn.pbm $outfile.pbm`
    if [ $? == 0 ]; then
      pixelstr="OK"
    else
      pixelstr=`echo $errorstr | awk '{ print $3 }'`
    fi
    pad "$pixelstr" 7

    infoA=`python/rtlinfo.py $corelfile`
    plA=`echo $infoA | awk '{print $2}'`
    lenA=`echo $infoA | awk '{print $4}'`
    infoB=`python/rtlinfo.py $outfile`
    plB=`echo $infoB | awk '{print $2}'`
    lenB=`echo $infoB | awk '{print $4}'`

    if [ $plA != $plB ]; then
        plstr="$plA->$plB"
    else
        plstr="OK"
    fi
    pad $plstr 14
    if [ $lenA != $lenB ]; then
        lenstr="$lenA->$lenB"
    else
        lenstr="OK"
    fi
    echo -n $lenstr
  fi
  echo
}

if [ $# -gt 1 ]; then
  echo "Usage: $0 [ps-file]" 
  exit 1
fi

if [ $# == 1 ]; then
  srcdir=`dirname $1`
  runtest $1
else
  pad "Test case" 22
  pad "bin" 5
  pad "img" 7
  pad "polylines" 14
  echo -n "length"
  echo
  srcdir=test-data/corel
  for f in $srcdir/*.ps; do
    runtest $f
  done
fi
