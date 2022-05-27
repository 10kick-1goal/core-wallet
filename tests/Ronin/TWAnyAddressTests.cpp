// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../interface/TWTestUtilities.h"

#include <TrustWalletCore/TWAnyAddress.h>
#include <TrustWalletCore/TWCoinType.h>

#include "Ronin/Address.h"
#include "Ronin/Entry.h"

#include <gtest/gtest.h>

const auto roninPrefixChecksummed = "ronin:EC49280228b0D05Aa8e8b756503254e1eE7835ab";

const auto tests = {
    roninPrefixChecksummed,
    "ronin:ec49280228b0d05aa8e8b756503254e1ee7835ab",
    "0xEC49280228b0D05Aa8e8b756503254e1eE7835ab",
    "ronin:0xEC49280228b0D05Aa8e8b756503254e1eE7835ab",
};

TEST(RoninAnyAddress, Validate) {
    for (const auto& t: tests) {
        EXPECT_TRUE(TWAnyAddressIsValid(STRING(t).get(), TWCoinTypeRonin));
    }
}

TEST(RoninAnyAddress, Normalize) {
    for (const auto& t: tests) {
        auto addr = WRAP(TWAnyAddress, TWAnyAddressCreateWithString(STRING(t).get(), TWCoinTypeRonin));
        auto string2 = WRAPS(TWAnyAddressDescription(addr.get()));
        EXPECT_TRUE(TWStringEqual(string2.get(), STRING(roninPrefixChecksummed).get()));

        auto keyHash = WRAPD(TWAnyAddressData(addr.get()));
        assertHexEqual(keyHash, "ec49280228b0d05aa8e8b756503254e1ee7835ab");
    }
}

TEST(RoninAnyAddress, Invalid) {
    const auto tests = {
        "EC49280228b0D05Aa8e8b756503254e1eE7835ab", // no prefix
        "ec49280228b0d05aa8e8b756503254e1ee7835ab", // no prefix
        "roni:EC49280228b0D05Aa8e8b756503254e1eE7835ab", // wrong prefix
        "ronin=EC49280228b0D05Aa8e8b756503254e1eE7835ab", // wrong prefix
        "0xronin:EC49280228b0D05Aa8e8b756503254e1eE7835ab", // wrong prefix
        "EC49280228b0D05Aa8e8b756503254e1eE7835", // too short
        "ronin:EC49280228b0D05Aa8e8b756503254e1eE7835", // too short
        "ronin:ec49280228b0d05aa8e8b756503254e1ee7835", // too short
    };

    for (const auto& t: tests) {
        EXPECT_FALSE(TWAnyAddressIsValid(STRING(t).get(), TWCoinTypeRonin));
    }
}
