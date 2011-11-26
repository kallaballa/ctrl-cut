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
. scripts/reports.sh

COL_I=0
COLUMNS=( "case" "type" "bin" "vimg" "rimg" "bbox" "polylines" "length" "move" )
PADDING=( "20"   "4"      "5"    "7"   "20"    "6"        "14"     "10"   "10" )

LEVELS=( "quick" "normal" "more" "all" )
REPORT=

shopt -s nullglob

printHeader() {
  colcnt=${#COLUMNS[@]}
  padcnt=${#PADDING[@]}
  [ $colcnt -ne $padcnt ] && error "column/padding count mismatch"
  report_begin
  COL_I=0
  for ((iheader=0; iheader<$colcnt; iheader++)); do
    report_print "${COLUMNS[$iheader]}"
  done
  report_term
}

# Usage: runtest <testpath>/<testcase>
# Example: runtest test-data/corel/quad/default
runtest()
{
  casedir="$1"
  testdir="`dirname $casedir`"
  testname="`basename $testdir`"
  testcase="`basename $casedir`"
  testapp="`dirname $testdir`"

  ignorefile="$casedir/.ignores"
  optionsfile="$casedir/.options"
  commonoptsfile="$testapp/common.options"
  psfile="$testdir/$testname.ps"
  svgfile="$testdir/$testname.svg"
  prnfile="$testdir/$testcase.prn"

  outdir="$testdir/$testcase"
  [ ! -d "$outdir" ]  && mkdir -p "$outdir"

  logfile="$outdir/$testcase.log"

  readIgnores "$ignorefile"

  if [ -f "$psfile" ]; then 
    logfile="$outdir/ps.raw.log"
    outfile="$outdir/ps.raw"
    prnv="$outdir/ps.prn-v.png"
    outv="$outdir/ps.raw-v.png"
    prnr="$outdir/ps.prn-r.png"
    outr="$outdir/ps.raw-r.png"

    report_begin "$testdir"
    report_print "$testcase" 2>> $logfile
    report_print "ps"  2>> $logfile

    # Generate a PCL/RTL file using our filter
    dotimeout  checkcat "run filter" "scripts/run-filter.sh $psfile $optionsfile $commonoptsfile"  > $outfile 2> $logfile
    errcode=$?
    if [ $errcode != 0 ]; then
	 report_print "N/A" red
         report_term
    else
         compareResults "$prnfile" "$outfile" "$prnv" "$outv" "$prnr" "$outr" "$logfile"
    fi
  fi

  if [ -f "$svgfile" ]; then
    logfile="$outdir/svg.raw.log"
    outfile="$outdir/svg.raw"
    prnv="$outdir/svg.prn-v.png"
    outv="$outdir/svg.raw-v.png"
    prnr="$outdir/svg.prn-r.png"
    outr="$outdir/svg.raw-r.png"

    report_begin "$testdir"
    report_print "$testcase" 2>> $logfile
    report_print "svg" 2>> $logfile
    # Generate a PCL/RTL file using our filter
    dotimeout  checkcat "run filter" "scripts/run-filter.sh $svgfile $optionsfile $commonoptsfile"  > $outfile 2> $logfile
    errcode=$?

    if [ $errcode != 0 ]; then
      report_print "N/A" red
      report_term
    else
      compareResults "$prnfile" "$outfile" "$prnv" "$outv" "$prnr" "$outr" "$logfile"
    fi
  fi
}

function compareResults() {
  prnfile="$1"
  outfile="$2"
  prnv="$3"
  outv="$4"
  prnr="$5"
  outr="$6"
  logfile="$7"

  if [ -f $prnfile ]; then 
      has_prnfile=1
  else 
      has_prnfile=0
  fi

  # Binary compare with the validated output (e.g. from the Windows drivers)
  binary_ok=0
  if [ $has_prnfile == 1 ]; then
    dotimeout check "compare binary" "cmp $prnfile $outfile" 2>> $logfile
    if [ $? == 0 ]; then
      binary_ok=1
    fi
  fi

  if [ $binary_ok == 1 ]; then
    report_print "ok" green 2>> $logfile
  else
    if [ $has_prnfile == 0 ]; then
      report_print "N/A" red 2>> $logfile
    else
    report_print "no" red 2>> $logfile
    color=red
    # Convert cut vectors to bitmaps and compare them
    errorstr=""
    rtlcompare=`scripts/rtlcompare.sh -o $outdir $VERBOSE $prnfile $outfile "$prnv" "$outv" "$prnr" "$outr" 2>> $logfile`
    errcode="$?"
    if [ $errcode -ne 0 -o ! -f "$outv" ]; then
       errorstr="Err"
      rawtopbmfailed=1
    fi

    if [ $errcode -ne 0 ]; then
      errorstr="Err"
      color=red
    elif [ ! -f $prnv ]; then
      errorstr="N/A"
      color=green
    fi
    if [ -z $errorstr ]; then
      errorstr=`scripts/compare-bitmaps.sh $VERBOSE $prnv $outv 2>> $logfile`
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
    report_print "$pixelstr" $color 2>> $logfile

    if [ ! -f $prnr ]; then
      pixelstr="N/A"
      color=green
    else
      errorstr=`scripts/compare-raster.sh $VERBOSE $prnr $outr 2>> $logfile`
      if [ $? == 0 ]; then
        pixelstr="OK"
        color=green
      else
        pixelstr="$errorstr"
        color=red
      fi
    fi
    report_print "$pixelstr" $color 2>> $logfile

    # Compare bboxes, number of polylines and total cut length
    color=red
    bbox_diff=`echo $rtlcompare | awk '{print $5}'`
    if [ "$bbox_diff" != "0" ]; then
        bboxstr="Err"
    else
        bboxstr="OK"
        color=green
    fi    
    report_print $bboxstr $color 2>> $logfile

    color=red
    polyline_diff=`echo $rtlcompare | awk '{ print $1 }'`
    if [ "$polyline_diff" != "0" ]; then
        plstr=$polyline_diff
    else
        plstr="OK"
        color=green
    fi
    report_print $plstr $color 2>> $logfile

    color=red
    length_diff=`echo $rtlcompare | awk '{print $2}'`
    if [ "$length_diff" != "0" ]; then
        lenstr="$length_diff"
    else
        lenstr="OK"
        color=green
    fi
    report_print $lenstr $color 2>> $logfile

    color=red
    move_diff=`echo $rtlcompare | awk '{print $3}'`
    if [ "$move_diff" != "0" ]; then
        lenstr="$move_diff"
    else
        lenstr="OK"
        color=green
    fi
    report_print $lenstr $color 2>> $logfile 
  fi

  fi

  report_term
}

function diffResult() {
  tmp="/tmp/ctrl-cut.XXXXXX"
  first="`mktemp $tmp`" || error "can't create temporary file: $tmp"
  second="`mktemp $tmp`" || error "can't create temporary file $tmp"

  echo "$1" > "$first"
  echo "$2" > "$second"

  diff --suppress-common-lines "$first" "$second" | grep -v "^[a-z0-9-]" | sed '/^$/d'
  rm "$first" "$second"
}

printUsage()
{
  echo "Find test cases and select which ones to execute"
  echo "see also: stash, cleanup, followlogs"
  echo
  echo "$0 [-c <file> -t <file> -o <file> -l <num/label>] [-R <regex>] [<testcase glob>]"
  echo "Options:"
  echo "  -l <num/label>     Test level or label "
  echo "  -R <regex>   Only run tests matching the regex"
  echo "  -c <file>   Dump test results to a csv file"
  echo "  -t <file>   Dump test results to a html file"
  echo "  -o <file>   Redirect console output to a file"
  exit 1
}

TEST_LEVEL=1
CONSOLE_OUT=/dev/null
[ $CC_VERBOSE ] && CONSOLE_OUT="/dev/stdout"

CSV_OUT=tmp/current.csv
HTML_OUT=

while getopts 'l:R:o:c:t:' c
do
    case $c in
        t) HTML_OUT="$OPTARG";;
        c) CSV_OUT="$OPTARG";;
        o) CONSOLE_OUT="$OPTARG";;
        l) TEST_LEVEL="$OPTARG";;
        R) TEST_REGEX="$OPTARG";;
        \?) echo "Invalid option: -$OPTARG" >&2; printUsage;;
    esac
done
shift $(($OPTIND - 1))

[ $CC_DEBUG ] && set -x

[ -n "$CONSOLE_OUT" ] && report_init "CONSOLE" "$CONSOLE_OUT"
[ -n "$CSV_OUT" ] && report_init "CSV" "$CSV_OUT"
[ -n "$HTML_OUT" ] && report_init "HTML" "$HTML_OUT"

report_open "test-any `date`"
printHeader

searchpath="test-data"
# Run given test or all tests
[ $# -gt 0 ] && searchpath=$@;
  $CC_SCRIPTS/cleanup.sh $searchpath;

  findtests "$searchpath" | while read testdir; do
    #lookup numerical values in the LEVELS array or directly search for the test level key
    printf "%d" "$TEST_LEVEL" > /dev/null 2>&1 \
      && testlevel="${LEVELS[$TEST_LEVEL]}" \
      || testlevel="$TEST_LEVEL";
   
       
    casefile="$testdir/.cases"
    cases="`readCases $casefile $testlevel`"
    echodir=1
    for c in $cases; do          
      case=$c
      fullname="$testdir/$c"
      if [[ -z "$TEST_REGEX" || $fullname =~ $TEST_REGEX ]]; then
        if [ $echodir == 1 ]; then
          echo -n "["
          yellow "`dirname $testdir`/`basename $testdir`"
          echo "]"
          echodir=0
        fi
        runtest "$fullname"
	casekey="$testdir;$case;"
	caselines="`grep "$casekey" "$CSV_OUT"`"
	reference="`grep "$casekey" test-data/test-any.csv`"
	casesfailed="`diffResult "$caselines" "$reference"`"
	[ -n "$casesfailed" ] \
	  && red "`echo -en "$casesfailed" | sed 's/^..//g' | cut -d";" -f2-3 | tr ";" " " | sed 's/^/\t/g' | sort | uniq `\n"
      fi
    done
  done

report_close

#get collected test cases from csv
cases="`cat "$CSV_OUT"`"
caseskeys="`cut -d";" -f1,2,3 "$CSV_OUT"`"

#search corresponding test case from the reference csv
reference="`echo -e "$caseskeys" | grep --file="/dev/stdin" test-data/test-any.csv`"

#diff selected cases but only show changes from the just generated csv
casesfailed="`diffResult "$cases" "$reference"`"

[ -n "$casesfailed" ] \
  && error "\n### TEST RESULTS CHANGED (`echo -e $casesfailed | wc -l`) ###\n$casesfailed\n" \
  || green "\nall tests ok\n"


