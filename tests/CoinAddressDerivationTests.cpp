// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Coin.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

namespace TW {

TEST(Coin, DeriveAddress) {
    auto dummyKeyData = parse_hex("0x4646464646464646464646464646464646464646464646464646464646464646");
    const auto privateKey = PrivateKey(dummyKeyData);
    const auto privateKeyExt = PrivateKey(dummyKeyData, dummyKeyData, dummyKeyData);
    
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeAion, privateKey), "0xa0010b0ea04ba4d76ca6e5e9900bacf19bc4402eaec7e36ea7ddd8eed48f60f3");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeBinance, privateKey), "bnb1hkfq3zahaqkkzx5mjnamwjsfpq2jk7z0mlq0d0");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeBitcoin, privateKey), "bc1qhkfq3zahaqkkzx5mjnamwjsfpq2jk7z00ppggv");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeBitcoinCash, privateKey), "bitcoincash:qz7eyzytkl5z6cg6nw20hd62pyyp22mcfuardfd2vn");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeCallisto, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeCosmos, privateKey), "cosmos1hkfq3zahaqkkzx5mjnamwjsfpq2jk7z0emlrvp");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeDash, privateKey), "XsyCV5yojxF4y3bYeEiVYqarvRgsWFELZL");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeDecred, privateKey), "Dsp4u8xxTHSZU2ELWTQLQP77xJhgeWrTsGK");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeDigiByte, privateKey), "dgb1qhkfq3zahaqkkzx5mjnamwjsfpq2jk7z0c69ssz");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeEthereum, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeEthereumClassic, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeGoChain, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeGroestlcoin, privateKey), "grs1qhkfq3zahaqkkzx5mjnamwjsfpq2jk7z0jsaf3d");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeICON, privateKey), "hx4728fc65c31728f0d3538b8783b5394b31a136b9");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeIoTeX, privateKey), "io1nk9x9ajk4rgkzhqjjn7hr6w0k0jg2kj0zgdt6h");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeLitecoin, privateKey), "ltc1qhkfq3zahaqkkzx5mjnamwjsfpq2jk7z0tamvsu");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeViacoin, privateKey), "via1qhkfq3zahaqkkzx5mjnamwjsfpq2jk7z09y9mn2");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeNimiq, privateKey), "NQ74 D40G N3M0 9EJD ET56 UPLR 02VC X6DU 8G1E");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeOntology, privateKey), "AeicEjZyiXKgUeSBbYQHxsU1X3V5Buori5");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypePOANetwork, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeXRP, privateKey), "rJHMeqKu8Ep7Fazx8MQG6JunaafBXz93YQ");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeStellar, privateKey), "GDXJHJHWN6GRNOAZXON6XH74ZX6NYFAS5B7642RSJQVJTIPA4ZYUQLEB");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeTezos, privateKey), "tz1gcEWswVU6dxfNQWbhTgaZrUrNUFwrsT4z");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeThunderToken, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeTomoChain, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeTron, privateKey), "TQLCsShbQNXMTVCjprY64qZmEA4rBarpQp");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeVeChain, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeWanchain, privateKey), "0x9D8a62F656A8D1615c1294FD71E9cfB3e4855a4f");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeZcash, privateKey), "t1b9xfAk3kZp5Qk3rinDPq7zzLkJGHTChDS");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeZcoin, privateKey), "aHzpPjmY132KseS4nkiQTbDahTEXqesY89");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeNano, privateKey), "nano_1qepdf4k95dhb5gsmhmq3iddqsxiafwkihunm7irn48jdiwdtnn6pe93k3f6");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeKin, privateKey), "GDXJHJHWN6GRNOAZXON6XH74ZX6NYFAS5B7642RSJQVJTIPA4ZYUQLEB");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeTheta, privateKey), "0x9d8A62f656a8d1615C1294fd71e9CFb3E4855A4F");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeQtum, privateKey), "QdtLm8ccxhuJFF5zCgikpaghbM3thdaGsW");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeNULS, privateKey), "NULSd6HgfXT3m5JBGxeCZXHRQbb82FKgZGT8o");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeBravoCoin, privateKey), "BRV5TrYnZP1RkDSUMzBY4GanCy6AP68kCMdkAb5EACkAwkdgRLShz");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeEOS, privateKey), "EOS5TrYnZP1RkDSUMzBY4GanCy6AP68kCMdkAb5EACkAwkdgRLShz");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeIoTeX, privateKey), "io1nk9x9ajk4rgkzhqjjn7hr6w0k0jg2kj0zgdt6h");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeDogecoin, privateKey), "DNRTC6GZ5evmM7BZWwPqF54fyDqUqULMyu");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeZilliqa, privateKey), "zil1j2cvtd7j9n7fnxfv2r3neucjw8tp4xz9sp07v4");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeRavencoin, privateKey), "RSZYjMDCP4q3t7NAFXPPnqEGrMZn971pdB");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeAeternity, privateKey), "ak_2p5878zbFhxnrm7meL7TmqwtvBaqcBddyp5eGzZbovZ5FeVfcw");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeTerra, privateKey), "terra1hkfq3zahaqkkzx5mjnamwjsfpq2jk7z0ll9rwp");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeNebulas, privateKey), "n1XTciu9ZRYt3ni7SxNBmivk9Y6XpP6VrhT");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeMonacoin, privateKey), "MRBWtGEKHGCHhmyJ1L4CwaWQZJzM5DnVcs");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeFIO, privateKey), "FIO5TrYnZP1RkDSUMzBY4GanCy6AP68kCMdkAb5EACkAwkdgRLShz");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeAlgorand, privateKey), "52J2J5TPRULLQGN3TPVZ77GN7TOBIEXIP7XGUMSMFKM2DYHGOFEOGBP2T4");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeTON, privateKey), "EQDww_SOMx3CT30jzaDPDuWkGaHjo8zQULH75spBGszlINDK");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeKusama, privateKey), "Hy8mqcexg5FMwMYnQvzrUvD723qMxDjMRU9HdNCnTsMAypY");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypePolkadot, privateKey), "16PpFrXrC6Ko3pYcyMAx6gPMp3mFFaxgyYMt4G5brkgNcSz8");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeKava, privateKey), "kava1hkfq3zahaqkkzx5mjnamwjsfpq2jk7z09wt76x");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeCardano, privateKeyExt), "Ae2tdPwUPEZGdVYxuFC5AidB5HWZAfpfN3bLMasYmiBMorjdoxo3WDC95TR");
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeNEO, privateKeyExt), "AeicEjZyiXKgUeSBbYQHxsU1X3V5Buori5");
}

} // namespace TW
