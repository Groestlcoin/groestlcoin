#!/usr/bin/env bash
#
# Copyright (c) 2019-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

export CONTAINER_NAME=ci_native_previous_releases
export CI_IMAGE_NAME_TAG="mirror.gcr.io/ubuntu:22.04"
# Use minimum supported python3.10 and gcc-11, see doc/dependencies.md
export PACKAGES="gcc-11 g++-11 python3-zmq"
export DEP_OPTS="CC=gcc-11 CXX=g++-11"
export RUN_UNIT_TESTS_SEQUENTIAL="false"
export RUN_UNIT_TESTS="false"
export GOAL="install"
export DOWNLOAD_PREVIOUS_RELEASES="true"
export GROESTLCOIN_CONFIG="\
 -DWITH_ZMQ=ON -DBUILD_GUI=ON -DREDUCE_EXPORTS=ON \
 -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_C_FLAGS='-funsigned-char' \
 -DCMAKE_C_FLAGS_DEBUG='-g2 -O2' \
 -DCMAKE_CXX_FLAGS='-funsigned-char' \
 -DCMAKE_CXX_FLAGS_DEBUG='-g2 -O2' \
 -DAPPEND_CPPFLAGS='-DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE' \
"
