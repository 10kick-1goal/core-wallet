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


TEST(TWHarmonyCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeHarmony));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeHarmony, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeHarmony, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeHarmony));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeHarmony));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeHarmony), 18);
    ASSERT_EQ(TWBlockchainHarmony, TWCoinTypeBlockchain(TWCoinTypeHarmony));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeHarmony));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeHarmony));
    assertStringsEqual(symbol, "ONE");
    assertStringsEqual(txUrl, "https://explorer.harmony.one/#/tx/t123");
    assertStringsEqual(accUrl, "https://explorer.harmony.one/#/address/a12");
    assertStringsEqual(id, "harmony");
    assertStringsEqual(name, "Harmony");
}
