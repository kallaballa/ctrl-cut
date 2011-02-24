#!/bin/bash

XQ=`type -p xqilla`
function usage { echo "test-filter.sh [-v] filtername"; }
function green { echo -e "\033[32;1m $1 \033[0m"; tput sgr0; }
function red { echo -e "\033[31;1m $1 \033[0m"; tput sgr0; }
function verbose  { [ $VERBOSE ] && echo $@; $@; }
function error { echo "error: $1" >&2; exit 1; }
function check { 
    echo -en "\tcheck: `basename $1` `basename $2` "
    cmd="$XQ -i $1 $2"
    [ $VERBOSE ] && echo -n "($cmd)";
    echo -n " ... "
    result="`$cmd`"
    if [ "$result" == "0" ]; then
        green "OK"
        return 0
    else
        red $result
        return 1
    fi
}

### main

[ $# -eq 0 ] && usage

res=0
for filter in "$@"; do
    echo "testing `basename $filter`..."
    for xql in $CC_TEST_DATA/xml/xql/*.xql
    do
        check $filter $xql
        res=$(($res || $?))
    done
done

exit $res
