// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Decred/Address.h"

#include "Coin.h"
#include "HDWallet.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Decred;

TEST(DecredAddress, FromPublicKey) {
    const auto publicKey = PublicKey(parse_hex("0279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), TWPublicKeyTypeSECP256k1);
    const auto address = Address(publicKey);
    ASSERT_EQ(address.string(), "DsmcYVbP1Nmag2H4AS17UTvmWXmGeA7nLDx");
}

TEST(DecredAddress, Valid) {
    ASSERT_TRUE(Address::isValid("DsmcYVbP1Nmag2H4AS17UTvmWXmGeA7nLDx"));
    ASSERT_TRUE(Address::isValid("Dcur2mcGjmENx4DhNqDctW5wJCVyT3Qeqkx"));
}

TEST(DecredAddress, Invalid) {
    ASSERT_FALSE(Address::isValid("rnBFvgZphmN39GWzUJeUitaP22Fr9be75H"));
    ASSERT_FALSE(Address::isValid("t3gQDEavk5VzAAHK8TrQu2BWDLxEiF1unBm"));
}

TEST(DecredAddress, FromString) {
    const auto string = "DsmcYVbP1Nmag2H4AS17UTvmWXmGeA7nLDx";
    const auto address = Address(string);

    ASSERT_EQ(address.string(), string);
}

TEST(DecredAddress, Derive) {
    const auto mnemonic = "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal";
    const auto wallet = HDWallet(mnemonic, "");
    const auto path = TW::derivationPath(TWCoinTypeDecred);
    const auto address = TW::deriveAddress(TWCoinTypeDecred, wallet.getKey(path));
    ASSERT_EQ(address, "DsVMHD5D86dpRnt2GPZvv4bYUJZg6B9Pzqa");
}
