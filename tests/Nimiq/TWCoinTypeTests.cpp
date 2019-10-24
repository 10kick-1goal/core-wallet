// Copyright © 2017-2019 Trust Wallet.
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


TEST(TWNimiqCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeNimiq));
    auto txId = TWStringCreateWithUTF8Bytes("123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeNimiq, txId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeNimiq));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeNimiq));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeNimiq), 5);
    ASSERT_EQ(TWBlockchainNimiq, TWCoinTypeBlockchain(TWCoinTypeNimiq));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeNimiq));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeNimiq));
    assertStringsEqual(symbol, "NIM");
    assertStringsEqual(txUrl, "https://nimiq.watch/#123");
    assertStringsEqual(id, "nimiq");
    assertStringsEqual(name, "Nimiq");
}
