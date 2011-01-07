#!/bin/sh
# cd to epilogcups basedir
cd "`dirname $(readlink -f $0)`/.."

gs -sDEVICE=nullpage -q -dBATCH -dNOPAUSE -r600 -g14400x7200 -
