#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

while getopts 'p' c
do
    case $c in
        c) PRINTER_INSTALL=-p ; shift;;
        --) shift; break ;;
    esac
done

PRINTER=$1
PPDFILE=$2
DEVICEURL=$3

FILTER_PATH="$CUPS_SERVER_BIN/filter/"
BACKEND_PATH="$CUPS_SERVER_BIN/backend/"
MODELS_PATH="$CUPS_SERVER_DATA/model/"

$CC_SCRIPTS/install-desktop.sh

try "Install ppds $MODELS_PATH" "cp -r etc/ppd/Epilog $MODELS_PATH"
try "Install filter binary $FILTER_PATH" "cp $CC_BINARY $FILTER_PATH/"
try "Install lpd backend $BACKEND_PATH" "cp $CC_BASE/src/lpd-epilog/lpd-epilog $BACKEND_PATH/"
#try "Installing dump backend" "cp $CC_TEST_CODE/dump $BACKEND_PATH"
#try "Fixing file permissions" "chmod u+x $BACKEND_PATH/dump"
[ $PRINTER_INSTALL ] && try "Installing printer Name: $PRINTER PPD: $PPDFILE Device:$DEVICEURL" "lpadmin -E -p $PRINTER -P $PPDFILE -v $DEVICEURL"
