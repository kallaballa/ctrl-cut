#!/bin/bash

# this script should be used to call all other project scripts. 
# it configures the necessary environment variables for the project context.
# e.g. to call "$EC_BASE/scripts/test-corel.sh <args>" do "./ec test-corel <args>"
# alternatively you may also source it and call scripts directly

# switch to bash 3.1 compatibility mode for mac osx
shopt -s compat31

cd `dirname $0` 

VERBOSE=

while getopts 'v' c
do
    case $c in
        v) export VERBOSE=-v; shift ;;
	--) shift; break;;
    esac
done



export EC_BASE="`pwd`"
export EC_TEST_CHROOT="$EC_BASE/chroot"
export EC_CHROOT_MIRROR="http://debian.inode.at/debian/"
export EC_CHROOT_FLAVOUR="squeeze"
export EC_SCRIPTS="$EC_BASE/scripts"
export EC_FUNCTIONS="$EC_SCRIPTS/functions.sh"
export EC_GIT_URL="git://github.com/Metalab/epilogcups.git"
export EC_PPD_DIR="$EC_BASE/etc/ppd"
export EC_TEST_DATA="$EC_BASE/test-data"
export EC_TEST_CODE="$EC_BASE/test-code"
export EC_PYTHON="$EC_BASE/python"
export EC_ETC="$EC_BASE/etc"

. $EC_FUNCTIONS

verbose "EC_BASE: $EC_BASE"
verbose "EC_TEST_CHROOT: $EC_TEST_CHROOT"
verbose "EC_CHROOT_FLAVOUR: $EC_CHROOT_FLAVOUR"
verbose "EC_SCRIPTS: $EC_SCRIPTS"
verbose "EC_FUNCTIONS: $EC_FUNCTIONS"
verbose "EC_GIT_URL: $EC_GIT_URL"
verbose "EC_PPD_DIR: $EC_PPD_DIR"
verbose "EC_TEST_DATA: $EC_TEST_DATA"
verbose "EC_TEST_CODE: $EC_TEST_CODE"
verbose "EC_PYTHON: $EC_PYTHON"
verbose "EC_ETC: $EC_ETC"

SCRIPTNAME=$1
shift

[ $SCRIPTNAME ] && "$EC_SCRIPTS/$SCRIPTNAME.sh" "$@" || echo "No script to run"


