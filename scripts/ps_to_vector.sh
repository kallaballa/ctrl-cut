#!/bin/sh

. $EC_FUNCTIONS

try "ps_to_eps | nullpage..." "$EC_TEST_CODE/ps_to_eps $1 | gs -sDEVICE=nullpage -q -dBATCH -dNOPAUSE -r600 -g21600x14400 -"
