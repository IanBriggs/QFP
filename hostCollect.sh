#!/bin/bash -x

#this is for Cloudlab
if [[ ! -e "~/.postgresql" ]]; then
    mkdir ~/.postgresql
    cp /local/.postgresql/* ~/.postresql/
fi

mkdir -p results
#rm results/*
cd qfpc
make -j $1

if [[ $VERBOSE != 'verbose' ]]; then
    cd ../results
    tar -zxf *.tgz
    cat *out_ >> masterRes_$(hostname)
    # git add masterRes_$(hostname)
    # git commit -m 'data collecton from $(hostname)'
    # git push
    # $(which psql) -d qfp -h bihexal.cs.utah.edu -U sawaya -c "select importQFPResults('$PWD/masterRes');"
    exit $?
fi
exit 0
