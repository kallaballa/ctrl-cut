#!/bin/bash

cd xml

./test-filter.sh $@ input
./test-filter.sh $@ explode
./test-filter.sh $@ join
./test-filter.sh $@ deonion

