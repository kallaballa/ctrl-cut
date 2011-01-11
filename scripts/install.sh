#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDNAME"
fi

cd $EC_BASE

. $EC_FUNCTIONS

PRINTER=$1
PPDNAME=$2
DEVICEURL=$3

try "find cups-config..." "type -p cups-config"

FILTER_PATH="`cups-config --serverbin`/filter"
BACKEND_PATH="`cups-config --serverbin`/backend"

# generate ppd files
try "Generating PPD files..." "$EC_SCRIPTS/generate_ppds.sh"

# copy epilog filter into the cups filter path
try "Installing filter binary..." "cp EpilogFilter $FILTER_PATH/epilogcups"

# copy dump backend
try "Installing dump backend..." "cp $EC_TEST_CODE/dump $BACKEND_PATH"

# copy passthroughfilter
try "Instralling passthrough filter" "cp $EC_TEST_CODE/passthroughfilter $FILTER_PATH"

# install the printer
try "Installing the passthrough filter" "/usr/sbin/lpadmin -p \"$PRINTER\" -P \"$EC_PPD_DIR/$PPDNAME\" -v \"$DEVICEURL\""

