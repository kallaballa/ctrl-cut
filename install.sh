#!/bin/bash

# cd to epilogcups basedir
cd "`dirname $(readlink -f $0)`"

[ ! $# -eq 2 ] && echo "install.sh PRINTERNAME PPDNAME"

PRINTER=$1
PPDNAME=$2
FILTER_PATH="`cups-config --serverbin`/filter"
KERNELMODDIR="/lib/modules/`uname -r`/"

# if theres no kernel module directory for the host kernel we're probably in a chroot
[ ! -d "$KERNELMODDIR" ] && mkdir "$KERNELMODDIR"
depmod -a

# start cupsd
/etc/init.d/cupsd start

# generate ppd files
etc/ppd/generate_ppds.sh

# copy epilog filter into the cups filter path
cp EpilogFilter $FILTER_PATH/epilogcups

# install the printer
lpadmin -p "$PRINTER" -P "etc/ppd/$PPDNAME"

