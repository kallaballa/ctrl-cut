#!/bin/bash



while getopts 'v' c
do
  case $c in
    v) set -x ;;
  esac
done

shift $(($OPTIND - 1))

function scale() {
    echo "scale=1; $1/1" | bc
}

diff="`echo ${1%.prn-r.png}`_diff.png"
mean=`trycat "generating difference mask" "convert $1 $2 -depth 8 -blur 3x3 -compose difference -composite -blur 1x1 -threshold 30% -format \"%[fx:round(mean*1000))]\" info:"`

if [ $? -ne 0 ]; then
    echo "General error: Ouch"
    exit 1 # Compare failed to read image
else
    if [ "$mean" == 0 ]; then 
      exit 0
    fi
    echo "$mean"
  exit 1
fi
exit 0

