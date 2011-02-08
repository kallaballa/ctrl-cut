#!/bin/bash

VERBOSE=
POPULATE=
PURGE_JOBS_TIMEOUT=
CHRTFS="$CC_TEST_CHROOT"
CC_ENV="$CC_BASE/cc"
chrtrun="$CC_ENV chrtsetup test-$CC_CHROOT_FLAVOUR --run "
bootstrap="$CC_ENV bootstrap-chroot"
setup="$CC_ENV chroot/setup -c"
testany="$CC_ENV test-any"

while getopts 'cs' c
do
    case $c in
        p) PURGE_JOBS_TIMEOUT="$2"; shift;;
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
    $chrtrun "$setup"
fi

#run tests
$chrtrun "$testany"

#test print
$chrtrun "lp -d $CC_PRINTERNAME $TEST_JOB"

