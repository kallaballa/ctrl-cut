#!/bin/bash

BASEDIR="`dirname $(readlink -f $0)`"
cd $BASEDIR

function sedpath(){
     echo $1
}

TEST_FILTER_PATH=`sedpath "$BASEDIR/test-code"`
FILTER_PATH="`cups-config --serverbin`/filter"
FILTER_PATH="`sedpath $FILTER_PATH`"

sed "s|\$FILTER|$TEST_FILTER_PATH/passthroughfilter|g" stubs/passthrough.ppd > passthrough.ppd
sed "s|\$FILTER|$FILTER_PATH/epilogcups|g" stubs/EpilogLegend36EXT.ppd > EpilogLegend36EXT.ppd
sed "s|\$FILTER|$FILTER_PATH/epilogcups|g" stubs/EpilogPostscript.ppd > EpilogPostscript.ppd

