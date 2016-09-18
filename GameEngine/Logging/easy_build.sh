#!/bin/bash

clang++ -DBOOST_LOG_DYN_LINK -std=c++14 main.cpp -I$BOOST_ROOT -L$BOOST_ROOT/stage/lib -lboost_log -lpthread
