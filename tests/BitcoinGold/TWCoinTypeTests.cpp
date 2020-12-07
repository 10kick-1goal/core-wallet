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


TEST(TWBitcoinGoldCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeBitcoinGold));
    auto txId = WRAPS(TWStringCreateWithUTF8Bytes("2f807d7734de35d2236a1b3d8704eb12954f5f82ea66987949b10e94d9999b23"));
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeBitcoinGold, txId.get()));
    auto accId = WRAPS(TWStringCreateWithUTF8Bytes("GJjz2Du9BoJQ3CPcoyVTHUJZSj62i1693U"));
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeBitcoinGold, accId.get()));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeBitcoinGold));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeBitcoinGold));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeBitcoinGold), 8);
    ASSERT_EQ(TWBlockchainBitcoin, TWCoinTypeBlockchain(TWCoinTypeBitcoinGold));
    ASSERT_EQ(23, TWCoinTypeP2shPrefix(TWCoinTypeBitcoinGold));
    ASSERT_EQ(0, TWCoinTypeStaticPrefix(TWCoinTypeBitcoinGold));
    assertStringsEqual(symbol, "BTG");
    assertStringsEqual(txUrl, "https://explorer.bitcoingold.org/insight/tx/2f807d7734de35d2236a1b3d8704eb12954f5f82ea66987949b10e94d9999b23");
    assertStringsEqual(accUrl, "https://explorer.bitcoingold.org/insight/address/GJjz2Du9BoJQ3CPcoyVTHUJZSj62i1693U");
    assertStringsEqual(id, "bitcoingold");
    assertStringsEqual(name, "Bitcoin Gold");
}
