#!/bin/bash

cd $CC_BASE

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"

findcases $dir | while read c; do
  rm -rf $c/out/*;
done

