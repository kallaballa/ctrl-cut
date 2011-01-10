#!/bin/bash

echo "Running tests"
srcdir=$EC_TEST_DATA/corel
svg2pdf="$EC_TEST_CODE/svg2pdf/svg2pdf"

for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  echo "*$testcase...."
  $EC_PYTHON/rtltest.py $f
  svgfile=`dirname $f`/`basename $f .prn`.svg
  pdffile=`dirname $f`/`basename $f .prn`.pdf
  $svg2pdf $svgfile $pdffile
done
