// Copyright © 2017-2019 Trust.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "PublicKey.h"
#include "Waves/Signer.h"
#include "Waves/Transaction.h"

#include <TrezorCrypto/sodium/keypair.h>
#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Waves;

TEST(WavesSigner, SignTransaction) {
    const auto privateKey =
        PrivateKey(parse_hex("9864a747e1b97f131fabb6b447296c9b6f0201e79fb3c5356e6c77e89b6a806a"));
    const auto publicKeyCurve25519 = privateKey.getPublicKey(TWPublicKeyTypeCURVE25519);
    ASSERT_EQ(hex(Data(publicKeyCurve25519.bytes.begin(), publicKeyCurve25519.bytes.end())),
              "559a50cb45a9a8e8d4f83295c354725990164d10bb505275d1a3086c08fb935d");
    // 3P2uzAzX9XTu1t32GkWw68YFFLwtapWvDds
    const auto address = Address(publicKeyCurve25519);

    auto tx1 = Transaction(
        /* amount */ 100000000,
        /* amount asset */ Transaction::WAVES,
        /* fee */ 100000000,
        /* fee asset*/ Transaction::WAVES,
        /* to */ address,
        /* attachment */ parse_hex("66616c6166656c"),
        /* timestamp */ 1526641218066,
        /* pub_key */
        parse_hex("559a50cb45a9a8e8d4f83295c354725990164d10bb505275d1a3086c08fb935d"));

    auto signature = Signer::sign(privateKey, tx1);

    EXPECT_EQ(hex(tx1.serializeToSign()),
              "0402559a50cb45a9a8e8d4f83295c354725990164d10bb505275d1a3086c08fb935d00000000016372e8"
              "52120000000005f5e1000000000005f5e10001570acc4110b78a6d38b34d879b5bba38806202ecf1732f"
              "8542000766616c6166656c");
    EXPECT_EQ(hex(signature), "af7989256f496e103ce95096b3f52196dd9132e044905fe486da3b829b5e403bcba9"
                              "5ab7e650a4a33948c2d05cfca2dce4d4df747e26402974490fb4c49fbe8f");

    ASSERT_TRUE(publicKeyCurve25519.verify(signature, tx1.serializeToSign()));
}

TEST(WavesSigner, curve25519_pk_to_ed25519) {
    const auto publicKeyCurve25519 =
        parse_hex("559a50cb45a9a8e8d4f83295c354725990164d10bb505275d1a3086c08fb935d");
    auto r = Data();
    r.resize(32);
    curve25519_pk_to_ed25519(r.data(), publicKeyCurve25519.data());
    EXPECT_EQ(hex(r), "ff84c4bfc095df25b01e48807715856d95af93d88c5b57f30cb0ce567ca4ce56");
}