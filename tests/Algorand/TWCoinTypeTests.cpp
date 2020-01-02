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


TEST(TWAlgorandCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeAlgorand));
    auto txId = TWStringCreateWithUTF8Bytes("CR7POXFTYDLC7TV3IXHA7AZKWABUJC52BACLHJQNXAKZJGRPQY3A");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeAlgorand, txId));
    auto accId = TWStringCreateWithUTF8Bytes("J4AEINCSSLDA7LNBNWM4ZXFCTLTOZT5LG3F5BLMFPJYGFWVCMU37EZI2AM");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeAlgorand, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeAlgorand));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeAlgorand));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeAlgorand), 6);
    ASSERT_EQ(TWBlockchainAlgorand, TWCoinTypeBlockchain(TWCoinTypeAlgorand));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeAlgorand));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeAlgorand));
    assertStringsEqual(symbol, "ALGO");
    assertStringsEqual(txUrl, "https://algoexplorer.io/tx/CR7POXFTYDLC7TV3IXHA7AZKWABUJC52BACLHJQNXAKZJGRPQY3A");
    assertStringsEqual(accUrl, "https://algoexplorer.io/address/J4AEINCSSLDA7LNBNWM4ZXFCTLTOZT5LG3F5BLMFPJYGFWVCMU37EZI2AM");
    assertStringsEqual(id, "algorand");
    assertStringsEqual(name, "Algorand");
}
