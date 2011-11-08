#!/bin/bash

cd $CC_BASE

while getopts 'o:' c
do
  case $c in
    o) OUTARCHIVE="$2";;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done
shift $(($OPTIND - 1))

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"
[ -z "$OUTARCHIVE" ] && OUTARCHIVE="$dir.tar.gz"

findcases "$dir" | xargs tar -czf "$OUTARCHIVE"

