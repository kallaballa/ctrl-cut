#!/bin/sh

<<<<<<< HEAD
filter=./EpilogFilter
if [ $# != 1 ]; then
  echo "Usage: $0 ps-file"
  exit 1
fi
file=$1
=======
if [ $# == 0 ]; then
  filter=./EpilogFilter
else
  filter=$1
fi
>>>>>>> Refactor batch - general cleaning, coding style etc.

export CHARSET=utf-8
export CONTENT_TYPE=application/pdf
export CUPS_CACHEDIR=/private/var/spool/cups/cache
export CUPS_DATADIR=/usr/share/cups
export CUPS_FILETYPE=document
export CUPS_SERVERROOT=/private/etc/cups
export DEVICE_URI=file:/tmp/passthrough.ps
export FINAL_CONTENT_TYPE=printer/passthrough
export LANG=en_US.UTF-8
export PATH=/usr/libexec/cups/filter:/usr/bin:/usr/sbin:/bin:/usr/bin
export PPD=/private/etc/cups/ppd/passthrough.ppd
export PRINTER=passthrough
export SOFTWARE=CUPS/1.4.3
export USER=root

$filter 32 kintel `basename $file ps`cdr 1 options $file
