#!/bin/bash

VERBOSE=
POPULATE=
CHRTFS="$EC_TEST_CHROOT"
chrtrun="$EC_SCRIPTS/chrtsetup.sh test-$EC_CHROOT_FLAVOUR --run "
bootstrap="$EC_SCRIPTS/bootstrap-chroot.sh"

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
        echo -e "\nClearing chroot $CHRTFS"
        rm -r $CHRTFS
    fi

    if [ $POPULATE ]; then
        echo -e "\nPopulating chroot $CHRTFS"
        mkdir $CHRTFS
        cd $CHRTFS
        $bootstrap
    else
        echo -e "\nAssuming chroot already populated $CHRTFS"
    fi
    cd $BASEDIR
}

populate

echo -e "\nDownloading and building..."

# clone ec source and build
$chrtrun "cd ~/; rm -r epilogcups; git clone $EC_GIT_URL; \
    cd epilogcups/; qmake; make; \
    cd test-code/; qmake passthroughfilter.pro; make; \
    cd svg2pdf/; make"

# download and install pysvg 
$chrtrun "cd /tmp; wget http://pysvg.googlecode.com/files/pysvg-0.2.1.zip; unzip pysvg-0.2.1.zip; \
   cd pysvg-0.2.1/; python setup.py install" 


echo -e "\nInstalling passthrough printer"
# install the printer with passthrough backend
$chrtrun "cd ~/epilogcups; ./ec install lazzzor EpilogLegend36EXT.ppd dump://"

