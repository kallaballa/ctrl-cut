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

#killall all child processes on SIGINT
trap "kill 0" SIGINT

. $CC_FUNCTIONS
COL_I=0
COLUMNS=( "case" "bin" "vimg" "rimg" "bbox" "polylines" "length" "move" )
PADDING=( "20"   "5"    "7"   "20"    "6"        "14"     "10"   "10" )

LEVELS=( "quick" "normal" "more" "all" )

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

  COL_I=0
  for i in `seq 0 $[$colcnt - 1]`; do
    printCol "${COLUMNS[$i]}"
  done
  echo
}

printCol() {
  [ $COL_I -ge ${#COLUMNS[@]} ]\
    && COL_I=0;

  pad "$1" ${PADDING[${COL_I}]} $2
  COL_I=$[ $COL_I + 1 ]
}

readCases() {
 casesfile="$1"
 testlevel="$2"
 grep "^$testlevel " $casesfile | cut -d" " -f2-
}

# Usage: runtest <testfile> <testtype>
# Example: runtest test-data/corel/quad.ps corel
runtest()
{
  testdir="$1"
  testname="`basename $1`"
  testcase="$2"

  testtype="`dirname $testdir`"

  casedir="$testdir/$testcase"
  optionsfile="$casedir/.options"
  commonoptsfile="$testtype/common.options"
  psfile="$casedir/$testcase.ps"
  prnfile="$casedir/$testcase.prn"

  outdir="$testdir/out/$testcase"
  [ ! -d "$outdir" ]  && mkdir "$outdir"

  logfile="$outdir/$testcase.log"
  outfile="$casedir/$tescase.raw"

  printCol "$testcase"

  # Generate a PCL/RTL file using our filter
  scripts/run-filter.sh $psfile $optionsfile $commonoptsfile > $outfile 2> $logfile
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
    cmp $prnfile $outfile &>> $logfile
    if [ $? == 0 ]; then
      binary_ok=1
    fi
  fi

  if [ $binary_ok == 1 ]; then
    printCol "ok" green
  else
    printCol "no" red
 
    if [ $has_prnfile == 0 ]; then
      printCol "No prn file found. Generating output.."
    else

    color=red
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    rtlcompare=`scripts/rtlcompare.sh -o "$outdir" $VERBOSE $prnfile $outfile 2>> $logfile`

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
      errorstr=`scripts/compare-bitmaps.sh $VERBOSE $prnfile-v.png $outfile-v.png 2>> $logfile`
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
      errorstr=`scripts/compare-raster.sh $VERBOSE $prnfile-r.png $outfile-r.png 2>> $logfile`
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

  echo
}

printUsage()
{
  echo "Usage: $0 [-l] [<testcase>]"
  echo "Options:"
  echo "  -v        Verbose"
  echo "  -l        Test level"
}

TEST_LEVEL=1

while getopts 'vl:' c
do
    case $c in
        v) VERBOSE=-v;;
        l) TEST_LEVEL="$2";;
        \?) echo "Invalid option: -$OPTARG" >&2;;
    esac
done
shift $(($OPTIND - 1))
if test $VERBOSE; then
  set -x
fi

printHeader

# Run given test or all tests
if [ $# -gt 0 ]; then
  find $@ -name ".cases" | while read casefile; do
    testdir="`dirname $casefile`"
    cases="`readCases $casefile ${LEVELS[$TEST_LEVEL]}`"
    echo -n "["
    yellow "`dirname $testdir`/`basename $testdir`"
    echo "]"
    for c in $cases; do          
      runtest $testdir $c
    done
  done
fi
