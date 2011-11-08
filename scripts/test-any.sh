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
REPORT=

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
  REPORT="$REPORT\n`blue \"${COLUMNS[${COL_I}]} $1\"`"
  COL_I=$[ $COL_I + 1 ]  
}

# Usage: runtest <testfile> <testtype>
# Example: runtest test-data/corel/quad.ps corel
runtest()
{
  REPORT=
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
  [ ! -d "$outdir" ]  && mkdir -p "$outdir"

  logfile="$outdir/$testcase.log"
  outfile="$outdir/$testcase.raw"
  prnv="$outdir/$testcase.prn-v.png"
  outv="$outdir/$testcase.raw-v.png"
  prnr="$outdir/$testcase.prn-r.png"
  outr="$outdir/$testcase.raw-r.png"

  green "### Commencing $casedir ###\n" &>> $logfile
  printCol "$testcase" 2>> $logfile

  # Generate a PCL/RTL file using our filter
  checkcat "run filter" "scripts/run-filter.sh $psfile $optionsfile $commonoptsfile"  > $outfile 2> $logfile
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
    check "compare binary" "cmp $prnfile $outfile" &>> $logfile
    if [ $? == 0 ]; then
      binary_ok=1
    fi
  fi

  if [ $binary_ok == 1 ]; then
    printCol "ok" green 2>> $logfile
  else
    printCol "no" red 2>> $logfile
 
    if [ $has_prnfile == 0 ]; then
      printCol "No prn file found. Generating output.." 2>> $logfile
    else

    color=red
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    rtlcompare=`checkcat "compare rtl/pcl" "scripts/rtlcompare.sh -o $outdir $VERBOSE $prnfile $outfile" 2>> $logfile`

    if [ $? -ne 0 -o ! -f $outv ]; then
      errorstr="Err"
      rawtopbmfailed=1
    fi

    if [ $? -ne 0 ]; then
      errorstr="Err"
      color=red
    elif [ ! -f $prnv ]; then
      errorstr="N/A"
      color=green
    fi
    if [ -z $errorstr ]; then
      errorstr=`checkcat "compare vector bitmaps" "scripts/compare-bitmaps.sh $VERBOSE $prnv $outv" 2>> $logfile`
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
    printCol "$pixelstr" $color 2>> $logfile

    if [ ! -f $prnr ]; then
      pixelstr="N/A"
      color=green
    else
      errorstr=`checkcat "compare raster bitmaps" "scripts/compare-raster.sh $VERBOSE $prnr $outr" 2>> $logfile`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr=`echo $errorstr | awk '{ print $3 }'`
        color=red
      fi
    fi
    printCol "$pixelstr" $color 2>> $logfile

    # Compare bboxes, number of polylines and total cut length
    color=red
    bbox_diff=`echo $rtlcompare | awk '{print $5}'`
    if [ $bbox_diff != "0" ]; then
        bboxstr="Err"
    else
        bboxstr="OK"
        color=green
    fi    
    printCol $bboxstr $color 2>> $logfile
    

    color=red
    polyline_diff=`echo $rtlcompare | awk '{ print $1 }'`
    if [ $polyline_diff != "0" ]; then
        plstr=$polyline_diff
    else
        plstr="OK"
        color=green
    fi
    printCol $plstr $color 2>> $logfile

    color=red
    length_diff=`echo $rtlcompare | awk '{print $2}'`
    if [ $length_diff != "0" ]; then
        lenstr="$length_diff"
    else
        lenstr="OK"
        color=green
    fi
    printCol $lenstr $color 2>> $logfile

    color=red
    move_diff=`echo $rtlcompare | awk '{print $3}'`
    if [ $move_diff != "0" ]; then
        lenstr="$move_diff"
    else
        lenstr="OK"
        color=green
    fi
    printCol $lenstr $color 2>> $logfile 
  fi

  fi

  echo
  echo -e $REPORT >> $logfile
  green "### End of $casedir ###\n\n" &>> $logfile
}

printUsage()
{
  echo "Usage: $0 [-l] [<testcase>]"
  echo "Options:"
  echo "  -l        Test level"
}

TEST_LEVEL=1

while getopts 'l:' c
do
    case $c in
        l) TEST_LEVEL="$2";;
        \?) echo "Invalid option: -$OPTARG" >&2;;
    esac
done
shift $(($OPTIND - 1))

[ $CC_DEBUG ] && set -x

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
