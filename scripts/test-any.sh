#!/bin/bash

# This script is the main test runner for regression tests.
# It expects postscript files as input and will compare processed files
# with manually accepted results to catch regressions.
#
# Usage variants:
# ec test-any            - Runs all tests
# ec test-any <dir>      - Runs all tests in the given dir
# ec test-any <file.ps>  - Runs the specified test

# Makes unmatched globs return a zero list instead of literal glob characters

COL_I=0
COLUMNS=( "Test" "bin" "vimg" "rimg" "bbox" "polylines" "length" "move" )
PADDING=(    15      5      7     20      6          14       10     10 )

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

printHeader() {
  colcnt=${#COLUMNS[@]}
  padcnt=${#PADDING[@]}
  [ $colcnt -ne $padcnt ] && error "column/padding count mismatch"

  for i in `seq 0 $[$colcnt - 1]`; do
    printCol "${COLUMNS[$i]}"
  done
  echo
}

printCol() {
  [ $COL_I -ge ${#COLUMNS[@]} ]\
    && COL_I=0\
    || COL_I=$[ $COL_I + 1 ];
  pad "$1" ${PADDING[$I_COL]} $2
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
  printCol "$testcase"
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
    printCol "no"
 
    if [ $has_prnfile == 0 ]; then
      printCol "No prn file found. Generating output.."
    else

    color=red
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    rtlcompare=`scripts/rtlcompare.sh $VERBOSE $prnfile $outfile 2>> $testcase.log`
    if [ $? -ne 0 -o ! -f $outfile-v.png ]; then
      errorstr="Err"
      rawtopbmfailed=1
    fi

    if [ $? -ne 0 ]; then
      errorstr="Err"
      color=red
    elif [ ! -f $prnfile-v.png ]; then
      errorstr="N/A"
      color=green
    fi
    if [ -z $errorstr ]; then
      errorstr=`scripts/compare-bitmaps.sh $VERBOSE $srcdir/$testcase.prn-v.png $outfile-v.png 2>> $testcase.log`
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
    printCol "$pixelstr" $color


    if [ ! -f $prnfile-r.png ]; then
      pixelstr="N/A"
      color=green
    else
      errorstr=`scripts/compare-raster.sh $VERBOSE $srcdir/$testcase.prn-r.png $outfile-r.png 2>> $testcase.log`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr=`echo $errorstr | awk '{ print $3 }'`
        color=red
      fi
    fi
    printCol "$pixelstr" $color

    # Compare bboxes, number of polylines and total cut length
    color=red
    bbox_diff=`echo $rtlcompare | awk '{print $5}'`
    if [ $bbox_diff != "0" ]; then
        bboxstr="Err"
    else
        bboxstr="OK"
        color=green
    fi    
    printCol $bboxstr $color

    color=red
    polyline_diff=`echo $rtlcompare | awk '{ print $1 }'`
    if [ $polyline_diff != "0" ]; then
        plstr=$polyline_diff
    else
        plstr="OK"
        color=green
    fi
    printCol $plstr $color

    color=red
    length_diff=`echo $rtlcompare | awk '{print $2}'`
    if [ $length_diff != "0" ]; then
        lenstr="$length_diff"
    else
        lenstr="OK"
        color=green
    fi
    printCol $lenstr $color

    color=red
    move_diff=`echo $rtlcompare | awk '{print $3}'`
    if [ $move_diff != "0" ]; then
        lenstr="$move_diff"
    else
        lenstr="OK"
        color=green
    fi
    printCol $lenstr $color
  fi

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
    printCol $xmlstr $color
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

while getopts 'v' c
do
    case $c in
        v)
            VERBOSEL=-v
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

printHeader
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
