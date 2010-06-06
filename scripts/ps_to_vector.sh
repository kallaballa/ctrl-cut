#!/bin/sh
`dirname $0`/../test-code/ps_to_eps $1 | gs -sDEVICE=nullpage -q -dBATCH -dNOPAUSE -r600 -g21600x14400 -
