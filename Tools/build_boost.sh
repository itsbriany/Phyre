#!/bin/sh

if [ -z $BOOST_ROOT ]; then
    echo "Could not find BOOST_ROOT"
    exit 1
fi

pushd $BOOST_ROOT
./b2 -j 8 link=static runtime-link=static threading=multi address-model=64
popd
