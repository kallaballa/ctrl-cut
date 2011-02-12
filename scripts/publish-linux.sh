#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

cd $CC_PACKAGING

RELEASE_NAME="ctrl-cut-$CC_VERSION"
FILES="`cat included`"

function  cleanup() {
    [ -d "$RELEASE_NAME/" ] && rm -r "$RELEASE_NAME"
}

function populate() {
    mkdir $RELEASE_NAME

    cp ctrl-cut.spec $RELEASE_NAME
    echo "$FILES" | while read incl; do
        cp -r "$CC_BASE/$incl" "$RELEASE_NAME/"
    done
}

cleanup
populate
tar -X excluded -chjf "$RELEASE_NAME.tar.bz2" "$RELEASE_NAME"
rpmbuild -ta $RELEASE_NAME.tar.bz2 --define "CUPSBIN `cups-config --serverbin`" --define "CUPSDATA `cups-config --datadir`"
cleanup


