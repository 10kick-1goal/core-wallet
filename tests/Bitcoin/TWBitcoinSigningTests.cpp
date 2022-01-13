// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Bitcoin/Address.h"
#include "Bitcoin/SegwitAddress.h"
#include "Bitcoin/OutPoint.h"
#include "Bitcoin/Script.h"
#include "Bitcoin/Transaction.h"
#include "Bitcoin/TransactionBuilder.h"
#include "Bitcoin/TransactionSigner.h"
#include "Bitcoin/SigHashType.h"
#include "Bitcoin/SegwitAddress.h"
#include "Base58.h"
#include "Hash.h"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "proto/Bitcoin.pb.h"
#include "TxComparisonHelper.h"
#include "../interface/TWTestUtilities.h"

#include <TrustWalletCore/TWAnySigner.h>
#include <TrustWalletCore/TWBitcoinScript.h>
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWHash.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWPublicKeyType.h>

#include <gtest/gtest.h>
#include <cassert>

using namespace TW;
using namespace TW::Bitcoin;

SigningInput buildInputP2PKH(bool omitKey = false) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    SigningInput input;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.amount = 335'790'000;
    input.byteFee = 1;
    input.toAddress = "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx";
    input.changeAddress = "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU";
    input.coinType = TWCoinTypeBitcoin;

    auto utxoKey0 = PrivateKey(parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866"));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash0 = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    assert(hex(utxoPubkeyHash0) == "b7cd046b6d522a3d61dbcb5235c0e9cc97265457");
    if (!omitKey) {
        input.privateKeys.push_back(utxoKey0);
    }

    auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
    auto pubKey1 = utxoKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash1 = Hash::ripemd(Hash::sha256(pubKey1.bytes));
    assert(hex(utxoPubkeyHash1) == "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    if (!omitKey) {
        input.privateKeys.push_back(utxoKey1);
    }

    auto utxo0Script = Script::buildPayToPublicKeyHash(utxoPubkeyHash0);
    Data scriptHash;
    utxo0Script.matchPayToPublicKeyHash(scriptHash);
    assert(hex(scriptHash) == "b7cd046b6d522a3d61dbcb5235c0e9cc97265457");

    UTXO utxo0;
    utxo0.script = utxo0Script;
    utxo0.amount = 625'000'000;
    utxo0.outPoint = OutPoint(hash0, 0, UINT32_MAX);
    input.utxos.push_back(utxo0);

    UTXO utxo1;
    utxo1.script = Script(parse_hex("0014" "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    utxo1.amount = 600'000'000;
    utxo1.outPoint = OutPoint(hash1, 1, UINT32_MAX);
    input.utxos.push_back(utxo1);

    return input;
}

TEST(BitcoinSigning, SignP2PKH) {
    auto input = buildInputP2PKH();

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {625'000'000}, 335'790'000, 226));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{228, 225, 226}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "01" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "6a"  "47304402202819d70d4bec472113a1392cadc0860a7a1b34ea0869abb4bdce3290c3aba086022023eff75f410ad19cdbe6c6a017362bd554ce5fb906c13534ddc306be117ad30a012103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432"  "ffffffff"
        "02" // outputs
            "b0bf031400000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "aefd3c1100000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2PKH_NegativeMissingKey) {
    auto input = buildInputP2PKH(true);

    {
        // test plan (but do not reuse plan result). Plan works even with missing keys.
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {625'000'000}, 335'790'000, 226));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_private_key);
}

TEST(BitcoinSigning, EncodeP2WPKH) {
    auto unsignedTx = Transaction(1, 0x11);

    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto outpoint0 = TW::Bitcoin::OutPoint(hash0, 0);
    unsignedTx.inputs.emplace_back(outpoint0, Script(), 0xffffffee);

    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");
    auto outpoint1 = TW::Bitcoin::OutPoint(hash1, 1);
    unsignedTx.inputs.emplace_back(outpoint1, Script(), UINT32_MAX);

    auto outScript0 = Script(parse_hex("76a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac"));
    unsignedTx.outputs.emplace_back(112340000, outScript0);

    auto outScript1 = Script(parse_hex("76a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac"));
    unsignedTx.outputs.emplace_back(223450000, outScript1);

    Data unsignedData;
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::Segwit);
    ASSERT_EQ(unsignedData.size(), 164);
    ASSERT_EQ(hex(unsignedData),
        "01000000" // version
        "0001" // marker & flag
        "02" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "00"  ""  "eeffffff"
            "ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a"  "01000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "202cb20600000000"  "19"  "76a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac"
            "9093510d00000000"  "19"  "76a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac"
        // witness
            "00"
            "00"
        "11000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WPKH_Bip143) {
    // https://github.com/bitcoin/bips/blob/master/bip-0143.mediawiki#native-p2wpkh

    SigningInput input;
    input.hashType = TWBitcoinSigHashTypeAll;
    const auto amount = 112340000; // 0x06B22C20
    input.amount = amount;
    input.byteFee = 20; // not relevant
    input.toAddress = "1Cu32FVupVCgHkMMRJdYJugxwo2Aprgk7H";
    input.changeAddress = "16TZ8J6Q5iZKBWizWzFAYnrsaox5Z5aBRV";

    const auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    const auto utxoKey0 = PrivateKey(parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866"));
    const auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(hex(pubKey0.bytes), "03c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432");

    const auto utxo0Script = Script::buildPayToPublicKey(pubKey0.bytes);
    Data key2;
    utxo0Script.matchPayToPublicKey(key2);
    EXPECT_EQ(hex(key2), hex(pubKey0.bytes));
    input.privateKeys.push_back(utxoKey0);

    const auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");
    const auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
    const auto pubKey1 = utxoKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(hex(pubKey1.bytes), "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357");
    const auto utxoPubkeyHash1 = Hash::ripemd(Hash::sha256(pubKey1.bytes));
    EXPECT_EQ(hex(utxoPubkeyHash1), "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    input.privateKeys.push_back(utxoKey1);
    input.lockTime = 0x11;

    UTXO utxo0;
    utxo0.script = utxo0Script;
    utxo0.amount = 1000000; // note: this amount is not specified in the test
    utxo0.outPoint = OutPoint(hash0, 0, 0xffffffee);
    input.utxos.push_back(utxo0);

    UTXO utxo1;
    auto utxo1Script = Script::buildPayToV0WitnessProgram(utxoPubkeyHash1);
    utxo1.script = utxo1Script;
    utxo1.amount = 600000000; // 0x23C34600 0046c323
    utxo1.outPoint = OutPoint(hash1, 1, UINT32_MAX);
    input.utxos.push_back(utxo1); 

    // Set plan to force both UTXOs and exact output amounts
    TransactionPlan plan;
    plan.amount = amount;
    plan.availableAmount = 600000000 + 1000000;
    plan.fee = 265210000; // very large, the amounts specified (in1, out0, out1) are not consistent/realistic
    plan.change = 223450000; // 0x0d519390
    plan.branchId = {0};
    plan.utxos.push_back(utxo0);
    plan.utxos.push_back(utxo1);
    input.plan = plan;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    const auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{343, 233, 261}));
    // expected in one string for easy comparison/copy:
    ASSERT_EQ(hex(serialized), "01000000000102fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f00000000494830450221008b9d1dc26ba6a9cb62127b02742fa9d754cd3bebf337f7a55d114c8e5cdd30be022040529b194ba3f9281a99f2b1c0a19c0489bc22ede944ccf4ecbab4cc618ef3ed01eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac000247304402203609e17b84f6a7d30c80bfa610b5b4542f32a8a0d5447a12fb1366d7f01cc44a0220573a954c4518331561406f90300e8f3358f51928d43c212a8caed02de67eebee0121025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee635711000000");
    // expected in structured format:
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "02" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "49"  "4830450221008b9d1dc26ba6a9cb62127b02742fa9d754cd3bebf337f7a55d114c8e5cdd30be022040529b194ba3f9281a99f2b1c0a19c0489bc22ede944ccf4ecbab4cc618ef3ed01"  "eeffffff"
            "ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a"  "01000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "202cb20600000000"  "19"  "76a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac"
            "9093510d00000000"  "19"  "76a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac"
        // witness
            "00"
            "02"
                "47"  "304402203609e17b84f6a7d30c80bfa610b5b4542f32a8a0d5447a12fb1366d7f01cc44a0220573a954c4518331561406f90300e8f3358f51928d43c212a8caed02de67eebee01"
                "21"  "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357"
        "11000000" // nLockTime
    );
}

SigningInput buildInputP2WPKH(int64_t amount, TWBitcoinSigHashType hashType, int64_t utxo0Amount, int64_t utxo1Amount, bool useMaxAmount = false) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    SigningInput input;
    input.hashType = hashType;
    input.amount = amount;
    input.useMaxAmount = useMaxAmount;
    input.byteFee = 1;
    input.toAddress = "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx";
    input.changeAddress = "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU";
    input.coinType = TWCoinTypeBitcoin;

    auto utxoKey0 = PrivateKey(parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866"));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash0 = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    assert(hex(utxoPubkeyHash0) == "b7cd046b6d522a3d61dbcb5235c0e9cc97265457");
    input.privateKeys.push_back(utxoKey0);

    auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
    auto pubKey1 = utxoKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash1 = Hash::ripemd(Hash::sha256(pubKey1.bytes));
    assert(hex(utxoPubkeyHash1) == "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    input.privateKeys.push_back(utxoKey1);

    auto scriptPub1 = Script(parse_hex("0014" "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    Data scriptHash;
    scriptPub1.matchPayToWitnessPublicKeyHash(scriptHash);
    auto scriptHashHex = hex(scriptHash);
    assert(scriptHashHex == "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");

    auto redeemScript = Script::buildPayToPublicKeyHash(parse_hex("1d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    input.scripts[scriptHashHex] = redeemScript;

    UTXO utxo0;
    utxo0.script = Script(parse_hex("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac"));
    utxo0.amount = utxo0Amount;
    utxo0.outPoint = OutPoint(hash0, 0, UINT32_MAX);
    input.utxos.push_back(utxo0);

    UTXO utxo1;
    utxo1.script = Script(parse_hex("0014" "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    utxo1.amount = utxo1Amount;
    utxo1.outPoint = OutPoint(hash1, 1, UINT32_MAX);
    input.utxos.push_back(utxo1);

    return input;
}

TEST(BitcoinSigning, SignP2WPKH) {
    auto input = buildInputP2WPKH(335'790'000, TWBitcoinSigHashTypeAll, 625'000'000, 600'000'000);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {625'000'000}, 335'790'000, 192));
    }

    // Signs
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{195, 192, 193}));
    EXPECT_EQ(serialized.size(), 192);
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "01" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "49"  "483045022100c327babdd370f0fc5b24cf920736446bf7d9c5660e4a5f7df432386fd652fe280220269c4fc3690c1c248e50c8bf2435c20b4ef00f308b403575f4437f862a91c53a01"  "ffffffff"
        "02" // outputs
            "b0bf031400000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "d0fd3c1100000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        "00000000" // nLockTime
    );

    {
        // Non-segwit encoded, for comparison
        Data serialized;
        signedTx.encode(serialized, Transaction::SegwitFormatMode::NonSegwit);
        EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{195, 192, 193}));
        EXPECT_EQ(serialized.size(), 192);
        ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
            "01000000" // version
            "01" // inputs
                "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "49"  "483045022100c327babdd370f0fc5b24cf920736446bf7d9c5660e4a5f7df432386fd652fe280220269c4fc3690c1c248e50c8bf2435c20b4ef00f308b403575f4437f862a91c53a01"  "ffffffff"
            "02" // outputs
                "b0bf031400000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
                "d0fd3c1100000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
            "00000000" // nLockTime
        );
    }
}

TEST(BitcoinSigning, SignP2WPKH_HashSingle_TwoInput) {
    auto input = buildInputP2WPKH(335'790'000, TWBitcoinSigHashTypeSingle, 210'000'000, 210'000'000);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {210'000'000, 210'000'000}, 335'790'000, 261));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{343, 233, 261}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "02" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "49"  "483045022100fd8591c3611a07b55f509ec850534c7a9c49713c9b8fa0e844ea06c2e65e19d702205e3806676192e790bc93dd4c28e937c4bf97b15f189158ba1a30d7ecff5ee75503"  "ffffffff"
            "ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a"  "01000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "b0bf031400000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "4bf0040500000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "00"
            "02"  "47"  "30440220096d20c7e92f991c2bf38dc28118feb34019ae74ec1c17179b28cb041de7517402204594f46a911f24bdc7109ca192e6860ebf2f3a0087579b3c128d5ce0cd5ed46803"  "21"  "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WPKH_HashAnyoneCanPay_TwoInput) {
    auto input = buildInputP2WPKH(335'790'000, TWBitcoinSigHashTypeAnyoneCanPay, 210'000'000, 210'000'000);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {210'000'000, 210'000'000}, 335'790'000, 261));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{344, 233, 261}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "02" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "49"  "483045022100e21fb2f1cfd59bdb3703fd45db38fd680d0c06e5d0be86fb7dc233c07ee7ab2f02207367220a73e43df4352a6831f6f31d8dc172c83c9f613a9caf679f0f15621c5e80"  "ffffffff"
            "ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a"  "01000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "b0bf031400000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "4bf0040500000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "00"
            "02"  "48"  "304502210095f9cc913d2f0892b953f2380112533e8930b67c53e00a7bbd7a01d547156adc022026efe3a684aa7432a00a919dbf81b63e635fb92d3149453e95b4a7ccea59f7c480"  "21"  "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WPKH_MaxAmount) {
    auto input = buildInputP2WPKH(1'000, TWBitcoinSigHashTypeAll, 625'000'000, 600'000'000, true);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {625'000'000, 600'000'000}, 1224999773, 227));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{310, 199, 227}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "02" // inputs
            "fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f"  "00000000"  "49"  "483045022100a8b3c1619e985923994e80efdc0be0eac12f2419e11ce5e4286a0a5ac27c775d02205d6feee85ffe19ae0835cba1562beb3beb172107cd02ac4caf24a8be3749811f01"  "ffffffff"
            "ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a"  "01000000"  "00"  ""  "ffffffff"
        "01" // outputs
            "5d03044900000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
        // witness
            "00"
            "02"  "48"  "3045022100db1199de92f6fb638a0ba706d13ec686bb01138a254dec2c397616cd74bad30e02200d7286d6d2d4e00d145955bf3d3b848b03c0d1eef8899e4645687a3035d7def401"  "21"  "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, EncodeP2WSH) {
    auto unsignedTx = Transaction(1);

    auto outpoint0 = OutPoint(parse_hex("0001000000000000000000000000000000000000000000000000000000000000"), 0);
    unsignedTx.inputs.emplace_back(outpoint0, Script(), UINT32_MAX);

    auto outScript0 = Script(parse_hex("76a9144c9c3dfac4207d5d8cb89df5722cb3d712385e3f88ac"));
    unsignedTx.outputs.emplace_back(1000, outScript0);

    Data unsignedData;
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::NonSegwit);
    ASSERT_EQ(hex(unsignedData),
        "01000000" // version
        "01" // inputs
            "0001000000000000000000000000000000000000000000000000000000000000"  "00000000"  "00"  ""  "ffffffff"
        "01" // outputs
            "e803000000000000"  "19"  "76a9144c9c3dfac4207d5d8cb89df5722cb3d712385e3f88ac"
        "00000000" // nLockTime
    );
}

SigningInput buildInputP2WSH(enum TWBitcoinSigHashType hashType, bool omitScript = false, bool omitKeys = false) {
    SigningInput input;
    input.hashType = hashType;
    input.amount = 1000;
    input.byteFee = 1;
    input.toAddress = "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx";
    input.changeAddress = "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU";

    if (!omitKeys) {
        auto utxoKey0 = PrivateKey(parse_hex("ed00a0841cd53aedf89b0c616742d1d2a930f8ae2b0fb514765a17bb62c7521a"));
        input.privateKeys.push_back(utxoKey0);

        auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
        input.privateKeys.push_back(utxoKey1);
    }

    if (!omitScript) {
        auto redeemScript = Script(parse_hex("2103596d3451025c19dbbdeb932d6bf8bfb4ad499b95b6f88db8899efac102e5fc71ac"));
        auto scriptHash = "593128f9f90e38b706c18623151e37d2da05c229";
        input.scripts[scriptHash] = redeemScript;
    }

    UTXO utxo0;
    auto p2wsh = Script::buildPayToWitnessScriptHash(parse_hex("ff25429251b5a84f452230a3c75fd886b7fc5a7865ce4a7bb7a9d7c5be6da3db"));
    utxo0.script = p2wsh;
    utxo0.amount = 1226;
    auto hash0 = parse_hex("0001000000000000000000000000000000000000000000000000000000000000");
    utxo0.outPoint = OutPoint(hash0, 0, UINT32_MAX);
    input.utxos.push_back(utxo0);
    
    return input;
}

TEST(BitcoinSigning, SignP2WSH) {
    // Setup input
    const auto input = buildInputP2WSH(hashTypeForCoin(TWCoinTypeBitcoin));

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 147));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{231, 119, 147}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "0001000000000000000000000000000000000000000000000000000000000000"  "00000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "e803000000000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "4f00000000000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "02"  "48"  "30450221009eefc1befe96158f82b74e6804f1f713768c6172636ca11fcc975c316ea86f75022057914c48bc24f717498b851a47a2926f96242e3943ebdf08d5a97a499efc8b9001"  "23"  "2103596d3451025c19dbbdeb932d6bf8bfb4ad499b95b6f88db8899efac102e5fc71ac"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WSH_HashNone) {
    // Setup input
    const auto input = buildInputP2WSH(TWBitcoinSigHashTypeNone);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 147));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{231, 119, 147}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "0001000000000000000000000000000000000000000000000000000000000000"  "00000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "e803000000000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "4f00000000000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "02"  "48"  "3045022100caa585732cfc50226a90834a306d23d5d2ab1e94af2c66136a637e3d9bad3688022069028750908e53a663bb1f434fd655bcc0cf8d394c6fa1fd5a4983790135722e02"  "23"  "2103596d3451025c19dbbdeb932d6bf8bfb4ad499b95b6f88db8899efac102e5fc71ac"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WSH_HashSingle) {
    // Setup input
    const auto input = buildInputP2WSH(TWBitcoinSigHashTypeSingle);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 147));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{230, 119, 147}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "0001000000000000000000000000000000000000000000000000000000000000"  "00000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "e803000000000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "4f00000000000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "02"  "47"  "304402201ba80b2c48fe82915297dc9782ae2141e40263001fafd21b02c04a092503f01e0220666d6c63475c6c52abd09371c200ac319bcf4a7c72eb3782e95790f5c847f0b903"  "23"  "2103596d3451025c19dbbdeb932d6bf8bfb4ad499b95b6f88db8899efac102e5fc71ac"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WSH_HashAnyoneCanPay) {
    // Setup input
    const auto input = buildInputP2WSH(TWBitcoinSigHashTypeAnyoneCanPay);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 147));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(serialized.size(), 231);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{231, 119, 147}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "0001000000000000000000000000000000000000000000000000000000000000"  "00000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "e803000000000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "4f00000000000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "02"  "48"  "3045022100d14699fc9b7337768bcd1430098d279cfaf05f6abfa75dd542da2dc038ae1700022063f0751c08796c086ac23b39c25f4320f432092e0c11bec46af0723cc4f55a3980"  "23"  "2103596d3451025c19dbbdeb932d6bf8bfb4ad499b95b6f88db8899efac102e5fc71ac"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2WSH_NegativeMissingScript) {
    const auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll, true);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 174));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_script_redeem);
}

TEST(BitcoinSigning, SignP2WSH_NegativeMissingKeys) {
    const auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll, false, true);

    {
        // test plan (but do not reuse plan result). Plan works even with missing keys.
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 147));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_private_key);
}

TEST(BitcoinSigning, SignP2WSH_NegativePlanWithError) {
    // Setup input
    auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll);
    input.plan = TransactionBuilder::plan(input);
    input.plan->error = Common::Proto::Error_missing_input_utxos;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_input_utxos);
}

TEST(BitcoinSigning, SignP2WSH_NegativeNoUTXOs) {
    // Setup input
    auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll);
    input.utxos.clear();
    ASSERT_FALSE(input.plan.has_value());

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_input_utxos);
}

TEST(BitcoinSigning, SignP2WSH_NegativePlanWithNoUTXOs) {
    // Setup input
    auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll);
    input.plan = TransactionBuilder::plan(input);
    input.plan->utxos.clear();

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_input_utxos);
}

TEST(BitcoinSigning, EncodeP2SH_P2WPKH) {
    auto unsignedTx = Transaction(1, 0x492);

    auto outpoint0 = OutPoint(parse_hex("db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a5477"), 1);
    unsignedTx.inputs.emplace_back(outpoint0, Script(), 0xfffffffe);

    auto outScript0 = Script(parse_hex("76a914a457b684d7f0d539a46a45bbc043f35b59d0d96388ac"));
    unsignedTx.outputs.emplace_back(199'996'600, outScript0);

    auto outScript1 = Script(parse_hex("76a914fd270b1ee6abcaea97fea7ad0402e8bd8ad6d77c88ac"));
    unsignedTx.outputs.emplace_back(800'000'000, outScript1);

    Data unsignedData;
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::NonSegwit);
    ASSERT_EQ(hex(unsignedData),
        "01000000" // version
        "01" // inputs
            "db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a5477"  "01000000"  "00"  ""  "feffffff"
        "02" // outputs
            "b8b4eb0b00000000"  "19"  "76a914a457b684d7f0d539a46a45bbc043f35b59d0d96388ac"
            "0008af2f00000000"  "19"  "76a914fd270b1ee6abcaea97fea7ad0402e8bd8ad6d77c88ac"
        "92040000" // nLockTime
    );
}

SigningInput buildInputP2SH_P2WPKH(bool omitScript = false, bool omitKeys = false, bool invalidOutputScript = false, bool invalidRedeemScript = false) {
    // Setup input
    SigningInput input;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.amount = 200'000'000;
    input.byteFee = 1;
    input.toAddress = "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx";
    input.changeAddress = "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU";
    input.coinType = TWCoinTypeBitcoin;

    auto utxoKey0 = PrivateKey(parse_hex("eb696a065ef48a2192da5b28b694f87544b30fae8327c4510137a922f32c6dcf"));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    assert(hex(utxoPubkeyHash) == "79091972186c449eb1ded22b78e40d009bdf0089");
    if (!omitKeys) {
        input.privateKeys.push_back(utxoKey0);
    }

    if (!omitScript && !invalidRedeemScript) {
        auto redeemScript = Script::buildPayToWitnessPublicKeyHash(utxoPubkeyHash);
        auto scriptHash = Hash::ripemd(Hash::sha256(redeemScript.bytes));
        assert(hex(scriptHash) == "4733f37cf4db86fbc2efed2500b4f4e49f312023");
        input.scripts[hex(scriptHash)] = redeemScript;
    } else if (invalidRedeemScript) {
        auto redeemScript = Script(parse_hex("FAFBFCFDFE"));
        auto scriptHash = Hash::ripemd(Hash::sha256(redeemScript.bytes));
        input.scripts[hex(scriptHash)] = redeemScript;
    }

    UTXO utxo0;
    auto utxo0Script = Script(parse_hex("a9144733f37cf4db86fbc2efed2500b4f4e49f31202387"));
    if (invalidOutputScript) {
        utxo0Script = Script(parse_hex("FFFEFDFCFB"));
    }
    utxo0.script = utxo0Script;
    utxo0.amount = 1'000'000'000;
    auto hash0 = parse_hex("db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a5477");
    utxo0.outPoint = OutPoint(hash0, 1, UINT32_MAX);
    input.utxos.push_back(utxo0);

    return input;
}

TEST(BitcoinSigning, SignP2SH_P2WPKH) {
    auto input = buildInputP2SH_P2WPKH();
    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'000'000'000}, 200'000'000, 170));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{251, 142, 170}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a5477"  "01000000"  "17"  "16001479091972186c449eb1ded22b78e40d009bdf0089"  "ffffffff"
        "02" // outputs
            "00c2eb0b00000000"  "19"  "76a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            "5607af2f00000000"  "19"  "76a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        // witness
            "02"  "47"  "3044022062b408cc7f92c8add622f3297b8992d68403849c6421ef58274ed6fc077102f30220250696eacc0aad022f55882d742dda7178bea780c03705bf9cdbee9f812f785301"  "21"  "03ad1d8e89212f0b92c74d23bb710c00662ad1470198ac48c43f7d6f93a2a26873"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2SH_P2WPKH_NegativeOmitScript) {
    auto input = buildInputP2SH_P2WPKH(true, false);
    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'000'000'000}, 200'000'000, 174));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_script_redeem);
}

TEST(BitcoinSigning, SignP2SH_P2WPKH_NegativeInvalidOutputScript) {
    auto input = buildInputP2SH_P2WPKH(false, false, true);
    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'000'000'000}, 200'000'000, 174));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_script_output);
}

TEST(BitcoinSigning, SignP2SH_P2WPKH_NegativeInvalidRedeemScript) {
    auto input = buildInputP2SH_P2WPKH(false, false, false, true);
    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'000'000'000}, 200'000'000, 174));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_script_redeem);
}

TEST(BitcoinSigning, SignP2SH_P2WPKH_NegativeOmitKeys) {
    auto input = buildInputP2SH_P2WPKH(false, true);
    {
        // test plan (but do not reuse plan result). Plan works even with missing keys.
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'000'000'000}, 200'000'000, 170));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_private_key);
}

TEST(BitcoinSigning, EncodeP2SH_P2WSH) {
    auto unsignedTx = Transaction(1);

    auto hash0 = parse_hex("36641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e");
    auto outpoint0 = OutPoint(hash0, 1);
    unsignedTx.inputs.emplace_back(outpoint0, Script(), 0xffffffff);

    auto outScript0 = Script(parse_hex("76a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688ac"));
    unsignedTx.outputs.emplace_back(0x0000000035a4e900, outScript0);

    auto outScript1 = Script(parse_hex("76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac"));
    unsignedTx.outputs.emplace_back(0x00000000052f83c0, outScript1);

    Data unsignedData;
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::NonSegwit);
    ASSERT_EQ(hex(unsignedData),
        "01000000" // version
        "01" // inputs
            "36641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e"  "01000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "00e9a43500000000"  "19"  "76a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688ac"
            "c0832f0500000000"  "19"  "76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, SignP2SH_P2WSH) {
    // Setup signing input
    SigningInput input;
    input.amount = 900000000;
    input.hashType = (TWBitcoinSigHashType)0;
    input.toAddress = "16AQVuBMt818u2HBcbxztAZTT2VTDKupPS";
    input.changeAddress = "1Bd1VA2bnLjoBk4ook3H19tZWETk8s6Ym5";

    auto key0 = parse_hex("730fff80e1413068a05b57d6a58261f07551163369787f349438ea38ca80fac6");
    input.privateKeys.push_back(PrivateKey(key0));
    auto key1 = parse_hex("11fa3d25a17cbc22b29c44a484ba552b5a53149d106d3d853e22fdd05a2d8bb3");
    input.privateKeys.push_back(PrivateKey(key1));
    auto key2 = parse_hex("77bf4141a87d55bdd7f3cd0bdccf6e9e642935fec45f2f30047be7b799120661");
    input.privateKeys.push_back(PrivateKey(key2));
    auto key3 = parse_hex("14af36970f5025ea3e8b5542c0f8ebe7763e674838d08808896b63c3351ffe49");
    input.privateKeys.push_back(PrivateKey(key3));
    auto key4 = parse_hex("fe9a95c19eef81dde2b95c1284ef39be497d128e2aa46916fb02d552485e0323");
    input.privateKeys.push_back(PrivateKey(key4));
    auto key5 = parse_hex("428a7aee9f0c2af0cd19af3cf1c78149951ea528726989b2e83e4778d2c3f890");
    input.privateKeys.push_back(PrivateKey(key5));

    auto redeemScript = Script::buildPayToWitnessScriptHash(parse_hex("a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54"));
    auto scriptHash = Hash::ripemd(Hash::sha256(redeemScript.bytes));
    input.scripts[hex(scriptHash)] = redeemScript;

    auto witnessScript = Script(parse_hex(
        "56"
            "210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba3"
            "2103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b"
            "21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a"
            "21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f4"
            "2103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac16"
            "2102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b"
        "56ae"
    ));
    auto witnessScriptHash = Hash::ripemd(Hash::sha256(witnessScript.bytes));
    input.scripts[hex(witnessScriptHash)] = witnessScript;

    auto utxo0Script = Script(parse_hex("a9149993a429037b5d912407a71c252019287b8d27a587"));
    UTXO utxo;
    utxo.outPoint = OutPoint(parse_hex("36641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e"), 1, UINT32_MAX);
    utxo.script = utxo0Script;
    utxo.amount = 987654321;
    input.utxos.push_back(utxo);

    TransactionPlan plan;
    plan.amount = input.amount;
    plan.availableAmount = input.utxos[0].amount;
    plan.change = 87000000;
    plan.fee = plan.availableAmount - plan.amount - plan.change;
    plan.utxos = input.utxos;
    input.plan = plan;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    auto expected =
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "36641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e"  "01000000"  "23"  "220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54"  "ffffffff"
        "02" // outputs
            "00e9a43500000000"  "19"  "76a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688ac"
            "c0832f0500000000"  "19"  "76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac"
        // witness
            "08"
                "00"  ""
                "47"  "304402201992f5426ae0bab04cf206d7640b7e00410297bfe5487637f6c2427ee8496be002204ad4e64dc2d269f593cc4820db1fc1e8dc34774f602945115ce485940e05c64200"
                "47"  "304402201e412363fa554b994528fd44149f3985b18bb901289ef6b71105b27c7d0e336c0220595e4a1e67154337757562ed5869127533e3e5084c3c2e128518f5f0b85b721800"
                "47"  "3044022003b0a20ccf545b3f12c5ade10db8717e97b44da2e800387adfd82c95caf529d902206aee3a2395530d52f476d0ddd9d20ba062820ae6f4e1be4921c3630395743ad900"
                "48"  "3045022100ed7a0eeaf72b84351bceac474b0c0510f67065b1b334f77e6843ed102f968afe022004d97d0cfc4bf5651e46487d6f87bd4af6aef894459f9778f2293b0b2c5b7bc700"
                "48"  "3045022100934a0c364820588154aed2d519cbcc61969d837b91960f4abbf0e374f03aa39d022036b5c58b754bd44cb5c7d34806c89d9778ea1a1c900618a841e9fbfbe805ff9b00"
                "47"  "3044022044e3b59b06931d46f857c82fa1d53d89b116a40a581527eac35c5eb5b7f0785302207d0f8b5d063ffc6749fb4e133db7916162b540c70dee40ec0b21e142d8843b3a00"
                "cf"  "56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae"
        "00000000" // nLockTime
    ;

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{800, 154, 316}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), expected);
}

TEST(BitcoinSigning, Sign_NegativeNoUtxos) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    SigningInput input;
    input.hashType = TWBitcoinSigHashTypeAll;
    input.amount = 335'790'000;
    input.byteFee = 1;
    input.toAddress = "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx";
    input.changeAddress = "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU";

    auto scriptPub1 = Script(parse_hex("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    Data scriptHash;
    scriptPub1.matchPayToWitnessPublicKeyHash(scriptHash);
    auto scriptHashHex = hex(scriptHash);
    ASSERT_EQ(scriptHashHex, "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");

    auto redeemScript = Script::buildPayToPublicKeyHash(scriptHash);
    input.scripts[scriptHashHex] = redeemScript;

    {
        // plan returns empty, as there are 0 utxos
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {}, 0, 0, Common::Proto::Error_missing_input_utxos));
    }

    // Invoke Sign nonetheless
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    // Fails as there are 0 utxos
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_input_utxos);
}

TEST(BitcoinSigning, Sign_NegativeInvalidAddress) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    SigningInput input;
    input.hashType = TWBitcoinSigHashTypeAll;
    input.amount = 335'790'000;
    input.byteFee = 1;
    input.toAddress = "THIS-IS-NOT-A-BITCOIN-ADDRESS";
    input.changeAddress = "THIS-IS-NOT-A-BITCOIN-ADDRESS-EITHER";

    auto utxoKey0 = PrivateKey(parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866"));
    input.privateKeys.push_back(utxoKey0);

    auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
    input.privateKeys.push_back(utxoKey1);

    auto scriptPub1 = Script(parse_hex("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    Data scriptHash;
    scriptPub1.matchPayToWitnessPublicKeyHash(scriptHash);
    auto scriptHashHex = hex(scriptHash);
    ASSERT_EQ(scriptHashHex, "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");

    auto redeemScript = Script::buildPayToPublicKeyHash(scriptHash);
    input.scripts[scriptHashHex] = redeemScript;

    UTXO utxo0;
    auto utxo0Script = Script(parse_hex("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac"));
    utxo0.script = utxo0Script;
    utxo0.amount = 625'000'000;
    utxo0.outPoint = OutPoint(hash0, 0, UINT32_MAX);
    input.utxos.push_back(utxo0);

    UTXO utxo1;
    auto utxo1Script = Script(parse_hex("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    utxo1.script = utxo1Script;
    utxo1.amount = 600'000'000;
    utxo1.outPoint = OutPoint(hash1, 1, UINT32_MAX);
    input.utxos.push_back(utxo1);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(std::move(input));
        EXPECT_TRUE(verifyPlan(plan, {625'000'000}, 335'790'000, 174));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Common::Proto::Error_missing_input_utxos);
}

TEST(BitcoinSigning, Plan_10input_MaxAmount) {
    auto ownAddress = "bc1q0yy3juscd3zfavw76g4h3eqdqzda7qyf58rj4m";
    auto ownPrivateKey = "eb696a065ef48a2192da5b28b694f87544b30fae8327c4510137a922f32c6dcf";

    SigningInput input;

    for (int i = 0; i < 10; ++i) {
        auto utxoScript = Script::lockScriptForAddress(ownAddress, TWCoinTypeBitcoin);
        Data keyHash;
        EXPECT_TRUE(utxoScript.matchPayToWitnessPublicKeyHash(keyHash));
        EXPECT_EQ(hex(keyHash), "79091972186c449eb1ded22b78e40d009bdf0089");

        auto redeemScript = Script::buildPayToPublicKeyHash(keyHash);
        input.scripts[std::string(keyHash.begin(), keyHash.end())] = redeemScript;

        UTXO utxo;
        utxo.script = utxoScript;
        utxo.amount = 1'000'000 + i * 10'000;
        auto hash = parse_hex("a85fd6a9a7f2f54cacb57e83dfd408e51c0a5fc82885e3fa06be8692962bc407");
        std::reverse(hash.begin(), hash.end());
        utxo.outPoint = OutPoint(hash, 0, UINT32_MAX);
        input.utxos.push_back(utxo);
    }

    input.coinType = TWCoinTypeBitcoin;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.useMaxAmount = true;
    input.amount = 2'000'000;
    input.byteFee = 1;
    input.toAddress = "bc1qauwlpmzamwlf9tah6z4w0t8sunh6pnyyjgk0ne";
    input.changeAddress = ownAddress;

    // Plan.  
    // Estimated size: witness size: 10 * (1 + 1 + 72 + 1 + 33) + 2 = 1082; base 451; raw 451 + 1082 = 1533; vsize 451 + 1082/4 --> 722
    // Actual size:    witness size:                                  1078; base 451; raw 451 + 1078 = 1529; vsize 451 + 1078/4 --> 721
    auto plan = TransactionBuilder::plan(input);
    EXPECT_TRUE(verifyPlan(plan, {1'000'000, 1'010'000, 1'020'000, 1'030'000, 1'040'000, 1'050'000, 1'060'000, 1'070'000, 1'080'000, 1'090'000}, 10'449'278, 722));

    // Extend input with keys, reuse plan, Sign
    auto privKey = PrivateKey(parse_hex(ownPrivateKey));
    input.privateKeys.push_back(privKey);
    input.plan = plan;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{1529, 451, 721}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));

    ASSERT_EQ(serialized.size(), 1529);
}

TEST(BitcoinSigning, Sign_LitecoinReal_a85f) {
    auto coin = TWCoinTypeLitecoin;
    auto ownAddress = "ltc1qt36tu30tgk35tyzsve6jjq3dnhu2rm8l8v5q00";
    auto ownPrivateKey = "b820f41f96c8b7442f3260acd23b3897e1450b8c7c6580136a3c2d3a14e34674";

    // Setup input
    SigningInput input;
    input.coinType = coin;
    input.hashType = hashTypeForCoin(coin);
    input.amount = 3'899'774;
    input.useMaxAmount = true;
    input.byteFee = 1;
    input.toAddress = "ltc1q0dvup9kzplv6yulzgzzxkge8d35axkq4n45hum";
    input.changeAddress = ownAddress;

    auto privKey = PrivateKey(parse_hex(ownPrivateKey));
    input.privateKeys.push_back(privKey);

    auto utxo0Script = Script::lockScriptForAddress(ownAddress, coin);
    Data keyHash0;
    EXPECT_TRUE(utxo0Script.matchPayToWitnessPublicKeyHash(keyHash0));
    EXPECT_EQ(hex(keyHash0), "5c74be45eb45a3459050667529022d9df8a1ecff");

    auto redeemScript = Script::buildPayToPublicKeyHash(keyHash0);
    input.scripts[std::string(keyHash0.begin(), keyHash0.end())] = redeemScript;

    UTXO utxo0;
    utxo0.script = utxo0Script;
    utxo0.amount = 3'900'000;
    auto hash0 = parse_hex("7051cd18189401a844abf0f9c67e791315c4c154393870453f8ad98a818efdb5");
    std::reverse(hash0.begin(), hash0.end());
    utxo0.outPoint = OutPoint(hash0, 9, UINT32_MAX - 1);
    input.utxos.push_back(utxo0);

    // set plan, to match real tx
    TransactionPlan plan;
    plan.availableAmount = 3'900'000;
    plan.amount = 3'899'774;
    plan.fee = 226;
    plan.change = 0;
    plan.utxos.push_back(input.utxos[0]);
    input.plan = plan;
    EXPECT_TRUE(verifyPlan(input.plan.value(), {3'900'000}, 3'899'774, 226));

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);

    // https://blockchair.com/litecoin/transaction/a85fd6a9a7f2f54cacb57e83dfd408e51c0a5fc82885e3fa06be8692962bc407
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "b5fd8e818ad98a3f4570383954c1c41513797ec6f9f0ab44a801941818cd5170"  "09000000"  "00"  ""  "feffffff"
        "01" // outputs
            "7e813b0000000000"  "16"  "00147b59c096c20fd9a273e240846b23276c69d35815"
        // witness
            "02"
                "47"  "3044022029153096af176f9cca0ba9b827e947689a8bb8d11dda570c880f9108bc590b3002202410c78b666722ade1ef4547ad85a128ddcbd4695c40f942457bea3d043b9bb301"
                "21"  "036739829f2cfec79cfe6aaf1c22ecb7d4867dfd8ab4deb7121b36a00ab646caed"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, PlanAndSign_LitecoinReal_8435) {
    auto coin = TWCoinTypeLitecoin;
    auto ownAddress = "ltc1q0dvup9kzplv6yulzgzzxkge8d35axkq4n45hum";
    auto ownPrivateKey = "690b34763f34e0226ad2a4d47098269322e0402f847c97166e8f39959fcaff5a";

    // Setup input for Plan
    SigningInput input;
    input.coinType = coin;
    input.hashType = hashTypeForCoin(coin);
    input.amount = 1'200'000;
    input.useMaxAmount = false;
    input.byteFee = 1;
    input.toAddress = "ltc1qt36tu30tgk35tyzsve6jjq3dnhu2rm8l8v5q00";
    input.changeAddress = ownAddress;

    auto utxo0Script = Script::lockScriptForAddress(ownAddress, coin);
    Data keyHash0;
    EXPECT_TRUE(utxo0Script.matchPayToWitnessPublicKeyHash(keyHash0));
    EXPECT_EQ(hex(keyHash0), "7b59c096c20fd9a273e240846b23276c69d35815");

    auto redeemScript = Script::buildPayToPublicKeyHash(keyHash0);
    input.scripts[std::string(keyHash0.begin(), keyHash0.end())] = redeemScript;

    UTXO utxo0;
    utxo0.script = utxo0Script;
    utxo0.amount = 3'899'774;
    auto hash0 = parse_hex("a85fd6a9a7f2f54cacb57e83dfd408e51c0a5fc82885e3fa06be8692962bc407");
    std::reverse(hash0.begin(), hash0.end());
    utxo0.outPoint = OutPoint(hash0, 0, UINT32_MAX);
    input.utxos.push_back(utxo0);

    // Plan
    auto plan = TransactionBuilder::plan(input);
    EXPECT_TRUE(verifyPlan(plan, {3'899'774}, 1'200'000, 141));

    // Extend input with keys and plan, for Sign
    auto privKey = PrivateKey(parse_hex(ownPrivateKey));
    input.privateKeys.push_back(privKey);
    input.plan = plan;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{222, 113, 141}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));

    // https://blockchair.com/litecoin/transaction/8435d205614ee70066060734adf03af4194d0c3bc66dd01bb124ab7fd25e2ef8
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "07c42b969286be06fae38528c85f0a1ce508d4df837eb5ac4cf5f2a7a9d65fa8"  "00000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "804f120000000000"  "16"  "00145c74be45eb45a3459050667529022d9df8a1ecff"
            "7131290000000000"  "16"  "00147b59c096c20fd9a273e240846b23276c69d35815"
        // witness
            "02"
                "47"  "304402204139b82927dd80445f27a5d2c29fa4881dbd2911714452a4a706145bc43cc4bf022016fbdf4b09bc5a9c43e79edb1c1061759779a20c35535082bdc469a61ed0771f01"
                "21"  "02499e327a05cc8bb4b3c34c8347ecfcb152517c9927c092fa273be5379fde3226"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, Sign_ManyUtxos_400) {
    auto ownAddress = "bc1q0yy3juscd3zfavw76g4h3eqdqzda7qyf58rj4m";
    auto ownPrivateKey = "eb696a065ef48a2192da5b28b694f87544b30fae8327c4510137a922f32c6dcf";

    // Setup input
    SigningInput input;

    const auto n = 400;
    uint64_t utxoSum = 0;
    for (int i = 0; i < n; ++i) {
        auto utxoScript = Script::lockScriptForAddress(ownAddress, TWCoinTypeBitcoin);
        Data keyHash;
        EXPECT_TRUE(utxoScript.matchPayToWitnessPublicKeyHash(keyHash));
        EXPECT_EQ(hex(keyHash), "79091972186c449eb1ded22b78e40d009bdf0089");

        auto redeemScript = Script::buildPayToPublicKeyHash(keyHash);
        input.scripts[std::string(keyHash.begin(), keyHash.end())] = redeemScript;

        UTXO utxo;
        utxo.script = utxoScript;
        utxo.amount = 1000 + (i + 1) * 10;
        auto hash = parse_hex("a85fd6a9a7f2f54cacb57e83dfd408e51c0a5fc82885e3fa06be8692962bc407");
        std::reverse(hash.begin(), hash.end());
        utxo.outPoint = OutPoint(hash, 0, UINT32_MAX);
        input.utxos.push_back(utxo);
        utxoSum += utxo.amount;
    }
    EXPECT_EQ(utxoSum, 1'202'000);

    input.coinType = TWCoinTypeBitcoin;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.useMaxAmount = false;
    input.amount = 300'000;
    input.byteFee = 1;
    input.toAddress = "bc1qauwlpmzamwlf9tah6z4w0t8sunh6pnyyjgk0ne";
    input.changeAddress = ownAddress;

    // Plan
    auto plan = TransactionBuilder::plan(input);

    // expected result: 66 utxos, with the largest amounts
    std::vector<int64_t> subset;
    uint64_t subsetSum = 0;
    for (int i = n - 66; i < n; ++i) {
        const uint64_t val = 1000 + (i + 1) * 10;
        subset.push_back(val);
        subsetSum += val;
    }
    EXPECT_EQ(subset.size(), 66);
    EXPECT_EQ(subsetSum, 308'550);
    EXPECT_TRUE(verifyPlan(plan, subset, 300'000, 4'561));

    // Extend input with keys, reuse plan, Sign
    auto privKey = PrivateKey(parse_hex(ownPrivateKey));
    input.privateKeys.push_back(privKey);
    input.plan = plan;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);

    EXPECT_EQ(serialized.size(), 9871);
}

TEST(BitcoinSigning, Sign_ManyUtxos_2000) {
    auto ownAddress = "bc1q0yy3juscd3zfavw76g4h3eqdqzda7qyf58rj4m";
    auto ownPrivateKey = "eb696a065ef48a2192da5b28b694f87544b30fae8327c4510137a922f32c6dcf";

    // Setup input
    SigningInput input;

    const auto n = 2000;
    uint64_t utxoSum = 0;
    for (int i = 0; i < n; ++i) {
        auto utxoScript = Script::lockScriptForAddress(ownAddress, TWCoinTypeBitcoin);
        Data keyHash;
        EXPECT_TRUE(utxoScript.matchPayToWitnessPublicKeyHash(keyHash));
        EXPECT_EQ(hex(keyHash), "79091972186c449eb1ded22b78e40d009bdf0089");

        auto redeemScript = Script::buildPayToPublicKeyHash(keyHash);
        input.scripts[std::string(keyHash.begin(), keyHash.end())] = redeemScript;

        UTXO utxo;
        utxo.script = utxoScript;
        utxo.amount = 1000 + (i + 1) * 10;
        auto hash = parse_hex("a85fd6a9a7f2f54cacb57e83dfd408e51c0a5fc82885e3fa06be8692962bc407");
        std::reverse(hash.begin(), hash.end());
        utxo.outPoint = OutPoint(hash, 0, UINT32_MAX);
        input.utxos.push_back(utxo);
        utxoSum += utxo.amount;
    }
    EXPECT_EQ(utxoSum, 22'010'000);

    input.coinType = TWCoinTypeBitcoin;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.useMaxAmount = false;
    input.amount = 2'000'000;
    input.byteFee = 1;
    input.toAddress = "bc1qauwlpmzamwlf9tah6z4w0t8sunh6pnyyjgk0ne";
    input.changeAddress = ownAddress;

    // Plan
    auto plan = TransactionBuilder::plan(input);

    // expected result: 601 utxos (smaller ones)
    std::vector<int64_t> subset;
    uint64_t subsetSum = 0;
    for (int i = 0; i < 601; ++i) {
        const uint64_t val = 1000 + (i + 1) * 10;
        subset.push_back(val);
        subsetSum += val;
    }
    EXPECT_EQ(subset.size(), 601);
    EXPECT_EQ(subsetSum, 2'410'010);
    EXPECT_TRUE(verifyPlan(plan, subset, 2'000'000, 40'943));

    // Extend input with keys, reuse plan, Sign
    auto privKey = PrivateKey(parse_hex(ownPrivateKey));
    input.privateKeys.push_back(privKey);
    input.plan = plan;

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);

    EXPECT_EQ(serialized.size(), 89'339);
}

TEST(BitcoinSigning, EncodeThreeOutput) {
    auto coin = TWCoinTypeLitecoin;
    auto ownAddress = "ltc1qt36tu30tgk35tyzsve6jjq3dnhu2rm8l8v5q00";
    auto ownPrivateKey = "b820f41f96c8b7442f3260acd23b3897e1450b8c7c6580136a3c2d3a14e34674";
    auto toAddress0 = "ltc1qgknskahmm6svn42e33gum5wc4dz44wt9vc76q4";
    auto toAddress1 = "ltc1qulgtqdgxyd9nxnn5yxft6jykskz0ffl30nu32z";
    auto utxo0Amount = 3'851'829;
    auto toAmount0 = 1'000'000;
    auto toAmount1 = 2'000'000;

    auto unsignedTx = Transaction(1);

    auto hash0 = parse_hex("bbe736ada63c4678025dff0ff24d5f38970a3e4d7a2f77808689ed68004f55fe");
    std::reverse(hash0.begin(), hash0.end());
    auto outpoint0 = TW::Bitcoin::OutPoint(hash0, 0);
    unsignedTx.inputs.emplace_back(outpoint0, Script(), UINT32_MAX);

    auto lockingScript0 = Script::lockScriptForAddress(toAddress0, coin);
    unsignedTx.outputs.emplace_back(toAmount0, lockingScript0);
    auto lockingScript1 = Script::lockScriptForAddress(toAddress1, coin);
    unsignedTx.outputs.emplace_back(toAmount1, lockingScript1);
    // change
    auto lockingScript2 = Script::lockScriptForAddress(ownAddress, coin);
    unsignedTx.outputs.emplace_back(utxo0Amount - toAmount0 - toAmount1 - 172, lockingScript2);

    Data unsignedData;
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::Segwit);
    EXPECT_EQ(unsignedData.size(), 147);
    EXPECT_EQ(hex(unsignedData), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "fe554f0068ed898680772f7a4d3e0a97385f4df20fff5d0278463ca6ad36e7bb"  "00000000"  "00"  ""  "ffffffff"
        "03" // outputs
            "40420f0000000000"  "16"  "001445a70b76fbdea0c9d5598c51cdd1d8ab455ab965"
            "80841e0000000000"  "16"  "0014e7d0b03506234b334e742192bd48968584f4a7f1"
            "c9fe0c0000000000"  "16"  "00145c74be45eb45a3459050667529022d9df8a1ecff"
        // witness
            "00"
        "00000000" // nLockTime
    );

    // add signature

    auto privkey = PrivateKey(parse_hex(ownPrivateKey));
    auto pubkey = PrivateKey(privkey).getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(hex(pubkey.bytes), "036739829f2cfec79cfe6aaf1c22ecb7d4867dfd8ab4deb7121b36a00ab646caed");

    auto utxo0Script = Script::lockScriptForAddress(ownAddress, coin); // buildPayToV0WitnessProgram()
    Data keyHashIn0;
    EXPECT_TRUE(utxo0Script.matchPayToWitnessPublicKeyHash(keyHashIn0));
    EXPECT_EQ(hex(keyHashIn0), "5c74be45eb45a3459050667529022d9df8a1ecff");

    auto redeemScript0 = Script::buildPayToPublicKeyHash(keyHashIn0);
    EXPECT_EQ(hex(redeemScript0.bytes), "76a9145c74be45eb45a3459050667529022d9df8a1ecff88ac");

    auto hashType = TWBitcoinSigHashType::TWBitcoinSigHashTypeAll;
    Data sighash = unsignedTx.getSignatureHash(redeemScript0, unsignedTx.inputs[0].previousOutput.index,
        hashType, utxo0Amount, static_cast<SignatureVersion>(unsignedTx.version));
    auto sig = privkey.signAsDER(sighash, TWCurveSECP256k1);
    ASSERT_FALSE(sig.empty());
    sig.push_back(hashType);
    EXPECT_EQ(hex(sig), "30450221008d88197a37ffcb51ecacc7e826aa588cb1068a107a82373c4b54ec42318a395c02204abbf5408504614d8f943d67e7873506c575e85a5e1bd92a02cd345e5192a82701");
    
    // add witness stack
    unsignedTx.inputs[0].scriptWitness.push_back(sig);
    unsignedTx.inputs[0].scriptWitness.push_back(pubkey.bytes);

    unsignedData.clear();
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::Segwit);
    EXPECT_EQ(unsignedData.size(), 254);
    // https://blockchair.com/litecoin/transaction/9e3fe98565a904d2da5ec1b3ba9d2b3376dfc074f43d113ce1caac01bf51b34c
    EXPECT_EQ(hex(unsignedData), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "fe554f0068ed898680772f7a4d3e0a97385f4df20fff5d0278463ca6ad36e7bb"  "00000000"  "00"  ""  "ffffffff"
        "03" // outputs
            "40420f0000000000"  "16"  "001445a70b76fbdea0c9d5598c51cdd1d8ab455ab965"
            "80841e0000000000"  "16"  "0014e7d0b03506234b334e742192bd48968584f4a7f1"
            "c9fe0c0000000000"  "16"  "00145c74be45eb45a3459050667529022d9df8a1ecff"
        // witness
            "02"
                "48"  "30450221008d88197a37ffcb51ecacc7e826aa588cb1068a107a82373c4b54ec42318a395c02204abbf5408504614d8f943d67e7873506c575e85a5e1bd92a02cd345e5192a82701"
                "21"  "036739829f2cfec79cfe6aaf1c22ecb7d4867dfd8ab4deb7121b36a00ab646caed"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, RedeemExtendedPubkeyUTXO) {
    auto wif = "L4BeKzm3AHDUMkxLRVKTSVxkp6Hz9FcMQPh18YCKU1uioXfovzwP";
    auto decoded = Base58::bitcoin.decodeCheck(wif);
    auto key = PrivateKey(Data(decoded.begin() + 1, decoded.begin() + 33));
    auto pubkey = key.getPublicKey(TWPublicKeyTypeSECP256k1Extended);
    auto hash = Hash::sha256ripemd(pubkey.bytes.data(), pubkey.bytes.size());

    Data data;
    append(data, 0x00);
    append(data, hash);
    auto address = Bitcoin::Address(data);
    auto addressString = address.string();

    EXPECT_EQ(addressString, "1PAmpW5igXUJnuuzRa5yTcsWHwBamZG7Y2");

    // Setup input for Plan
    SigningInput input;
    input.coinType = TWCoinTypeBitcoin;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.amount = 26972;
    input.useMaxAmount = true;
    input.byteFee = 1;
    input.toAddress = addressString;

    auto utxo0Script = Script::lockScriptForAddress(addressString, TWCoinTypeBitcoin);

    UTXO utxo0;
    utxo0.script = utxo0Script;
    utxo0.amount = 16874;
    auto hash0 = parse_hex("6ae3f1d245521b0ea7627231d27d613d58c237d6bf97a1471341a3532e31906c");
    std::reverse(hash0.begin(), hash0.end());
    utxo0.outPoint = OutPoint(hash0, 0, UINT32_MAX);
    input.utxos.push_back(utxo0);

    UTXO utxo1;
    utxo1.script = utxo0Script;
    utxo1.amount = 10098;
    auto hash1 = parse_hex("fd1ea8178228e825d4106df0acb61a4fb14a8f04f30cd7c1f39c665c9427bf13");
    std::reverse(hash1.begin(), hash1.end());
    utxo1.outPoint = OutPoint(hash1, 0, UINT32_MAX);
    input.utxos.push_back(utxo1);

    input.privateKeys.push_back(key);

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data encoded;
    signedTx.encode(encoded);
    EXPECT_EQ(encoded.size(), 402);
}

TEST(BitcoinSigning, SignP2TR_5df51e) {
    const auto privateKey = "13fcaabaf9e71ffaf915e242ec58a743d55f102cf836968e5bd4881135e0c52c";
    const auto ownAddress = "bc1qpjult34k9spjfym8hss2jrwjgf0xjf40ze0pp8";
    const auto toAddress = "bc1ptmsk7c2yut2xah4pgflpygh2s7fh0cpfkrza9cjj29awapv53mrslgd5cf"; // Taproot
    const auto coin =  TWCoinTypeBitcoin;

    // Setup input
    SigningInput input;
    input.hashType = hashTypeForCoin(coin);
    input.amount = 1100;
    input.useMaxAmount = false;
    input.byteFee = 1;
    input.toAddress = toAddress;
    input.changeAddress = ownAddress;
    input.coinType = coin;

    auto utxoKey0 = PrivateKey(parse_hex(privateKey));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(hex(pubKey0.bytes), "021e582a887bd94d648a9267143eb600449a8d59a0db0653740b1378067a6d0cee");
    EXPECT_EQ(SegwitAddress(pubKey0, "bc").string(), ownAddress);
    auto utxoPubkeyHash = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    EXPECT_EQ(hex(utxoPubkeyHash), "0cb9f5c6b62c03249367bc20a90dd2425e6926af");
    input.privateKeys.push_back(utxoKey0);

    auto redeemScript = Script::lockScriptForAddress(input.toAddress, coin);
    EXPECT_EQ(hex(redeemScript.bytes), "51205ee16f6144e2d46edea1427e1222ea879377e029b0c5d2e252517aee85948ec7");
    auto scriptHash = Hash::ripemd(Hash::sha256(redeemScript.bytes));
    EXPECT_EQ(hex(scriptHash), "e0a5001e7b394a1a6b2978cdcab272241280bf46");
    input.scripts[hex(scriptHash)] = redeemScript;

    UTXO utxo0;
    auto utxo0Script = Script::lockScriptForAddress(ownAddress, coin);
    EXPECT_EQ(hex(utxo0Script.bytes), "00140cb9f5c6b62c03249367bc20a90dd2425e6926af");
    utxo0.script = utxo0Script;
    utxo0.amount = 49429;
    auto hash0 = parse_hex("c24bd72e3eaea797bd5c879480a0db90980297bc7085efda97df2bf7d31413fb");
    std::reverse(hash0.begin(), hash0.end());
    utxo0.outPoint = OutPoint(hash0, 1, UINT32_MAX);
    input.utxos.push_back(utxo0);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {49429}, 1100, 153));
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{234, 125, 153}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    // https://mempool.space/tx/5df51e13bfeb79f386e1e17237f06d1b5c87c5bfcaa907c0c1cfe51cd7ca446d
    EXPECT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "fb1314d3f72bdf97daef8570bc97029890dba08094875cbd97a7ae3e2ed74bc2"  "01000000"  "00"  ""  "ffffffff"
        "02" // outputs
            "4c04000000000000"  "22"  "51205ee16f6144e2d46edea1427e1222ea879377e029b0c5d2e252517aee85948ec7"
            "30bc000000000000"  "16"  "00140cb9f5c6b62c03249367bc20a90dd2425e6926af"
        // witness
            "02"
                "47"  "3044022021cea91157fdab33226e38ee7c1a686538fc323f5e28feb35775cf82ba8c62210220723743b150cea8ead877d8b8d059499779a5df69f9bdc755c9f968c56cfb528f01"
                "21"  "021e582a887bd94d648a9267143eb600449a8d59a0db0653740b1378067a6d0cee"
        "00000000" // nLockTime
    );
}

TEST(BitcoinSigning, Build_OpReturn_THORChainSwap_eb4c) {
    auto coin = TWCoinTypeBitcoin;
    auto ownAddress = "bc1q7s0a2l4aguksehx8hf93hs9yggl6njxds6m02g";
    auto toAddress = "bc1qxu5a8gtnjxw3xwdlmr2gl9d76h9fysu3zl656e";
    auto utxoAmount = 342101;
    auto toAmount = 300000;
    int fee = 36888;

    auto unsignedTx = Transaction(2, 0);

    auto hash0 = parse_hex("30b82960291a39de3664ec4c844a815e3e680e29b4d3a919e450f0c119cf4e35");
    std::reverse(hash0.begin(), hash0.end());
    auto outpoint0 = TW::Bitcoin::OutPoint(hash0, 1);
    unsignedTx.inputs.emplace_back(outpoint0, Script(), UINT32_MAX);

    auto lockingScriptTo = Script::lockScriptForAddress(toAddress, coin);
    unsignedTx.outputs.push_back(TransactionOutput(toAmount, lockingScriptTo));
    // change
    auto lockingScriptChange = Script::lockScriptForAddress(ownAddress, coin);
    unsignedTx.outputs.push_back(TransactionOutput(utxoAmount - toAmount - fee, lockingScriptChange));
    // memo OP_RETURN
    Data memo = data("SWAP:THOR.RUNE:thor1tpercamkkxec0q0jk6ltdnlqvsw29guap8wmcl:");
    auto lockingScriptOpReturn = Script::buildOpReturnScript(memo);
    EXPECT_EQ(hex(lockingScriptOpReturn.bytes), "6a3b535741503a54484f522e52554e453a74686f72317470657263616d6b6b7865633071306a6b366c74646e6c7176737732396775617038776d636c3a");
    unsignedTx.outputs.push_back(TransactionOutput(0, lockingScriptOpReturn));

    Data unsignedData;
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::Segwit);
    EXPECT_EQ(unsignedData.size(), 186);
    EXPECT_EQ(hex(unsignedData), // printed using prettyPrintTransaction
        "02000000" // version
        "0001" // marker & flag
        "01" // inputs
            "354ecf19c1f050e419a9d3b4290e683e5e814a844cec6436de391a296029b830"  "01000000"  "00"  ""  "ffffffff"
        "03" // outputs
            "e093040000000000"  "16"  "00143729d3a173919d1339bfd8d48f95bed5ca924391"
            "5d14000000000000"  "16"  "0014f41fd57ebd472d0cdcc7ba4b1bc0a4423fa9c8cd"
            "0000000000000000"  "3d"  "6a3b535741503a54484f522e52554e453a74686f72317470657263616d6b6b7865633071306a6b366c74646e6c7176737732396775617038776d636c3a"
        // witness
            "00"
        "00000000" // nLockTime
    );

    // add signature
    auto pubkey = parse_hex("0206121b83ebfddbb1997b50cb87b968190857269333e21e295142c8b88af9312a");
    auto sig = parse_hex("3045022100876eba8f9324d3fbb00b9dad9a34a8166dd75127d4facda63484c19703e9c178022052495a6229cc465d5f0fcf3cde3b22a0f861e762d0bb10acde26a57598bfe7e701");

    // add witness stack
    unsignedTx.inputs[0].scriptWitness.push_back(sig);
    unsignedTx.inputs[0].scriptWitness.push_back(pubkey);

    unsignedData.clear();
    unsignedTx.encode(unsignedData, Transaction::SegwitFormatMode::Segwit);
    EXPECT_EQ(unsignedData.size(), 293);
    // https://blockchair.com/bitcoin/transaction/eb4c1b064bfaf593d7cc6a5c73b75f932ffefe12a0478acf5a7e3145476683fc
    EXPECT_EQ(hex(unsignedData),
        "02000000000101354ecf19c1f050e419a9d3b4290e683e5e814a844cec6436de391a296029b8300100000000ffffffff03e0930400000000001600143729d3a1"
        "73919d1339bfd8d48f95bed5ca9243915d14000000000000160014f41fd57ebd472d0cdcc7ba4b1bc0a4423fa9c8cd00000000000000003d6a3b535741503a54"
        "484f522e52554e453a74686f72317470657263616d6b6b7865633071306a6b366c74646e6c7176737732396775617038776d636c3a02483045022100876eba8f"
        "9324d3fbb00b9dad9a34a8166dd75127d4facda63484c19703e9c178022052495a6229cc465d5f0fcf3cde3b22a0f861e762d0bb10acde26a57598bfe7e70121"
        "0206121b83ebfddbb1997b50cb87b968190857269333e21e295142c8b88af9312a00000000"
    );
}

TEST(BitcoinSigning, Sign_OpReturn_THORChainSwap) {
    PrivateKey privateKey = PrivateKey(parse_hex("6bd4096fa6f08bd3af2b437244ba0ca2d35045c5233b8d6796df37e61e974de5"));
    PublicKey publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto ownAddress = SegwitAddress(publicKey, "bc");
    auto ownAddressString = ownAddress.string();
    EXPECT_EQ(ownAddressString, "bc1q2gzg42w98ytatvmsgxfc8vrg6l24c25pydup9u");
    auto toAddress = "bc1qxu5a8gtnjxw3xwdlmr2gl9d76h9fysu3zl656e";
    auto utxoAmount = 342101;
    auto toAmount = 300000;
    int byteFee = 126;
    Data memo = data("SWAP:THOR.RUNE:thor1tpercamkkxec0q0jk6ltdnlqvsw29guap8wmcl:");

    SigningInput input;
    input.coinType = TWCoinTypeBitcoin;
    input.hashType = hashTypeForCoin(TWCoinTypeBitcoin);
    input.amount = toAmount;
    input.byteFee = byteFee;
    input.toAddress = toAddress;
    input.changeAddress = ownAddressString;

    input.privateKeys.push_back(privateKey);
    input.outputOpReturn = memo;

    UTXO utxo;
    auto utxoHash = parse_hex("30b82960291a39de3664ec4c844a815e3e680e29b4d3a919e450f0c119cf4e35");
    std::reverse(utxoHash.begin(), utxoHash.end());
    utxo.outPoint = OutPoint(utxoHash, 1, UINT32_MAX);
    utxo.amount = utxoAmount;

    auto utxoPubkeyHash = Hash::ripemd(Hash::sha256(publicKey.bytes));
    EXPECT_EQ(hex(utxoPubkeyHash), "52048aa9c53917d5b370419383b068d7d55c2a81");
    auto utxoScript = Script::buildPayToWitnessPublicKeyHash(utxoPubkeyHash);
    EXPECT_EQ(hex(utxoScript.bytes), "001452048aa9c53917d5b370419383b068d7d55c2a81");
    utxo.script = utxoScript;
    input.utxos.push_back(utxo);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {342101}, 300000, 26586));
        EXPECT_EQ(plan.outputOpReturn.size(), 59);
    }

    // Sign
    auto result = TransactionSigner<Transaction, TransactionBuilder>::sign(input);

    ASSERT_TRUE(result) << std::to_string(result.error());
    auto signedTx = result.payload();

    Data serialized;
    signedTx.encode(serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{293, 183, 211}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), // printed using prettyPrintTransaction
        "01000000" // version
        "0001" // marker & flag
        "01" // inputs
            "354ecf19c1f050e419a9d3b4290e683e5e814a844cec6436de391a296029b830"  "01000000"  "00"  ""  "ffffffff"
        "03" // outputs
            "e093040000000000"  "16"  "00143729d3a173919d1339bfd8d48f95bed5ca924391"
            "9b3c000000000000"  "16"  "001452048aa9c53917d5b370419383b068d7d55c2a81"
            "0000000000000000"  "3d"  "6a3b535741503a54484f522e52554e453a74686f72317470657263616d6b6b7865633071306a6b366c74646e6c7176737732396775617038776d636c3a"
        // witness
            "02"
                "48"  "3045022100ff6c0aaef512aa52f3036161bfbcef39046ac89eb9617fa461a0c9c43fe45eb3022055d208d3f81736e72e3ad8ef761dc79ac5dd3dc00721174bc69db416a74960e301"
                "21"  "02c2e5c8b4927812fb37444a7862466ad23978a4ac626f8eaf93e1d1a60d6abb80"
        "00000000" // nLockTime
    );
}
