#!/bin/bash


GHOSTPDL=$HOME/Desktop/Lazzzor/CUPS/ghostpdl-8.71/main/debugobj/pcl6

if [ $# != 1 ]; then
  echo "Usage: $0 prn-file"
  exit 1
fi
file=$1
basename=`basename $file .prn`
dirname=`dirname $file`

awk '{gsub(/\x1b%0B\x1b%1BPU/,"");print}' < $file > filtered.prn
$GHOSTPDL -dNOPAUSE -PRTL -sDEVICE=pdfwrite -sOutputFile=$dirname/$basename.pdf filtered.prn
