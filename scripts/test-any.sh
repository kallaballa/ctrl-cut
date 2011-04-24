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

# Makes unmatched globs return a zero list instead of literal glob characters
shopt -s nullglob

pad()
{
  if [ $# -ge 3 ]; then
      $3 $1
  else
    echo -n $1
  fi
  s=$(printf "%$(($2-${#1}))s"); echo -n "${s// /.}"
}

# Usage: runtest <testfile> <testtype>
# Example: runtest test-data/corel/quad.ps corel
runtest()
{
  testtype=$2
  srcdir=`dirname $1`
  filename=`basename $1`
  testcase=${filename%.*} # basename without suffix
  prnfile=$srcdir/$testcase.prn
  pad "$testcase" 22
  outfile=$srcdir/$testcase.raw

  # Generate a PCL/RTL file using our filter
  scripts/run-filter.sh $VERBOSE $XML $1 > $outfile 2> $testcase.log
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
    cmp $prnfile $outfile >> $testcase.log
    if [ $? == 0 ]; then
      binary_ok=1
    fi
  fi

  if [ $binary_ok == 1 ]; then
    echo -n OK
  else
    pad "no" 5

    color=red
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    rtlcompare=`scripts/rtlcompare.sh $prnfile $outfile 2>> $testcase.log`
    if [ $? -ne 0 -o ! -f $outfile-v.png ]; then
      errorstr="Err"
      rawtopbmfailed=1
    fi

    if [ $has_prnfile == 0 ]; then
      echo "No prn file found. Generating output.."
      return
    fi

    if [ $? -ne 0 ]; then
      errorstr="Err"
      color=red
    elif [ ! -f $prnfile-v.png ]; then
      errorstr="N/A"
      color=green
    fi
    if [ -z $errorstr ]; then
      errorstr=`scripts/compare-bitmaps.sh $srcdir/$testcase.prn-v.png $outfile-v.png`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr=`echo $errorstr | awk '{ print $3 }'`
        color=red
      fi
    else 
      pixelstr=$errorstr
    fi
    pad "$pixelstr" 7 $color

    

    if [ ! -f $prnfile-r.png ]; then
      pixelstr="N/A"
      color=green
    else
      errorstr=`scripts/compare-raster.sh $srcdir/$testcase.prn-r.png $outfile-r.png`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr=`echo $errorstr | awk '{ print $3 }'`
        color=red
      fi
    fi
    pad "$pixelstr" 20 $color

    # Compare bboxes, number of polylines and total cut length
    color=red
    bbox_diff=`echo $rtlcompare | awk '{print $5}'`
    if [ $bbox_diff != "0" ]; then
        bboxstr="Err"
    else
        bboxstr="OK"
        color=green
    fi    
    pad $bboxstr 6 $color

    color=red
    polyline_diff=`echo $rtlcompare | awk '{ print $1 }'`
    if [ $polyline_diff != "0" ]; then
        plstr=$polyline_diff
    else
        plstr="OK"
        color=green
    fi
    pad $plstr 14 $color

    color=red
    length_diff=`echo $rtlcompare | awk '{print $2}'`
    if [ $length_diff != "0" ]; then
        lenstr="$length_diff"
    else
        lenstr="OK"
        color=green
    fi
    pad $lenstr 10 $color

    color=red
    move_diff=`echo $rtlcompare | awk '{print $3}'`
    if [ $move_diff != "0" ]; then
        lenstr="$move_diff"
    else
        lenstr="OK"
        color=green
    fi
    pad $lenstr 10 $color
  fi

  if [ x$XML_TEST != "x" ]; then
    color=red
    $CC_SCRIPTS/xml-test-filter.sh $srcdir/$testcase-*.xml >> $testcase.log
    xmlstatus="$?"
    if [ $xmlstatus != "0" ]; then
        xmlstr="Err"
    else
        xmlstr="OK"
        color=green
    fi
    pad $xmlstr 2 $color
  fi

  echo
}

rundir()
{
  for testdir in $@
  do
    testtype=`basename $testdir`
    echo "[$testtype]"
    for f in $testdir/*.ps $testdir/*.vector; do
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

while getopts 'xy' c
do
    case $c in
        x) 
            XML=-x
            XML_TEST=true
            ;;
        y)
            XML=-x
            XML_TEST=
            ;;
        \?) 
            echo "Invalid option: -$OPTARG" >&2
            ;;
    esac
done

shift $(($OPTIND - 1))

#if [ $# -gt 1 ]; then
#  printUsage
#  exit 1
#fi

if test $VERBOSE; then
  set -x
fi

#Print header
pad "Test case" 22
pad "bin" 5
pad "vimg" 7
pad "rimg" 20
pad "bbox" 6
pad "polylines" 14
pad "length" 10
pad "move" 10
if [ x$XML_TEST != "x" ]; then pad "XML" 3; fi
echo

# Run given test or all tests
if [ $# -gt 0 ]; then
  for case in $@; do
    if [ -d $case ]; then
      rundir $case
    else
      runtest $case `basename $(dirname $case)`
    fi
  done
else
 testdirs="corel-v corel-r corel-b qcad inkscape"
 testpaths=""
 for testdir in $testdirs
 do
   testpaths="$testpaths $CC_TEST_DATA/$testdir"
 done
 rundir $testpaths
fi
