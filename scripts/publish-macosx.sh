#!/bin/sh

VERSION=`date "+%Y.%m.%d"`
#VERSION=2011.02

# This is the same location as DEPLOYDIR in macosx-build-dependencies.sh
export MACOSX_DEPLOY_DIR=$PWD/../libraries/install

echo "Building ctrl-cut-$VERSION"
qmake -recursive VERSION=$VERSION CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
make -s clean
make -j4
if [[ $? != 0 ]]; then
  exit 1
fi

INSTALL_PRINTERS_DIR=/Library/Printers
INSTALL_CTRLCUT_DIR=${INSTALL_PRINTERS_DIR}/Ctrl-Cut
INSTALL_FILTER_DIR=/usr/libexec/cups/filter
INSTALL_BACKEND_DIR=/usr/libexec/cups/backend

FILTER_EXE=src/cups-filter/ctrl-cut
BACKEND_EXE=src/lpd-epilog/lpd-epilog

echo "Sanity check of the executables..."
echo `basename $FILTER_EXE`
DYLD_LIBRARY_PATH=${MACOSX_DEPLOY_DIR}/lib `dirname $0`/macosx-sanity-check.py $FILTER_EXE
if [[ $? != 0 ]]; then
  exit 1
fi
echo `basename $BACKEND_EXE`
DYLD_LIBRARY_PATH=${MACOSX_DEPLOY_DIR}/lib `dirname $0`/macosx-sanity-check.py $BACKEND_EXE
if [[ $? != 0 ]]; then
  exit 1
fi

echo "Building package"
# Point the executable to the installed location of the libraries
install_name_tool -change libgs.9.00.dylib ${INSTALL_CTRLCUT_DIR}/lib/libgs.9.00.dylib $FILTER_EXE

# Copy files to folder structure
rm -rf root
mkdir -p root/Ctrl-Cut/Icons
cp images/EpilogLegend36EXT.icns root/Ctrl-Cut/Icons
mkdir -p root/Ctrl-Cut/lib
cp ${MACOSX_DEPLOY_DIR}/lib/libgs.9.00.dylib root/Ctrl-Cut/lib
mkdir -p root/ppd
cp etc/ppd/Epilog/Legend36EXT.ppd "root/ppd/Epilog Legend 36EXT"
gzip "root/ppd/Epilog Legend 36EXT"

# Build package
/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --doc packaging/Ctrl-Cut.pmdoc --out Ctrl-Cut-$VERSION.pkg

rm -r root

hdiutil create Ctrl-Cut-$VERSION.dmg -ov -srcfolder Ctrl-Cut-$VERSION.pkg -format UDZO -volname Ctrl-Cut
hdiutil internet-enable -yes -quiet Ctrl-Cut-$VERSION.dmg
