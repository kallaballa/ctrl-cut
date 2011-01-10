#!/bin/bash

export EC_BASE="`dirname $(readlink -f $0)`"
export EC_TEST_CHROOT="$EC_BASE/chroot"
export EC_CHROOT_FLAVOUR="squeeze"
export EC_SCRIPTS="$EC_BASE/scripts"
export EC_GIT_URL="git://github.com/Metalab/epilogcups.git"
export EC_PPD_DIR="$EC_BASE/etc/ppd"
export EC_TEST_DATA="$EC_BASE/test-data"
export EC_TEST_CODE="$EC_BASE/test-code"
export EC_PYTHON="$EC_BASE/python"
export EC_ETC="$EC_BASE/etc"

echo "EC_BASE: $EC_BASE"
echo "EC_TEST_CHROOT: $EC_TEST_CHROOT"
echo "EC_CHROOT_FLAVOUR: $EC_CHROOT_FLAVOUR"
echo "EC_SCRIPTS: $EC_SCRIPTS"
echo "EC_GIT_URL: $EC_GIT_URL"
echo "EC_PPD_DIR: $EC_PPD_DIR"
echo "EC_TEST_DATA: $EC_TEST_DATA"
echo "EC_TEST_CODE: $EC_TEST_CODE"
echo "EC_PYTHON: $EC_PYTHON"
echo "EC_ETC: $EC_ETC"

SCRIPTNAME=$1
shift

"$EC_SCRIPTS/$SCRIPTNAME.sh" "$@"