Groestlcoin Core version 25.0 is now available from:

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
using the Linux kernel, macOS 10.15+, and Windows 7 and newer. Groestlcoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them. It is not recommended to use Groestlcoin Core on
unsupported systems.

Notable changes
===============

P2P and network changes
-----------------------

- Transactions of non-witness size 65 and above are now allowed by mempool
  and relay policy. This is to better reflect the actual afforded protections
  against CVE-2017-12842 and open up additional use-cases of smaller transaction sizes.

New RPCs
--------

- The scanblocks RPC returns the relevant blockhashes from a set of descriptors by
  scanning all blockfilters in the given range. It can be used in combination with
  the getblockheader and rescanblockchain RPCs to achieve fast wallet rescans. Note
  that this functionality can only be used if a compact block filter index
  (-blockfilterindex=1) has been constructed by the node.

Updated RPCs
------------

- All JSON-RPC methods accept a new [named
  parameter](JSON-RPC-interface.md#parameter-passing) called `args` that can
  contain positional parameter values. This is a convenience to allow some
  parameter values to be passed by name without having to name every value. The
  python test framework and `groestlcoin-cli` tool both take advantage of this, so
  for example:

```sh
groestlcoin-cli -named createwallet wallet_name=mywallet load_on_startup=1
```

Can now be shortened to:

```sh
groestlcoin-cli -named createwallet mywallet load_on_startup=1
```

- The `verifychain` RPC will now return `false` if the checks didn't fail,
  but couldn't be completed at the desired depth and level. This could be due
  to missing data while pruning, due to an insufficient dbcache or due to
  the node being shutdown before the call could finish.

- `sendrawtransaction` has a new, optional argument, `maxburnamount` with a default value of `0`.
  Any transaction containing an unspendable output with a value greater than `maxburnamount` will
  not be submitted. At present, the outputs deemed unspendable are those with scripts that begin
  with an `OP_RETURN` code (known as 'datacarriers'), scripts that exceed the maximum script size,
  and scripts that contain invalid opcodes.

- The `testmempoolaccept` RPC now returns 2 additional results within the "fees" result:
  "effective-feerate" is the feerate including fees and sizes of transactions validated together if
  package validation was used, and also includes any modified fees from prioritisetransaction. The
  "effective-includes" result lists the wtxids of transactions whose modified fees and sizes were used
  in the effective-feerate.

- `decodescript` may now infer a Miniscript descriptor under P2WSH context if it is not lacking
  information.

- `finalizepsbt` is now able to finalize a transaction with inputs spending Miniscript-compatible
  P2WSH scripts.

Changes to wallet related RPCs can be found in the Wallet section below.

Build System
------------

- The `--enable-upnp-default` and `--enable-natpmp-default` options
  have been removed. If you want to use port mapping, you can
  configure it using a .conf file, or by passing the relevant
  options at runtime.

Updated settings
----------------

- If the `-checkblocks` or `-checklevel` options are explicitly provided by the
user, but the verification checks cannot be completed due to an insufficient
dbcache, Groestlcoin Core will now return an error at startup.

- Ports specified in `-port` and `-rpcport` options are now validated at startup.
  Values that previously worked and were considered valid can now result in errors.

- Setting `-blocksonly` will now reduce the maximum mempool memory
  to 5MB (users may still use `-maxmempool` to override). Previously,
  the default 300MB would be used, leading to unexpected memory usage
  for users running with `-blocksonly` expecting it to eliminate
  mempool memory usage.

  As unused mempool memory is shared with dbcache, this also reduces
  the dbcache size for users running with `-blocksonly`, potentially
  impacting performance.
- Setting `-maxconnections=0` will now disable `-dnsseed`
  and `-listen` (users may still set them to override).

Changes to GUI or wallet related settings can be found in the GUI or Wallet section below.

New settings
------------

- The `shutdownnotify` option is used to specify a command to execute synchronously
before Groestlcoin Core has begun its shutdown sequence.


Wallet
------

- The `minconf` option, which allows a user to specify the minimum number
of confirmations a UTXO being spent has, and the `maxconf` option,
which allows specifying the maximum number of confirmations, have been
added to the following RPCs:
  - `fundrawtransaction`
  - `send`
  - `walletcreatefundedpsbt`
  - `sendall`

- Added a new `next_index` field in the response in `listdescriptors` to
  have the same format as `importdescriptors`.

- RPC `listunspent` now has a new argument `include_immature_coinbase`
  to include coinbase UTXOs that don't meet the minimum spendability
  depth requirement (which before were silently skipped).

- Rescans for descriptor wallets are now significantly faster if compact
  block filters (BIP158) are available. Since those are not constructed
  by default, the configuration option "-blockfilterindex=1" has to be
  provided to take advantage of the optimization. This improves the
  performance of the RPC calls `rescanblockchain`, `importdescriptors`
  and `restorewallet`.

- RPC `unloadwallet` now fails if a rescan is in progress.

- Wallet passphrases may now contain null characters.
  Prior to this change, only characters up to the first
  null character were recognized and accepted.

- Address Purposes strings are now restricted to the currently known values of "send",
  "receive", and "refund". Wallets that have unrecognized purpose strings will have
  loading warnings, and the `listlabels` RPC will raise an error if an unrecognized purpose
  is requested.

- In the `createwallet`, `loadwallet`, `unloadwallet`, and `restorewallet` RPCs, the
  "warning" string field is deprecated in favor of a "warnings" field that
  returns a JSON array of strings to better handle multiple warning messages and
  for consistency with other wallet RPCs. The "warning" field will be fully
  removed from these RPCs in v26. It can be temporarily re-enabled during the
  deprecation period by launching groestlcoind with the configuration option
  `-deprecatedrpc=walletwarningfield`.

- Descriptor wallets can now spend coins sent to P2WSH Miniscript descriptors.

GUI changes
-----------

- The "Mask values" is a persistent option now.
- The "Mask values" option affects the "Transaction" view now, in addition to the
  "Overview" one.

REST
----

- A new `/rest/deploymentinfo` endpoint has been added for fetching various
  state info regarding deployments of consensus changes.

Binary verification
----

- The binary verification script has been updated. In previous releases it
  would verify that the binaries had been signed with a single "release key".
  In this release and moving forward it will verify that the binaries are
  signed by a _threshold of trusted keys_. For more details and
  examples, see:
  https://github.com/Groestlcoin/groestlcoin/blob/master/contrib/verify-binaries/README.md

Low-level changes
=================

RPC
---

- The JSON-RPC server now rejects requests where a parameter is specified multiple
  times with the same name, instead of silently overwriting earlier parameter values
  with later ones.
- RPC `listsinceblock` now accepts an optional `label` argument
  to fetch incoming transactions having the specified label.
- Previously `setban`, `addpeeraddress`, `walletcreatefundedpsbt`, methods
  allowed non-boolean and non-null values to be passed as boolean parameters.
  Any string, number, array, or object value that was passed would be treated
  as false. After this change, passing any value except `true`, `false`, or
  `null` now triggers a JSON value is not of expected type error.

Credits
=======

Thanks to everyone who directly contributed to this release.

As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com/bitcoin/bitcoin/).
