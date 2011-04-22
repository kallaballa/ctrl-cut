#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

while getopts 'x' c
do
    case $c in
        x) 
            XML=-x
            ;;
        \?) 
            echo "Invalid option: -$OPTARG" >&2
            ;;
    esac
done

# Get rid of processed arguments
shift $(($OPTIND - 1))

if test $VERBOSE; then
  set -x
fi

[ ! -d "./tmp" ] && mkdir "./tmp"

export RASTER_OFF="y"
[ $# != 1 ] && error "Usage: $0 ps-file" 1
file=$1

try "locating cups-config" "type cups-config" 

export CHARSET=utf-8
export CONTENT_TYPE=application/pdf
#export CUPS_CACHEDIR=/private/var/spool/cups/cache
export CUPS_DATADIR=`cups-config --datadir`
export CUPS_FILETYPE=document
export CUPS_SERVERROOT=`cups-config --serverroot`
export DEVICE_URI=file:/tmp/passthrough.ps
export FINAL_CONTENT_TYPE=printer/passthrough
export LANG=en_US.UTF-8
#export PATH=/usr/libexec/cups/filter:/usr/bin:/usr/sbin:/bin:/usr/bin
export PPD="$CUPS_DATADIR/ppd/passthrough.ppd"
export PRINTER=passthrough
#export SOFTWARE=CUPS/1.4.3
export USER=root

filename=`basename $file`

commonoptions=`dirname $file`/common.options
if [ -f $commonoptions ]; then
  read < $commonoptions commonoptions
fi

optionsfile=`dirname $file`/${filename%.*}.options
if [ -f $optionsfile ]; then
  read < $optionsfile options
fi

#echo ctrl-cut invocation into log
echo "BLA $CC_BINARY $XML 32 kintel $filename 1 \"$commonoptions $options $CC_FILTER_OPTIONS\" $file" &> ${file%.ps}.run

# Don't run this with try since it produces stdout which is passed to our caller
$CC_BINARY $XML 32 kintel $filename 1 "$commonoptions $options $CC_FILTER_OPTIONS" $file
exit $?
