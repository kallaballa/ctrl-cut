#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDFILE"
fi

cd $CC_BASE

. $CC_FUNCTIONS

PRINTER=$1
PPDFILE=$2
DEVICEURL=$3

try "find cups-config..." "type -p cups-config"

FILTER_PATH="`cups-config --serverbin`/filter"
BACKEND_PATH="`cups-config --serverbin`/backend"

try "Installing filter binary..." "cp $CC_BINARY $FILTER_PATH/"
try "Installing dump backend..." "cp $CC_TEST_CODE/dump $BACKEND_PATH"
try "Installing printer..." "lpadmin -p \"$PRINTER\" -P \"$PPDFILE\" -v \"$DEVICEURL\""

