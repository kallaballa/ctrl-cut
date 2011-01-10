#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDNAME"
fi

PRINTER=$1
PPDNAME=$2
FILTER_PATH="`cups-config --serverbin`/filter"

echo "\nGenerating PPD files..."
# generate ppd files
$EC_SCRIPTS/generate_ppds.sh

echo "\nInstalling filter binary..."
# copy epilog filter into the cups filter path
cp EpilogFilter $FILTER_PATH/epilogcups

echo "\nInstalling the passthrough filter"
# install the printer
/usr/sbin/lpadmin -p "$PRINTER" -P "$EC_PPD_DIR/$PPDNAME"

