#!/bin/sh
#
# This script builds all library dependencies of Ctrl-Cut for Mac OS X.
# The libraries will be build in 32- and 64-bit mode and backwards compatible with 
# 10.5 "Leopard".
# 
# Usage:
# - Edit the BASEDIR variable. This is where libraries will be built and installed
# - Edit the CTRLCUTDIR variable. This is where patches are fetched from
#
# Prerequisites:
#
# FIXME:
# o Verbose option
# o Port to other platforms?
#

BASEDIR=/Users/kintel/code/metalab/checkout/Ctrl-Cut/libraries
CTRLCUTDIR=/Users/kintel/code/metalab/checkout/Ctrl-Cut/ctrl-cut
SRCDIR=$BASEDIR/src
DEPLOYDIR=$BASEDIR/install

build_ghostscript()
{
  set -x
  version=$1
  echo "Building Ghostscript" $version "..."
  cd $BASEDIR/src
  curl -O http://ghostscript.com/releases/ghostscript-$version.tar.gz
  tar xzf ghostscript-$version.tar.gz
  cd ghostscript-$version
  patch -p0 < $CTRLCUTDIR/patches/ghostscript-$version-macosx.patch
  sed -e "s|__PREFIX__|${prefix}|" -i "" base/unix-dll.mak
  ./configure --prefix=$DEPLOYDIR --enable-compile-inits --disable-cups --disable-gtk --without-x --disable-fontconfig --with-drivers=PBM "CFLAGS=-mmacosx-version-min=10.5 -arch x86_64 -arch i386" LDFLAGS="-mmacosx-version-min=10.5 -arch x86_64 -arch i386"
  make -j4 soinstall
  install_name_tool -id libgs.9.00.dylib $DEPLOYDIR/lib/libgs.9.00.dylib
}

echo "Using basedir:" $BASEDIR
mkdir -p $SRCDIR $DEPLOYDIR
build_ghostscript 9.00
