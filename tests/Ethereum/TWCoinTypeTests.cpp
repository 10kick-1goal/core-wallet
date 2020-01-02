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


TEST(TWEthereumCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeEthereum));
    auto txId = TWStringCreateWithUTF8Bytes("0x9edaf0f7d9c6629c31bbf0471fc07d696c73b566b93783f7e25d8d5d2b62fa4f");
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeEthereum, txId));
    auto accId = TWStringCreateWithUTF8Bytes("0x5bb497e8d9fe26e92dd1be01e32076c8e024d167");
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeEthereum, accId));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeEthereum));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeEthereum));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeEthereum), 18);
    ASSERT_EQ(TWBlockchainEthereum, TWCoinTypeBlockchain(TWCoinTypeEthereum));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeEthereum));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeEthereum));
    assertStringsEqual(symbol, "ETH");
    assertStringsEqual(txUrl, "https://etherscan.io/tx/0x9edaf0f7d9c6629c31bbf0471fc07d696c73b566b93783f7e25d8d5d2b62fa4f");
    assertStringsEqual(accUrl, "https://etherscan.io/address/0x5bb497e8d9fe26e92dd1be01e32076c8e024d167");
    assertStringsEqual(id, "ethereum");
    assertStringsEqual(name, "Ethereum");
}
