#!/bin/sh

set -e

if [ -z $PHYRE_ROOT ]; then
    echo "Please point the PHYRE_ROOT environment variable to the Phyre root directory"
    exit 1
fi

git submodule update --init --recursive

BUILD_DIR="Build"
pushd $PHYRE_ROOT
mkdir $BUILD_DIR
pushd $BUILD_DIR
if [ -z $TRAVIS ]; then
    cmake ..
else
    cmake .. -DBOOST_LOG_DYN_LINK=ON
fi
make -j
popd
popd
