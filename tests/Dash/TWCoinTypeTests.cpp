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


TEST(TWDashCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeDash));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeDash, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeDash, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeDash));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeDash));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeDash), 8);
    ASSERT_EQ(TWBlockchainBitcoin, TWCoinTypeBlockchain(TWCoinTypeDash));
    ASSERT_EQ(0x10, TWCoinTypeP2shPrefix(TWCoinTypeDash));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeDash));
    assertStringsEqual(symbol, "DASH");
    assertStringsEqual(txUrl, "https://blockchair.com/dash/transaction/t123");
    assertStringsEqual(accUrl, "https://blockchair.com/dash/address/a12");
    assertStringsEqual(id, "dash");
    assertStringsEqual(name, "Dash");
}
