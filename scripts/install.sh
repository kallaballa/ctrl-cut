#!/bin/bash

cd $CC_BASE

libdir="/usr/lib/"
if [ -d "/usr/lib64/" ]; then
	libdir="/usr/lib64";
fi

try "Install ctrl-cut library" "cp src/ctrl-cut/libctrl-cut.so $libdir"
try "Install ctrl-cut gui" "cp src/gui/gui /usr/bin/

