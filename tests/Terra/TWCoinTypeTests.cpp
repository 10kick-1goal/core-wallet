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


TEST(TWTerraCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeTerra));
    auto txId = TWStringCreateWithUTF8Bytes("123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeTerra, txId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeTerra));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeTerra));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeTerra), 6);
    ASSERT_EQ(TWBlockchainCosmos, TWCoinTypeBlockchain(TWCoinTypeTerra));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeTerra));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeTerra));
    assertStringsEqual(symbol, "LUNA");
    assertStringsEqual(txUrl, "https://terra.stake.id/?#/tx/123");
    assertStringsEqual(id, "terra");
    assertStringsEqual(name, "Terra");
}
