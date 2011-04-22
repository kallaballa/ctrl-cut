#!/bin/sh



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
convert $1 $2 -depth 24 -scale 10% -scale 1000% -depth 1 -compose difference -composite "$diff"
statistic=`identify -verbose -unique "$diff"`

mean=`echo "$statistic" | grep "mean:" | awk '{ print $2 }'`
deviation=`echo "$statistic" | grep "standard deviation:" | awk '{ print $3 }'`
skew=`echo "$statistic" | grep "skewness:" | awk '{ print $2 }'`
kurtosis=`echo "$statistic" | grep "kurtosis:" | awk '{ print $2 }'`

mean=`scale $mean`
deviation=`scale $deviation`
skew=`scale $skew`
kurtosis=`scale $kurtosis`

if [ $? -ne 0 ]; then
    echo "General error: Ouch"
    exit 1 # Compare failed to read image
else
    if [ "$pixelerror" == 0 ]; then 
      exit 0
    fi
    echo "Pixel error: $mean|$deviation|$skew|$kurtosis"
  exit 1
fi
exit 0

