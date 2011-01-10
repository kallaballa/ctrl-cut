#!/bin/bash


tfilter="$EC_SCRIPTS/xml-test-filter.sh $@"
$tfilter input
$tfilter explode
$tfilter join
$tfilter deonion

