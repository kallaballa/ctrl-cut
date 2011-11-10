#!/bin/bash

cd $CC_BASE

function printUsage() {
  echo "collects and saves test results in a tar archive"
  echo
  echo "stash [-o<archive>] searchpath"
  echo "    -o<archive>    The path of the output archive"
  exit 1
}

while getopts 'o:' c
do
  case $c in
    o) OUTARCHIVE="$OPTARG";;
    \?) printUsage;;
  esac
done
shift $(($OPTIND - 1))

[ $# -ne 1 ] && printUsage;

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"
[ -z "$OUTARCHIVE" ] && OUTARCHIVE="$dir.tar.gz"

findtests "$dir" | xargs tar -czf "$OUTARCHIVE"

