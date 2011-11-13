#!/bin/bash

cd $CC_BASE

function printUsage() {
  echo "generates a report of test case images"
  echo
  echo "testreport [-l] [-o<outputfile>] searchpath"
  echo "    -l             Include a navigational index of cases"
  echo "    -o<outputfile>    The path of the output file"
  exit 1
}

while getopts 'lo:' c
do
  case $c in
    o) OUTHTML="$OPTARG";;
    l) GENLIST="-l";;
    \?) printUsage;;
  esac
done
shift $(($OPTIND - 1))

#[ $# -ne 1 ] && printUsage;

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"
[ -z "$OUTHTML" ] && OUTHTML="$dir.tar.gz"

function testimg() {
  testcasedir="$1"
  testtype="$2"
  testcase="`basename $testcasedir`"
  imgpath="$testcasedir/../out/$testcase/$testcase.$testtype.png"
#  echo "<td style=\"border:1px solid gray;\">"
  echo "<td>"
  if [ -f "$imgpath" ]; then
    echo "<a href="$imgpath"><img width="100%" src=\"$imgpath\" alt=\"`dirname $testcasedir`/`basename $testcasedir`\"></img></a>"
  else
    echo "<span width="100%">missing: $testcase.$testtype.png</span>"
  fi
  echo "</td>"
}

function generateHeader() {
  while read testcasedir; do
    testcase="`basename $testcasedir`"
    testdir="`dirname $testcasedir`"
    testname="`basename $testdir`"
    echo "<a href=\"#$testname/$testcase\">$testname/$testcase</a><br>"
  done
}

function generateReport() {
  while read testcasedir; do
    testcase="`basename $testcasedir`"
    testdir="`dirname $testcasedir`"
    testname="`basename $testdir`"
    echo "<a name=\"$testname/$testcase\"></a>"
    echo "<table padding=\"0\" margin=\"0\"><tr>"
    echo "<h1>$testname/$testcase</h1>"
    testimg "$testcasedir" "prn-v"
    testimg "$testcasedir" "raw-v"
    testimg "$testcasedir" "prn-r"
    testimg "$testcasedir" "raw-r"
    echo "</tr></table>"
  done
}
  echo "<html><head><style type="text/css"> body { background-color:#ccc; } a { font-size:12px; } table { width:100%; } td { margin:10px; padding:0px; border:3px dashed #0f0;} span { color:#f00; }</style></head><body>" > "$OUTHTML"

cases="`findcases \"$dir\"`"

[ ! -z $GENLIST ] && echo "$cases" | generateHeader >> "$OUTHTML"
echo "$cases" | generateReport >> "$OUTHTML"


echo "</body></html>" >> "$OUTHTML"

