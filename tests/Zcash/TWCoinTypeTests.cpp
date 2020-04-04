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


TEST(TWZcashCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeZcash));
    auto txId = TWStringCreateWithUTF8Bytes("f2438a93039faf08d39bd3df1f7b5f19a2c29ffe8753127e2956ab4461adab35");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeZcash, txId));
    auto accId = TWStringCreateWithUTF8Bytes("t1Yfrf1dssDLmaMBsq2LFKWPbS5vH3nGpa2");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeZcash, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeZcash));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeZcash));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeZcash), 8);
    ASSERT_EQ(TWBlockchainBitcoin, TWCoinTypeBlockchain(TWCoinTypeZcash));
    ASSERT_EQ(0xbd, TWCoinTypeP2shPrefix(TWCoinTypeZcash));
    ASSERT_EQ(0x1c, TWCoinTypeStaticPrefix(TWCoinTypeZcash));
    assertStringsEqual(symbol, "ZEC");
    assertStringsEqual(txUrl, "https://blockchair.com/zcash/transaction/f2438a93039faf08d39bd3df1f7b5f19a2c29ffe8753127e2956ab4461adab35");
    assertStringsEqual(accUrl, "https://blockchair.com/zcash/address/t1Yfrf1dssDLmaMBsq2LFKWPbS5vH3nGpa2");
    assertStringsEqual(id, "zcash");
    assertStringsEqual(name, "Zcash");
}
