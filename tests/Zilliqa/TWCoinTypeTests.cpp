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


TEST(TWZilliqaCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeZilliqa));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeZilliqa, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeZilliqa, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeZilliqa));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeZilliqa));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeZilliqa), 12);
    ASSERT_EQ(TWBlockchainZilliqa, TWCoinTypeBlockchain(TWCoinTypeZilliqa));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeZilliqa));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeZilliqa));
    assertStringsEqual(symbol, "ZIL");
    assertStringsEqual(txUrl, "https://viewblock.io/zilliqa/tx/t123");
    assertStringsEqual(accUrl, "https://viewblock.io/zilliqa/address/a12");
    assertStringsEqual(id, "zilliqa");
    assertStringsEqual(name, "Zilliqa");
}
