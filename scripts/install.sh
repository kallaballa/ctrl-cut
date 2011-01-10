#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDNAME"
fi

PRINTER=$1
PPDNAME=$2
FILTER_PATH="`cups-config --serverbin`/filter"

echo -e "\nGenerating PPD files..."
# generate ppd files
$EC_SCRIPTS/generate_ppds.sh

echo -e "\nInstalling filter binary..."
# copy epilog filter into the cups filter path
cp EpilogFilter $FILTER_PATH/epilogcups

echo -e "\nInstalling the passthrough filter"
# install the printer
ls -l /usr/sbin/lpadmin
/usr/sbin/lpadmin -p "$PRINTER" -P "$EC_PPD_DIR/$PPDNAME"

