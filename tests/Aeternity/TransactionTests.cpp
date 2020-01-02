// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Aeternity/Address.cpp"
#include "Aeternity/Transaction.cpp"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "../interface/TWTestUtilities.h"
#include <TrustWalletCore/TWPKCS8.h>

#include "HexCoding.h"
#include <Aeternity/Signer.h>
#include <gtest/gtest.h>

TEST(AeternityTransaction, EncodeRlp) {
    std::string sender_id = "ak_2a1j2Mk9YSmC1gioUq4PWRm3bsv887MbuRVwyv4KaUGoR1eiKi";
    std::string recipient_id = "ak_Egp9yVdpxmvAfQ7vsXGvpnyfNq71msbdUpkMNYGTeTe8kPL3v";
    uint64_t amount = 10;
    uint64_t fee = 2e13;
    std::string payload = "Hello World";
    uint64_t ttl = 82757;
    uint64_t nonce = 49;

    auto tx = Transaction(sender_id, recipient_id, amount, fee, payload, ttl, nonce);
    auto encodedTx = tx.encode();
    auto encodedTxHex = hex(encodedTx);

    ASSERT_EQ(encodedTxHex, "f85f0c01a101cea7ade470c9f99d9d4e400880a86f1d49bb444b62f11a9ebb64bbcfeb73fef3a1011f13a3b08bf001400662a68b69d875f7803cec4c08647f6ed5d84c7897bd50a30a8612309ce5400083014345318b48656c6c6f20576f726c64");
}

TEST(AeternityTransaction, EncodeRlpWithZeroAmount) {
    std::string sender_id = "ak_2a1j2Mk9YSmC1gioUq4PWRm3bsv887MbuRVwyv4KaUGoR1eiKi";
    std::string recipient_id = "ak_Egp9yVdpxmvAfQ7vsXGvpnyfNq71msbdUpkMNYGTeTe8kPL3v";
    uint64_t amount = 0;
    uint64_t fee = 2e13;
    std::string payload = "Hello World";
    uint64_t ttl = 82757;
    uint64_t nonce = 49;

    auto tx = Transaction(sender_id, recipient_id, amount, fee, payload, ttl, nonce);
    auto encodedTx = tx.encode();
    auto encodedTxHex = hex(encodedTx);

    ASSERT_EQ(encodedTxHex, "f85f0c01a101cea7ade470c9f99d9d4e400880a86f1d49bb444b62f11a9ebb64bbcfeb73fef3a1011f13a3b08bf001400662a68b69d875f7803cec4c08647f6ed5d84c7897bd50a3008612309ce5400083014345318b48656c6c6f20576f726c64");
}

TEST(AeternityTransaction, EncodeRlpWithZeroTtl) {
    std::string sender_id = "ak_2a1j2Mk9YSmC1gioUq4PWRm3bsv887MbuRVwyv4KaUGoR1eiKi";
    std::string recipient_id = "ak_Egp9yVdpxmvAfQ7vsXGvpnyfNq71msbdUpkMNYGTeTe8kPL3v";
    uint64_t amount = 10;
    uint64_t fee = 2e13;
    std::string payload = "Hello World";
    uint64_t ttl = 0;
    uint64_t nonce = 49;

    auto tx = Transaction(sender_id, recipient_id, amount, fee, payload, ttl, nonce);
    auto encodedTx = tx.encode();
    auto encodedTxHex = hex(encodedTx);

    ASSERT_EQ(encodedTxHex, "f85c0c01a101cea7ade470c9f99d9d4e400880a86f1d49bb444b62f11a9ebb64bbcfeb73fef3a1011f13a3b08bf001400662a68b69d875f7803cec4c08647f6ed5d84c7897bd50a30a8612309ce5400000318b48656c6c6f20576f726c64");
}

