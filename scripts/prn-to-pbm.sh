#!/bin/bash

if [ $# != 1 ]; then
  echo "Usage: $0 prn-file"
  exit 1
fi
file=$1

python/rtltest.py $file
basefile=`dirname $file`/`basename $file .prn`
svg2pdf $basefile.svg $basefile.pdf
pdftoppm -q -r 36 -mono $basefile.pdf $basefile
