// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.
//
// This is a GENERATED FILE, changes made here MAY BE LOST.
// Generated one-time (codegen/bin/cointests)
//

#include "../interface/TWTestUtilities.h"
#include <TrustWalletCore/TWCoinTypeConfiguration.h>
#include <gtest/gtest.h>


TEST(TWGoChainCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeGoChain));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeGoChain, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeGoChain, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeGoChain));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeGoChain));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeGoChain), 18);
    ASSERT_EQ(TWBlockchainEthereum, TWCoinTypeBlockchain(TWCoinTypeGoChain));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeGoChain));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeGoChain));
    assertStringsEqual(symbol, "GO");
    assertStringsEqual(txUrl, "https://explorer.gochain.io/tx/t123");
    assertStringsEqual(accUrl, "https://explorer.gochain.io/addr/a12");
    assertStringsEqual(id, "gochain");
    assertStringsEqual(name, "GoChain");
}
