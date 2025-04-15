# Multiprocess Groestlcoin

_This document describes usage of the multiprocess feature. For design information, see the [design/multiprocess.md](design/multiprocess.md) file._

## Build Option

On Unix systems, the `-DENABLE_IPC=ON` build option can be passed to build the supplemental `groestlcoin-node` and `groestlcoin-gui` multiprocess executables.

## Debugging

The `-debug=ipc` command line option can be used to see requests and responses between processes.

## Installation

Specifying `-DENABLE_IPC=ON` requires [Cap'n Proto](https://capnproto.org/) to be installed. See [build-unix.md](build-unix.md) and [build-osx.md](build-osx.md) for information about installing dependencies.

### Depends installation

Alternately the [depends system](../depends) can be used to avoid need to install local dependencies. A simple way to get started is to pass the `MULTIPROCESS=1` [dependency option](../depends#dependency-options) to make:

```
cd <GROESTLCOIN_SOURCE_DIRECTORY>
make -C depends NO_QT=1 MULTIPROCESS=1
# Set host platform to output of gcc -dumpmachine or clang -dumpmachine or check the depends/ directory for the generated subdirectory name
HOST_PLATFORM="x86_64-pc-linux-gnu"
cmake -B build --toolchain=depends/$HOST_PLATFORM/toolchain.cmake
cmake --build build
build/bin/groestlcoin-node -regtest -printtoconsole -debug=ipc
GROESTLCOIND=$(pwd)/build/bin/groestlcoin-node build/test/functional/test_runner.py
```

The `cmake` build will pick up settings and library locations from the depends directory, so there is no need to pass `-DENABLE_IPC=ON` as a separate flag when using the depends system (it's controlled by the `MULTIPROCESS=1` option).

### Cross-compiling

When cross-compiling and not using depends, native code generation tools from [libmultiprocess](https://github.com/bitcoin-core/libmultiprocess) and [Cap'n Proto](https://capnproto.org/) are required. They can be passed to the cmake build by specifying `-DMPGEN_EXECUTABLE=/path/to/mpgen -DCAPNP_EXECUTABLE=/path/to/capnp -DCAPNPC_CXX_EXECUTABLE=/path/to/capnpc-c++` options.

## Usage

`groestlcoin-node` is a drop-in replacement for `groestlcoind`, and `groestlcoin-gui` is a drop-in replacement for `groestlcoin-qt`, and there are no differences in use or external behavior between the new and old executables. But internally after [#10102](https://github.com/bitcoin/bitcoin/pull/10102), `groestlcoin-gui` will spawn a `groestlcoin-node` process to run P2P and RPC code, communicating with it across a socket pair, and `groestlcoin-node` will spawn `groestlcoin-wallet` to run wallet code, also communicating over a socket pair. This will let node, wallet, and GUI code run in separate address spaces for better isolation, and allow future improvements like being able to start and stop components independently on different machines and environments.
[#19460](https://github.com/bitcoin/bitcoin/pull/19460) also adds a new `groestlcoin-node` `-ipcbind` option and a `groestlcoind-wallet` `-ipcconnect` option to allow new wallet processes to connect to an existing node process.
And [#19461](https://github.com/bitcoin/bitcoin/pull/19461) adds a new `groestlcoin-gui` `-ipcconnect` option to allow new GUI processes to connect to an existing node process.
