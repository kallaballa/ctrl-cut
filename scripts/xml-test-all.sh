#!/bin/bash

. $EC_FUNCTIONS

tfilter="$EC_SCRIPTS/xml-test-filter.sh $@"
try "checking input..." "$tfilter input"
try "checking explode..." "$tfilter explode"
try "checking join..." "$tfilter join"
try "checkung deonion..." "$tfilter deonion"


