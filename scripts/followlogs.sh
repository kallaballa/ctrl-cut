#!/bin/bash

cd $CC_BASE

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"

findcases $dir | while read casedir; do
  casename="`basename $casedir`"
  echo "`dirname $casedir`/out/$casename/$casename.log"
done | xargs -I'[cc_caselog]' tail -n0 -q --retry -f -s0.3 "[cc_caselog]" 2> /dev/null

