#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDNAME"
fi

cd $CC_BASE

. $CC_FUNCTIONS

PRINTER=$1
PPDNAME=$2
DEVICEURL=$3

try "find cups-config..." "type -p cups-config"

FILTER_PATH="`cups-config --serverbin`/filter"
BACKEND_PATH="`cups-config --serverbin`/backend"

try "Installing filter binary..." "cp $CC_BINARY $FILTER_PATH/"
try "Installing dump backend..." "cp $CC_TEST_CODE/dump $BACKEND_PATH"
try "Installing passthrough filter" "cp $CC_TEST_CODE/passthroughfilter $FILTER_PATH"
try "/usr/sbin/lpadmin -p \"$PRINTER\" -P \"$CC_PPD_DIR/$PPDNAME\" -v \"$DEVICEURL\""

