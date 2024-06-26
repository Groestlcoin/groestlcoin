#!/usr/bin/env bash
#
# Copyright (c) 2019-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

export CI_IMAGE_NAME_TAG="docker.io/ubuntu:24.04"
export CONTAINER_NAME=ci_native_valgrind
export PACKAGES="valgrind clang-16 llvm-16 libclang-rt-16-dev python3-zmq libevent-dev libboost-dev libdb5.3++-dev libminiupnpc-dev libnatpmp-dev libzmq3-dev libsqlite3-dev"
export NO_DEPENDS=1
export GOAL="install"
export GROESTLCOIN_CONFIG="--enable-zmq --with-incompatible-bdb --with-gui=no CC=clang-16 CXX=clang++-16" # TODO enable GUI
