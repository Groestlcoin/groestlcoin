Groestlcoin Core version 27.0 is now available from:

  <https://www.groestlcoin.org/groestlcoin-core-wallet/>

This release includes new features, various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/groestlcoin/groestlcoin/issues>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Groestlcoin-Qt` (on macOS)
or `groestlcoind`/`groestlcoin-qt` (on Linux).

Upgrading directly from a version of Groestlcoin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of Groestlcoin Core are generally supported.

Compatibility
==============

Groestlcoin Core is supported and extensively tested on operating systems
using the Linux Kernel 3.17+, macOS 11.0+, and Windows 7 and newer. Groestlcoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them. It is not recommended to use Groestlcoin Core on
unsupported systems.

Notable changes
===============

libgroestlcoinconsensus
-------------------

- libgroestlcoinconsensus is deprecated and will be removed for v28. This library has
  existed for nearly 10 years with very little known uptake or impact. It has
  become a maintenance burden.

  The underlying functionality does not change between versions, so any users of
  the library can continue to use the final release indefinitely, with the
  understanding that Taproot is its final consensus update.

  In the future, libgroestlcoinkernel will provide a much more useful API that is
  aware of the UTXO set, and therefore be able to fully validate transactions and
  blocks.

mempool.dat compatibility
-------------------------

- The `mempool.dat` file created by -persistmempool or the savemempool RPC will
  be written in a new format. This new format includes the XOR'ing of transaction
  contents to mitigate issues where external programs (such as anti-virus) attempt
  to interpret and potentially modify the file.

  This new format can not be read by previous software releases. To allow for a
  downgrade, a temporary setting `-persistmempoolv1` has been added to fall back
  to the legacy format.

P2P and network changes
-----------------------

- BIP324 v2 transport is now enabled by default. It remains possible to disable v2
  by running with `-v2transport=0`.
- Manual connection options (`-connect`, `-addnode` and `-seednode`) will
  now follow `-v2transport` to connect with v2 by default. They will retry with
  v1 on failure.

- Network-adjusted time has been removed from consensus code. It is replaced
  with (unadjusted) system time. The warning for a large median time offset
  (70 minutes or more) is kept. This removes the implicit security assumption of
  requiring an honest majority of outbound peers, and increases the importance
  of the node operator ensuring their system time is (and stays) correct to not
  fall out of consensus with the network.

Mempool Policy Changes
----------------------

- Opt-in Topologically Restricted Until Confirmation (TRUC) Transactions policy
  (aka v3 transaction policy) is available for use on test networks when
  `-acceptnonstdtxn=1` is set. By setting the transaction version number to 3, TRUC transactions
  request the application of limits on spending of their unconfirmed outputs. These
  restrictions simplify the assessment of incentive compatibility of accepting or
  replacing TRUC transactions, thus ensuring any replacements are more profitable for
  the node and making fee-bumping more reliable. TRUC transactions are currently
  nonstandard and can only be used on test networks where the standardness rules are
  relaxed or disabled (e.g. with `-acceptnonstdtxn=1`).

External Signing
----------------

- Support for external signing on Windows has been disabled. It will be re-enabled
  once the underlying dependency (Boost Process), has been replaced with a different
  library.

Updated RPCs
------------

- The addnode RPC now follows the `-v2transport` option (now on by default, see above) for making connections.
  It remains possible to specify the transport type manually with the v2transport argument of addnode.

Build System
------------

- A C++20 capable compiler is now required to build Groestlcoin Core.
- MacOS releases are configured to use the hardened runtime libraries

Wallet
------

- The CoinGrinder coin selection algorithm has been introduced to mitigate unnecessary
  large input sets and lower transaction costs at high feerates. CoinGrinder
  searches for the input set with minimal weight. Solutions found by
  CoinGrinder will produce a change output. CoinGrinder is only active at
  elevated feerates (default: 30+ sat/vB, based on `-consolidatefeerate`×3).
- The Branch And Bound coin selection algorithm will be disabled when the subtract fee
  from outputs feature is used.
- If the birth time of a descriptor is detected to be later than the first transaction
  involving that descriptor, the birth time will be reset to the earlier time.

Low-level changes
=================

Pruning
-------

- When pruning during initial block download, more blocks will be pruned at each
  flush in order to speed up the syncing of such nodes.

Init
----

- Various fixes to prevent issues where subsequent instances of Groestlcoin Core would
  result in deletion of files in use by an existing instance.
- Improved handling of empty `settings.json` files.

Credits
=======

Thanks to everyone who directly contributed to this release.

As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com/bitcoin/bitcoin/).
