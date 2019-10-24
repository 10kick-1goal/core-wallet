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


TEST(TWCosmosCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeCosmos));
    auto txId = TWStringCreateWithUTF8Bytes("123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeCosmos, txId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeCosmos));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeCosmos));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeCosmos), 6);
    ASSERT_EQ(TWBlockchainCosmos, TWCoinTypeBlockchain(TWCoinTypeCosmos));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeCosmos));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeCosmos));
    assertStringsEqual(symbol, "ATOM");
    assertStringsEqual(txUrl, "https://www.mintscan.io/txs/123");
    assertStringsEqual(id, "cosmos");
    assertStringsEqual(name, "Cosmos");
}
