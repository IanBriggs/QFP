#!/bin/bash -x

if [[ ! -e results ]]; then
    mkdir results
fi
rm results/*
cd perpVects
make -j $1 -f Makefile2

if [[ $VERBOSE != 'verbose' ]]; then
    cd ../results
    tar -zxf *.tgz
    cat *out_ >> masterRes_$(hostname)
    exit $?
fi
exit 0
