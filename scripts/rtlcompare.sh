#!/bin/bash

. $CC_FUNCTIONS

F1="$1"
F2="$2"

function filter(){
    echo "$1" | grep "$2"
}

function value(){
    val=`filter "$1" "$2" | cut -d'=' -f2`
    #apply scale
    calc "$val"
}

function calc() {
    if [ -z "$2" ]; then
        scale=2
    else
        scale=$2
    fi
    # divide by 1 to make sure scale is applied
    echo "scale=$scale; $1/1" | bc
}

function abs() {
    if [ -z "`echo $1 | grep '-'`" ] ; then
        calc "0 - $1" $2 
    else
        calc "$1" $2 #make sure scale is applied
    fi           
}

function diff() {
    d=`calc "$1 - $2"`
    if [ `abs "$d"` ==  "0" ]; then
        echo 0;
    else
        echo "$1->$2";
    fi
}

function diffBBox() {
    BBOX1=$1
    BBOX2=$2
    set -- $BBOX1
    ulx1=$1
    uly1=$2
    lrx1=$3
    lry1=$4

    set -- $BBOX2
    ulx2=$1
    uly2=$2
    lrx2=$3
    lry2=$4

    dulx=`calc "$ulx1 - $ulx2"`
    duly=`calc "$uly1 - $uly2"`
    dlrx=`calc "$lrx1 - $lrx2"`
    dlry=`calc "$lry1 - $lry2"`

    if [ `abs $dulx 0` -gt 1 -o `abs $duly 0` -gt 1 -o `abs $dlrx 0` -gt 1 -o `abs $dlry 0` -gt 1 ]; then
        echo 1
    else 
        echo 0
    fi
}

function fitCanvas() {
  IMG1=$1
  IMG2=$2
  BBOX1=$3
  BBOX2=$4

  set -- $BBOX1
  ulx1=$1
  uly1=$2
  lrx1=$3
  lry1=$4

  set -- $BBOX2
  ulx2=$1
  uly2=$2
  lrx2=$3
  lry2=$4

  ulxc=0
  ulyc=0
  lrxc=0
  lryc=0

  rpx1=0
  rpy1=0
  rpw1=0
  rph1=0

  rpx2=0
  rpy2=0
  rpw2=0
  rph2=0

  if [ $ulx1 -lt $ulx2 ]; then
    rpx2=`calc "$ulx2-$ulx1" 0`
    rpx1=0
  else
    rpx1=`calc "$ulx1-$ulx2" 0`
    rpx2=0
  fi

  if [ $uly1 -lt $uly2 ]; then
    rpy2=`calc "$uly2-$uly1" 0`
    rpy1=0
  else
    rpy1=`calc "$uly1-$uly2" 0`
    rpy2=0
  fi

  rpw1=`calc "$lrx1-$ulx1" 0`
  rpw2=`calc "$lrx2-$ulx2" 0`
  rph1=`calc "$lry1-$uly1" 0`
  rph2=`calc "$lry2-$uly2" 0`

  if [ $rpw1 -gt $rpw2 ]; then
    rpw1=$rpw1
    rpw2=$rpw1
  else
    rpw1=$rpw2
    rpw2=$rpw2
  fi

  if [ $rph1 -gt $rph2 ]; then
    rph1=$rph1
    rph2=$rph1
  else
    rph1=$rph2
    rph2=$rph2
  fi


  convert "$IMG1" -extent ${rpw1}x${rph1}+${rpx1}+${rpy1} "$IMG1"
  convert "$IMG2" -extent ${rpw2}x${rph2}+${rpx2}+${rpy2} "$IMG2"
}

RES1=`$CC_PCLINT -dinfo -a -r "$F1-r.png" -v "$F1-v.png" "$F1" | grep "|"`
RES2=`$CC_PCLINT -dinfo -a -r "$F2-r.png" -v "$F2-v.png" "$F2" | grep "|"`

R_RES1=`filter "$RES1" "RASTER"`
R_RES2=`filter "$RES2" "RASTER"`

V_RES1=`filter "$RES1" "VECTOR"`
V_RES2=`filter "$RES2" "VECTOR"`

V_PDCNT1=`value "$V_RES1" "penDown"`
V_PDCNT2=`value "$V_RES2" "penDown"`

V_PDDIFF=`diff $V_PDCNT1 $V_PDCNT2`

V_WORKLEN1=`value "$V_RES1" "work"`
V_WORKLEN2=`value "$V_RES2" "work"`

V_WORKDIFF=`diff $V_WORKLEN1 $V_WORKLEN2`

V_MOVELEN1=`value "$V_RES1" "move"`
V_MOVELEN2=`value "$V_RES2" "move"`

V_MOVEDIFF=`diff $V_MOVELEN1 $V_MOVELEN2`

V_SEGCNT1=`value "$V_RES1" "segment"`
V_SEGCNT2=`value "$V_RES2" "segment"`

V_SEGDIFF=`diff $V_SEGCNT1 $V_SEGCNT2`

V_BBOX1=`filter "$V_RES1" "bounding" | cut -d"=" -f2`
V_BBOX2=`filter "$V_RES2" "bounding" | cut -d"=" -f2`

V_BBOXDIFF=`diffBBox "$V_BBOX1" "$V_BBOX2"`

R_PDCNT1=`value "$R_RES1" "penDown"`
R_PDCNT2=`value "$R_RES2" "penDown"`

R_PDDIFF=`diff $R_PDCNT1 $R_PDCNT2`

R_WORKLEN1=`value "$R_RES1" "work"`
R_WORKLEN2=`value "$R_RES2" "work"`

R_WORKDIFF=`diff $R_WORKLEN1 $R_WORKLEN2`

R_MOVELEN1=`value "$R_RES1" "move"`
R_MOVELEN2=`value "$R_RES2" "move"`

R_MOVEDIFF=`diff $R_MOVELEN1 $R_MOVELEN2`

R_SEGCNT1=`value "$R_RES1" "segment"`
R_SEGCNT2=`value "$R_RES2" "segment"`

R_SEGDIFF=`diff $R_SEGCNT1 $R_SEGCNT2`

R_BBOX1=`filter "$R_RES1" "bounding" | cut -d"=" -f2`
R_BBOX2=`filter "$R_RES2" "bounding" | cut -d"=" -f2`

R_BBOXDIFF=`diffBBox "$R_BBOX1" "$R_BBOX2"`

fitCanvas "$F1-r.png" "$F2-r.png" "$R_BBOX1" "$R_BBOX2"
fitCanvas "$F1-v.png" "$F2-v.png" "$V_BBOX1" "$V_BBOX2"

echo "$V_PDDIFF $V_WORKDIFF $V_MOVEDIFF $V_SEGDIFF $V_BBOXDIFF $R_PDDIFF $R_WORKDIFF $R_MOVEDIFF $R_SEGDIFF $R_BBOXDIFF"
