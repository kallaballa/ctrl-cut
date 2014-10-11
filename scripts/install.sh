#!/bin/bash

cd $CC_BASE
set -x
export
destdir="$DESTDIR"
prefix="$PREFIX"

libdir="lib/"
if [ -d "/usr/lib64/" ]; then
	libdir="lib64/";
fi

echo $destdir/$prefix/bin $destdir/$prefix/$libdir
try "Create directories" "mkdir -p $destdir/$prefix/bin $destdir/$prefix/$libdir"
try "Install ctrl-cut library" "cp src/ctrl-cut/libctrl-cut.so $destdir/$prefix/$libdir/libctrl-cut.so.1"
try "Install ctrl-cut gui" "cp src/gui/gui $destdir/$prefix/bin/ctrl-cut"

