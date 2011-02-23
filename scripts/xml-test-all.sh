#!/bin/bash

. $CC_FUNCTIONS

# Makes unmatched globs return a zero list instead of literal glob characters
shopt -s nullglob

rundir()
{
  for testdir in $@
  do
    testtype=`basename $testdir`
    echo "[$testtype]"
    for f in $testdir/*.xml; do
      $CC_SCRIPTS/xml-test-filter.sh $f
    done
  done
}

testdirs="corel qcad inkscape"
testpaths=""
for testdir in $testdirs
do
  testpaths="$testpaths $CC_TEST_DATA/$testdir"
done
rundir $testpaths
