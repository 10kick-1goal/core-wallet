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


TEST(TWKavaCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeKava));
    auto txId = TWStringCreateWithUTF8Bytes("2988DF83FCBFAA38179D583A96734CBD071541D6768221BB23111BC8136D5E6A");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeKava, txId));
    auto accId = TWStringCreateWithUTF8Bytes("kava1jf9aaj9myrzsnmpdr7twecnaftzmku2mdpy2a7");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeKava, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeKava));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeKava));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeKava), 6);
    ASSERT_EQ(TWBlockchainCosmos, TWCoinTypeBlockchain(TWCoinTypeKava));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeKava));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeKava));
    assertStringsEqual(symbol, "KAVA");
    assertStringsEqual(txUrl, "https://kava.mintscan.io/txs/2988DF83FCBFAA38179D583A96734CBD071541D6768221BB23111BC8136D5E6A");
    assertStringsEqual(accUrl, "https://kava.mintscan.io/account/kava1jf9aaj9myrzsnmpdr7twecnaftzmku2mdpy2a7");
    assertStringsEqual(id, "kava");
    assertStringsEqual(name, "Kava");
}
