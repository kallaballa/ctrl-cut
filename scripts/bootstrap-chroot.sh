#!/bin/bash

chrtrun="$EC_SCRIPTS/chrtsetup.sh test-$EC_CHROOT_FLAVOUR --run "

# bootstrap debian squeeze
debootstrap --verbose $EC_CHROOT_FLAVOUR $EC_TEST_CHROOT http://debian.inode.at/debian/

# install dependencies and build tool chain
$chrtrun "apt-get install build-essential libcups2-dev libboost-dev qt4-qmake git-core lpr libqtgui4"

# generate modules.dep for the host kernel
$chrtrun "mkdir /lib/modules/`uname -r`/; depmod -a"

$chrtrun "ln -s /usr/lib/libQtCore.so.4 /usr/lib/libQtCore.so"
$chrtrun "ln -s /usr/lib/libQtGui.so.4 /usr/lib/libQtGui.so"

