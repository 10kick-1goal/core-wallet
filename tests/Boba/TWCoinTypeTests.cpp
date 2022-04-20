// Copyright © 2017-2021 Trust Wallet.
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


TEST(TWBobaCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeBoba));
    auto txId = WRAPS(TWStringCreateWithUTF8Bytes("0x31533707c3feb3b10f7deeea387ff8893f229253e65ca6b14d2400bf95b5d103"));
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeBoba, txId.get()));
    auto accId = WRAPS(TWStringCreateWithUTF8Bytes("0x4F96F50eDB37a19216d87693E5dB241e31bD3735"));
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeBoba, accId.get()));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeBoba));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeBoba));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeBoba), 18);
    ASSERT_EQ(TWBlockchainEthereum, TWCoinTypeBlockchain(TWCoinTypeBoba));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeBoba));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeBoba));
    assertStringsEqual(symbol, "BOBAETH");
    assertStringsEqual(txUrl, "https://blockexplorer.boba.network/tx/0x31533707c3feb3b10f7deeea387ff8893f229253e65ca6b14d2400bf95b5d103");
    assertStringsEqual(accUrl, "https://blockexplorer.boba.network/address/0x4F96F50eDB37a19216d87693E5dB241e31bD3735");
    assertStringsEqual(id, "boba");
    assertStringsEqual(name, "Boba");
}
