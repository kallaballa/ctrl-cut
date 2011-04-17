#!/bin/sh



while getopts 'v' c
do
  case $c in
    v) set -x ;;
  esac
done

shift $(($OPTIND - 1))

statistic=`convert $1 $2 -scale 10% -scale 1000% -compose difference -composite  PPM:- | identify -verbose -unique -`

mean=`echo "$statistic" | grep "mean:" | awk '{ print $2 }' | head -c5`
deviation=`echo "$statistic" | grep "standard deviation:" | awk '{ print $3 }' | head -c5`

if [ $? -ne 0 ]; then
    echo "General error: Ouch"
    exit 1 # Compare failed to read image
else
    if [ "$pixelerror" == 0 ]; then 
      exit 0
    fi
    echo "Pixel error: $mean|$deviation"
  exit 1
fi
exit 0

