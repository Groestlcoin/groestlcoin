#!/usr/bin/env python3
#
# Copyright (c) 2018-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Download or build previous releases.
# Needs curl and tar to download a release, or the build dependencies when
# building a release.

import argparse
import contextlib
from fnmatch import fnmatch
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import hashlib

SHA256_SUMS = {
    "e83406ebf473c7d69da02d2f09539a710cb82b090116ff8f9b63e35a41608577": {"tag": "v2.17.2", "tarball": "groestlcoin-2.17.2-aarch64-linux-gnu.tar.gz"},
    "5db693a615ce60e817af16cdcae51c4ac03e31f9588ed63447a57c343e1e4c7c": {"tag": "v2.17.2", "tarball": "groestlcoin-2.17.2-arm-linux-gnueabihf.tar.gz"},
    "b3fe245752a445ce56cac265af7ed63906c7c1c8e2c932891369be72c290307d": {"tag": "v2.17.2", "tarball": "groestlcoin-2.17.2-x86_64-apple-darwin14.tar.gz"},
    "e90f6ceb56fbc86ae17ee3c5d6d3913c422b7d98aa605226adb669acdf292e9e": {"tag": "v2.17.2", "tarball": "groestlcoin-2.17.2-x86_64-linux-gnu.tar.gz"},
    #
    "99f7a11f9f59c09f44b64f5631b73b7e98cb27a8fc35ba67c85310001b644e43": {"tag": "v2.18.2", "tarball": "groestlcoin-2.18.2-aarch64-linux-gnu.tar.gz"},
    "93c093b7684623c1cdf864cfacdec653ac6808d01f48ec9d1ffe26479623c6f5": {"tag": "v2.18.2", "tarball": "groestlcoin-2.18.2-arm-linux-gnueabihf.tar.gz"},
    "fdb722b326433501b179a33ac20e88b5fd587a249878eb94a9981da2097c42a5": {"tag": "v2.18.2", "tarball": "groestlcoin-2.18.2-osx64.tar.gz"},
    "08472eb96cb12b6ff6bebf18237ed05e0c236376446655bdb5901a42d781f75a": {"tag": "v2.18.2", "tarball": "groestlcoin-2.18.2-riscv64-linux-gnu.tar.gz"},
    "9ee26e1cd7967d0dc88670dbbdb99f95236ebc218f75977efb23f03ad8b74250": {"tag": "v2.18.2", "tarball": "groestlcoin-2.18.2-x86_64-linux-gnu.tar.gz"},
    #
    "f151cfada51e981db2cb90e78f50e13ba3c64640339f41d4f212d428df8ca1bf": {"tag": "v2.19.1", "tarball": "groestlcoin-2.19.1-aarch64-linux-gnu.tar.gz"},
    "218218b18b49d924fc4085545ed71a59a985dbecdc2dab48c401d3ac622f10d4": {"tag": "v2.19.1", "tarball": "groestlcoin-2.19.1-arm-linux-gnueabihf.tar.gz"},
    "902d38bea03fded2762acd1855cddd4a7b210acac9921ea56d816e622c4244ba": {"tag": "v2.19.1", "tarball": "groestlcoin-2.19.1-osx64.tar.gz"},
    "624f8d6a442557a87aed8f84c5949bd3b424f87a0f7cccb927ba8ade99768a78": {"tag": "v2.19.1", "tarball": "groestlcoin-2.19.1-riscv64-linux-gnu.tar.gz"},
    "0646cae023a0be0821f357d33bdbf81fc05fc9a9e3e9d4e5936d5053f1a988d4": {"tag": "v2.19.1", "tarball": "groestlcoin-2.19.1-x86_64-linux-gnu.tar.gz"},
    #
    "989a626db8178f9d26181fc4c38ba920b5a27ad236ab9d7035f59d291ad8ed6b": {"tag": "v2.20.1", "tarball": "groestlcoin-2.20.1-aarch64-linux-gnu.tar.gz"},
    "4c5915aed6e3c7aabc77040c91c133d29bc757a1f9f19477204f5aa75a485444": {"tag": "v2.20.1", "tarball": "groestlcoin-2.20.1-arm-linux-gnueabihf.tar.gz"},
    "16564cf6df5f4edead0f8a807f285e34e9f20b2770c2f66ab803de5152e38d3b": {"tag": "v2.20.1", "tarball": "groestlcoin-2.20.1-osx64.tar.gz"},
    "b0e7a1a18d29975b76281450dc744a64461aa4660fa8a09fafa7582e9c7b386b": {"tag": "v2.20.1", "tarball": "groestlcoin-2.20.1-riscv64-linux-gnu.tar.gz"},
    "0a877be9dac14f4d9aab95d6bfd51547275acbcc3e6553f0cb82c5c9f35f333c": {"tag": "v2.20.1", "tarball": "groestlcoin-2.20.1-x86_64-linux-gnu.tar.gz"},

    "aac5246f1be90f34914a82c65308b3981e78e6ce0c7d51f3c8fe6bae7455eef0": {"tag": "v2.21.1", "tarball": "groestlcoin-2.21.1-aarch64-linux-gnu.tar.gz"},
    "3fd6c2f1cd01d73314bc6e9abc13947f3477446cab5e8665a8cb0919063066f2": {"tag": "v2.21.1", "tarball": "groestlcoin-2.21.1-arm-linux-gnueabihf.tar.gz"},
    "d3b36a30f2a9a624087414820237df0ec5289a52fc24c3a1a38f67be7698073c": {"tag": "v2.21.1", "tarball": "groestlcoin-2.21.1-osx64.tar.gz"},
    "d6e200ad3000951a460cee66e6a5d3e2067112b40841a18a0a6dab6b70f531f0": {"tag": "v2.21.1", "tarball": "groestlcoin-2.21.1-riscv64-linux-gnu.tar.gz"},
    "b03bd4211f9473b39a12f6e57e64fe7bc9f2f54f27491f46f5b1bff5b96db7a5": {"tag": "v2.21.1", "tarball": "groestlcoin-2.21.1-x86_64-linux-gnu.tar.gz"},

    "8ab192b779a694701c0e8e990162a59adac7c9694ec6fc982a49a69dc3726706": {"tag": "v22.0", "tarball": "groestlcoin-22.0-aarch64-linux-gnu.tar.gz"},
    "823520bd8c0b75aee99321ce42109577deaeefb521e469ab2d43f2652640b1ab": {"tag": "v22.0", "tarball": "groestlcoin-22.0-arm-linux-gnueabihf.tar.gz"},
    "bdcdfac563eb54bc3de185c9b92200a36ccbd10d018aebd665e0bbe65a4480db": {"tag": "v22.0", "tarball": "groestlcoin-22.0-osx64.tar.gz"},
    "13ee5cb19c5ea99db34976850d9ed035b77329712e564f611be977e642adc14b": {"tag": "v22.0", "tarball": "groestlcoin-22.0-powerpc64-linux-gnu.tar.gz"},
    "81958ed496fb2141b9adbbabea5de3494278eca54d0f5974494dbc37bcd2e881": {"tag": "v22.0", "tarball": "groestlcoin-22.0-powerpc64le-linux-gnu.tar.gz"},
    "4e195a69a92075e69923715a9f0e01a2444fefecae7421ab7188fdd7b7a6e212": {"tag": "v22.0", "tarball": "groestlcoin-22.0-riscv64-linux-gnu.tar.gz"},
    "b30c5353dd3d9cfd7e8b31f29eac125925751165f690bacff57effd76560dddd": {"tag": "v22.0", "tarball": "groestlcoin-22.0-x86_64-linux-gnu.tar.gz"},

    "a8a68643bc05a4f2bbc1898e6c48c666c33ff4558d5eb87e6853eba94c091648": {"tag": "v23.0", "tarball": "groestlcoin-23.0-aarch64-linux-gnu.tar.gz"},
    "72b22d3cc6a9120d57c8141da6aca2ea445a42b2a5a8ed8864562db43187b1df": {"tag": "v23.0", "tarball": "groestlcoin-23.0-arm-linux-gnueabihf.tar.gz"},
    "9bf749e3940c57fd0591cfade179795feb97439d936910cb1edfb4cb05605ef4": {"tag": "v23.0", "tarball": "groestlcoin-23.0-arm64-apple-darwin.tar.gz"},
    "43fefb1678347b4684e2e9efcbd98c865c35ba26e7e4b771a3806c7a471a87af": {"tag": "v23.0", "tarball": "groestlcoin-23.0-powerpc64-linux-gnu.tar.gz"},
    "dd456872789cd3c17d4cddf15be150c77108a0a68290db44aa17861ceebb655a": {"tag": "v23.0", "tarball": "groestlcoin-23.0-powerpc64le-linux-gnu.tar.gz"},
    "5dfbd033c31bf974301c0f4cb4bc3bd91783a8233f79ddc9e4e788ddd3de46dd": {"tag": "v23.0", "tarball": "groestlcoin-23.0-riscv64-linux-gnu.tar.gz"},
    "ff80c843a29c515e052972019915fa302584b0c045ca69ee3a1572013743d385": {"tag": "v23.0", "tarball": "groestlcoin-23.0-x86_64-apple-darwin.tar.gz"},
    "46ab078422d0d2aaf5b89ac9603cb61a6ebf6c26a73b9440365a4df5f9bce7de": {"tag": "v23.0", "tarball": "groestlcoin-23.0-x86_64-linux-gnu.tar.gz"},

    "ca316c369728348406778c30b2b567bb2ede1ebcc87fb0305c0bed3dacae762b": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-aarch64-linux-gnu.tar.gz"},
    "28299bc5eccbf715c9b876467da7c12cd8b5261753ed614a7d3a81c9da79dcb8": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-arm-linux-gnueabihf.tar.gz"},
    "569d3519b5855f7576452c8b666eb26cec175296bfa685f311652e3e718ab49d": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-arm64-apple-darwin.tar.gz"},
    "6aad23f3b7f1c80a8834f5c36b8e43cc6786cac4cad1e42bdae604a9d90b82ca": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-powerpc64-linux-gnu.tar.gz"},
    "08b1e015b2719c5bc45a3dd62c335185a428fd0e54b70ba886403e7255a11e35": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-powerpc64le-linux-gnu.tar.gz"},
    "9372153a559ae034176c7cdd51a231a4d3afa1ab3b5f53b4769d7fa7b1beff96": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-riscv64-linux-gnu.tar.gz"},
    "8a99765cd01686b81480dc29ee70aa5619d429402685f164e2a4b0b4d592ac10": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-x86_64-apple-darwin.tar.gz"},
    "4b69743190e2697d7b7772bf6f63cde595d590ff6664abf15a7201dab2a6098b": {"tag": "v24.0.1", "tarball": "groestlcoin-24.0.1-x86_64-linux-gnu.tar.gz"},

    "d8776b405113b46d6be6e4921c5a5e62cbfaa5329087abbec14cc24d750f9c94": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-aarch64-linux-gnu.tar.gz"},
    "4e984db13cd6e8294e0b868b3937e7d4eb24464eac371d2978a8338f3f80c16f": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-arm-linux-gnueabihf.tar.gz"},
    "bd3d71be0d3b76cd9d02b2dda8372a7c483b475eda7a4d15037f434c387309ad": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-arm64-apple-darwin.tar.gz"},
    "249726b3b4b2d9011d0866776d97ffa76be9ea5135dc4157b56fc30fc50168f1": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-powerpc64-linux-gnu.tar.gz"},
    "0385b840381afdd7f0d4115d1fa9e298a984823a0f4875556029f72a9a38aadd": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-powerpc64le-linux-gnu.tar.gz"},
    "6e7562dd44cf052fc14c61090fdcb048e6fcc95f0c1702edf774a76b56a508c3": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-riscv64-linux-gnu.tar.gz"},
    "f9ca13def63d0722100417b880cd83b178f0770c2030838482bd5ef420547a11": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-x86_64-apple-darwin.tar.gz"},
    "bcca36b5a2f1e83a4fd9888bc0016d3f46f9ef01238dc23a8e03f2f4ac3b9707": {"tag": "v24.0.1", "tarball": "groestlcoin-25.0-x86_64-linux-gnu.tar.gz"},
}


@contextlib.contextmanager
def pushd(new_dir) -> None:
    previous_dir = os.getcwd()
    os.chdir(new_dir)
    try:
        yield
    finally:
        os.chdir(previous_dir)


def download_binary(tag, args) -> int:
    if Path(tag).is_dir():
        if not args.remove_dir:
            print('Using cached {}'.format(tag))
            return 0
        shutil.rmtree(tag)
    Path(tag).mkdir()
    bin_path = 'Groestlcoin/groestlcoin/releases/download/v{}'.format(tag[1:])
    match = re.compile('v(.*)(rc[0-9]+)$').search(tag)
    if match:
        bin_path = 'Groestlcoin/groestlcoin/releases/download/v{}/test.{}'.format(
            match.group(1), match.group(2))
    platform = args.platform
    if tag < "v23" and platform in ["x86_64-apple-darwin", "arm64-apple-darwin"]:
        platform = "osx64"
    tarball = 'groestlcoin-{tag}-{platform}.tar.gz'.format(
        tag=tag[1:], platform=platform)
    tarballUrl = 'https://github.com/{bin_path}/{tarball}'.format(
        bin_path=bin_path, tarball=tarball)

    print('Fetching: {tarballUrl}'.format(tarballUrl=tarballUrl))

    ret = subprocess.run(['curl', '-L', '--fail', '--remote-name', tarballUrl]).returncode
    if ret:
        return ret

    hasher = hashlib.sha256()
    with open(tarball, "rb") as afile:
        hasher.update(afile.read())
    tarballHash = hasher.hexdigest()

    if tarballHash not in SHA256_SUMS or SHA256_SUMS[tarballHash]['tarball'] != tarball:
        if tarball in [v['tarball'] for v in SHA256_SUMS.values()]:
            print("Checksum did not match")
            return 1

        print("Checksum for given version doesn't exist")
        return 1
    print("Checksum matched")

    # Extract tarball
    ret = subprocess.run(['tar', '-zxf', tarball, '-C', tag,
                          '--strip-components=1',
                          'groestlcoin-{tag}'.format(tag=tag[1:])]).returncode
    if ret != 0:
        print(f"Failed to extract the {tag} tarball")
        return ret

    Path(tarball).unlink()

    if tag >= "v23" and platform == "arm64-apple-darwin":
        # Starting with v23 there are arm64 binaries for ARM (e.g. M1, M2) macs, but they have to be signed to run
        binary_path = f'{os.getcwd()}/{tag}/bin/'

        for arm_binary in os.listdir(binary_path):
            # Is it already signed?
            ret = subprocess.run(
                ['codesign', '-v', binary_path + arm_binary],
                stderr=subprocess.DEVNULL,  # Suppress expected stderr output
            ).returncode
            if ret == 1:
                # Have to self-sign the binary
                ret = subprocess.run(
                    ['codesign', '-s', '-', binary_path + arm_binary]
                ).returncode
                if ret != 0:
                    print(f"Failed to self-sign {tag} {arm_binary} arm64 binary")
                    return 1

                # Confirm success
                ret = subprocess.run(
                    ['codesign', '-v', binary_path + arm_binary]
                ).returncode
                if ret != 0:
                    print(f"Failed to verify the self-signed {tag} {arm_binary} arm64 binary")
                    return 1

    return 0


def build_release(tag, args) -> int:
    githubUrl = "https://github.com/groestlcoin/groestlcoin"
    if args.remove_dir:
        if Path(tag).is_dir():
            shutil.rmtree(tag)
    if not Path(tag).is_dir():
        # fetch new tags
        subprocess.run(
            ["git", "fetch", githubUrl, "--tags"])
        output = subprocess.check_output(['git', 'tag', '-l', tag])
        if not output:
            print('Tag {} not found'.format(tag))
            return 1
    ret = subprocess.run([
        'git', 'clone', f'--branch={tag}', '--depth=1', githubUrl, tag
    ]).returncode
    if ret:
        return ret
    with pushd(tag):
        host = args.host
        if args.depends:
            with pushd('depends'):
                ret = subprocess.run(['make', 'NO_QT=1']).returncode
                if ret:
                    return ret
                host = os.environ.get(
                    'HOST', subprocess.check_output(['./config.guess']))
        config_flags = '--prefix={pwd}/depends/{host} '.format(
            pwd=os.getcwd(),
            host=host) + args.config_flags
        cmds = [
            './autogen.sh',
            './configure {}'.format(config_flags),
            'make',
        ]
        for cmd in cmds:
            ret = subprocess.run(cmd.split()).returncode
            if ret:
                return ret
        # Move binaries, so they're in the same place as in the
        # release download
        Path('bin').mkdir(exist_ok=True)
        files = ['groestlcoind', 'groestlcoin-cli', 'groestlcoin-tx']
        for f in files:
            Path('src/'+f).rename('bin/'+f)
    return 0


def check_host(args) -> int:
    args.host = os.environ.get('HOST', subprocess.check_output(
        './depends/config.guess').decode())
    if args.download_binary:
        platforms = {
            'aarch64-*-linux*': 'aarch64-linux-gnu',
            'powerpc64le-*-linux-*': 'powerpc64le-linux-gnu',
            'riscv64-*-linux*': 'riscv64-linux-gnu',
            'x86_64-*-linux*': 'x86_64-linux-gnu',
            'x86_64-apple-darwin*': 'x86_64-apple-darwin',
            'aarch64-apple-darwin*': 'arm64-apple-darwin',
        }
        args.platform = ''
        for pattern, target in platforms.items():
            if fnmatch(args.host, pattern):
                args.platform = target
        if not args.platform:
            print('Not sure which binary to download for {}'.format(args.host))
            return 1
    return 0


def main(args) -> int:
    Path(args.target_dir).mkdir(exist_ok=True, parents=True)
    print("Releases directory: {}".format(args.target_dir))
    ret = check_host(args)
    if ret:
        return ret
    if args.download_binary:
        with pushd(args.target_dir):
            for tag in args.tags:
                ret = download_binary(tag, args)
                if ret:
                    return ret
        return 0
    args.config_flags = os.environ.get('CONFIG_FLAGS', '')
    args.config_flags += ' --without-gui --disable-tests --disable-bench'
    with pushd(args.target_dir):
        for tag in args.tags:
            ret = build_release(tag, args)
            if ret:
                return ret
    return 0


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-r', '--remove-dir', action='store_true',
                        help='remove existing directory.')
    parser.add_argument('-d', '--depends', action='store_true',
                        help='use depends.')
    parser.add_argument('-b', '--download-binary', action='store_true',
                        help='download release binary.')
    parser.add_argument('-t', '--target-dir', action='store',
                        help='target directory.', default='releases')
    all_tags = sorted([*set([v['tag'] for v in SHA256_SUMS.values()])])
    parser.add_argument('tags', nargs='*', default=all_tags,
                        help='release tags. e.g.: v2.18.2 v2.20.1 '
                        '(if not specified, the full list needed for '
                        'backwards compatibility tests will be used)'
                        )
    args = parser.parse_args()
    sys.exit(main(args))
