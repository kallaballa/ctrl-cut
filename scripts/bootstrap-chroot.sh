#!/bin/bash

chrtrun="$EC_SCRIPTS/chrtsetup.sh test-$EC_CHROOT_FLAVOUR --run "

echo "\nDebootstrap: $EC_CHROOT_FLAVOUR into $EC_TEST_CHROOT"
# bootstrap debian squeeze
debootstrap --verbose $EC_CHROOT_FLAVOUR $EC_TEST_CHROOT $EC_CHROOT_MIRROR

echo "\nGenerate modules.dep"
# generate modules.dep for the host kernel
$chrtrun "mkdir /lib/modules/`uname -r`/; depmod -a"

echo "\nInstall dependencies"
# install dependencies and build tool chain
$chrtrun "apt-get -y --allow-unauthenticated install build-essential cups libcups2-dev libboost-dev qt4-qmake git-core lpr libqtgui4"

echo "\nFixing lib symlinks"
$chrtrun "ln -s /usr/lib/libQtCore.so.4 /usr/lib/libQtCore.so"
$chrtrun "ln -s /usr/lib/libQtGui.so.4 /usr/lib/libQtGui.so"
