#!/bin/bash

# Stop on error
set -e

export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib
for TEST in `find GameEngine -name "*Tests"`; do
    DIRECTORY=`dirname $TEST`
    cd $DIRECTORY
    for TEST_EXECUTABLE in `find . -depth 1 -name "*Tests"`; do
        ./$TEST_EXECUTABLE
    done
    cd $GAME_ENGINE_ROOT
done

