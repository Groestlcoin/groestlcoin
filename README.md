Groestlcoin integration/staging tree
=================================
Forked from Bitcoin reference wallet 0.8.6 on March 2014

Updated to Bitcoin reference wallet 0.11.0 on August 2015

Updated to Bitcoin reference wallet 0.13.3 on January 2017

Updated to Bitcoin reference wallet 0.16.0 on June 2018

Updated to Bitcoin reference wallet 0.16.3 on September 2018

Updated to Bitcoin reference wallet 0.17.2 on March 2019

Updated to Bitcoin reference wallet 0.18.2 on March 2020

Updated to Bitcoin reference wallet 0.19.1 on June 2020

Updated to Bitcoin reference wallet 0.20.1 on September 2020

Updated to Bitcoin reference wallet 0.21.0 on December 2020

Updated to Bitcoin reference wallet 0.21.1 on June 2021

Updated to Bitcoin reference wallet 22.0.0 on September 2021

Updated to Bitcoin reference wallet 23.0.0 on June 2022

Updated to Bitcoin reference wallet 24.0.0 on November 2022

Updated to Bitcoin reference wallet 24.0.1 on December 2022

Updated to Bitcoin reference wallet 25.0.0 on June 2023

Updated to Bitcoin reference wallet 26.0.0 on December 2023

Updated to Bitcoin reference wallet 27.0.0 on April 2024

Updated to Bitcoin reference wallet 28.0.0 on September 2024

Groestlcoin Core Wallet

https://www.groestlcoin.org

The algorithm was written as a candidate for sha3

https://bitcointalk.org/index.php?topic=525926.0

What is Groestlcoin Core?
-----------------

Groestlcoin Core connects to the Groestlcoin peer-to-peer network to download and fully
validate blocks and transactions. It also includes a wallet and graphical user
interface, which can be optionally built.

Further information about Groestlcoin Core is available in the [doc folder](/doc).

License
-------

Groestlcoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see http://opensource.org/licenses/MIT.

Development Process
-------------------

Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.

If it is a simple/trivial/non-controversial change, then one of the Groestlcoin
development team members simply pulls it.

If it is a *more complicated or potentially controversial* change, then the patch
submitter will be asked to start a discussion

The patch will be accepted if there is broad consensus that it is a good thing.
Developers should expect to rework and resubmit patches if the code doesn't
match the project's coding conventions or are controversial.

The `master` branch is regularly built (see `doc/build-*.md` for instructions) and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/groestlcoin/groestlcoin/tags) are created
regularly from release branches to indicate new official, stable release versions of Groestlcoin Core.

Development tips and tricks
---------------------------

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled during the generation of the build system) with: `ctest`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python.
These tests can be run (if the [test dependencies](/test) are installed) with: `build/test/functional/test_runner.py`
(assuming `build` is your build directory).


The -debug=... command-line option controls debugging; running with just -debug will turn
on all categories (and give you a very large debug.log file).

The Qt code routes qDebug() output to debug.log under category "qt": run with -debug=qt
to see it.

**testnet and regtest modes**

Run with the -testnet option to run with "play groestlcoins" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the -regtest option.

Translations
------------

Translations are periodically pulled from Transifex and merged into the git repository.

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.
