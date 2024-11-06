Release Process
====================

## Branch updates

### Before every release candidate

* Update release candidate version in `CMakeLists.txt` (`CLIENT_VERSION_RC`).
* Update manpages (after rebuilding the binaries), see [gen-manpages.py](/contrib/devtools/README.md#gen-manpagespy).
* Update groestlcoin.conf and commit changes if they exist, see [gen-groestlcoin-conf.sh](/contrib/devtools/README.md#gen-groestlcoin-confsh).

### Before every major and minor release

* Update [bips.md](bips.md) to account for changes since the last release.
* Update version in `CMakeLists.txt` (don't forget to set `CLIENT_VERSION_RC` to `0`).
* Update manpages (see previous section)
* Write release notes (see "Write the release notes" below) in doc/release-notes.md. If necessary,
  archive the previous release notes as doc/release-notes/release-notes-${VERSION}.md.

### Before every major release

* On both the master branch and the new release branch:
  - update `CLIENT_VERSION_MAJOR` in [`CMakeLists.txt`](../CMakeLists.txt)
* On the new release branch in [`CMakeLists.txt`](../CMakeLists.txt):
  - set `CLIENT_VERSION_MINOR` to `0`
  - set `CLIENT_VERSION_BUILD` to `0`
  - set `CLIENT_VERSION_IS_RELEASE` to `true`

#### Before branch-off

* Update translations see [translation_process.md](/doc/translation_process.md#synchronising-translations).
* Update hardcoded [seeds](/contrib/seeds/README.md) .
* Update the following variables in [`src/kernel/chainparams.cpp`](/src/kernel/chainparams.cpp) for mainnet, testnet, and signet:
  - `m_assumed_blockchain_size` and `m_assumed_chain_state_size` with the current size plus some overhead (see
    [this](#how-to-calculate-assumed-blockchain-and-chain-state-size) for information on how to calculate them).
  - The following updates should be reviewed with `reindex-chainstate` and `assumevalid=0` to catch any defect
    that causes rejection of blocks in the past history.
  - `chainTxData` with statistics about the transaction count and rate. Use the output of the `getchaintxstats` RPC with an
    `nBlocks` of 4096 (28 days) and a `bestblockhash` of RPC `getbestblockhash`; see
    [this commit](https://github.com/Groestlcoin/groestlcoin/commit/30b51886e98b860855f26959504de9978d441650) for an example. Reviewers can verify the results by running
    `getchaintxstats <window_block_count> <window_final_block_hash>` with the `window_block_count` and `window_final_block_hash` from your output.
  - `defaultAssumeValid` with the output of RPC `getblockhash` using the `height` of `window_final_block_height` above
    (and update the block height comment with that height), taking into account the following:
    - On mainnet, the selected value must not be orphaned, so it may be useful to set the height two blocks back from the tip.
    - Testnet should be set with a height some tens of thousands back from the tip, due to reorgs there.
  - `nMinimumChainWork` with the "chainwork" value of RPC `getblockheader` using the same height as that selected for the previous step.
* Consider updating the headers synchronization tuning parameters to account for the chainparams updates.
  The optimal values change very slowly, so this isn't strictly necessary every release, but doing so doesn't hurt.
  - Update configuration variables in [`contrib/devtools/headerssync-params.py`](/contrib/devtools/headerssync-params.py):
    - Set `TIME` to the software's expected supported lifetime -- after this time, its ability to defend against a high bandwidth timewarp attacker will begin to degrade.
    - Set `MINCHAINWORK_HEADERS` to the height used for the `nMinimumChainWork` calculation above.
    - Check that the other variables still look reasonable.
  - Run the script. It works fine in CPython, but PyPy is much faster (seconds instead of minutes): `pypy3 contrib/devtools/headerssync-params.py`.
  - Paste the output defining `HEADER_COMMITMENT_PERIOD` and `REDOWNLOAD_BUFFER_SIZE` into the top of [`src/headerssync.cpp`](/src/headerssync.cpp).

#### After branch-off (on the major release branch)

- Update the versions.

#### Tagging a release (candidate)

To tag the version (or release candidate) in git, use the `make-tag.py` script from [groestlcoin-maintainer-tools](https://github.com/groestlcoin/groestlcoin-maintainer-tools). From the root of the repository run:

    ../groestlcoin-maintainer-tools/make-tag.py v(new version, e.g. 25.0)

This will perform a few last-minute consistency checks in the build system files, and if they pass, create a signed tag.

## Building

### First time / New builders

Install Guix using one of the installation methods detailed in
[contrib/guix/INSTALL.md](/contrib/guix/INSTALL.md).

Check out the source code in the following directory hierarchy.

    cd /path/to/your/toplevel/build
    git clone https://github.com/groestlcoin/guix.sigs.git
    git clone https://github.com/groestlcoin/groestlcoin-detached-sigs.git
    git clone https://github.com/groestlcoin/groestlcoin.git

### Setup and perform Guix builds

Checkout the Groestlcoin Core version you'd like to build:

```sh
pushd ./groestlcoin
SIGNER='(your builder key, ie jackielove4u, hashengineering, etc)'
VERSION='(new version without v-prefix, e.g. 25.0)'
git fetch origin "v${VERSION}"
git checkout "v${VERSION}"
popd
```

Ensure your guix.sigs are up-to-date if you wish to `guix-verify` your builds
against other `guix-attest` signatures.

```sh
git -C ./guix.sigs pull
```

### Create the macOS SDK tarball (first time, or when SDK version changes)

Create the macOS SDK tarball, see the [macdeploy
instructions](/contrib/macdeploy/README.md#deterministic-macos-app-notes) for
details.

### Build and attest to build outputs

Follow the relevant Guix README.md sections:
- [Building](/contrib/guix/README.md#building)
- [Attesting to build outputs](/contrib/guix/README.md#attesting-to-build-outputs)

### Verify other builders' signatures to your own (optional)

- [Verifying build output attestations](/contrib/guix/README.md#verifying-build-output-attestations)

### Commit your non codesigned signature to guix.sigs

```sh
pushd ./guix.sigs
git add "${VERSION}/${SIGNER}"/noncodesigned.SHA256SUMS{,.asc}
git commit -m "Add attestations by ${SIGNER} for ${VERSION} non-codesigned"
popd
```

Then open a Pull Request to the [guix.sigs repository](https://github.com/Groestlcoin/guix.sigs).

## Codesigning

### macOS codesigner only: Create detached macOS signatures (assuming [signapple](https://github.com/achow101/signapple/) is installed and up to date with master branch)

In the `guix-build-${VERSION}/output/x86_64-apple-darwin` and `guix-build-${VERSION}/output/arm64-apple-darwin` directories:

    tar xf groestlcoin-osx-unsigned.tar.gz
    ./detached-sig-create.sh /path/to/codesign.p12
    Enter the keychain password and authorize the signature
    signature-osx.tar.gz will be created

### Windows codesigner only: Create detached Windows signatures

In the `guix-build-${VERSION}/output/x86_64-w64-mingw32` directory:

    tar xf groestlcoin-win-unsigned.tar.gz
    ./detached-sig-create.sh -key /path/to/codesign.key
    Enter the passphrase for the key when prompted
    signature-win.tar.gz will be created

### Windows and macOS codesigners only: test code signatures
It is advised to test that the code signature attaches properly prior to tagging by performing the `guix-codesign` step.
However if this is done, once the release has been tagged in the groestlcoin-detached-sigs repo, the `guix-codesign` step must be performed again in order for the guix attestation to be valid when compared against the attestations of non-codesigner builds. The directories created by `guix-codesign` will need to be cleared prior to running `guix-codesign` again.

### Windows and macOS codesigners only: Commit the detached codesign payloads

```sh
pushd ./groestlcoin-detached-sigs
# checkout or create the appropriate branch for this release series
git checkout --orphan <branch>
# if you are the macOS codesigner
rm -rf osx
tar xf signature-osx.tar.gz
# if you are the windows codesigner
rm -rf win
tar xf signature-win.tar.gz
git add -A
git commit -m "<version>: {osx,win} signature for {rc,final}"
git tag -s "v${VERSION}" HEAD
git push the current branch and new tag
popd
```

### Non-codesigners: wait for Windows and macOS detached signatures

- Once the Windows and macOS builds each have 3 matching signatures, they will be signed with their respective release keys.
- Detached signatures will then be committed to the [groestlcoin-detached-sigs](https://github.com/Groestlcoin/groestlcoin-detached-sigs) repository, which can be combined with the unsigned apps to create signed binaries.

### Create the codesigned build outputs

- [Codesigning build outputs](/contrib/guix/README.md#codesigning-build-outputs)

### Verify other builders' signatures to your own (optional)

- [Verifying build output attestations](/contrib/guix/README.md#verifying-build-output-attestations)

### Commit your codesigned signature to guix.sigs (for the signed macOS/Windows binaries)

```sh
pushd ./guix.sigs
git add "${VERSION}/${SIGNER}"/all.SHA256SUMS{,.asc}
git commit -m "Add attestations by ${SIGNER} for ${VERSION} codesigned"
popd
```

Then open a Pull Request to the [guix.sigs repository](https://github.com/Groestlcoin/guix.sigs).

## After 6 or more people have guix-built and their results match

After verifying signatures, combine the `all.SHA256SUMS.asc` file from all signers into `SHA256SUMS.asc`:

```bash
cat "$VERSION"/*/all.SHA256SUMS.asc > SHA256SUMS.asc
```

- Update groestlcoin.org version

- Archive the release notes for the new version to `doc/release-notes/release-notes-${VERSION}.md`
  (branch `master` and branch of the release).

- Update packaging repo

      - Push the flatpak to flathub, e.g. https://github.com/flathub/org.groestlcoin.groestlcoin-qt/pull/6

      - Push the snap, see https://github.com/groestlcoin/packaging/blob/master/snap/build.md

- This repo

      - Archive the release notes for the new version to `doc/release-notes/` (branch `master` and branch of the release)

      - Create a [new GitHub release](https://github.com/groestlcoin/groestlcoin/releases/new) with a link to the archived release notes

- Announce the release:

      - groestlcoin.org/forum post

      - Update title of #groestlcoin on Freenode IRC

      - Twitter, Reddit /r/Groestlcoin, Facebook, Telegram, Discord...

      - Build [the PPAs](https://launchpad.net/~groestlcoin/+archive/ubuntu/groestlcoin)

      - Create a [new GitHub release](https://github.com/groestlcoin/groestlcoin/releases/new) with a link to the archived release notes.

      - Celebrate

### Additional information

#### <a name="how-to-calculate-assumed-blockchain-and-chain-state-size"></a>How to calculate `m_assumed_blockchain_size` and `m_assumed_chain_state_size`

Both variables are used as a guideline for how much space the user needs on their drive in total, not just strictly for the blockchain.
Note that all values should be taken from a **fully synced** node and have an overhead of 5-10% added on top of its base value.

To calculate `m_assumed_blockchain_size`, take the size in GiB of these directories:
- For `mainnet` -> the data directory, excluding the `/testnet3`, `/testnet4`, `/signet`, and `/regtest` directories and any overly large files, e.g. a huge `debug.log`
- For `testnet` -> `/testnet3`
- For `testnet4` -> `/testnet4`
- For `signet` -> `/signet`

To calculate `m_assumed_chain_state_size`, take the size in GiB of these directories:
- For `mainnet` -> `/chainstate`
- For `testnet` -> `/testnet3/chainstate`
- For `testnet4` -> `/testnet4/chainstate`
- For `signet` -> `/signet/chainstate`

Notes:
- When taking the size for `m_assumed_blockchain_size`, there's no need to exclude the `/chainstate` directory since it's a guideline value and an overhead will be added anyway.
- The expected overhead for growth may change over time. Consider whether the percentage needs to be changed in response; if so, update it here in this section.
