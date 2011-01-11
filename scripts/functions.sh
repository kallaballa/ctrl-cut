#!/bin/bash

function green { echo -e "\033[32;1m $1 \033[0m"; tput sgr0; }
function red { echo -e "\033[31;1m $1 \033[0m"; tput sgr0; }
function warn { red "$1"; }
function error { red "failed" >&2; exit 1; }
function ok { green "ok"; }
function try { echo -n "$1 "; [ $VERBOSE ] && echo -n "$2 "; $2 1> /dev/null && ok || error; }

