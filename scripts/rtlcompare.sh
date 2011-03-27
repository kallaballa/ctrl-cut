#!/bin/bash

. $CC_FUNCTIONS

F1="$1"
F2="$2"

function filter(){
    echo "$1" | grep "$2"
}

function value(){
    filter "$1" "$2" | cut -d":" -f2
}

function calc() {
    echo "scale=2; $1" | bc
}

function abs() {
    if "$1" ; then
        calc "0 - $1"  
    fi           
}

function diff() {
    d=`calc "$1 - $2"`
    if [ `abs "$d"` -eq 0 ]; then
        echo 0;
    else
        echo 1;
    fi
}

RES1=`$CC_PCLINT -a -b "$F1.png" "$F1" | grep "|"`
RES2=`$CC_PCLINT -a -b "$F2.png" "$F2" | grep "|"`

RRES1=`filter "$RES1" "RASTER"`
RRES2=`filter "$RES2" "RASTER"`

VRES1=`filter "$RES1" "VECTOR"`
VRES2=`filter "$RES2" "VECTOR"`

echo -e "RRES1:\n$RRES1"
echo -e "RRES2:\n$RRES2"
echo -e "VRES1:\n$VRES1"
echo -e "VRES2:\n$VRES2"

PCNT1=`value "$VRES1" "penDown"
echo "################ PCNT1: $PCNT1"
PCNT2=`value "$VRES2" "penDown"

PDIFF=`diff $PCNT1 $PCNT2`

TOTCNT1=`value "$VRES1" "total"`
TOTCNT2=`value "$VRES2" "total"`

TOTDIFF=`diff $TOTCNT1 $TOTCNT2`

MCNT1=`value "$VRES1" "move"
MCNT2=`value "$VRES2" "move"

MDIFF=`diff $MCNT1 $MCNT2`

SEGCNT1=`value "$VRES1" "segment"
SEGCNT2=`value "$VRES2" "segment"

SEGDIFF=`diff $SEGCNT1 $SEGCNT2`

BBOXCNT1=`value "$VRES1" "bounding"
BBOXCNT2=`value "$VRES2" "bounding"

BBOXDIFF=1
if [ "$BBOXCNT1" == "$BBOXCNT2" ]; then 
    BBOXDIFF=0
fi

echo "$PDIFF $TOTDIFF $MDIFF $SEGDIFF $BBOXDIFF"

