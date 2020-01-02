// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HDWallet.h"
#include "Bitcoin/Address.h"
#include "Bitcoin/CashAddress.h"
#include "Bitcoin/SegwitAddress.h"
#include "HexCoding.h"
#include "PublicKey.h"
#include "Hash.h"
#include "Base58.h"
#include "Coin.h"

#include <gtest/gtest.h>

namespace TW {

TEST(HDWallet, privateKeyFromXPRV) {
    const std::string xprv = "xprv9yqEgpMG2KCjvotCxaiMkzmKJpDXz2xZi3yUe4XsURvo9DUbPySW1qRbdeDLiSxZt88hESHUhm2AAe2EqfWM9ucdQzH3xv1HoKoLDqHMK9n";
    auto privateKey = HDWallet::getPrivateKeyFromExtended(xprv, DerivationPath(TWPurposeBIP44, TWCoinTypeBitcoinCash, 0, 0, 3));
    auto publicKey = privateKey->getPublicKey(TWPublicKeyTypeSECP256k1);
    auto address = Bitcoin::CashAddress(publicKey);

    EXPECT_EQ(hex(publicKey.bytes), "025108168f7e5aad52f7381c18d8f880744dbee21dc02c15abe512da0b1cca7e2f");
    EXPECT_EQ(address.string(), "bitcoincash:qp3y0dyg6ya8nt4n3algazn073egswkytqs00z7rz4");
}

TEST(HDWallet, privateKeyFromMptv) {
    const std::string mptv = "Mtpv7SkyM349Svcf1WiRtB5hC91ZZkVsGuv3kz1V7tThGxBFBzBLFnw6LpaSvwpHHuy8dAfMBqpBvaSAHzbffvhj2TwfojQxM7Ppm3CzW67AFL5";
    auto privateKey = HDWallet::getPrivateKeyFromExtended(mptv, DerivationPath(TWPurposeBIP44, TWCoinTypeBitcoinCash, 0, 0, 4));
    auto publicKey = privateKey->getPublicKey(TWPublicKeyTypeSECP256k1);

    auto witness = Data{0x00, 0x14};
    auto keyHash = Hash::sha256ripemd(publicKey.bytes.data(), publicKey.bytes.data() + 33);
    witness.insert(witness.end(), keyHash.begin(), keyHash.end());

    auto prefix = Data{TW::p2shPrefix(TWCoinTypeLitecoin)};
    auto redeemScript = Hash::sha256ripemd(witness.data(), witness.data() + witness.size());
    prefix.insert(prefix.end(), redeemScript.begin(), redeemScript.end());

    auto address = Bitcoin::Address(prefix);

    EXPECT_EQ(hex(publicKey.bytes), "02c36f9c3051e9cfbb196ecc35311f3ad705ea6798ffbe6b039e70f6bd047e6f2c");
    EXPECT_EQ(address.string(), "MBzcCaoLk9626cLj2UVvcxs6nsVUi39zEy");
}

TEST(HDWallet, privateKeyFromZprv) {
    const std::string zprv = "zprvAdzGEQ44z4WPLNCRpDaup2RumWxLGgR8PQ9UVsSmJigXsHVDaHK1b6qGM2u9PmxB2Gx264ctAz4yRoN3Xwf1HZmKcn6vmjqwsawF4WqQjfd";
    auto privateKey = HDWallet::getPrivateKeyFromExtended(zprv, DerivationPath(TWPurposeBIP44, TWCoinTypeBitcoin, 0, 0, 5));
    auto publicKey = privateKey->getPublicKey(TWPublicKeyTypeSECP256k1);
    auto address = Bitcoin::SegwitAddress(publicKey, 0, "bc");

    EXPECT_EQ(hex(publicKey.bytes), "022dc3f5a3fcfd2d1cc76d0cb386eaad0e30247ba729da0d8847a2713e444fdafa");
    EXPECT_EQ(address.string(), "bc1q5yyq60jepll68hds7exa7kpj20gsvdu0aztw5x");
}

TEST(HDWallet, privateKeyFromDGRV) {
    const std::string dgpv = "dgpv595jAJYGBLanByCJXRzrWBZFVXdNisfuPmKRDquCQcwBbwKbeR21AtkETf4EpjBsfsK3kDZgMqhcuky1B9PrT5nxiEcjghxpUVYviHXuCmc";
    auto privateKey = HDWallet::getPrivateKeyFromExtended(dgpv, DerivationPath(TWPurposeBIP44, TWCoinTypeDogecoin, 0, 0, 1));
    auto publicKey = privateKey->getPublicKey(TWPublicKeyTypeSECP256k1);
    auto address = Bitcoin::Address(publicKey, TW::p2pkhPrefix(TWCoinTypeDogecoin));

    EXPECT_EQ(hex(publicKey.bytes), "03eb6bf281990ee074a39c71ed8ce78c486066ac433bcf066dd5eb08f87d3a6c34");
    EXPECT_EQ(address.string(), "D5taDndQJ1fDF3AM1yWavmJY2BgSi17CUv");
}

TEST(HDWallet, privateKeyFromXPRVForDGB) {
    const std::string xprvForDgb = "xprv9ynLofyuR3uCqCMJADwzBaPnXB53EVe5oLujvPfdvCxae3NzgEpYjZMgcUeS8EUeYfYVLG61ZgPXm9TZWiwBnLVCgd551vCwpXC19hX3mFJ";
    auto privateKey = HDWallet::getPrivateKeyFromExtended(xprvForDgb, DerivationPath(TWPurposeBIP44, TWCoinTypeDigiByte, 0, 0, 1));
    auto publicKey = privateKey->getPublicKey(TWPublicKeyTypeSECP256k1);
    auto address = Bitcoin::Address(publicKey, TW::p2pkhPrefix(TWCoinTypeDigiByte));

    EXPECT_EQ(hex(publicKey.bytes), "03238a5c541c2cbbf769dbe0fb2a373c22db4da029370767fbe746d59da4de07f1");
    EXPECT_EQ(address.string(), "D9Gv7jWSVsS9Y5q98C79WyfEj6P2iM5Nzs");
}
} // namespace
