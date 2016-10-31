#!/bin/bash

# Stop on error
set -e

export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib
for TEST in `find GameEngine -name "*Tests"`; do
    ./$TEST
done
