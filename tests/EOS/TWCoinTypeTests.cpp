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


TEST(TWEOSCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeEOS));
    auto txId = TWStringCreateWithUTF8Bytes("123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeEOS, txId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeEOS));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeEOS));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeEOS), 4);
    ASSERT_EQ(TWBlockchainEOS, TWCoinTypeBlockchain(TWCoinTypeEOS));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeEOS));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeEOS));
    assertStringsEqual(symbol, "EOS");
    assertStringsEqual(txUrl, "https://bloks.io/transaction/123");
    assertStringsEqual(id, "eos");
    assertStringsEqual(name, "EOS");
}
