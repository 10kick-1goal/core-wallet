// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TWTestUtilities.h"

#include <TrustWalletCore/TWHRP.h>
#include <TrustWalletCore/TWCoinType.h>

#include <gtest/gtest.h>

TEST(TWHRP, StringForHRP) {
    ASSERT_STREQ(stringForHRP(TWHRPUnknown), nullptr);
    ASSERT_STREQ(stringForHRP(TWHRPBitcoin), "bc");
    ASSERT_STREQ(stringForHRP(TWHRPLitecoin), "ltc");
    ASSERT_STREQ(stringForHRP(TWHRPBitcoinCash), "bitcoincash");
    ASSERT_STREQ(stringForHRP(TWHRPBinance), "bnb");
    ASSERT_STREQ(stringForHRP(TWHRPCosmos), "cosmos");
    ASSERT_STREQ(stringForHRP(TWHRPDigiByte), "dgb");
    ASSERT_STREQ(stringForHRP(TWHRPGroestlcoin), "grs");
    ASSERT_STREQ(stringForHRP(TWHRPIoTeX), "io");
    ASSERT_STREQ(stringForHRP(TWHRPQtum), "qc");
    ASSERT_STREQ(stringForHRP(TWHRPZilliqa), "zil");
    ASSERT_STREQ(stringForHRP(TWHRPViacoin), "via");
    ASSERT_STREQ(stringForHRP(TWHRPTerra), "terra");
    ASSERT_STREQ(stringForHRP(TWHRPMonacoin), "mona");
    ASSERT_STREQ(stringForHRP(TWHRPKava), "kava");
}

TEST(TWHRP, HRPForString) {
    ASSERT_EQ(hrpForString(""), TWHRPUnknown);
    ASSERT_EQ(hrpForString("bc"), TWHRPBitcoin);
    ASSERT_EQ(hrpForString("ltc"), TWHRPLitecoin);
    ASSERT_EQ(hrpForString("bitcoincash"), TWHRPBitcoinCash);
    ASSERT_EQ(hrpForString("bnb"), TWHRPBinance);
    ASSERT_EQ(hrpForString("cosmos"), TWHRPCosmos);
    ASSERT_EQ(hrpForString("dgb"), TWHRPDigiByte);
    ASSERT_EQ(hrpForString("grs"), TWHRPGroestlcoin);
    ASSERT_EQ(hrpForString("io"), TWHRPIoTeX);
    ASSERT_EQ(hrpForString("qc"), TWHRPQtum);
    ASSERT_EQ(hrpForString("zil"), TWHRPZilliqa);
    ASSERT_EQ(hrpForString("via"), TWHRPViacoin);
    ASSERT_EQ(hrpForString("terra"), TWHRPTerra);
    ASSERT_EQ(hrpForString("mona"), TWHRPMonacoin);
    ASSERT_EQ(hrpForString("kava"), TWHRPKava);
}

TEST(TWHPR, HPRByCoinType) {
    ASSERT_EQ(TWHRPBitcoin, TWCoinTypeHRP(TWCoinTypeBitcoin));
    ASSERT_EQ(TWHRPBitcoinCash, TWCoinTypeHRP(TWCoinTypeBitcoinCash));
    ASSERT_EQ(TWHRPBinance, TWCoinTypeHRP(TWCoinTypeBinance));
    ASSERT_EQ(TWHRPCosmos, TWCoinTypeHRP(TWCoinTypeCosmos));
    ASSERT_EQ(TWHRPDigiByte, TWCoinTypeHRP(TWCoinTypeDigiByte));
    ASSERT_EQ(TWHRPLitecoin, TWCoinTypeHRP(TWCoinTypeLitecoin));
    ASSERT_EQ(TWHRPGroestlcoin, TWCoinTypeHRP(TWCoinTypeGroestlcoin));
    ASSERT_EQ(TWHRPIoTeX, TWCoinTypeHRP(TWCoinTypeIoTeX));
    ASSERT_EQ(TWHRPViacoin, TWCoinTypeHRP(TWCoinTypeViacoin));
    ASSERT_EQ(TWHRPQtum, TWCoinTypeHRP(TWCoinTypeQtum));
    ASSERT_EQ(TWHRPZilliqa, TWCoinTypeHRP(TWCoinTypeZilliqa));
    ASSERT_EQ(TWHRPTerra, TWCoinTypeHRP(TWCoinTypeTerra));
    ASSERT_EQ(TWHRPMonacoin, TWCoinTypeHRP(TWCoinTypeMonacoin));
    ASSERT_EQ(TWHRPKava, TWCoinTypeHRP(TWCoinTypeKava));

    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeAion));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeBravoCoin));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeCallisto));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeDash));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeDecred));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeDogecoin));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeEOS));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeEthereum));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeEthereumClassic));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeGoChain));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeICON));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeKin));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeNULS));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeNano));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeNimiq));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeOntology));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypePOANetwork));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeXRP));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeStellar));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeTON));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeTezos));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeTheta));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeThunderToken));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeTomoChain));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeTron));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeVeChain));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeWanchain));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeZcash));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeZcoin));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeZelcash));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeRavencoin));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeWaves));
    ASSERT_EQ(TWHRPUnknown, TWCoinTypeHRP(TWCoinTypeNEO));
}
