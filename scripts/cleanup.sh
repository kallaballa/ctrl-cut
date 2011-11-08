#!/bin/bash

cd $CC_BASE

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"

findtests $dir | xargs  -I'[cc_test_dir]' rm " -I[cc_test_dir]/out/*"

