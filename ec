#!/bin/bash

# this script should be used to call all other project scripts. 
# it configures the necessary environment variables for the project context.
# e.g. to call "$CC_BASE/scripts/test-corel.sh <args>" do "./ec test-corel <args>"
# alternatively you may also source it and call scripts directly

# switch to bash 3.1 compatibility mode for mac osx
shopt -s compat31

cd `dirname $0` 

VERBOSE=

while getopts 'v' c
do
    case $c in
        v) export VERBOSE=-v; shift; break ;;
	--) shift; break;;
    esac
done



export CC_BASE="`pwd`"
export CC_TEST_CHROOT="$CC_BASE/chroot"
export CC_CHROOT_MIRROR="http://debian.inode.at/debian/"
export CC_CHROOT_FLAVOUR="squeeze"
export CC_SCRIPTS="$CC_BASE/scripts"
export CC_FUNCTIONS="$CC_SCRIPTS/functions.sh"
export CC_GIT_URL="git://github.com/Metalab/epilogcups.git"
export CC_PPD_DIR="$CC_BASE/etc/ppd"
export CC_TEST_DATA="$CC_BASE/test-data"
export CC_TEST_CODE="$CC_BASE/test-code"
export CC_PYTHON="$CC_BASE/python"
export CC_ETC="$CC_BASE/etc"
export CC_BINARY="$CC_BASE/ctrl-cut"

. $CC_FUNCTIONS

verbose "CC_BASE: $CC_BASE"
verbose "CC_TEST_CHROOT: $CC_TEST_CHROOT"
verbose "CC_CHROOT_FLAVOUR: $CC_CHROOT_FLAVOUR"
verbose "CC_SCRIPTS: $CC_SCRIPTS"
verbose "CC_FUNCTIONS: $CC_FUNCTIONS"
verbose "CC_GIT_URL: $CC_GIT_URL"
verbose "CC_PPD_DIR: $CC_PPD_DIR"
verbose "CC_TEST_DATA: $CC_TEST_DATA"
verbose "CC_TEST_CODE: $CC_TEST_CODE"
verbose "CC_PYTHON: $CC_PYTHON"
verbose "CC_ETC: $CC_ETC"
verbose "CC_BINARY: $CC_BINARY"

SCRIPTNAME=$1
shift

[ $SCRIPTNAME ] && "$CC_SCRIPTS/$SCRIPTNAME.sh" "$@" || echo "No script to run"


