#!/bin/bash

VERBOSE=
POPULATE=
CHRTFS="$CC_TEST_CHROOT"
chrtrun="$CC_SCRIPTS/chrtsetup.sh test-$CC_CHROOT_FLAVOUR --run "
bootstrap="$CC_SCRIPTS/bootstrap-chroot.sh"

while getopts 'cs' c
do
    case $c in
        s) SKIP_BUILD=-s; shift;;
        c) CLEAR=-c ; shift;;
        ?) printUsage; exit 1 ;;
    esac
done

TEST_JOB=$1

function populate() {
    if [ $CLEAR ]; then
        echo -e "\nClearing chroot $CHRTFS"
        rm -r $CHRTFS
    fi

    if [ ! -d $CHRTFS ]; then
        echo -e "\nPopulating chroot $CHRTFS"
        mkdir $CHRTFS
        cd $CHRTFS
        $bootstrap
    else
        echo -e "\nChroot already populated $CHRTFS"
    fi

    cd $BASEDIR
}

populate


if [ ! $SKIP_BUILD ]; then 
    echo -e "\nDownloading and building..."

    # clone ec source and build
    $chrtrun "cd ~/; rm -r ctrl-cut; git clone $CC_GIT_URL; \
    cd ctrl-cut/; qmake -recursive; make;"

    # download and install pysvg 
    $chrtrun "cd /tmp; rm -r pysvg-0.2.1.zip pysvg-0.2.1/; \
        wget http://pysvg.googlecode.com/files/pysvg-0.2.1.zip; unzip pysvg-0.2.1.zip; \
        cd pysvg-0.2.1/; python setup.py install" 

    echo -e "\nInstalling passthrough printer"
    # install the printer with passthrough backend
    $chrtrun "cd ~/ctrl-cut; ./cc install $CC_PRINTERNAME $CC_PPD_DIR/$CC_PRINTERPPD dump://"
fi

#run tests
$chrtrun "cd ~/ctrl-cut; ./cc test-any"

#test print
$chrtrun "lp -d $CC_PRINTERNAME $TEST_JOB"

