#!/bin/sh

VERSION=`date "+%Y.%m.%d"`
#VERSION=2010.05

# This is the same location as DEPLOYDIR in macosx-build-dependencies.sh
export MACOSX_DEPLOY_DIR=$PWD/../libraries/install

echo "Building ctrl-cut-$VERSION..."
qmake VERSION=$VERSION CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
make -s clean
make -j2
if [[ $? != 0 ]]; then
  exit 1
fi

INSTALL_LIB_DIR=/Library/Printers/CtrlCut/lib

install_name_tool -change /lib/libgs.9.00.dylib ${INSTALL_LIB_DIR}/libgs.9.00.dylib ctrl-cut
install_name_tool -id libgs.9.00.dylib ${INSTALL_LIB_DIR}libgs.9.00.dylib


echo "Sanity check of the executable..."
DYLD_LIBRARY_PATH=${INSTALL_LIB_DIR} `dirname $0`/macosx-sanity-check.py ctrl-cut
if [[ $? != 0 ]]; then
  exit 1
fi

echo "FIXME: Create installer package"
