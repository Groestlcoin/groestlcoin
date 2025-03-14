// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <groestlcoin.h>
#include <boost/assign/list_of.hpp>
#include <arith_uint256.h>
#include <chain.h>
#include <consensus/params.h>
#include <crypto/sha256.h>

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

using namespace util::hex_literals;

#include <assert.h>
#include <memory>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

// Workaround MSVC bug triggering C7595 when calling consteval constructors in
// initializer lists.
// A fix may be on the way:
// https://developercommunity.visualstudio.com/t/consteval-conversion-function-fails/1579014
#if defined(_MSC_VER)
auto consteval_ctor(auto&& input) { return input; }
#else
#define consteval_ctor(input) (input)
#endif

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ac5927c59, ver=112, hashPrevBlock=00000000000000, hashMerkleRoot=3ce968, nTime=1395342829, nBits=1e0fffff, nNonce=220035, vtx=1)
 *   CTransaction(hash=3ce968, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104325072657373757265206d75737420626520707574206f6e20566c6164696d697220507574696e206f766572204372696d6561)
 *     CTxOut(nValue=0.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 3ce968
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Pressure must be put on Vladimir Putin over Crimea";
    const CScript genesisOutputScript = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        // GRS consensus.nSubsidyHalvingInterval = 210000;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"00000000000002dc756eebf4f49723ed8d30cc28a5f108eb94b1ba88ac4f9c22"}, SCRIPT_VERIFY_NONE);
        consensus.script_flag_exceptions.emplace( // Taproot exception
            uint256{"0000000000000000000f14c35b2d841e986ab5441de8c585d5ffe55ea1e395ad"}, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_WITNESS);
        consensus.BIP34Height = 800000;
        consensus.BIP34Hash = uint256{"0000000007f3f37410d5f7e71a07bf09bb802d5af6726fc891f0248ad857708c"};
        consensus.BIP65Height = 2464000; // 00000000000030f90269dd2c0fb5f7502f332cd183b1596817f0cc4cfd6966b1
        consensus.BIP66Height = 800000; // 0000000007f3f37410d5f7e71a07bf09bb802d5af6726fc891f0248ad857708c
        consensus.CSVHeight = 1439424; // 00000000023a4b8e347ca3b40fe9f3001203a49c77046d4d1ae4c2c87b00bdd6
        consensus.SegwitHeight = 1439424; // 00000000023a4b8e347ca3b40fe9f3001203a49c77046d4d1ae4c2c87b00bdd6
        consensus.MinBIP9WarningHeight = 2466016; // BIP65 activation height + miner confirmation window
        consensus.powLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        // GRS consensus.fPowNoRetargeting = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1607990401; // December 15, 2020
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 1640908799; // December 31, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000001b0b4c379072797c72e"}; // groestlcoin-cli getblockheader 000000000000001cd6439ca77fb9c863617a30aaa03c4d0e01211aea9f84cefe | jq '{chainwork}'
        consensus.defaultAssumeValid = uint256{"000000000000001cd6439ca77fb9c863617a30aaa03c4d0e01211aea9f84cefe"}; // groestlcoin-cli getblockhash 5242424

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xd4;
        nDefaultPort = 1331;
        nPruneAfterHeight = 10000000;
        m_assumed_blockchain_size = 6; // du -cha --max-depth=1 --exclude=debug.log --exclude=.groestlcoin/testnet3 --exclude=.groestlcoin/signet --exclude=.groestlcoin/regtest .groestlcoin
        m_assumed_chain_state_size = 1; // du -cha --max-depth=1 .groestlcoin/chainstate/

        genesis = CreateGenesisBlock(1395342829, 220035, 0x1e0fffff, 112, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000ac5927c594d49cc0bdb81759d0da8297eb614683d3acb62f0703b639023"});
        assert(genesis.hashMerkleRoot == uint256{"3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"});

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("dnsseed1.groestlcoin.org."); // only supports x1, x5, x9, and xd
        vSeeds.emplace_back("dnsseed2.groestlcoin.org."); // only supports x1, x5, x9, and xd
        vSeeds.emplace_back("dnsseed3.groestlcoin.org."); // only supports x1, x5, x9, and xd
        vSeeds.emplace_back("dnsseed4.groestlcoin.org."); // only supports x1, x5, x9, and xd

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,36);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "grs";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {28888, uint256{"00000000000228ce19f55cf0c45e04c7aa5a6a873ed23902b3654c3c49884502"}},
                {58888, uint256{"0000000000dd85f4d5471febeb174a3f3f1598ab0af6616e9f266b56272274ef"}},
                {111111, uint256{"00000000013de206275ee83f93bee57622335e422acbf126a37020484c6e113c"}},
                {1000000, uint256{"000000000df8560f2612d5f28b52ed1cf81b0f87ac0c9c7242cbcf721ca6854a"}},
                {2000000, uint256{"00000000000434d5b8d1c3308df7b6e3fd773657dfb28f5dd2f70854ef94cc66"}},
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch like this: ./contrib/devtools/utxo_snapshot.sh 4794500 utxo.dat groestlcoin-cli
        };

        chainTxData = ChainTxData{
            // Data from RPC: groestlcoin-cli getchaintxstats 4096 000000000000001cd6439ca77fb9c863617a30aaa03c4d0e01211aea9f84cefe | jq '{time,txcount,txrate}'
            .nTime    = 1725356434,
            .tx_count = 6681557,
            .dTxRate  = 0.01872759335697537,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        // GRS consensus.nSubsidyHalvingInterval = 210000;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"00000000dd30457c001f4095d208cc1296b0eed002427aa599874af7a432b105"}, SCRIPT_VERIFY_NONE);
        consensus.BIP34Height = 286;
        consensus.BIP34Hash = uint256{"0000004b7778ba253a75b716c55b2c6609b5fb97691b3260978f9ce4a633106d"};
        consensus.BIP65Height = 982000; // 000000204a7e703f80543d9329d4b90e4269e08f36ad746cfe145add340b8738
        consensus.BIP66Height = 286; // 0000004b7778ba253a75b716c55b2c6609b5fb97691b3260978f9ce4a633106d
        consensus.CSVHeight = 6048; // 00000045ca094ba21d5ae2820b526cd4973b49746737b2e6b5b02c5ddba0ea26
        consensus.SegwitHeight = 6048; // 00000045ca094ba21d5ae2820b526cd4973b49746737b2e6b5b02c5ddba0ea26
        consensus.MinBIP9WarningHeight = 984016; // BIP65 activation height + miner confirmation window
        consensus.powLimit = uint256{"000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        // GRS consensus.fPowNoRetargeting = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1607990401; // December 15, 2020
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 1640908799; // December 31, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000000000565b4b0a22be"}; // groestlcoin-cli -testnet getblockheader 0000002e250f3192476ec31f70d36ab807894c05d92f43b32edcea7e380b1a3f | jq '{chainwork}'
        consensus.defaultAssumeValid = uint256{"0000002e250f3192476ec31f70d36ab807894c05d92f43b32edcea7e380b1a3f"}; // groestlcoin-cli -testnet getblockhash 3848484

        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x07;
        nDefaultPort = 17777;
        nPruneAfterHeight = 1000000;
        m_assumed_blockchain_size = 3; //  du -cha --max-depth=1 .groestlcoin/testnet3
        m_assumed_chain_state_size = 1; // du -cha --max-depth=1 .groestlcoin/testnet3/chainstate/

        genesis = CreateGenesisBlock(1440000002, 6556309, 0x1e00ffff, 3, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36"});
        assert(genesis.hashMerkleRoot == uint256{"3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"});

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-seed1.groestlcoin.org."); // only supports x1, x5, x9, and xd
        vSeeds.emplace_back("testnet-seed2.groestlcoin.org."); // only supports x1, x5, x9, and xd

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tgrs";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                { 50000 , uint256{"00000081951486bb535f8cffec8ac0641bd24b814f89641f6cc2cad737f18950"}},
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: groestlcoin-cli -testnet getchaintxstats 4096 0000002e250f3192476ec31f70d36ab807894c05d92f43b32edcea7e380b1a3f | jq '{time,txcount,txrate}'
            .nTime    = 1725544040,
            .tx_count = 3869795,
            .dTxRate  = 0.01566120410800725,
        };
    }
};

/**
 * Testnet (v4): public test network which is reset from time to time.
 */
class CTestNet4Params : public CChainParams {
public:
    CTestNet4Params() {
        m_chain_type = ChainType::TESTNET4;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        // GRS consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x1c;
        pchMessageStart[1] = 0x16;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x28;
        nDefaultPort = 48333;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        const char* testnet4_genesis_msg = "Pressure must be put on Vladimir Putin over Crimea";
        const CScript testnet4_genesis_script = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;
        genesis = CreateGenesisBlock(testnet4_genesis_msg,
                testnet4_genesis_script,
                1725525340,
                6485621,
                0x1e00ffff,
                3,
                0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"000000519399f41322d7a8c24bfdc409aa13a80160b43a57668c2ad62802fa5c"});
        assert(genesis.hashMerkleRoot == uint256{"3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"});

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet4-seed1.groestlcoin.org.");
        vSeeds.emplace_back("testnet4-seed2.groestlcoin.org.");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tgrs";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_testnet4), std::end(chainparams_seed_testnet4));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {},
            }
        };

        m_assumeutxo_data = {
            {}
        };

        chainTxData = ChainTxData{
            .nTime    = 0,
            .tx_count = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vFixedSeeds.clear();
        vSeeds.clear();

        if (!options.challenge) {
            bin = "51210379156a07950b904a74e0d276e6d96bb61c4e0f89c9f69d3a5f75f161c9f8684051ae"_hex_v_u8;
            vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_signet), std::end(chainparams_seed_signet));
            vSeeds.emplace_back("198.199.105.43");
            vSeeds.emplace_back("2604:a880:1:20::96:6001");
            vSeeds.emplace_back("ubmgcth2ngfb7qapyrkpnn3i6p2dmu76zvd3hfs2mw3u4t54v2qa66id.onion:31331");

            consensus.nMinimumChainWork = uint256{"000000000000000000000000000000000000000000000000000008101a6b9e1f"}; // groestlcoin-cli -signet getblockheader 000001e7f1aaa450a37cd29f349acf60cb176cae3c3ee5f4bf0e712d90e950d4 | jq '{chainwork}'
            consensus.defaultAssumeValid = uint256{"000001e7f1aaa450a37cd29f349acf60cb176cae3c3ee5f4bf0e712d90e950d4"}; // groestlcoin-cli -signet getblockhash 1750000
            m_assumed_blockchain_size = 2; // du -cha --max-depth=1 .groestlcoin/signet
            m_assumed_chain_state_size = 1; // du -cha --max-depth=1 .groestlcoin/signet/chainstate/
            chainTxData = ChainTxData{
                // Data from RPC: groestlcoin-cli -signet getchaintxstats 4096 000001e7f1aaa450a37cd29f349acf60cb176cae3c3ee5f4bf0e712d90e950d4 | jq '{time,txcount,txrate}'
                .nTime    = 1718365862,
                .tx_count = 1750015,
                .dTxRate  = 0.0159724848990606,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        // GRS consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        // GRS consensus.fPowNoRetargeting = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000377ae000000000000000000000000000000000000000000000000000000"};
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 31331;
        nPruneAfterHeight = 10000000;

        genesis = CreateGenesisBlock(1606082400, 14675970, 0x1e00ffff, 3, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"0000007fcaa2a27993c6cde9e7818c254357af517b876ceba2f23592bb14ab31"});
        assert(genesis.hashMerkleRoot == uint256{"3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"});

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tgrs";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        // GRS consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 1;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = opts.enforce_bip94;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nDefaultPort = 18888;
        nPruneAfterHeight = opts.fastprune ? 100 : 10000000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1440000002, 6556309, 0x1e00ffff, 3, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36"});
        assert(genesis.hashMerkleRoot == uint256{"3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"});

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256{"000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36"}},
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "grsrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    return std::make_unique<const CTestNet4Params>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto testnet4_msg = CChainParams::TestNet4()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::ranges::equal(message, mainnet_msg)) {
        return ChainType::MAIN;
    } else if (std::ranges::equal(message, testnet_msg)) {
        return ChainType::TESTNET;
    } else if (std::ranges::equal(message, testnet4_msg)) {
        return ChainType::TESTNET4;
    } else if (std::ranges::equal(message, regtest_msg)) {
        return ChainType::REGTEST;
    } else if (std::ranges::equal(message, signet_msg)) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
