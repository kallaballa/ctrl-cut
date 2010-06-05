#!/bin/bash

echo "Running tests"
srcdir=test-data/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  echo "*$testcase...."
  python/rtltest.py $f
  svgfile=`dirname $f`/`basename $f .prn`.svg
  pdffile=`dirname $f`/`basename $f .prn`.pdf
  svg2pdf $svgfile $pdffile
done
