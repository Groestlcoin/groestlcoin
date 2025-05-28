Groestlcoin Core
=============

Setup
---------------------
Groestlcoin Core is the original Groestlcoin client and it builds the backbone of the network. It downloads and, by default, stores the entire history of Groestlcoin transactions, which requires 3 gigabytes or more of disk space. Depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to several days or more.

To download Groestlcoin Core, visit [groestlcoin.org](https://groestlcoin.org/downloads/).

Running
---------------------
The following are some helpful notes on how to run Groestlcoin on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/groestlcoin-qt` (GUI) or
- `bin/groestlcoind` (headless)
- `bin/groestlcoin` (wrapper command)

The `groestlcoin` command supports subcommands like `groestlcoin gui`, `groestlcoin node`, and `groestlcoin rpc` exposing different functionality. Subcommands can be listed with `groestlcoin help`.

### Windows

Unpack the files into a directory, and then run groestlcoin-qt.exe.

### macOS

Drag Groestlcoin-Core to your applications folder, and then run Groestlcoin-Core.

### Need Help?

* Ask for help on #groestlcoin on Libera Chat. If you don't have an IRC client, you can use [web.libera.chat](https://web.libera.chat/#groestlcoin).
* Ask for help on the [GroestlTalk](https://groestlcointalk.org/) forum or in in the [Alternate cryptocurrencies board](https://bitcointalk.org/index.php?topic=525926.0).

Building
---------------------
The following are developer notes on how to build Groestlcoin on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows-msvc.md)
- [FreeBSD Build Notes](build-freebsd.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [NetBSD Build Notes](build-netbsd.md)

Development
---------------------
The Groestlcoin repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Productivity Notes](productivity.md)
- [Release Process](release-process.md)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [JSON-RPC Interface](JSON-RPC-interface.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)
- [Internal Design Docs](design/)

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [groestlcoin.conf Configuration File](groestlcoin-conf.md)
- [CJDNS Support](cjdns.md)
- [Files](files.md)
- [Fuzz-testing](fuzzing.md)
- [I2P Support](i2p.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [Managing Wallets](managing-wallets.md)
- [Multisig Tutorial](multisig-tutorial.md)
- [Offline Signing Tutorial](offline-signing-tutorial.md)
- [P2P bad ports definition and list](p2p-bad-ports.md)
- [PSBT support](psbt.md)
- [Reduce Memory](reduce-memory.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Transaction Relay Policy](policy/README.md)
- [ZMQ](zmq.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
