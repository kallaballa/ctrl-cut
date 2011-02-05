#!/bin/bash

. $CC_FUNCTIONS

function chrtrun {
	try "$1" "$CC_SCRIPTS/chrtsetup.sh test-$CC_CHROOT_FLAVOUR --run \"$2\""
}
KERNEL=`uname -r`

try "Debootstrap: $CC_CHROOT_FLAVOUR into $CC_TEST_CHROOT"      "debootstrap --verbose $CC_CHROOT_FLAVOUR $CC_TEST_CHROOT $CC_CHROOT_MIRROR"
chrtrun "Create kernel mod directory..."               		"mkdir /lib/modules/$KERNEL/"
chrtrun "Generate modules.dep"... 				"depmod -a"
chrtrun "Install dependencies and build tool chain..."          "apt-get -y --allow-unauthenticated install build-essential cups libcups2-dev libboost-dev qt4-qmake git-core libqtgui4"
chrtrun "install packages required for testing..."              "apt-get -y --allow-unauthenticated install pkg-config python-minimal xqilla librsvg2-dev libcairo2 unzip"
chrtrun "Fixing qt lib symlinks"                                "ln -s /usr/lib/libQtCore.so.4 /usr/lib/libQtCore.so; ln -s /usr/lib/libQtGui.so.4 /usr/lib/libQtGui.so"



