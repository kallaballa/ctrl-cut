#!/bin/bash

cd $CC_BASE

. $CC_FUNCTIONS

VERBOSE=
POPULATE=
PURGE_JOBS_TIMEOUT=
CHRTFS="$CC_TEST_CHROOT"
chrtrun="$CC_BASE/cc chrtsetup test-$CC_CHROOT_FLAVOUR --run "
bootstrap="$CC_CHROOT_BUILDDIR/cc bootstrap-chroot"
setup="$CC_CHROOT_BUILDDIR/cc chroot/setup -c $CC_CHROOT_BUILDDIR"
testany="$CC_CHROOT_BUILDDIR/cc test-any"

while getopts 'pcs' c
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
        try "Clear chroot $CHRTFS" "rm -r $CHRTFS"
    fi

    if [ ! -d $CHRTFS ]; then
        try "Create $CHRTFS" "mkdir $CHRTFS"
        cd $CHRTFS
        try "Bootstrap" "$bootstrap"
    else
        echo -e "\nChroot already populated $CHRTFS"
    fi

    cd $CC_BASE
}

populate


if [ ! $SKIP_BUILD ]; then 
    $chrtrun "$setup"
fi

#run tests
$chrtrun "$testany"

#test print
$chrtrun "lp -d $CC_PRINTERNAME $TEST_JOB"

if [ $PURGE_JOBS_TIMEOUT ]; then
    try "Waiting for $PURGE_JOBS_TIMEOUT sec before purging jobs" "sleep $PURGE_JOBS_TIMEOUT"
    $chrtrun "lprm -P $CC_PRINTERNAME -"
fi
