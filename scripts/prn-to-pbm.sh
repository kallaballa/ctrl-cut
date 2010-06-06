#!/bin/bash

if [ $# != 1 ]; then
  echo "Usage: $0 prn-file"
  exit 1
fi
file=$1

python/rtltest.py $file $file.svg
svg2pdf $file.svg $file.pdf
pdftoppm -q -r 12 -mono $file.pdf tmpfile 2>/dev/null
mv tmpfile-000001.pbm $file.pbm
