# CJDNS support in Groestlcoin Core

It is possible to run Groestlcoin Core over CJDNS, an encrypted IPv6 network that
uses public-key cryptography for address allocation and a distributed hash table
for routing.

## What is CJDNS?

CJDNS is like a distributed, shared VPN with multiple entry points where every
participant can reach any other participant. All participants use addresses from
the `fc00::/8` network (reserved IPv6 range). Installation and configuration is
done outside of Groestlcoin Core, similarly to a VPN (either in the host/OS or on
the network router). See https://github.com/cjdelisle/cjdns#readme for more
information.

Compared to IPv4/IPv6, CJDNS provides end-to-end encryption and protects nodes
from traffic analysis and filtering.

Used with Tor and I2P, CJDNS is a complementary option that can enhance network
redundancy and robustness for both the Groestlcoin network and individual nodes.

Each network has different characteristics. For instance, Tor is widely used but
somewhat centralized. I2P connections have a source address and I2P is slow.
CJDNS is fast but does not hide the sender and the recipient from intermediate
routers.

## Installing CJDNS

To install and set up CJDNS, follow the instructions at
https://github.com/cjdelisle/cjdns#how-to-install-cjdns.

## Connecting to the CJDNS network

As of CJDNS v22, nodes automatically discover and connect to peers via DNS
seeding. After installation, you can verify that your node has peers:

    cjdnstool peers show

If you see peers with status `ESTABLISHED`, your node is connected and no
further peering setup is needed.

Manual peering may be useful if you want to guarantee a connection to a
specific node, or if you have disabled DNS seeding for privacy reasons. See
[doc/peering.md](https://github.com/cjdelisle/cjdns/blob/master/doc/peering.md)
in the CJDNS repository for details.

## Run Groestlcoin Core with CJDNS

Once you are connected to the CJDNS network, the following Groestlcoin Core
configuration option makes CJDNS peers automatically reachable:

```
-cjdnsreachable
```

When enabled, this option tells Groestlcoin Core that it is running in an
environment where a connection to an `fc00::/8` address will be to the CJDNS
network instead of to an [RFC4193](https://datatracker.ietf.org/doc/html/rfc4193)
IPv6 local network. This helps Groestlcoin Core perform better address management:
  - Your node can consider incoming `fc00::/8` connections to be from the CJDNS
    network rather than from an IPv6 private one.
  - If one of your node's local addresses is `fc00::/8`, then it can choose to
    gossip that address to peers.

## Additional configuration options related to CJDNS

```
-onlynet=cjdns
```

Make automatic outbound connections only to CJDNS addresses. Inbound and manual
connections are not affected by this option. It can be specified multiple times
to allow multiple networks, e.g. onlynet=cjdns, onlynet=i2p, onlynet=onion.

CJDNS support was added to Groestlcoin Core in version 23.0 and there may be fewer
CJDNS peers than Tor or IP ones. You can use `groestlcoin-cli -addrinfo` to see the
number of CJDNS addresses known to your node.

In general, a node can be run with both an onion service and CJDNS (or any/all
of IPv4/IPv6/onion/I2P/CJDNS), which can provide a potential fallback if one of
the networks has issues. There are a number of ways to configure this; see
[doc/tor.md](/doc/tor.md) for
details.

## CJDNS-related information in Groestlcoin Core

There are several ways to see your CJDNS address in Groestlcoin Core:
- in the "Local addresses" output of CLI `-netinfo`
- in the "localaddresses" output of RPC `getnetworkinfo`

To see which CJDNS peers your node is connected to, use `groestlcoin-cli -netinfo 4`
or the `getpeerinfo` RPC (i.e. `groestlcoin-cli getpeerinfo`).

You can use the `getnodeaddresses` RPC to fetch a number of CJDNS peers known to your node; run `groestlcoin-cli help getnodeaddresses` for details.

`groestlcoin rpc` can also be substituted for `groestlcoin-cli`.
