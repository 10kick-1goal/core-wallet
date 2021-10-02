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


TEST(TWTerraCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeTerra));
    auto txId = WRAPS(TWStringCreateWithUTF8Bytes("D28D8AFC7CE89F2A22FA2DBF78D2C0A36E549BB830C4D9FA7459E3F723CA7182"));
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeTerra, txId.get()));
    auto accId = WRAPS(TWStringCreateWithUTF8Bytes("terra16t3gx5rqvz6ru37yzn3shuu20erv4ngmfr59zf"));
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeTerra, accId.get()));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeTerra));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeTerra));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeTerra), 6);
    ASSERT_EQ(TWBlockchainCosmos, TWCoinTypeBlockchain(TWCoinTypeTerra));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeTerra));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeTerra));
    assertStringsEqual(symbol, "LUNA");
    assertStringsEqual(txUrl, "https://finder.terra.money/tx/tx/D28D8AFC7CE89F2A22FA2DBF78D2C0A36E549BB830C4D9FA7459E3F723CA7182");
    assertStringsEqual(accUrl, "https://finder.terra.money/tx/address/terra16t3gx5rqvz6ru37yzn3shuu20erv4ngmfr59zf");
    assertStringsEqual(id, "terra");
    assertStringsEqual(name, "Terra");
}
