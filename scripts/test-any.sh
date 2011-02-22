#!/bin/bash

# This script is the main test runner for regression tests.
# It expects postscript files as input and will compare processed files
# with manually accepted results to catch regressions.
#
# Usage variants:
# ec test-any            - Runs all tests
# ec test-any <dir>      - Runs all tests in the given dir
# ec test-any <file.ps>  - Runs the specified test

. $CC_FUNCTIONS

pad()
{
  echo -n $1
  s=$(printf "%$(($2-${#1}))s"); echo -n "${s// /.}"
}

# Usage: runtest <testfile> <testtype>
# Example: runtest test-data/corel/quad.ps corel
runtest()
{
  testtype=$2
  testcase=`basename $1 .ps`
  srcdir=`dirname $1`
  prnfile=$srcdir/$testcase.prn
  pad "*$testcase" 22
  outfile=$srcdir/$testcase.raw

  # Generate a PCL/RTL file using our filter
  scripts/run-filter.sh $1 > $outfile 2> $testcase.log
  if [ $? != 0 ]; then
    error "filter failed with return code $?"
    return
  fi

  if [ -f $prnfile ]; then 
      has_prnfile=1
  else 
      has_prnfile=0
  fi

  # Binary compare with the validated output (e.g. from the Windows drivers)
  binary_ok=0
  if [ $has_prnfile == 1 ]; then
    diff -a $prnfile $outfile >> $testcase.log
    if [ $? == 0 ]; then
      binary_ok=1
    fi
  fi

  if [ $binary_ok == 1 ]; then
    echo -n OK
  else
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    scripts/prn-to-pbm.sh $outfile 2>> $testcase.log
    if [ $? -ne 0 ]; then
      errorstr="Err"
      rawtopbmfailed=1
    fi

    if [ $has_prnfile == 0 ]; then
      echo "No prn file found. Generating output.."
      return
    fi

    pad "no" 5
    scripts/prn-to-pbm.sh $prnfile 2>> $testcase.log
    if [ $? -ne 0 ]; then
      errorstr="Err"
    fi
    if [ -z $errorstr ]; then
      errorstr=`scripts/compare-bitmaps.sh $srcdir/$testcase.prn.pbm $outfile.pbm`
      if [ $? == 0 ]; then
        pixelstr="OK"
      else
        pixelstr=`echo $errorstr | awk '{ print $3 }'`
      fi
    else 
      pixelstr=$errorstr
    fi
    pad "$pixelstr" 7

    # Compare bboxes, number of polylines and total cut length
    rtlcompare=`python/rtlinfo.py $prnfile $outfile`

    bbox_diff=`echo $rtlcompare | awk '{print $4}'`
    if [ $bbox_diff != "0" ]; then
        bboxstr="Err"
    else
        bboxstr="OK"
    fi    
    pad $bboxstr 6

    polyline_diff=`echo $rtlcompare | awk '{ print $1 }'`
    if [ $polyline_diff != "0" ]; then
        plstr=$polyline_diff
    else
        plstr="OK"
    fi
    pad $plstr 14

    length_diff=`echo $rtlcompare | awk '{print $2}'`
    if [ $length_diff != "0" ]; then
        lenstr="$length_diff"
    else
        lenstr="OK"
    fi
    echo -n $lenstr
  fi
  echo
}

rundir()
{
  for testdir in $@
  do
    testtype=`basename $testdir`
    echo "[$testtype]"
    for f in $testdir/*.ps; do
      runtest $f $testtype
    done
  done
}

printUsage()
{
  echo "Usage: $0 [-v] [<testcase>]"
  echo "Options:"
  echo "  -v        Verbose"
}

shift $(($OPTIND - 1))

if [ $# -gt 1 ]; then
  printUsage
  exit 1
fi

if test $VERBOSE; then
  set -x
fi

#Print header
pad "Test case" 22
pad "bin" 5
pad "img" 7
pad "bbox" 6
pad "polylines" 14
echo -n "length"
echo

# Run given test or all tests
if [ $# == 1 ]; then
  if [ -d $1 ]; then
    rundir $1
  else
    runtest $1 `basename $(dirname $1)`
  fi
else
 testdirs="corel qcad inkscape"
 testpaths=""
 for testdir in $testdirs
 do
   testpaths="$testpaths $CC_TEST_DATA/$testdir"
 done
 rundir $testpaths
fi
