#!/bin/bash

# Usage: ./run_tests.sh <gtest filter pattern>
# Example: ./run_tests.sh TCP*
# Runs all tests starting with TCP.

GTEST_FILTER=$1
export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib

cd $GAME_ENGINE_ROOT
for TEST_RESOURCE_DIRECTORY in `find $PWD GameEngine -name "*TestResources"`; do
    cp -r $TEST_RESOURCE_DIRECTORY $GAME_ENGINE_ROOT/GameEngine/GameEngineTesting 2> /dev/null
done

cd $GAME_ENGINE_ROOT/GameEngine/GameEngineTesting
./GameEngineTests --gtest_filter=$GTEST_FILTER
cd $GAME_ENGINE_ROOT
