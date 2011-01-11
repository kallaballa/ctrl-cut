#!/bin/bash


if [ ! $# -eq 2 ]; then
    echo "install.sh PRINTERNAME PPDNAME"
fi

cd $EC_BASE

PRINTER=$1
PPDNAME=$2
DEVICEURL=$3

FILTER_PATH="`cups-config --serverbin`/filter"
BACKEND_PATH="`cups-config --serverbin`/backend"

echo -e "\nGenerating PPD files..."
# generate ppd files
$EC_SCRIPTS/generate_ppds.sh

echo -e "\nInstalling filter binary..."
# copy epilog filter into the cups filter path
cp epilogcups $FILTER_PATH

echo -e "\nInstalling debug backends/filters..."
# copy dump backend
cp $EC_TEST_CODE/dump $BACKEND_PATH

# copy passthroughfilter
cp $EC_TEST_CODE/passthroughfilter $FILTER_PATH

echo -e "\nInstalling the passthrough filter"
# install the printer
/usr/sbin/lpadmin -p "$PRINTER" -P "$EC_PPD_DIR/$PPDNAME" -v "$DEVICEURL"


