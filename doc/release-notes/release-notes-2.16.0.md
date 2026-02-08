Groestlcoin Core version *2.16.0* is now available from:

  <https://groestlcoin.org/downloads/>

This is a new major version release, including various bugfixes and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/groestlcoin/groestlcoin/issues>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over `/Applications/Groestlcoin-Qt` (on Mac)
or `groestlcoind`/`groestlcoin-qt` (on Linux).

The first time you run version 2.16.0, your chainstate database will
be converted to a new format, which will take anywhere from a few minutes to
half an hour, depending on the speed of your machine.

The file format of `fee_estimates.dat` changed in version 2.16.0. Hence, a
downgrade from version 2.16 or upgrade to version 2.16 will cause all fee
estimates to be discarded.

Downgrading warning
-------------------

Wallets created in 2.16 and later are not compatible with versions prior to 2.16 and will not work if you try to use newly created wallets in older versions. Existing wallets that were created with older versions are not affected by this.

Compatibility
==============

Groestlcoin Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.8+, and Windows Vista and later. Windows XP is not supported.

Groestlcoin Core should also work on most other Unix-like systems but is not
frequently tested on them.


Notable changes
===============

Performance Improvements
--------------

Validation speed and network propagation performance have been greatly
improved, leading to much shorter sync and initial block download times.

- The script signature cache has been reimplemented as a "cuckoo cache",
  allowing for more signatures to be cached and faster lookups.
- Assumed-valid blocks have been introduced which allows script validation to
  be skipped for ancestors of known-good blocks, without changing the security
  model. See below for more details.
- In some cases, compact blocks are now relayed before being fully validated as
  per BIP152.
- P2P networking has been refactored with a focus on concurrency and
  throughput. Network operations are no longer bottlenecked by validation. As a
  result, block fetching is several times faster than previous releases in many
  cases.
- The UTXO cache now claims unused mempool memory. This speeds up initial block
  download as UTXO lookups are a major bottleneck there, and there is no use for
  the mempool at that stage.


Manual Pruning
--------------

Groestlcoin Core has supported automatically pruning the blockchain since 2.11.0 Pruning
the blockchain allows for significant storage space savings as the vast majority of
the downloaded data can be discarded after processing so very little of it remains
on the disk.

Manual block pruning can now be enabled by setting `-prune=1`. Once that is set,
the RPC command `pruneblockchain` can be used to prune the blockchain up to the
specified height or timestamp.

`getinfo` Deprecated
--------------------

The `getinfo` RPC command has been deprecated. Each field in the RPC call
has been moved to another command's output with that command also giving
additional information that `getinfo` did not provide. The following table
shows where each field has been moved to:

|`getinfo` field   | Moved to                                  |
|------------------|-------------------------------------------|
`"version"`       | `getnetworkinfo()["version"]`
`"protocolversion"`| `getnetworkinfo()["protocolversion"]`
`"walletversion"`  | `getwalletinfo()["walletversion"]`
`"balance"`       | `getwalletinfo()["balance"]`
`"blocks"`       | `getblockchaininfo()["blocks"]`
`"timeoffset"`       | `getnetworkinfo()["timeoffset"]`
`"connections"`       | `getnetworkinfo()["connections"]`
`"proxy"`       | `getnetworkinfo()["networks"][0]["proxy"]`
`"difficulty"`       | `getblockchaininfo()["difficulty"]`
`"testnet"`       | `getblockchaininfo()["chain"] == "test"`
`"keypoololdest"`  | `getwalletinfo()["keypoololdest"]`
`"keypoolsize"`       | `getwalletinfo()["keypoolsize"]`
`"unlocked_until"` | `getwalletinfo()["unlocked_until"]`
`"paytxfee"`       | `getwalletinfo()["paytxfee"]`
`"relayfee"`       | `getnetworkinfo()["relayfee"]`
`"errors"`       | `getnetworkinfo()["warnings"]`

ZMQ On Windows
--------------

Previously the ZeroMQ notification system was unavailable on Windows
due to various issues with ZMQ. These have been fixed upstream and
now ZMQ can be used on Windows. Please see [this document](https://github.com/bitcoin/bitcoin/blob/master/doc/zmq.md) for
help with using ZMQ in general.

Nested RPC Commands in Debug Console
------------------------------------

The ability to nest RPC commands has been added to the debug console. This
allows users to have the output of a command become the input to another
command without running the commands separately.

The nested RPC commands use bracket syntax (i.e. `getwalletinfo()`) and can
be nested (i.e. `getblock(getblockhash(1))`). Simple queries can be
done with square brackets where object values are accessed with either an
array index or a non-quoted string (i.e. `listunspent()[0][txid]`). Both
commas and spaces can be used to separate parameters in both the bracket syntax
and normal RPC command syntax.

Network Activity Toggle
-----------------------

A RPC command and GUI toggle have been added to enable or disable all p2p
network activity. The network status icon in the bottom right hand corner
is now the GUI toggle. Clicking the icon will either enable or disable all
p2p network activity. If network activity is disabled, the icon will
be grayed out with an X on top of it.

Additionally the `setnetworkactive` RPC command has been added which does
the same thing as the GUI icon. The command takes one boolean parameter,
`true` enables networking and `false` disables it.

Out-of-sync Modal Info Layer
----------------------------

When Groestlcoin Core is out-of-sync on startup, a semi-transparent information
layer will be shown over top of the normal display. This layer contains
details about the current sync progress and estimates the amount of time
remaining to finish syncing. This layer can also be hidden and subsequently
unhidden by clicking on the progress bar at the bottom of the window.

Support for JSON-RPC Named Arguments
------------------------------------

Commands sent over the JSON-RPC interface and through the `groestlcoin-cli` binary
can now use named arguments. This follows the [JSON-RPC specification](http://www.jsonrpc.org/specification)
for passing parameters by-name with an object.

`groestlcoin-cli` has been updated to support this by parsing `name=value` arguments
when the `-named` option is given.

Some examples:

    src/groestlcoin-cli -named help command="help"
    src/groestlcoin-cli -named getblockhash height=0
    src/groestlcoin-cli -named getblock blockhash=0000000000000036ae5aabb18a2d345d19d60ba1927f071af2aa8eec7a27d41d
    src/groestlcoin-cli -named sendtoaddress address="(snip)" amount="1.0" subtractfeefromamount=true

The order of arguments doesn't matter in this case. Named arguments are also
useful to leave out arguments that should stay at their default value. The
rarely-used arguments `comment` and `comment_to` to `sendtoaddress`, for example, can
be left out. However, this is not yet implemented for many RPC calls, this is
expected to land in a later release.

The RPC server remains fully backwards compatible with positional arguments.

Opt into RBF When Sending
-------------------------

A new startup option, `-walletrbf`, has been added to allow users to have all
transactions sent opt into RBF support. The default value for this option is
currently `false`, so transactions will not opt into RBF by default. The new
`bumpfee` RPC can be used to replace transactions that opt into RBF.

Sensitive Data Is No Longer Stored In Debug Console History
-----------------------------------------------------------

The debug console maintains a history of previously entered commands that can be
accessed by pressing the Up-arrow key so that users can easily reuse previously
entered commands. Commands which have sensitive information such as passphrases and
private keys will now have a `(...)` in place of the parameters when accessed through
the history.

Retaining the Mempool Across Restarts
-------------------------------------

The mempool will be saved to the data directory prior to shutdown
to a `mempool.dat` file. This file preserves the mempool so that when the node
restarts the mempool can be filled with transactions without waiting for new transactions
to be created. This will also preserve any changes made to a transaction through
commands such as `prioritisetransaction` so that those changes will not be lost.

GUI Changes
-----------

 - After resetting the options by clicking the `Reset Options` button
   in the options dialog or with the `-resetguioptions` startup option,
   the user will be prompted to choose the data directory again. This
   is to ensure that custom data directories will be kept after the
   option reset which clears the custom data directory set via the choose
   datadir dialog.

 - Multiple peers can now be selected in the list of peers in the debug
   window. This allows for users to ban or disconnect multiple peers
   simultaneously instead of banning them one at a time.

 - An indicator has been added to the bottom right hand corner of the main
   window to indicate whether the wallet being used is a HD wallet. This
   icon will be grayed out with an X on top of it if the wallet is not a
   HD wallet.

HTTP REST Changes
-----------------

 - UTXO set query (`GET /rest/getutxos/<checkmempool>/<txid>-<n>/<txid>-<n>
   /.../<txid>-<n>.<bin|hex|json>`) responses were changed to return status
   code `HTTP_BAD_REQUEST` (400) instead of `HTTP_INTERNAL_SERVER_ERROR` (500)
   when requests contain invalid parameters.

Minimum Fee Rate Policies
-------------------------

Since the changes in 2.13.3 to automatically limit the size of the mempool and improve the performance of block creation in mining code it has not been important for relay nodes or miners to set `-minrelaytxfee`. With this release the following concepts that were tied to this option have been separated out:
- incremental relay fee used for calculating BIP 125 replacement and mempool limiting. (1000 gros/kB)
- calculation of threshold for a dust output. (effectively 3 * 1000 gros/kB)
- minimum fee rate of a package of transactions to be included in a block created by the mining code. If miners wish to set this minimum they can use the new `-blockmintxfee` option.  (defaults to 1000 gros/kB)

The `-minrelaytxfee` option continues to exist but is recommended to be left unset.

Fee Estimation Changes
----------------------

- Since 2.13.2 fee estimation for a confirmation target of 1 block has been
  disabled. The fee slider will no longer be able to choose a target of 1 block.
  This is only a minor behavior change as there was often insufficient
  data for this target anyway. `estimatefee 1` will now always return -1 and
  `estimatesmartfee 1` will start searching at a target of 2.

- The default target for fee estimation is changed to 6 blocks in both the GUI
  (previously 25) and for RPC calls (previously 2).

Removal of Priority Estimation
------------------------------

- Estimation of "priority" needed for a transaction to be included within a target
  number of blocks has been removed.  The RPC calls are deprecated and will either
  return -1 or 1e24 appropriately. The format for `fee_estimates.dat` has also
  changed to no longer save these priority estimates. It will automatically be
  converted to the new format which is not readable by prior versions of the
  software.

- Support for "priority" (coin age) transaction sorting for mining is
  considered deprecated in Core and will be removed in the next major version.
  This is not to be confused with the `prioritisetransaction` RPC which will remain
  supported by Core for adding fee deltas to transactions.

P2P connection management
--------------------------

- Peers manually added through the `-addnode` option or `addnode` RPC now have their own
  limit of eight connections which does not compete with other inbound or outbound
  connection usage and is not subject to the limitation imposed by the `-maxconnections`
  option.

- New connections to manually added peers are performed more quickly.

Introduction of assumed-valid blocks
-------------------------------------

- A significant portion of the initial block download time is spent verifying
  scripts/signatures.  Although the verification must pass to ensure the security
  of the system, no other result from this verification is needed: If the node
  knew the history of a given block were valid it could skip checking scripts
  for its ancestors.

- A new configuration option 'assumevalid' is provided to express this knowledge
  to the software.  Unlike the 'checkpoints' in the past this setting does not
  force the use of a particular chain: chains that are consistent with it are
  processed quicker, but other chains are still accepted if they'd otherwise
  be chosen as best. Also unlike 'checkpoints' the user can configure which
  block history is assumed true, this means that even outdated software can
  sync more quickly if the setting is updated by the user.

- Because the validity of a chain history is a simple objective fact it is much
  easier to review this setting.  As a result the software ships with a default
  value adjusted to match the current chain shortly before release.  The use
  of this default value can be disabled by setting -assumevalid=0

Fundrawtransaction change address reuse
----------------------------------------

- Before 2.16, `fundrawtransaction` was by default wallet stateless. In
  almost all cases `fundrawtransaction` does add a change-output to the
  outputs of the funded transaction. Before 2.16, the used keypool key was
  never marked as change-address key and directly returned to the keypool
  (leading to address reuse).  Before 2.16, calling `getnewaddress`
  directly after `fundrawtransaction` did generate the same address as
  the change-output address.

- Since 2.16, fundrawtransaction does reserve the change-output-key from
  the keypool by default (optional by setting  `reserveChangeKey`, default =
  `true`)

- Users should also consider using `getrawchangeaddress()` in conjunction
  with `fundrawtransaction`'s `changeAddress` option.

Unused mempool memory used by coincache
----------------------------------------

- Before 2.16, memory reserved for mempool (using the `-maxmempool` option)
  went unused during initial block download, or IBD. In 2.16, the UTXO DB cache
  (controlled with the `-dbcache` option) borrows memory from the mempool
  when there is extra memory available. This may result in an increase in
  memory usage during IBD for those previously relying on only the `-dbcache`
  option to limit memory during that time.

Mining
------

In previous versions, getblocktemplate required segwit support from downstream
clients/miners once the feature activated on the network. In this version, it
now supports non-segwit clients even after activation, by removing all segwit
transactions from the returned block template. This allows non-segwit miners to
continue functioning correctly even after segwit has activated.

Due to the limitations in previous versions, getblocktemplate also recommended
non-segwit clients to not signal for the segwit version-bit. Since this is no
longer an issue, getblocktemplate now always recommends signalling segwit for
all miners. This is safe because ability to enforce the rule is the only
required criteria for safe activation, not actually producing segwit-enabled
blocks.

UTXO memory accounting
----------------------

Memory usage for the UTXO cache is being calculated more accurately, so that
the configured limit (`-dbcache`) will be respected when memory usage peaks
during cache flushes.  The memory accounting in prior releases is estimated to
only account for half the actual peak utilization.

The default `-dbcache` has also been changed in this release to 450MiB.  Users
who currently set `-dbcache` to a high value (e.g. to keep the UTXO more fully
cached in memory) should consider increasing this setting in order to achieve
the same cache performance as prior releases.  Users on low-memory systems
(such as systems with 1GB or less) should consider specifying a lower value for
this parameter.

Additional information relating to running on low-memory systems can be found
here:
[reducing-bitcoind-memory-usage.md](https://gist.github.com/laanwj/efe29c7661ce9b6620a7).

miniupnp CVE-2017-8798
----------------------------

Bundled miniupnpc was updated to 2.0.20170509. This fixes an integer signedness error
(present in MiniUPnPc v1.4.20101221 through v2.0) that allows remote attackers
(within the LAN) to cause a denial of service or possibly have unspecified
other impact.

This only affects users that have explicitly enabled UPnP through the GUI
setting or through the `-upnp` option, as since the last UPnP vulnerability
(in Groestlcoin Core 2.1.0.6) it has been disabled by default.

If you use this option, it is recommended to upgrade to this version as soon as
possible.

Performance Improvements
------------------------

Version 2.16 contains a number of significant performance improvements, which make
Initial Block Download, startup, transaction and block validation much faster:

- The chainstate database (which is used for tracking UTXOs) has been changed
  from a per-transaction model to a per-output model (See [PR 10195](https://github.com/bitcoin/bitcoin/pull/10195)). Advantages of this model
  are that it:
    - avoids the CPU overhead of deserializing and serializing the unused outputs;
    - has more predictable memory usage;
    - uses simpler code;
    - is adaptable to various future cache flushing strategies.

  As a result, validating the blockchain during Initial Block Download (IBD) and reindex
  is ~30-40% faster, uses 10-20% less memory, and flushes to disk far less frequently.
  The only downside is that the on-disk database is 15% larger. During the conversion from the previous format
  a few extra gigabytes may be used.
- Earlier versions experienced a spike in memory usage while flushing UTXO updates to disk.
  As a result, only half of the available memory was actually used as cache, and the other half was
  reserved to accommodate flushing. This is no longer the case (See [PR 10148](https://github.com/bitcoin/bitcoin/pull/10148)), and the entirety of
  the available cache (see `-dbcache`) is now actually used as cache. This reduces the flushing
  frequency by a factor 2 or more.
- In previous versions, signature validation for transactions has been cached when the
  transaction is accepted to the mempool. Version 2.16 extends this to cache the entire script
  validity (See [PR 10192](https://github.com/bitcoin/bitcoin/pull/10192)). This means that if a transaction in a block has already been accepted to the
  mempool, the scriptSig does not need to be re-evaluated. Empirical tests show that
  this results in new block validation being 40-50% faster.
- LevelDB has been upgraded to version 1.20 (See [PR 10544](https://github.com/bitcoin/bitcoin/pull/10544)). This version contains hardware acceleration for CRC
  on architectures supporting SSE 4.2. As a result, synchronization and block validation are now faster.
- SHA256 hashing has been optimized for architectures supporting SSE 4 (See [PR 10821](https://github.com/bitcoin/bitcoin/pull/10821)). SHA256 is around
  50% faster on supported hardware, which results in around 5% faster IBD and block
  validation. In version 2.16, SHA256 hardware optimization is disabled in release builds by
  default, but can be enabled by using `--enable-experimental-asm` when building.
- Refill of the keypool no longer flushes the wallet between each key which resulted in a ~20x speedup in creating a new wallet. Part of this speedup was used to increase the default keypool to 1000 keys to make recovery more robust. (See [PR 10831](https://github.com/bitcoin/bitcoin/pull/10831)).

Fee Estimation Improvements
---------------------------

Fee estimation has been significantly improved in version 2.16, with more accurate fee estimates used by the wallet and a wider range of options for advanced users of the `estimatesmartfee` and `estimaterawfee` RPCs (See [PR 10199](https://github.com/bitcoin/bitcoin/pull/10199)).

### Changes to internal logic and wallet behavior

- Internally, estimates are now tracked on 3 different time horizons. This allows for longer targets and means estimates adjust more quickly to changes in conditions.
- Estimates can now be *conservative* or *economical*. *Conservative* estimates use longer time horizons to produce an estimate which is less susceptible to rapid changes in fee conditions. *Economical* estimates use shorter time horizons and will be more affected by short-term changes in fee conditions. Economical estimates may be considerably lower during periods of low transaction activity (for example over weekends), but may result in transactions being unconfirmed if prevailing fees increase rapidly.
- By default, the wallet will use conservative fee estimates to increase the reliability of transactions being confirmed within the desired target. For transactions that are marked as replaceable, the wallet will use an economical estimate by default, since the fee can be 'bumped' if the fee conditions change rapidly (See [PR 10589](https://github.com/bitcoin/bitcoin/pull/10589)).
- Estimates can now be made for confirmation targets up to 1008 blocks (one day).
- More data on historical fee rates is stored, leading to more precise fee estimates.
- Transactions which leave the mempool due to eviction or other non-confirmed reasons are now taken into account by the fee estimation logic, leading to more accurate fee estimates.
- The fee estimation logic will make sure enough data has been gathered to return a meaningful estimate. If there is insufficient data, a fallback default fee is used.

### Changes to fee estimate RPCs

- The `estimatefee` RPC is now deprecated in favor of using only `estimatesmartfee` (which is the implementation used by the GUI)
- The `estimatesmartfee` RPC interface has been changed (See [PR 10707](https://github.com/bitcoin/bitcoin/pull/10707)):
    - The `nblocks` argument has been renamed to `conf_target` (to be consistent with other RPC methods).
    - An `estimate_mode` argument has been added. This argument takes one of the following strings: `CONSERVATIVE`, `ECONOMICAL` or `UNSET` (which defaults to `CONSERVATIVE`).
    - The RPC return object now contains an `errors` member, which returns errors encountered during processing.
    - If Groestlcoin Core has not been running for long enough and has not seen enough blocks or transactions to produce an accurate fee estimation, an error will be returned (previously a value of -1 was used to indicate an error, which could be confused for a feerate).
- A new `estimaterawfee` RPC is added to provide raw fee data. External clients can query and use this data in their own fee estimation logic.

Multi-wallet support
--------------------

Groestkcoin Core now supports loading multiple, separate wallets (See [PR 8694](https://github.com/bitcoin/bitcoin/pull/8694), [PR 10849](https://github.com/bitcoin/bitcoin/pull/10849)). The wallets are completely separated, with individual balances, keys and received transactions.

Multi-wallet is enabled by using more than one `-wallet` argument when starting Groestlcoin, either on the command line or in the Groestlcoin config file.

**In Groestlcoin-Qt, only the first wallet will be displayed and accessible for creating and signing transactions.** GUI selectable multiple wallets will be supported in a future version. However, even in 2.16 other loaded wallets will remain synchronized to the node's current tip in the background. This can be useful if running a pruned node, since loading a wallet where the most recent sync is beyond the pruned height results in having to download and revalidate the whole blockchain. Continuing to synchronize all wallets in the background avoids this problem.

Groestlcoin Core 2.16.0 contains the following changes to the RPC interface and `groestlcoin-cli` for multi-wallet:

* When running Groestlcoin Core with a single wallet, there are **no** changes to the RPC interface or `groestlcoin-cli`. All RPC calls and `groestlcoin-cli` commands continue to work as before.
* When running Groestlcoin Core with multi-wallet, all *node-level* RPC methods continue to work as before. HTTP RPC requests should be send to the normal `<RPC IP address>:<RPC port>` endpoint, and `groestlcoin-cli` commands should be run as before. A *node-level* RPC method is any method which does not require access to the wallet.
* When running Groestlcoin Core with multi-wallet, *wallet-level* RPC methods must specify the wallet for which they're intended in every request. HTTP RPC requests should be send to the `<RPC IP address>:<RPC port>/wallet/<wallet name>` endpoint, for example `127.0.0.1:1441/wallet/wallet1.dat`. `groestlcoin-cli` commands should be run with a `-rpcwallet` option, for example `groestlcoin-cli -rpcwallet=wallet1.dat getbalance`.
* A new *node-level* `listwallets` RPC method is added to display which wallets are currently loaded. The names returned by this method are the same as those used in the HTTP endpoint and for the `rpcwallet` argument.

Note that while multi-wallet is now fully supported, the RPC multi-wallet interface should be considered unstable for version 2.16.0, and there may backwards-incompatible changes in future versions.

Replace-by-fee control in the GUI
---------------------------------

Groestlcoin Core has supported creating opt-in replace-by-fee (RBF) transactions
since version 2.13.3.

In version 2.16, creating an opt-in RBF transaction and replacing the unconfirmed
transaction with a higher-fee transaction are both supported in the GUI (See [PR 9592](https://github.com/bitcoin/bitcoin/pull/9592)).

Removal of Coin Age Priority
----------------------------

In previous versions of Groestlcoin Core, a portion of each block could be reserved for transactions based on the age and value of UTXOs they spent. This concept (Coin Age Priority) is a policy choice by miners, and there are no consensus rules around the inclusion of Coin Age Priority transactions in blocks. In practice, only a few miners continue to use Coin Age Priority for transaction selection in blocks. Groestlcoin Core 2.16 removes all remaining support for Coin Age Priority (See [PR 9602](https://github.com/bitcoin/bitcoin/pull/9602)). This has the following implications:

- The concept of *free transactions* has been removed. High Coin Age Priority transactions would previously be allowed to be relayed even if they didn't attach a miner fee. This is no longer possible since there is no concept of Coin Age Priority. The `-limitfreerelay` and `-relaypriority` options which controlled relay of free transactions have therefore been removed.
- The `-sendfreetransactions` option has been removed, since almost all miners do not include transactions which do not attach a transaction fee.
- The `-blockprioritysize` option has been removed.
- The `estimatepriority` and `estimatesmartpriority` RPCs have been removed.
- The `getmempoolancestors`, `getmempooldescendants`, `getmempoolentry` and `getrawmempool` RPCs no longer return `startingpriority` and `currentpriority`.
- The `prioritisetransaction` RPC no longer takes a `priority_delta` argument, which is replaced by a `dummy` argument for backwards compatibility with clients using positional arguments. The RPC is still used to change the apparent fee-rate of the transaction by using the `fee_delta` argument.
- `-minrelaytxfee` can now be set to 0. If `minrelaytxfee` is set, then fees smaller than `minrelaytxfee` (per kB) are rejected from relaying, mining and transaction creation. This defaults to 1000 gro/kB.
- The `-printpriority` option has been updated to only output the fee rate and hash of transactions included in a block by the mining code.

Mempool Persistence Across Restarts
-----------------------------------

Version 2.16 introduced mempool persistence across restarts (the mempool is saved to a `mempool.dat` file in the data directory prior to shutdown and restores the mempool when the node is restarted). Version 2.16 also allows this feature to be switched on or off using the `-persistmempool` command-line option (See [PR 9966](https://github.com/bitcoin/bitcoin/pull/9966)). By default, the option is set to true, and the mempool is saved on shutdown and reloaded on startup. If set to false, the `mempool.dat` file will not be loaded on startup or saved on shutdown.



Network fork safety enhancements
--------------------------------

A number of changes to the way Groestlcoin Core deals with peer connections and invalid blocks
have been made, as a safety precaution against blockchain forks and misbehaving peers.

- Unrequested blocks with less work than the minimum-chain-work are now no longer processed even
if they have more work than the tip (a potential issue during IBD where the tip may have low-work).
This prevents peers wasting the resources of a node.

- Peers which provide a chain with less work than the minimum-chain-work during IBD will now be disconnected.

- For a given outbound peer, we now check whether their best known block has at least as much work as our tip. If it
doesn't, and if we still haven't heard about a block with sufficient work after a 20 minute timeout, then we send
a single getheaders message, and wait 2 more minutes. If after two minutes their best known block has insufficient
work, we disconnect that peer. We protect 4 of our outbound peers from being disconnected by this logic to prevent
excessive network topology changes as a result of this algorithm, while still ensuring that we have a reasonable
number of nodes not known to be on bogus chains.

- Outbound (non-manual) peers that serve us block headers that are already known to be invalid (other than compact
block announcements, because BIP 152 explicitly permits nodes to relay compact blocks before fully validating them)
will now be disconnected.

- If the chain tip has not been advanced for over 30 minutes, we now assume the tip may be stale and will try to connect
to an additional outbound peer. A periodic check ensures that if this extra peer connection is in use, we will disconnect
the peer that least recently announced a new block.

- The set of all known invalid-themselves blocks (i.e. blocks which we attempted to connect but which were found to be
invalid) are now tracked and used to check if new headers build on an invalid chain. This ensures that everything that
descends from an invalid block is marked as such.


Miner block size limiting deprecated
------------------------------------

Though blockmaxweight has been preferred for limiting the size of blocks returned by
getblocktemplate since 2.13.3, blockmaxsize remained as an option for those who wished
to limit their block size directly. Using this option resulted in a few UI issues as
well as non-optimal fee selection and ever-so-slightly worse performance, and has thus
now been deprecated. Further, the blockmaxsize option is now used only to calculate an
implied blockmaxweight, instead of limiting block size directly. Any miners who wish
to limit their blocks by size, instead of by weight, will have to do so manually by
removing transactions from their block template directly.

GUI settings backed up on reset
-------------------------------

The GUI settings will now be written to `guisettings.ini.bak` in the data directory before wiping them when
the `-resetguisettings` argument is used. This can be used to retroactively troubleshoot issues due to the
GUI settings.

Debug `-minimumchainwork` argument added
----------------------------------------

A hidden debug argument `-minimumchainwork` has been added to allow a custom minimum work value to be used
when validating a chain.


Wallet changes
---------------

### Segwit Wallet

Groestlcoin Core 2.16.0 introduces full support for segwit in the wallet and user interfaces. A new `-addresstype` argument has been added, which supports `legacy`, `p2sh-segwit` (default), and `bech32` addresses. It controls what kind of addresses are produced by `getnewaddress`, `getaccountaddress`, and `createmultisigaddress`. A `-changetype` argument has also been added, with the same options, and by default equal to `-addresstype`, to control which kind of change is used.

A new `address_type` parameter has been added to the `getnewaddress` and `addmultisigaddress` RPCs to specify which type of address to generate.
A `change_type` argument has been added to the `fundrawtransaction` RPC to override the `-changetype` argument for specific transactions.

- All segwit addresses created through `getnewaddress` or `*multisig` RPCs explicitly get their redeemscripts added to the wallet file. This means that downgrading after creating a segwit address will work, as long as the wallet file is up to date.
- All segwit keys in the wallet get an implicit redeemscript added, without it being written to the file. This means recovery of an old backup will work, as long as you use new software.
- All keypool keys that are seen used in transactions explicitly get their redeemscripts added to the wallet files. This means that downgrading after recovering from a backup that includes a segwit address will work

Note that some RPCs do not yet support segwit addresses. Notably, `signmessage`/`verifymessage` doesn't support segwit addresses, nor does `importmulti` at this time. Support for segwit in those RPCs will continue to be added in future versions.

P2WPKH change outputs are now used by default if any destination in the transaction is a P2WPKH or P2WSH output. This is done to ensure the change output is as indistinguishable from the other outputs as possible in either case.

### BIP173 (Bech32) Address support ("grs1..." addresses)

Full support for native segwit addresses (BIP173 / Bech32) has now been added.
This includes the ability to send to BIP173 addresses (including non-v0 ones), and generating these
addresses (including as default new addresses, see above).

A checkbox has been added to the GUI to select whether a Bech32 address or P2SH-wrapped address should be generated when using segwit addresses. When launched with `-addresstype=bech32` it is checked by default. When launched with `-addresstype=legacy` it is unchecked and disabled.

### HD-wallets by default

Due to a backward-incompatible change in the wallet database, wallets created
with version 2.16.0 will be rejected by previous versions. Also, version 2.16.0
will only create hierarchical deterministic (HD) wallets. Note that this only applies
to new wallets; wallets made with previous versions will not be upgraded to be HD.

### Replace-By-Fee by default in GUI

The send screen now uses BIP125 RBF by default, regardless of `-walletrbf`.
There is a checkbox to mark the transaction as final.

The RPC default remains unchanged: to use RBF, launch with `-walletrbf=1` or
use the `replaceable` argument for individual transactions.

### Wallets directory configuration (`-walletdir`)

Groestlcoin Core now has more flexibility in where the wallets directory can be
located. Previously wallet database files were stored at the top level of the
groestlcoin data directory. The behavior is now:

- For new installations (where the data directory doesn't already exist),
  wallets will now be stored in a new `wallets/` subdirectory inside the data
  directory by default.
- For existing nodes (where the data directory already exists), wallets will be
  stored in the data directory root by default. If a `wallets/` subdirectory
  already exists in the data directory root, then wallets will be stored in the
  `wallets/` subdirectory by default.
- The location of the wallets directory can be overridden by specifying a
  `-walletdir=<path>` option where `<path>` can be an absolute path to a
  directory or directory symlink.

Care should be taken when choosing the wallets directory location, as if it
becomes unavailable during operation, funds may be lost.

Build: Minimum GCC bumped to 4.8.x
------------------------------------
The minimum version of the GCC compiler required to compile Groestlcoin Core is now 4.8. No effort will be
made to support older versions of GCC. See discussion in issue #11732 for more information.
The minimum version for the Clang compiler is still 3.3. Other minimum dependency versions can be found in `doc/dependencies.md` in the repository.

Support for signalling pruned nodes (BIP159)
---------------------------------------------
Pruned nodes can now signal BIP159's NODE_NETWORK_LIMITED using service bits, in preparation for
full BIP159 support in later versions. This would allow pruned nodes to serve the most recent blocks. However, the current change does not yet include support for connecting to these pruned peers.

Performance: SHA256 assembly enabled by default
-------------------------------------------------
The SHA256 hashing optimizations for architectures supporting SSE4, which lead to ~50% speedups in SHA256 on supported hardware (~5% faster synchronization and block validation), have now been enabled by default. In previous versions they were enabled using the `--enable-experimental-asm` flag when building, but are now the default and no longer deemed experimental.

GUI changes
-----------
- Uses of "µGRS" in the GUI now also show the more colloquial term "groestls", specified in BIP176.
- The option to reuse a previous address has now been removed. This was justified by the need to "resend" an invoice, but now that we have the request history, that need should be gone.
- Support for searching by TXID has been added, rather than just address and label.
- A "Use available balance" option has been added to the send coins dialog, to add the remaining available wallet balance to a transaction output.
- A toggle for unblinding the password fields on the password dialog has been added.


### Safe mode disabled by default

Safe mode is now disabled by default and must be manually enabled (with `-disablesafemode=0`) if you wish to use it. Safe mode is a feature that disables a subset of RPC calls - mostly related to the wallet and sending - automatically in case certain problem conditions with the network are detected. However, developers have come to regard these checks as not reliable enough to act on automatically. Even with safe mode disabled, they will still cause warnings in the `warnings` field of the `getneworkinfo` RPC and launch the `-alertnotify` command.

### Renamed script for creating JSON-RPC credentials

The `share/rpcuser/rpcuser.py` script was renamed to `share/rpcauth/rpcauth.py`. This script can be
used to create `rpcauth` credentials for a JSON-RPC user.

### Validateaddress improvements

The `validateaddress` RPC output has been extended with a few new fields, and support for segwit addresses (both P2SH and Bech32). Specifically:
* A new field `iswitness` is True for P2WPKH and P2WSH addresses ("grs1..." addresses), but not for P2SH-wrapped segwit addresses (see below).
* The existing field `isscript` will now also report True for P2WSH addresses.
* A new field `embedded` is present for all script addresses where the script is known and matches something that can be interpreted as a known address. This is particularly true for P2SH-P2WPKH and P2SH-P2WSH addresses. The value for `embedded` includes much of the information `validateaddress` would report if invoked directly on the embedded address.
* For multisig scripts a new `pubkeys` field was added that reports the full public keys involved in the script (if known). This is a replacement for the existing `addresses` field (which reports the same information but encoded as P2PKH addresses), represented in a more useful and less confusing way. The `addresses` field remains present for non-segwit addresses for backward compatibility.
* For all single-key addresses with known key (even when wrapped in P2SH or P2WSH), the `pubkey` field will be present. In particular, this means that invoking `validateaddress` on the output of `getnewaddress` will always report the `pubkey`, even when the address type is P2SH-P2WPKH.

Low-level RPC changes
----------------------

 - `importprunedfunds` only accepts two required arguments. Some versions accept
   an optional third arg, which was always ignored. Make sure to never pass more
   than two arguments.

 - The first boolean argument to `getaddednodeinfo` has been removed. This is
   an incompatible change.

 - RPC command `getmininginfo` loses the "testnet" field in favor of the more
   generic "chain" (which has been present for years).

 - A new RPC command `preciousblock` has been added which marks a block as
   precious. A precious block will be treated as if it were received earlier
   than a competing block.

 - A new RPC command `importmulti` has been added which receives an array of
   JSON objects representing the intention of importing a public key, a
   private key, an address and script/p2sh

 - Use of `getrawtransaction` for retrieving confirmed transactions with unspent
   outputs has been deprecated. For now this will still work, but in the future
   it may change to only be able to retrieve information about transactions in
   the mempool or if `txindex` is enabled.

 - A new RPC command `getmemoryinfo` has been added which will return information
   about the memory usage of groestlcoin Core. This was added in conjunction with
   optimizations to memory management. See [Pull #8753](https://github.com/bitcoin/bitcoin/pull/8753)
   for more information.

 - A new RPC command `bumpfee` has been added which allows replacing an
   unconfirmed wallet transaction that signaled RBF (see the `-walletrbf`
   startup option above) with a new transaction that pays a higher fee, and
   should be more likely to get confirmed quickly.

 - The "currentblocksize" value in getmininginfo has been removed.

 - `dumpwallet` no longer allows overwriting files. This is a security measure
   as well as prevents dangerous user mistakes.

 - `backupwallet` will now fail when attempting to backup to source file, rather than
   destroying the wallet.

 - `listsinceblock` will now throw an error if an unknown `blockhash` argument
   value is passed, instead of returning a list of all wallet transactions since
   the genesis block. The behaviour is unchanged when an empty string is provided.

 - A new RPC `rescanblockchain` has been added to manually invoke a blockchain rescan.
   The RPC supports start and end-height arguments for the rescan, and can be used in a
   multiwallet environment to rescan the blockchain at runtime.

 - A new `savemempool` RPC has been added which allows the current mempool to be saved to
   disk at any time to avoid it being lost due to crashes / power loss.


 - The first positional argument of `createrawtransaction` was renamed from
  `transactions` to `inputs`.

 - The argument of `disconnectnode` was renamed from `node` to `address`.

 - `abortrescan` stops current wallet rescan, e.g. when triggered by an `importprivkey` call (See [PR 10208](https://github.com/bitcoin/bitcoin/pull/10208)).
- `combinerawtransaction` accepts a JSON array of raw transactions and combines them into a single raw transaction (See [PR 10571](https://github.com/bitcoin/bitcoin/pull/10571)).
- `estimaterawfee` returns raw fee data so that customized logic can be implemented to analyze the data and calculate estimates. See [Fee Estimation Improvements](#fee-estimation-improvements) for full details on changes to the fee estimation logic and interface.
- `getchaintxstats` returns statistics about the total number and rate of transactions
  in the chain (See [PR 9733](https://github.com/bitcoin/bitcoin/pull/9733)).
- `listwallets` lists wallets which are currently loaded. See the *Multi-wallet* section
  of these release notes for full details (See [Multi-wallet support](#multi-wallet-support)).
- `uptime` returns the total runtime of the `groestlcoind` server since its last start (See [PR 10400](https://github.com/bitcoin/bitcoin/pull/10400)).

- When using Groestlcoin Core in multi-wallet mode, RPC requests for wallet methods must specify
  the wallet that they're intended for. See [Multi-wallet support](#multi-wallet-support) for full details.

- The new database model no longer stores information about transaction
  versions of unspent outputs (See [Performance improvements](#performance-improvements)). This means that:
  - The `gettxout` RPC no longer has a `version` field in the response.
  - The `gettxoutsetinfo` RPC reports `hash_serialized_2` instead of `hash_serialized`,
    which does not commit to the transaction versions of unspent outputs, but does
    commit to the height and coinbase information.
  - The `getutxos` REST path no longer reports the `txvers` field in JSON format,
    and always reports 0 for transaction versions in the binary format

- The `estimatefee` RPC is deprecated. Clients should switch to using the `estimatesmartfee` RPC, which returns better fee estimates. See [Fee Estimation Improvements](#fee-estimation-improvements) for full details on changes to the fee estimation logic and interface.

- The `gettxoutsetinfo` response now contains `disk_size` and `bogosize` instead of
  `bytes_serialized`. The first is a more accurate estimate of actual disk usage, but
  is not deterministic. The second is unrelated to disk usage, but is a
  database-independent metric of UTXO set size: it counts every UTXO entry as 50 + the
  length of its scriptPubKey (See [PR 10426](https://github.com/bitcoin/bitcoin/pull/10426)).

- `signrawtransaction` can no longer be used to combine multiple transactions into a single transaction. Instead, use the new `combinerawtransaction` RPC (See [PR 10571](https://github.com/bitcoin/bitcoin/pull/10571)).

- `fundrawtransaction` no longer accepts a `reserveChangeKey` option. This option used to allow RPC users to fund a raw transaction using an key from the keypool for the change address without removing it from the available keys in the keypool. The key could then be re-used for a `getnewaddress` call, which could potentially result in confusing or dangerous behaviour (See [PR 10784](https://github.com/bitcoin/bitcoin/pull/10784)).

- `estimatepriority` and `estimatesmartpriority` have been removed. See [Removal of Coin Age Priority](#removal-of-coin-age-priority).

- The `listunspent` RPC now takes a `query_options` argument (see [PR 8952](https://github.com/bitcoin/bitcoin/pull/8952)), which is a JSON object
  containing one or more of the following members:
  - `minimumAmount` - a number specifying the minimum value of each UTXO
  - `maximumAmount` - a number specifying the maximum value of each UTXO
  - `maximumCount` - a number specifying the minimum number of UTXOs
  - `minimumSumAmount` - a number specifying the minimum sum value of all UTXOs

- The `getmempoolancestors`, `getmempooldescendants`, `getmempoolentry` and `getrawmempool` RPCs no longer return `startingpriority` and `currentpriority`. See [Removal of Coin Age Priority](#removal-of-coin-age-priority).

- The `dumpwallet` RPC now returns the full absolute path to the dumped wallet. It
  used to return no value, even if successful (See [PR 9740](https://github.com/bitcoin/bitcoin/pull/9740)).

- In the `getpeerinfo` RPC, the return object for each peer now returns an `addrbind` member, which contains the ip address and port of the connection to the peer. This is in addition to the `addrlocal` member which contains the ip address and port of the local node as reported by the peer (See [PR 10478](https://github.com/bitcoin/bitcoin/pull/10478)).

- The `disconnectnode` RPC can now disconnect a node specified by node ID (as well as by IP address/port). To disconnect a node based on node ID, call the RPC with the new `nodeid` argument (See [PR 10143](https://github.com/bitcoin/bitcoin/pull/10143)).

- The second argument in `prioritisetransaction` has been renamed from `priority_delta` to `dummy` since Groestlcoin Core no longer has a concept of coin age priority. The `dummy` argument has no functional effect, but is retained for positional argument compatibility. See [Removal of Coin Age Priority](#removal-of-coin-age-priority).

- The `resendwallettransactions` RPC throws an error if the `-walletbroadcast` option is set to false (See [PR 10995](https://github.com/bitcoin/bitcoin/pull/10995)).

- The second argument in the `submitblock` RPC argument has been renamed from `parameters` to `dummy`. This argument never had any effect, and the renaming is simply to communicate this fact to the user (See [PR 10191](https://github.com/bitcoin/bitcoin/pull/10191))
  (Clients should, however, use positional arguments for `submitblock` in order to be compatible with BIP 22.)

- The `verbose` argument of `getblock` has been renamed to `verbosity` and now takes an integer from 0 to 2. Verbose level 0 is equivalent to `verbose=false`. Verbose level 1 is equivalent to `verbose=true`. Verbose level 2 will give the full transaction details of each transaction in the output as given by `getrawtransaction`. The old behavior of using the `verbose` named argument and a boolean value is still maintained for compatibility.

- Error codes have been updated to be more accurate for the following error cases (See [PR 9853](https://github.com/bitcoin/bitcoin/pull/9853)):
  - `getblock` now returns RPC_MISC_ERROR if the block can't be found on disk (for
  example if the block has been pruned). Previously returned RPC_INTERNAL_ERROR.
  - `pruneblockchain` now returns RPC_MISC_ERROR if the blocks cannot be pruned
  because the node is not in pruned mode. Previously returned RPC_METHOD_NOT_FOUND.
  - `pruneblockchain` now returns RPC_INVALID_PARAMETER if the blocks cannot be pruned
  because the supplied timestamp is too late. Previously returned RPC_INTERNAL_ERROR.
  - `pruneblockchain` now returns RPC_MISC_ERROR if the blocks cannot be pruned
  because the blockchain is too short. Previously returned RPC_INTERNAL_ERROR.
  - `setban` now returns RPC_CLIENT_INVALID_IP_OR_SUBNET if the supplied IP address
  or subnet is invalid. Previously returned RPC_CLIENT_NODE_ALREADY_ADDED.
  - `setban` now returns RPC_CLIENT_INVALID_IP_OR_SUBNET if the user tries to unban
  a node that has not previously been banned. Previously returned RPC_MISC_ERROR.
  - `removeprunedfunds` now returns RPC_WALLET_ERROR if `groestlcoind` is unable to remove
  the transaction. Previously returned RPC_INTERNAL_ERROR.
  - `removeprunedfunds` now returns RPC_INVALID_PARAMETER if the transaction does not
  exist in the wallet. Previously returned RPC_INTERNAL_ERROR.
  - `fundrawtransaction` now returns RPC_INVALID_ADDRESS_OR_KEY if an invalid change
  address is provided. Previously returned RPC_INVALID_PARAMETER.
  - `fundrawtransaction` now returns RPC_WALLET_ERROR if `groestlcoind` is unable to create
  the transaction. The error message provides further details. Previously returned
  RPC_INTERNAL_ERROR.
  - `bumpfee` now returns RPC_INVALID_PARAMETER if the provided transaction has
  descendants in the wallet. Previously returned RPC_MISC_ERROR.
  - `bumpfee` now returns RPC_INVALID_PARAMETER if the provided transaction has
  descendants in the mempool. Previously returned RPC_MISC_ERROR.
  - `bumpfee` now returns RPC_WALLET_ERROR if the provided transaction has
  has been mined or conflicts with a mined transaction. Previously returned
  RPC_INVALID_ADDRESS_OR_KEY.
  - `bumpfee` now returns RPC_WALLET_ERROR if the provided transaction is not
  BIP 125 replaceable. Previously returned RPC_INVALID_ADDRESS_OR_KEY.
  - `bumpfee` now returns RPC_WALLET_ERROR if the provided transaction has already
  been bumped by a different transaction. Previously returned RPC_INVALID_REQUEST.
  - `bumpfee` now returns RPC_WALLET_ERROR if the provided transaction contains
  inputs which don't belong to this wallet. Previously returned RPC_INVALID_ADDRESS_OR_KEY.
  - `bumpfee` now returns RPC_WALLET_ERROR if the provided transaction has multiple change
  outputs. Previously returned RPC_MISC_ERROR.
  - `bumpfee` now returns RPC_WALLET_ERROR if the provided transaction has no change
  output. Previously returned RPC_MISC_ERROR.
  - `bumpfee` now returns RPC_WALLET_ERROR if the fee is too high. Previously returned
  RPC_MISC_ERROR.
  - `bumpfee` now returns RPC_WALLET_ERROR if the fee is too low. Previously returned
  RPC_MISC_ERROR.
  - `bumpfee` now returns RPC_WALLET_ERROR if the change output is too small to bump the
  fee. Previously returned RPC_MISC_ERROR.

 - The deprecated RPC `getinfo` was removed. It is recommended that the more specific RPCs are used:
   * `getblockchaininfo`
   * `getnetworkinfo`
   * `getwalletinfo`
   * `getmininginfo`
 - The wallet RPC `getreceivedbyaddress` will return an error if called with an address not in the wallet.
 - The wallet RPC `addwitnessaddress` was deprecated and will be removed in future version,
   set the `address_type` argument of `getnewaddress`, or option `-addresstype=[bech32|p2sh-segwit]` instead.
 - `dumpwallet` now includes hex-encoded scripts from the wallet in the dumpfile, and
   `importwallet` now imports these scripts, but corresponding addresses may not be added
   correctly or a manual rescan may be required to find relevant transactions.
 - The RPC `getblockchaininfo` now includes an `errors` field.
 - A new `blockhash` parameter has been added to the `getrawtransaction` RPC which allows for a raw transaction to be fetched from a        specific block if known, even without `-txindex` enabled.
 - The `decoderawtransaction` and `fundrawtransaction` RPCs now have optional `iswitness` parameters to override the
   heuristic witness checks if necessary.
 - The `walletpassphrase` timeout is now clamped to 2^30 seconds.
 - Using addresses with the `createmultisig` RPC is now deprecated, and will be removed in a later version. Public keys should be used      instead.
 - Blockchain rescans now no longer lock the wallet for the entire rescan process, so other RPCs can now be used at the same time          (although results of balances / transactions may be incorrect or incomplete until the rescan is complete).
 - The `logging` RPC has now been made public rather than hidden.
 - An `initialblockdownload` boolean has been added to the `getblockchaininfo` RPC to indicate whether the node is currently in IBD or      not.
 - `minrelaytxfee` is now included in the output of `getmempoolinfo`

Other changed command-line options
----------------------------------
- `-debuglogfile=<file>` can be used to specify an alternative debug logging file.
- groestlcoin-cli now has an `-stdinrpcpass` option to allow the RPC password to be read from standard input.
- The `-usehd` option has been removed.
- groestlcoin-cli now supports a new `-getinfo` flag which returns an output like that of the now-removed `getinfo` RPC.

Testing changes
----------------
- Regtest support has been enabled and the JSON-RPC port is 18888.
- Segwit is always active in regtest mode by default.
