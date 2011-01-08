#!/bin/bash

cd "`dirname $(readlink -f $0)`"

function sedpath(){
     echo $1
}

TEST_FILTER_PATH=`sedpath "../../test-code"`
FILTER_PATH="`cups-config --serverbin`/filter"
FILTER_PATH="`sedpath $FILTER_PATH`"

echo "s|\$FILTER|$TEST_FILTER_PATH/passthroughfilter|g"
sed "s|\$FILTER|$TEST_FILTER_PATH/passthroughfilter|g" stubs/passthrough.ppd > passthrough.ppd
sed "s|\$FILTER|$FILTER_PATH/epilogcups|g" stubs/EpilogLegend36EXT.ppd > EpilogLegend36EXT.ppd
sed "s|\$FILTER|$FILTER_PATH/epilogcups|g" stubs/EpilogPostscript.ppd > EpilogPostscript.ppd

echo $TEST_FILTER_PATH
echo $FILTER_PATH
