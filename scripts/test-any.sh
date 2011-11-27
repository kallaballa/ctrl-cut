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
. scripts/runtest.sh

COL_I=0
COLUMNS=( "case" "type" "bin" "vimg" "rimg" "bbox" "polylines" "length" "move" )
PADDING=( "20"   "5"      "5"    "7"   "7"    "6"        "20"     "20"   "10" )

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

function diffResult() {
  tmp="/tmp/ctrl-cut.XXXXXX"
  first="`mktemp $tmp`" || error "can't create temporary file: $tmp"
  second="`mktemp $tmp`" || error "can't create temporary file $tmp"

  echo "$1" > "$first"
  echo "$2" > "$second"

  diff "$first" "$second" | sed '/^$/d'
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
	  && red "`echo -en "$casesfailed" | grep -v "^[a-z0-9-]" | sed 's/^..//g' | cut -d";" -f2-3 | tr ";" " " | sed 's/^/\t/g' | sort | uniq `\n"
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

if [ -n "$casesfailed" ]; then
  red "\n### TEST RESULTS CHANGED (`echo -e "$casesfailed" | wc -l`) ###\n\n"
  color=lightred
  echo -e "$casesfailed" | while read line; do
    left="^<"
    right="^>"
    if [[ $line =~ $left ]]; then
      color=red;
    elif [[ $line =~ $right ]]; then
      color=lightred;
    else
      color=white
    fi

    $color "$line\n"
  done
else
  green "\nall tests ok\n"
fi


