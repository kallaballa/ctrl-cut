#!/bin/bash

# cd to epilogcups basedir
cd "`dirname $(readlink -f $0)`/.."

while getopts 'v' c
do
  case $c in
    v) VERBOSE=-v ;;
  esac
done

shift $(($OPTIND - 1))

if [ $# != 1 ]; then
  echo "Usage: $0 prn-file"
  exit 1
fi
file=$1

if test $VERBOSE; then
  set -x
else
  # suppress segmentation fault output
  exec 2> /dev/null
fi

python/rtl2svg.py -c $file $file.svg
if [ $? -ne 0 ]; then
  echo "rtl2svn.py failed"
fi
svg2pdf $file.svg $file.pdf
if [ $? -ne 0 ]; then
  echo "svg2pdf failed"
fi
pdftoppm -q -r 36 -mono $file.pdf tmpfile 2>/dev/null
# Segmentation fault - it has happened
if [ $? -eq 139 ]; then
  exit 1
fi
mv tmpfile-1.pbm $file.pbm
