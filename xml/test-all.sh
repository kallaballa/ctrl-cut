#!/bin/bash
BASEDIR=`dirname $(readlink -f $0)`
tfilter="$BASEDIR/test-filter.sh $@"

$tfilter input
$tfilter explode
$tfilter join
$tfilter deonion

