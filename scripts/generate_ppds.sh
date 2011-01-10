#!/bin/bash

sed "s|\$FILTER|passthroughfilter|g" $EC_PPD_DIR/stubs/passthrough.ppd > $EC_PPD_DIR/passthrough.ppd
sed "s|\$FILTER|epilogcups|g" $EC_PPD_DIR/stubs/EpilogLegend36EXT.ppd > $EC_PPD_DIR/EpilogLegend36EXT.ppd
sed "s|\$FILTER|epilogcups|g" $EC_PPD_DIR/stubs/EpilogPostscript.ppd > $EC_PPD_DIR/EpilogPostscript.ppd

