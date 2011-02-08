#!/bin/bash

. $CC_FUNCTIONS

tfilter="$CC_SCRIPTS/xml-test-filter.sh $@"
try "checking input" "$tfilter input"
try "checking explode" "$tfilter explode"
try "checking join" "$tfilter join"
try "checkung deonion" "$tfilter deonion"


