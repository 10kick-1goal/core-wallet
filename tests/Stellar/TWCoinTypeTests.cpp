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


TEST(TWStellarCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeStellar));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeStellar, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeStellar, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeStellar));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeStellar));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeStellar), 7);
    ASSERT_EQ(TWBlockchainStellar, TWCoinTypeBlockchain(TWCoinTypeStellar));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeStellar));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeStellar));
    assertStringsEqual(symbol, "XLM");
    assertStringsEqual(txUrl, "https://stellarscan.io/transaction/t123");
    assertStringsEqual(accUrl, "https://stellarscan.io/account/a12");
    assertStringsEqual(id, "stellar");
    assertStringsEqual(name, "Stellar");
}
