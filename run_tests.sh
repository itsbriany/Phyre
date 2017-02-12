#!/bin/bash

# Usage: ./run_tests.sh <gtest filter pattern>
# Example: ./run_tests.sh TCP*
# Runs all tests starting with TCP.

GTEST_FILTER=$1
export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib

cd $PHYRE_ROOT
for TEST_RESOURCE_DIRECTORY in `find $PWD GameEngine -name "*TestResources"`; do
    cp -r $TEST_RESOURCE_DIRECTORY $PHYRE_ROOT/GameEngine/GameEngineTesting 2> /dev/null
done

cd $PHYRE_ROOT/GameEngine/GameEngineTesting
if [[ -z "$GTEST_FILTER" ]]; then
    ./GameEngineTests
else
    ./GameEngineTests --gtest_filter=$GTEST_FILTER
fi
cd $PHYRE_ROOT
