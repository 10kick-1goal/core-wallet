// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <gtest/gtest.h>

#include "HexCoding.h"
#include "PrivateKey.h"
#include "PublicKey.h"

#include "IoTeX/Address.h"

namespace TW::IoTeX {

TEST(IoTeXAddress, Invalid) {
    ASSERT_FALSE(Address::isValid("io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8"));
    ASSERT_FALSE(Address::isValid("io187wzp08vnhjpkydnr97qlh8kh0dpkkytfam8j"));
    ASSERT_FALSE(Address::isValid("it187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j"));
    ASSERT_FALSE(Address::isValid("ko187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j"));
    ASSERT_FALSE(Address::isValid("io187wzp18vnhjjpkydnr97qlh8kh0dpkkytfam8j"));
    ASSERT_FALSE(Address::isValid("io187wzp08vnhjbpkydnr97qlh8kh0dpkkytfam8j"));
    ASSERT_FALSE(Address::isValid("io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8k"));
}

TEST(IoTeXAddress, Valid) {
    ASSERT_TRUE(Address::isValid("io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j"));
}

TEST(IoTeXAddress, FromString) {
    const auto address = Address("io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j");
    ASSERT_EQ(address.string(), "io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j");

    EXPECT_THROW({
        try
        {
            const auto address = Address("io187wzp08vnhjbpkydnr97qlh8kh0dpkkytfam8j");
        }
        catch( const std::invalid_argument& e )
        {
            EXPECT_STREQ("IoTeX: Invalid address data", e.what());
            throw;
        }
    }, std::invalid_argument);
}

TEST(IoTeXAddress, FromPrivateKey) {
    const auto privateKey = PrivateKey(parse_hex("0806c458b262edd333a191e92f561aff338211ee3e18ab315a074a2d82aa343f"));
    const auto publicKey = PublicKey(privateKey.getPublicKey(TWPublicKeyTypeSECP256k1Extended));
    const auto address = Address(publicKey);
    ASSERT_EQ(address.string(), "io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j");

    EXPECT_THROW({
        try
        {
            const auto publicKey = PublicKey(privateKey.getPublicKey(TWPublicKeyTypeSECP256k1));
            const auto address = Address(publicKey);
        }
        catch( const std::invalid_argument& e )
        {
            EXPECT_STREQ("IoTeX: Address needs an extended SECP256k1 public key", e.what());
            throw;
        }
    }, std::invalid_argument);
}

TEST(IoTeXAddress, FromKeyHash) {
    const auto keyHash = parse_hex("3f9c20bcec9de520d88d98cbe07ee7b5ded0dac4");
    const auto address = Address(keyHash);
    ASSERT_EQ(address.string(), "io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j");

    EXPECT_THROW({
        try
        {
            const auto keyHash = parse_hex("3f9c20bcec9de520d88d98cbe07ee7b5ded0da");
            const auto address = Address(keyHash);
        }
        catch( const std::invalid_argument& e )
        {
            EXPECT_STREQ("IoTeX: Invalid address data", e.what());
            throw;
        }
    }, std::invalid_argument);
}

} // namespace TW::IoTeX
