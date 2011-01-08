#!/bin/bash

BASEDIR="`dirname $(readlink -f $0)`/.."

# cd to epilogcups base dir
cd $BASEDIR

VERBOSE=
POPULATE=
ARCHIVE="debian_squeeze_x86_chroot.tar.bz2"
ROOT="./test-data/chroot/root"
chrtrun="./scripts/chrtsetup.sh test-build-and-install --run "

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
        echo "Clearing chroot"
        rm -r $ROOT
    fi

    if [ $POPULATE ]; then
        echo "Populating chroot"
        mkdir $ROOT
        cd $ROOT
        tar -pxjf ../$ARCHIVE
    else
        echo "Chroot already populated"
    fi
    cd $BASEDIR
}

populate
$chrtrun "id"
$chrtrun "cd ~/; git clone git://github.com/Metalab/epilogcups.git; cd epilogcups; qmake; make"
