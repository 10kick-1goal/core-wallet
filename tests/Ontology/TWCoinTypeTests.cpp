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


TEST(TWOntologyCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeOntology));
    auto txId = TWStringCreateWithUTF8Bytes("t123");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeOntology, txId));
    auto accId = TWStringCreateWithUTF8Bytes("a12");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeOntology, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeOntology));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeOntology));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeOntology), 0);
    ASSERT_EQ(TWBlockchainOntology, TWCoinTypeBlockchain(TWCoinTypeOntology));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeOntology));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeOntology));
    assertStringsEqual(symbol, "ONT");
    assertStringsEqual(txUrl, "https://explorer.ont.io/transaction/t123");
    assertStringsEqual(accUrl, "https://explorer.ont.io/address/a12");
    assertStringsEqual(id, "ontology");
    assertStringsEqual(name, "Ontology");
}
