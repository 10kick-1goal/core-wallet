// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Solana/Address.h"
#include "Solana/Program.h"
#include "Base58.h"
#include "PrivateKey.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace std;
using namespace TW;
using namespace TW::Solana;

TEST(SolanaAddress, FromPublicKey) {
    const auto addressString = "2gVkYWexTHR5Hb2aLeQN3tnngvWzisFKXDUPrgMHpdST";
    const auto publicKey = PublicKey(Base58::bitcoin.decode(addressString), TWPublicKeyTypeED25519);
    const auto address = Address(publicKey);
    ASSERT_EQ(addressString, address.string());
}

TEST(SolanaAddress, FromString) {
    string addressString = "2gVkYWexTHR5Hb2aLeQN3tnngvWzisFKXDUPrgMHpdST";
    const auto address = Address(addressString);
    ASSERT_EQ(address.string(), addressString);
}

TEST(SolanaAddress, isValid) {
    ASSERT_TRUE(Address::isValid("2gVkYWexTHR5Hb2aLeQN3tnngvWzisFKXDUPrgMHpdST"));
    ASSERT_FALSE(Address::isValid(
        "2gVkYWexTHR5Hb2aLeQN3tnngvWzisFKXDUPrgMHpdSl")); // Contains invalid base-58 character
    ASSERT_FALSE(
        Address::isValid("2gVkYWexTHR5Hb2aLeQN3tnngvWzisFKXDUPrgMHpd")); // Is invalid length
}

TEST(SolanaAddress, isValidOnCurve) {
    EXPECT_TRUE(PublicKey(Base58::bitcoin.decode("HzqnaMjWFbK2io6WgV2Z5uBguCBU21RMUS16wsDUHkon"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_TRUE(PublicKey(Base58::bitcoin.decode("68io7dTfyeWua1wD1YcCMka4y5iiChceaFRCBjqCM5PK"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_TRUE(PublicKey(Base58::bitcoin.decode("Dra34QLFCjxnk8tUNcBwxs6pgb5spF4oseQYF2xn7ABZ"), TWPublicKeyTypeED25519).isValidED25519());
    // negative case
    EXPECT_FALSE(PublicKey(Base58::bitcoin.decode("6X4X1Ae24mkoWeCEpktevySVG9jzeCufut5vtUW3wFrD"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_FALSE(PublicKey(Base58::bitcoin.decode("EDNd1ycsydWYwVmrYZvqYazFqwk1QjBgAUKFjBoz1jKP"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_FALSE(PublicKey(Base58::bitcoin.decode("ANVCrmRw7Ww7rTFfMbrjApSPXEEcZpBa6YEiBdf98pAf"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_FALSE(PublicKey(Base58::bitcoin.decode("AbygL37RheNZv327cMvZPqKYLLkZ6wqWYexRxgNiZyeP"), TWPublicKeyTypeED25519).isValidED25519());
}

TEST(SolanaAddress, defaultTokenAddress) {
    const Address serumToken = Address("SRMuApVNdxXokk5GT7XD5cUUgXMBCoAz2LHeuAoKWRt");
    EXPECT_EQ(Address("HBYC51YrGFAZ8rM7Sj8e9uqKggpSrDYrinQDZzvMtqQp").defaultTokenAddress(serumToken).string(),
        "6X4X1Ae24mkoWeCEpktevySVG9jzeCufut5vtUW3wFrD");
    EXPECT_EQ(Address("B1iGmDJdvmxyUiYM8UEo2Uw2D58EmUrw4KyLYMmrhf8V").defaultTokenAddress(serumToken).string(),
        "EDNd1ycsydWYwVmrYZvqYazFqwk1QjBgAUKFjBoz1jKP");
    EXPECT_EQ(Address("Eg5jqooyG6ySaXKbQUu4Lpvu2SqUPZrNkM4zXs9iUDLJ").defaultTokenAddress(serumToken).string(),
        "ANVCrmRw7Ww7rTFfMbrjApSPXEEcZpBa6YEiBdf98pAf");
}