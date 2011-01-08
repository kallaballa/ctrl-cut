#!/bin/bash

# cd to epilogcups basedir
cd "`dirname $(readlink -f $0)`/.."

chrtrun="./scripts/chrtsetup.sh test-build-and-install --run "

# bootstrap debian squeeze into ./test-data/chroot/root/
debootstrap --verbose squeeze ./test-data/chroot/root/ http://debian.inode.at/debian/

# install dependencies and build tool chain
$chrtrun="apt-get install build-essential libcups2-dev libboost-dev qt4-qmake"

# generate modules.dep for the host kernel
$chrtrun="/lib/modules/`uname -r`/; depmod -a"


