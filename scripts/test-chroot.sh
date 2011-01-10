#!/bin/bash

VERBOSE=
POPULATE=
CHRTFS="$EC_TEST_CHROOT/root"
chrtrun="$EC_SCRIPTS/chrtsetup.sh test-$EC_CHROOT_FLAVOUR --run "

while getopts 'vcp' c
do
    case $c in
        v) VERBOSE=-v ;;
        p) POPULATE=-p ;;
        c) CLEAR=-c ;;
        ?) printUsage; exit 1 ;;
    esac
done


function populate() {
    if [ $CLEAR ]; then
        echo "Clearing chroot $CHRTFS"
        rm -r $CHRTFS
    fi

    if [ $POPULATE ]; then
        echo "Populating chroot $CHRTFS"
        mkdir $CHRTFS
        cd $CHRTFS
        $EC_TEST_CHROOT/bootstrap.sh
    else
        echo "Chroot already populated"
    fi
    cd $BASEDIR
}

populate

# clone ec source and build
$chrtrun "cd ~/; rm -r epilogcups; git clone $EC_GIT_URL; cd epilogcups; \
    qmake; make; \
    cd test-code/; qmake passthroughfilter.pro; make"

