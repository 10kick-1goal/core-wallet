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


TEST(TWNebulasCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeNebulas));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeNebulas, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeNebulas, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeNebulas));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeNebulas));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeNebulas), 18);
    ASSERT_EQ(TWBlockchainNebulas, TWCoinTypeBlockchain(TWCoinTypeNebulas));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeNebulas));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeNebulas));
    assertStringsEqual(symbol, "NAS");
    assertStringsEqual(txUrl, "https://explorer.nebulas.io/#/tx/t123");
    assertStringsEqual(accUrl, "https://explorer.nebulas.io/#/address/a12");
    assertStringsEqual(id, "nebulas");
    assertStringsEqual(name, "Nebulas");
}
