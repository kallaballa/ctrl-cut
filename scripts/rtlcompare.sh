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

RES1=`$CC_PCLINT -a -b "$F1.png" "$F1" | grep "|"`
RES2=`$CC_PCLINT -a -b "$F2.png" "$F2" | grep "|"`

RRES1=`filter "$RES1" "RASTER"`
RRES2=`filter "$RES2" "RASTER"`

VRES1=`filter "$RES1" "VECTOR"`
VRES2=`filter "$RES2" "VECTOR"`

PCNT1=`value "$VRES1" "penDown"`
PCNT2=`value "$VRES2" "penDown"`

PDIFF=`diff $PCNT1 $PCNT2`

WORKLEN1=`value "$VRES1" "work"`
WORKLEN2=`value "$VRES2" "work"`

WORKDIFF=`diff $WORKLEN1 $WORKLEN2`

MOVELEN1=`value "$VRES1" "move"`
MOVELEN2=`value "$VRES2" "move"`

MOVEDIFF=`diff $MOVELEN1 $MOVELEN2`

SEGCNT1=`value "$VRES1" "segment"`
SEGCNT2=`value "$VRES2" "segment"`

SEGDIFF=`diff $SEGCNT1 $SEGCNT2`

BBOX1=`filter "$VRES1" "bounding" | cut -d"=" -f2`
BBOX2=`filter "$VRES2" "bounding" | cut -d"=" -f2`

BBOXDIFF=`diffBBox "$BBOX1" "$BBOX2"`

echo "$PDIFF $WORKDIFF $MOVEDIFF $SEGDIFF $BBOXDIFF"

