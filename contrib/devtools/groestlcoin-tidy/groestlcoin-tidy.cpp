// Copyright (c) 2023 Bitcoin Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "nontrivial-threadlocal.h"

#include <clang-tidy/ClangTidyModule.h>

class BitcoinModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<bitcoin::NonTrivialThreadLocal>("groestlcoin-nontrivial-threadlocal");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<BitcoinModule>
    X("groestlcoin-module", "Adds groestlcoin checks.");

volatile int BitcoinModuleAnchorSource = 0;
