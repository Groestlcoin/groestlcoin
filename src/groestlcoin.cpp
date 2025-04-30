// Copyright (c) 2014-2025 The Groestlcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <groestlcoin.h>
#include <arith_uint256.h>
#include <chain.h>
#include <consensus/params.h>
#include <crypto/sha256.h>
#include <chainparams.h>
#include <consensus/merkle.h>
#include <climits>

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
