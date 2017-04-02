#!/bin/sh

# Bail on error
set -e

if [ -z $PHYRE_ROOT ]; then
    echo "Please point the PHYRE_ROOT environment variable to the Phyre root directory"
    exit 1
fi

BIN_DIR=$PHYRE_ROOT/Build/Bin
PHYRE_CONFIG=$PHYRE_ROOT/phyre.json

$BIN_DIR/PhyreTests $PHYRE_CONFIG
