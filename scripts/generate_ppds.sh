#!/bin/bash

TEST_FILTER_PATH="$EC_TEST_CODE"
FILTER_PATH="`cups-config --serverbin`/filter"

sed "s|\$FILTER|$TEST_FILTER_PATH/passthroughfilter|g" $EC_PPD_DIR/stubs/passthrough.ppd > $EC_PPD_DIR/passthrough.ppd
sed "s|\$FILTER|$FILTER_PATH/epilogcups|g" $EC_PPD_DIR/stubs/EpilogLegend36EXT.ppd > $EC_PPD_DIR/EpilogLegend36EXT.ppd
sed "s|\$FILTER|$FILTER_PATH/epilogcups|g" $EC_PPD_DIR/stubs/EpilogPostscript.ppd > $EC_PPD_DIR/EpilogPostscript.ppd

