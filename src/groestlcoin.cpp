// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2014-2022 The Groestlcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <groestlcoin.h>
#include <boost/assign/list_of.hpp>
#include <arith_uint256.h>
#include <chain.h>
#include <consensus/params.h>
#include <crypto/sha256.h>

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <deploymentinfo.h>
#include <hash.h> // for signet block challenge hash
#include <script/interpreter.h>
#include <util/system.h>

#include <assert.h>
#include <memory>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#ifdef _MSC_VER
#	include <intrin.h>
#endif

using namespace std;

static const int64_t nGenesisBlockRewardCoin = 0;
int64_t minimumSubsidy = 5.0 * COIN;
static const int64_t nPremine = 240640 * COIN;

int64_t static GetBlockSubsidy(int nHeight){


	if (nHeight == 0)
    {
        return nGenesisBlockRewardCoin;
    }

	if (nHeight == 1)
    {
        return nPremine;
		/*
		optimized standalone cpu miner 	60*512=30720
		standalone gpu miner 		120*512=61440
		first pool			70*512 =35840
		block-explorer		 	60*512 =30720
		mac wallet binary    		30*512 =15360
		linux wallet binary  		30*512 =15360
		web-site			100*512	=51200
		total				=240640
		*/
    }

	int64_t nSubsidy = 512 * COIN;

    // Subsidy is reduced by 6% every 10080 blocks, which will occur approximately every 1 week
    int exponent=(nHeight / 10080);
    for(int i=0;i<exponent;i++){
        nSubsidy=nSubsidy*47;
		nSubsidy=nSubsidy/50;
    }
    if(nSubsidy<minimumSubsidy){nSubsidy=minimumSubsidy;}
    return nSubsidy;
}

int64_t static GetBlockSubsidy120000(int nHeight)
{
	// Subsidy is reduced by 10% every day (1440 blocks)
	int64_t nSubsidy = 250 * COIN;
	int exponent = ((nHeight - 120000) / 1440);
	for(int i=0; i<exponent; i++)
		nSubsidy = (nSubsidy * 45) / 50;

	return nSubsidy;
}

int64_t static GetBlockSubsidy150000(int nHeight)
{
	static int heightOfMinSubsidy = INT_MAX;
	if (nHeight < heightOfMinSubsidy) {
		// Subsidy is reduced by 1% every week (10080 blocks)
		int64_t nSubsidy = 25 * COIN;
		int exponent = ((nHeight - 150000) / 10080);
		for (int i = 0; i < exponent; i++)
			nSubsidy = (nSubsidy * 99) / 100;

		if (nSubsidy >= minimumSubsidy)
			return nSubsidy;
		heightOfMinSubsidy = (min)(heightOfMinSubsidy, nHeight);
	}
	return minimumSubsidy;
}

CAmount GetBlockSubsidy(int nHeight, const Consensus::Params& consensusParams)
{
	return nHeight >= 150000 ? GetBlockSubsidy150000(nHeight)
		: nHeight >= 120000 ? GetBlockSubsidy120000(nHeight)
		: GetBlockSubsidy(nHeight);
}

//
// minimum amount of work that could possibly be required nTime after
// minimum work required was nBase
//
static const int64_t nTargetSpacing = 1 * 60; // groestlcoin every 60 seconds

//!!!BUG this function is non-deterministic  because FP-arithetics
unsigned int static DarkGravityWave(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params) {
    /* current difficulty formula, darkcoin - DarkGravity, written by Evan Duffield - evan@darkcoin.io */
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    int64_t nBlockTimeAverage = 0;
    int64_t nBlockTimeAveragePrev = 0;
    int64_t nBlockTimeCount = 0;
    int64_t nBlockTimeSum2 = 0;
    int64_t nBlockTimeCount2 = 0;
    int64_t LastBlockTime = 0;
    int64_t PastBlocksMin = 12;
    int64_t PastBlocksMax = 120;
    int64_t CountBlocks = 0;
    arith_uint256 PastDifficultyAverage;
    arith_uint256 PastDifficultyAveragePrev;

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMin) {
        return bnPowLimit.GetCompact();
	}

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        CountBlocks++;

        if(CountBlocks <= PastBlocksMin) {
            if (CountBlocks == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
            else { PastDifficultyAverage = ((arith_uint256().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / CountBlocks) + PastDifficultyAveragePrev; }
            PastDifficultyAveragePrev = PastDifficultyAverage;
        }

        if(LastBlockTime > 0){
            int64_t Diff = (LastBlockTime - BlockReading->GetBlockTime());
            if(Diff < 0) Diff = 0;
            if(nBlockTimeCount <= PastBlocksMin) {
                nBlockTimeCount++;

                if (nBlockTimeCount == 1) { nBlockTimeAverage = Diff; }
                else { nBlockTimeAverage = ((Diff - nBlockTimeAveragePrev) / nBlockTimeCount) + nBlockTimeAveragePrev; }
                nBlockTimeAveragePrev = nBlockTimeAverage;
            }
            nBlockTimeCount2++;
            nBlockTimeSum2 += Diff;
        }
        LastBlockTime = BlockReading->GetBlockTime();

        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }

    arith_uint256 bnNew(PastDifficultyAverage);
    if (nBlockTimeCount != 0 && nBlockTimeCount2 != 0) {
            double SmartAverage = (((nBlockTimeAverage)*0.7)+((nBlockTimeSum2 / nBlockTimeCount2)*0.3));
            if(SmartAverage < 1) SmartAverage = 1;
            double Shift = nTargetSpacing/SmartAverage;

            int64_t nActualTimespan = (CountBlocks*nTargetSpacing)/Shift;
            int64_t nTargetTimespan = (CountBlocks*nTargetSpacing);
            if (nActualTimespan < nTargetTimespan/3)
                nActualTimespan = nTargetTimespan/3;
            if (nActualTimespan > nTargetTimespan*3)
                nActualTimespan = nTargetTimespan*3;

            // Retarget
            bnNew *= nActualTimespan;
            bnNew /= nTargetTimespan;
    }

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int static DarkGravityWave3(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params) {
    /* current difficulty formula, darkcoin - DarkGravity v3, written by Evan Duffield - evan@darkcoin.io */
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    int64_t nActualTimespan = 0;
    int64_t LastBlockTime = 0;
    int64_t PastBlocksMin = 24;
    int64_t PastBlocksMax = 24;
    int64_t CountBlocks = 0;
    arith_uint256 PastDifficultyAverage;
    arith_uint256 PastDifficultyAveragePrev;

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMin) {
        return bnPowLimit.GetCompact();
    }

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        CountBlocks++;

        if(CountBlocks <= PastBlocksMin) {
            if (CountBlocks == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
            else { PastDifficultyAverage = ((PastDifficultyAveragePrev * CountBlocks)+(arith_uint256().SetCompact(BlockReading->nBits))) / (CountBlocks+1); }
            PastDifficultyAveragePrev = PastDifficultyAverage;
        }

        if(LastBlockTime > 0){
            int64_t Diff = (LastBlockTime - BlockReading->GetBlockTime());
            nActualTimespan += Diff;
        }
        LastBlockTime = BlockReading->GetBlockTime();

        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }

    arith_uint256 bnNew(PastDifficultyAverage);

    int64_t nTargetTimespan = CountBlocks*nTargetSpacing;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}
//----------------------

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params) {
    if (params.fPowAllowMinDifficultyBlocks)  {

		 // Special difficulty rule for testnet:
		 // If the new block's timestamp is more than 2* 10 minutes
		 // then allow mining of a min-difficulty block.

		if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
			return UintToArith256(params.powLimit).GetCompact();
    }

	if (pindexLast->nHeight >= (100000 - 1))
		return DarkGravityWave3(pindexLast, pblock, params);
    return DarkGravityWave(pindexLast, pblock, params);
}

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
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
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = CBaseChainParams::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        // GRS consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 800000;
        consensus.BIP34Hash = uint256S("0x0000000007f3f37410d5f7e71a07bf09bb802d5af6726fc891f0248ad857708c");
        consensus.BIP65Height = 2464000; // 00000000000030f90269dd2c0fb5f7502f332cd183b1596817f0cc4cfd6966b1
        consensus.BIP66Height = 800000; // 0000000007f3f37410d5f7e71a07bf09bb802d5af6726fc891f0248ad857708c
        consensus.CSVHeight = 1439424; // 00000000023a4b8e347ca3b40fe9f3001203a49c77046d4d1ae4c2c87b00bdd6
        consensus.SegwitHeight = 1439424; // 00000000023a4b8e347ca3b40fe9f3001203a49c77046d4d1ae4c2c87b00bdd6
        consensus.MinBIP9WarningHeight = 2466016; // BIP65 activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        // GRS consensus.fPowNoRetargeting = false;
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

        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000014299a42784ad100c55"); // groestlcoin-cli getblockheader 00000000000003d8488f78a40fec5c044d618bfadc61c46cc03cf732fe5bdb30 | jq '{chainwork}'
        consensus.defaultAssumeValid = uint256S("0x00000000000003d8488f78a40fec5c044d618bfadc61c46cc03cf732fe5bdb30"); // groestlcoin-cli getblockhash 4343434

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
        m_assumed_blockchain_size = 5;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1395342829, 220035, 0x1e0fffff, 112, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000ac5927c594d49cc0bdb81759d0da8297eb614683d3acb62f0703b639023"));
        assert(genesis.hashMerkleRoot == uint256S("0x3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"));

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
        fRequireStandard = true;
        m_is_test_chain = false;
        m_is_mockable_chain = false;

        #ifdef _MSC_VER //!!!
        checkpointData = CCheckpointData{
          #else
          checkpointData = (CCheckpointData) {
            #endif
            {
              {28888, uint256S("0x00000000000228ce19f55cf0c45e04c7aa5a6a873ed23902b3654c3c49884502")},
              {58888, uint256S("0x0000000000dd85f4d5471febeb174a3f3f1598ab0af6616e9f266b56272274ef")},
              {111111, uint256S("0x00000000013de206275ee83f93bee57622335e422acbf126a37020484c6e113c")},
              {1000000, uint256S("0x000000000df8560f2612d5f28b52ed1cf81b0f87ac0c9c7242cbcf721ca6854a")},
              {2000000, uint256S("0x00000000000434d5b8d1c3308df7b6e3fd773657dfb28f5dd2f70854ef94cc66")},
              {2372000, uint256S("0x000000000000117a4710e01e4f86d883ca491b96efa0b4f2139c4d49a9437f10")},
              {2785000, uint256S("0x00000000000013811b5078b06f3b98aaad29b94f09d047144e473de35f481474")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
         // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: groestlcoin-cli getchaintxstats 4096 00000000000003d8488f78a40fec5c044d618bfadc61c46cc03cf732fe5bdb30 | jq '{time,txcount,txrate}'
            /* nTime    */ 1668671210,
            /* nTxCount */ 5625708,
            /* dTxRate  */ 0.01891515456339227,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = CBaseChainParams::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        // GRS consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 286;
        consensus.BIP34Hash = uint256S("0x0000004b7778ba253a75b716c55b2c6609b5fb97691b3260978f9ce4a633106d");
        consensus.BIP65Height = 982000; // 000000204a7e703f80543d9329d4b90e4269e08f36ad746cfe145add340b8738
        consensus.BIP66Height = 286; // 0000004b7778ba253a75b716c55b2c6609b5fb97691b3260978f9ce4a633106d
        consensus.CSVHeight = 6048; // 00000045ca094ba21d5ae2820b526cd4973b49746737b2e6b5b02c5ddba0ea26
        consensus.SegwitHeight = 6048; // 00000045ca094ba21d5ae2820b526cd4973b49746737b2e6b5b02c5ddba0ea26
        consensus.MinBIP9WarningHeight = 984016; // BIP65 activation height + miner confirmation window
        consensus.powLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        // GRS consensus.fPowNoRetargeting = false;
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

        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000044d0bd050923"); // groestlcoin-cli -testnet getblockheader 00000010267849b3a75fb0882afd6aaf0e99f86e29a5d2a308efffaaa4cb94da | jq '{chainwork}'
        consensus.defaultAssumeValid = uint256S("0x00000010267849b3a75fb0882afd6aaf0e99f86e29a5d2a308efffaaa4cb94da"); // groestlcoin-cli -testnet getblockhash 2930293

        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x07;
        nDefaultPort = 17777;
        nPruneAfterHeight = 1000000;
        m_assumed_blockchain_size = 3;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1440000002, 6556309, 0x1e00ffff, 3, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36"));
        assert(genesis.hashMerkleRoot == uint256S("0x3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"));

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
        fRequireStandard = false;
        m_is_test_chain = true;
        m_is_mockable_chain = false;

        #ifdef _MSC_VER
        checkpointData = CCheckpointData{
          #else
          checkpointData = (CCheckpointData) {
            #endif
            {
              {0		, uint256S("0x000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36")},
              { 50000 , uint256S("0x00000081951486bb535f8cffec8ac0641bd24b814f89641f6cc2cad737f18950")},
              { 887766 , uint256S("0x000000cc069338b7cd182dbf3e98dedd80c9ce85f26b78daaec0a6c964ffa501")},
              { 1341000 , uint256S("0x00000022fb2732f1c237f9d90e5070e57500b19e9c65f51a2fb920b6580fc219")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: groestlcoin-cli -testnet getchaintxstats 4096 00000010267849b3a75fb0882afd6aaf0e99f86e29a5d2a308efffaaa4cb94da | jq '{time,txcount,txrate}'
            /* nTime    */ 1668663701,
            /* nTxCount */ 2949496,
            /* dTxRate  */ 0.01774712085893292,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const ArgsManager& args) {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!args.IsArgSet("-signetchallenge")) {
            bin = ParseHex("51210379156a07950b904a74e0d276e6d96bb61c4e0f89c9f69d3a5f75f161c9f8684051ae");
            // Hardcoded nodes can be removed once there are more DNS seeds
            vSeeds.emplace_back("198.199.105.43");
            vSeeds.emplace_back("2604:a880:1:20::96:6001");
            vSeeds.emplace_back("ubmgcth2ngfb7qapyrkpnn3i6p2dmu76zvd3hfs2mw3u4t54v2qa66id.onion:31331");

            consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000000000469e33250aa"); // groestlcoin-cli -signet getblockheader 0000023853e83e2f86258a245064f7cfbd260b9cdc52700833e6a186bfc30987 | jq '{chainwork}'
            consensus.defaultAssumeValid = uint256S("0x0000023853e83e2f86258a245064f7cfbd260b9cdc52700833e6a186bfc30987"); // groestlcoin-cli -signet getblockhash 960000
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                // Data from RPC: groestlcoin-cli -signet getchaintxstats 4096 0000023853e83e2f86258a245064f7cfbd260b9cdc52700833e6a186bfc30987 | jq '{time,txcount,txrate}'
                /* nTime    */ 1668664926,
                /* nTxCount */ 960007,
                /* dTxRate  */ 0.0159718620711169,
            };
        } else {
            const auto signet_challenge = args.GetArgs("-signetchallenge");
            if (signet_challenge.size() != 1) {
                throw std::runtime_error(strprintf("%s: -signetchallenge cannot be multiple values.", __func__));
            }
            bin = ParseHex(signet_challenge[0]);

            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", signet_challenge[0]);
        }

        if (args.IsArgSet("-signetseednode")) {
            vSeeds = args.GetArgs("-signetseednode");
        }

        strNetworkID = CBaseChainParams::SIGNET;
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
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000377ae000000000000000000000000000000000000000000000000000000");
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
        memcpy(pchMessageStart, hash.begin(), 4);

        nDefaultPort = 31331;
        nPruneAfterHeight = 10000000;

        genesis = CreateGenesisBlock(1606082400, 14675970, 0x1e00ffff, 3, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000007fcaa2a27993c6cde9e7818c254357af517b876ceba2f23592bb14ab31"));
        assert(genesis.hashMerkleRoot == uint256S("0x3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"));

        vFixedSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tgrs";

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams {
public:
    explicit CRegTestParams(const ArgsManager& args) {
        strNetworkID =  CBaseChainParams::REGTEST;
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
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 1;
        consensus.fPowAllowMinDifficultyBlocks = true;
        // GRS consensus.fPowNoRetargeting = true;
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
        nPruneAfterHeight = args.GetBoolArg("-fastprune", false) ? 100 : 10000000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

        genesis = CreateGenesisBlock(1440000002, 6556309, 0x1e00ffff, 3, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36"));
        assert(genesis.hashMerkleRoot == uint256S("0x3ce968df58f9c8a752306c4b7264afab93149dbc578bd08a42c446caaa6628bb"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = true;

        #ifdef _MSC_VER
        checkpointData = CCheckpointData{
          #else
          checkpointData = (CCheckpointData) {
            #endif
            {
              {0, uint256S("0x000000ffbb50fc9898cdd36ec163e6ba23230164c0052a28876255b7dcf2cd36")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
         // TODO to be specified in a future patch with https://github.com/groestlcoin/groestlcoin/blob/master/contrib/devtools/utxo_snapshot.sh
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

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int min_activation_height)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
        consensus.vDeployments[d].min_activation_height = min_activation_height;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

static void MaybeUpdateHeights(const ArgsManager& args, Consensus::Params& consensus)
{
    for (const std::string& arg : args.GetArgs("-testactivationheight")) {
        const auto found{arg.find('@')};
        if (found == std::string::npos) {
            throw std::runtime_error(strprintf("Invalid format (%s) for -testactivationheight=name@height.", arg));
        }
        const auto name{arg.substr(0, found)};
        const auto value{arg.substr(found + 1)};
        int32_t height;
        if (!ParseInt32(value, &height) || height < 0 || height >= std::numeric_limits<int>::max()) {
            throw std::runtime_error(strprintf("Invalid height value (%s) for -testactivationheight=name@height.", arg));
        }
        if (name == "segwit") {
            consensus.SegwitHeight = int{height};
        } else if (name == "bip34") {
            consensus.BIP34Height = int{height};
        } else if (name == "dersig") {
            consensus.BIP66Height = int{height};
        } else if (name == "cltv") {
            consensus.BIP65Height = int{height};
        } else if (name == "csv") {
            consensus.CSVHeight = int{height};
        } else {
            throw std::runtime_error(strprintf("Invalid name (%s) for -testactivationheight=name@height.", arg));
        }
    }
}

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    MaybeUpdateHeights(args, consensus);

    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() < 3 || 4 < vDeploymentParams.size()) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end[:min_activation_height]");
        }
        int64_t nStartTime, nTimeout;
        int min_activation_height = 0;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        if (vDeploymentParams.size() >= 4 && !ParseInt32(vDeploymentParams[3], &min_activation_height)) {
            throw std::runtime_error(strprintf("Invalid min_activation_height (%s)", vDeploymentParams[3]));
        }
        bool found = false;
        for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout, min_activation_height);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld, min_activation_height=%d\n", vDeploymentParams[0], nStartTime, nTimeout, min_activation_height);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const ArgsManager& args, const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN) {
        return std::unique_ptr<CChainParams>(new CMainParams());
    } else if (chain == CBaseChainParams::TESTNET) {
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    } else if (chain == CBaseChainParams::SIGNET) {
        return std::unique_ptr<CChainParams>(new SigNetParams(args));
    } else if (chain == CBaseChainParams::REGTEST) {
        return std::unique_ptr<CChainParams>(new CRegTestParams(args));
    }
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(gArgs, network);
}
