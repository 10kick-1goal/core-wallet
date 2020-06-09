// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Bitcoin/OutPoint.h"
#include "Bitcoin/Script.h"
#include "Bitcoin/Transaction.h"
#include "Bitcoin/TransactionBuilder.h"
#include "Bitcoin/TransactionSigner.h"
#include "Hash.h"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "proto/Bitcoin.pb.h"
#include "TxComparisonHelper.h"
#include "../interface/TWTestUtilities.h"

#include <TrustWalletCore/TWBitcoinScript.h>
#include <TrustWalletCore/TWAnySigner.h>
#include <TrustWalletCore/TWHash.h>
#include <TrustWalletCore/TWPrivateKey.h>

#include <gtest/gtest.h>
#include <cassert>

using namespace TW;
using namespace TW::Bitcoin;

Proto::SigningInput buildInputP2PKH(bool omitKey = false) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    Proto::SigningInput input;
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_amount(335'790'000);
    input.set_byte_fee(1);
    input.set_to_address("1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx");
    input.set_change_address("1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU");

    auto utxoKey0 = PrivateKey(parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866"));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash0 = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    assert(hex(utxoPubkeyHash0) == "b7cd046b6d522a3d61dbcb5235c0e9cc97265457");
    if (!omitKey) {
        input.add_private_key(utxoKey0.bytes.data(), utxoKey0.bytes.size());
    }

    auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
    auto pubKey1 = utxoKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash1 = Hash::ripemd(Hash::sha256(pubKey1.bytes));
    assert(hex(utxoPubkeyHash1) == "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    if (!omitKey) {
        input.add_private_key(utxoKey1.bytes.data(), utxoKey1.bytes.size());
    }

    auto utxo0Script = Script::buildPayToPublicKeyHash(utxoPubkeyHash0);
    Data scriptHash;
    utxo0Script.matchPayToPublicKeyHash(scriptHash);
    assert(hex(scriptHash) == "b7cd046b6d522a3d61dbcb5235c0e9cc97265457");

    auto utxo0 = input.add_utxo();
    utxo0->set_script(utxo0Script.bytes.data(), utxo0Script.bytes.size());
    utxo0->set_amount(625'000'000);
    utxo0->mutable_out_point()->set_hash(hash0.data(), hash0.size());
    utxo0->mutable_out_point()->set_index(0);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);

    auto utxo1 = input.add_utxo();
    auto utxo1Script = parse_hex("0014" "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    utxo1->set_script(utxo1Script.data(), utxo1Script.size());
    utxo1->set_amount(600'000'000);
    utxo1->mutable_out_point()->set_hash(hash1.data(), hash1.size());
    utxo1->mutable_out_point()->set_index(1);
    utxo1->mutable_out_point()->set_sequence(UINT32_MAX);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {625'000'000}, 335'790'000, 226));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_FALSE(result);
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

Proto::SigningInput buildInputP2WPKH(int64_t amount, TWBitcoinSigHashType hashType, int64_t utxo0Amount, int64_t utxo1Amount, bool useMaxAmount = false) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    Proto::SigningInput input;
    input.set_hash_type(hashType);
    input.set_amount(amount);
    input.set_use_max_amount(useMaxAmount);
    input.set_byte_fee(1);
    input.set_to_address("1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx");
    input.set_change_address("1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU");

    auto utxoKey0 = PrivateKey(parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866"));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash0 = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    assert(hex(utxoPubkeyHash0) == "b7cd046b6d522a3d61dbcb5235c0e9cc97265457");
    input.add_private_key(utxoKey0.bytes.data(), utxoKey0.bytes.size());

    auto utxoKey1 = PrivateKey(parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9"));
    auto pubKey1 = utxoKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash1 = Hash::ripemd(Hash::sha256(pubKey1.bytes));
    assert(hex(utxoPubkeyHash1) == "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    input.add_private_key(utxoKey1.bytes.data(), utxoKey1.bytes.size());

    auto scriptPub1 = Script(parse_hex("0014" "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    Data scriptHash;
    scriptPub1.matchPayToWitnessPublicKeyHash(scriptHash);
    auto scriptHashHex = hex(scriptHash);
    assert(scriptHashHex == "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");

    auto redeemScript = Script::buildPayToPublicKeyHash(parse_hex("1d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[scriptHashHex] = scriptString;

    auto utxo0 = input.add_utxo();
    auto utxo0Script = parse_hex("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac");
    utxo0->set_script(utxo0Script.data(), utxo0Script.size());
    utxo0->set_amount(utxo0Amount);
    utxo0->mutable_out_point()->set_hash(hash0.data(), hash0.size());
    utxo0->mutable_out_point()->set_index(0);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);

    auto utxo1 = input.add_utxo();
    auto utxo1Script = parse_hex("0014" "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    utxo1->set_script(utxo1Script.data(), utxo1Script.size());
    utxo1->set_amount(utxo1Amount);
    utxo1->mutable_out_point()->set_hash(hash1.data(), hash1.size());
    utxo1->mutable_out_point()->set_index(1);
    utxo1->mutable_out_point()->set_sequence(UINT32_MAX);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto unsignedTx = Transaction(1, 0);

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

Proto::SigningInput buildInputP2WSH(enum TWBitcoinSigHashType hashType, bool omitScript = false) {
    Proto::SigningInput input;
    input.set_hash_type(hashType);
    input.set_amount(1000);
    input.set_byte_fee(1);
    input.set_to_address("1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx");
    input.set_change_address("1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU");

    auto utxoKey0 = parse_hex("ed00a0841cd53aedf89b0c616742d1d2a930f8ae2b0fb514765a17bb62c7521a");
    input.add_private_key(utxoKey0.data(), utxoKey0.size());

    auto utxoKey1 = parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9");
    input.add_private_key(utxoKey1.data(), utxoKey1.size());

    if (!omitScript) {
        auto redeemScript = Script(parse_hex("2103596d3451025c19dbbdeb932d6bf8bfb4ad499b95b6f88db8899efac102e5fc71ac"));
        auto scriptHash = "593128f9f90e38b706c18623151e37d2da05c229";
        auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
        (*input.mutable_scripts())[scriptHash] = scriptString;
    }

    auto utxo0 = input.add_utxo();
    auto p2wsh = Script::buildPayToWitnessScriptHash(parse_hex("ff25429251b5a84f452230a3c75fd886b7fc5a7865ce4a7bb7a9d7c5be6da3db"));
    utxo0->set_script(p2wsh.bytes.data(), p2wsh.bytes.size());
    utxo0->set_amount(1226);
    auto hash0 = parse_hex("0001000000000000000000000000000000000000000000000000000000000000");
    utxo0->mutable_out_point()->set_hash(hash0.data(), hash0.size());
    utxo0->mutable_out_point()->set_index(0);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);
    return input;
}

TEST(BitcoinSigning, SignP2WSH) {
    // Setup input
    const auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 147));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
        EXPECT_TRUE(verifyPlan(plan, {1'226}, 1'000, 226));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_FALSE(result);
}

TEST(BitcoinSigning, SignP2WSH_NegativePlanWithNoUTXOs) {
    // Setup input
    auto input = buildInputP2WSH(TWBitcoinSigHashTypeAll);
    auto plan = Bitcoin::TransactionPlan();
    input.mutable_plan()->clear_utxos();

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_FALSE(result);
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

TEST(BitcoinSigning, SignP2SH_P2WPKH) {
    // Setup input
    Proto::SigningInput input;
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_amount(200'000'000);
    input.set_byte_fee(1);
    input.set_to_address("1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx");
    input.set_change_address("1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU");

    auto utxoKey0 = PrivateKey(parse_hex("eb696a065ef48a2192da5b28b694f87544b30fae8327c4510137a922f32c6dcf"));
    auto pubKey0 = utxoKey0.getPublicKey(TWPublicKeyTypeSECP256k1);
    auto utxoPubkeyHash = Hash::ripemd(Hash::sha256(pubKey0.bytes));
    ASSERT_EQ(hex(utxoPubkeyHash), "79091972186c449eb1ded22b78e40d009bdf0089");
    input.add_private_key(utxoKey0.bytes.data(), utxoKey0.bytes.size());

    auto redeemScript = Script::buildPayToWitnessPublicKeyHash(utxoPubkeyHash);
    auto scriptHash = Hash::ripemd(Hash::sha256(redeemScript.bytes));
    ASSERT_EQ(hex(scriptHash), "4733f37cf4db86fbc2efed2500b4f4e49f312023");
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[hex(scriptHash)] = scriptString;

    auto utxo0 = input.add_utxo();
    auto utxo0Script = Script(parse_hex("a9144733f37cf4db86fbc2efed2500b4f4e49f31202387"));
    utxo0->set_script(utxo0Script.bytes.data(), utxo0Script.bytes.size());
    utxo0->set_amount(1'000'000'000);
    auto hash0 = DATA("db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a5477");
    utxo0->mutable_out_point()->set_hash(TWDataBytes(hash0.get()), TWDataSize(hash0.get()));
    utxo0->mutable_out_point()->set_index(1);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {1'000'000'000}, 200'000'000, 170));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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

TEST(BitcoinSigning, EncodeP2SH_P2WSH) {
    auto unsignedTx = Transaction(1, 0);

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
    auto emptyScript = Script();
    auto unsignedTx = Transaction(1, 0);

    auto outpoint0 = OutPoint(parse_hex("36641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e"), 1);
    unsignedTx.inputs.emplace_back(outpoint0, emptyScript, 0xffffffff);

    auto outScript0 = Script(parse_hex("76a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688ac"));
    unsignedTx.outputs.emplace_back(0x0000000035a4e900, outScript0);

    auto outScript1 = Script(parse_hex("76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac"));
    unsignedTx.outputs.emplace_back(0x00000000052f83c0, outScript1);

    // Setup signing input
    auto input = Proto::SigningInput();

    auto key0 = parse_hex("730fff80e1413068a05b57d6a58261f07551163369787f349438ea38ca80fac6");
    input.add_private_key(key0.data(), key0.size());
    auto key1 = parse_hex("11fa3d25a17cbc22b29c44a484ba552b5a53149d106d3d853e22fdd05a2d8bb3");
    input.add_private_key(key1.data(), key1.size());
    auto key2 = parse_hex("77bf4141a87d55bdd7f3cd0bdccf6e9e642935fec45f2f30047be7b799120661");
    input.add_private_key(key2.data(), key2.size());
    auto key3 = parse_hex("14af36970f5025ea3e8b5542c0f8ebe7763e674838d08808896b63c3351ffe49");
    input.add_private_key(key3.data(), key3.size());
    auto key4 = parse_hex("fe9a95c19eef81dde2b95c1284ef39be497d128e2aa46916fb02d552485e0323");
    input.add_private_key(key4.data(), key4.size());
    auto key5 = parse_hex("428a7aee9f0c2af0cd19af3cf1c78149951ea528726989b2e83e4778d2c3f890");
    input.add_private_key(key5.data(), key5.size());

    auto redeemScript = Script::buildPayToWitnessScriptHash(parse_hex("a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54"));
    auto scriptHash = Hash::ripemd(Hash::sha256(redeemScript.bytes));
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[hex(scriptHash)] = scriptString;

    auto witnessScript = Script(parse_hex(""
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
    auto witnessScriptString = std::string(witnessScript.bytes.begin(), witnessScript.bytes.end());
    (*input.mutable_scripts())[hex(witnessScriptHash)] = witnessScriptString;

    auto utxo0Script = Script(parse_hex("a9149993a429037b5d912407a71c252019287b8d27a587"));
    auto utxo = input.add_utxo();
    utxo->mutable_out_point()->set_hash(outpoint0.hash.data(), outpoint0.hash.size());
    utxo->mutable_out_point()->set_index(outpoint0.index);
    utxo->mutable_out_point()->set_sequence(UINT32_MAX);
    utxo->set_script(utxo0Script.bytes.data(), utxo0Script.bytes.size());
    utxo->set_amount(987654321);

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input));
    signer.transaction = unsignedTx;
    signer.plan.utxos = {*utxo};
    auto result = signer.sign();
    ASSERT_TRUE(result) << result.error();
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
            "08"  "00"  ""  "47"  "304402201992f5426ae0bab04cf206d7640b7e00410297bfe5487637f6c2427ee8496be002204ad4e64dc2d269f593cc4820db1fc1e8dc34774f602945115ce485940e05c64200"  "47"  "304402201e412363fa554b994528fd44149f3985b18bb901289ef6b71105b27c7d0e336c0220595e4a1e67154337757562ed5869127533e3e5084c3c2e128518f5f0b85b721800"  "47"  "3044022003b0a20ccf545b3f12c5ade10db8717e97b44da2e800387adfd82c95caf529d902206aee3a2395530d52f476d0ddd9d20ba062820ae6f4e1be4921c3630395743ad900"  "48"  "3045022100ed7a0eeaf72b84351bceac474b0c0510f67065b1b334f77e6843ed102f968afe022004d97d0cfc4bf5651e46487d6f87bd4af6aef894459f9778f2293b0b2c5b7bc700"  "48"  "3045022100934a0c364820588154aed2d519cbcc61969d837b91960f4abbf0e374f03aa39d022036b5c58b754bd44cb5c7d34806c89d9778ea1a1c900618a841e9fbfbe805ff9b00"  "47"  "3044022044e3b59b06931d46f857c82fa1d53d89b116a40a581527eac35c5eb5b7f0785302207d0f8b5d063ffc6749fb4e133db7916162b540c70dee40ec0b21e142d8843b3a00"  "cf"  "56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae"
        "00000000" // nLockTime
    ;

    Data serialized;
    signer.encodeTx(signedTx, serialized);
    EXPECT_EQ(getEncodedTxSize(signedTx), (EncodedTxSize{800, 154, 316}));
    EXPECT_TRUE(validateEstimatedSize(signedTx, -1, 1));
    ASSERT_EQ(hex(serialized), expected);
}

TEST(BitcoinSigning, Sign_NegativeNoUtxos) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    Proto::SigningInput input;
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_amount(335'790'000);
    input.set_byte_fee(1);
    input.set_to_address("1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx");
    input.set_change_address("1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU");

    auto scriptPub1 = Script(parse_hex("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    Data scriptHash;
    scriptPub1.matchPayToWitnessPublicKeyHash(scriptHash);
    auto scriptHashHex = hex(scriptHash);
    ASSERT_EQ(scriptHashHex, "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");

    auto redeemScript = Script::buildPayToPublicKeyHash(scriptHash);
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[scriptHashHex] = scriptString;

    {
        // plan returns empty, as there are 0 utxos
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {}, 0, 0));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    // Fails as there are 0 utxos
    ASSERT_FALSE(result);
}

TEST(BitcoinSigning, Sign_NegativeInvalidAddress) {
    auto hash0 = parse_hex("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f");
    auto hash1 = parse_hex("ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a");

    // Setup input
    Proto::SigningInput input;
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_amount(335'790'000);
    input.set_byte_fee(1);
    input.set_to_address("THIS-IS-NOT-A-BITCOIN-ADDRESS");
    input.set_change_address("THIS-IS-NOT-A-BITCOIN-ADDRESS-EITHER");

    auto utxoKey0 = parse_hex("bbc27228ddcb9209d7fd6f36b02f7dfa6252af40bb2f1cbc7a557da8027ff866");
    input.add_private_key(utxoKey0.data(), utxoKey0.size());

    auto utxoKey1 = parse_hex("619c335025c7f4012e556c2a58b2506e30b8511b53ade95ea316fd8c3286feb9");
    input.add_private_key(utxoKey1.data(), utxoKey1.size());

    auto scriptPub1 = Script(parse_hex("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"));
    Data scriptHash;
    scriptPub1.matchPayToWitnessPublicKeyHash(scriptHash);
    auto scriptHashHex = hex(scriptHash);
    ASSERT_EQ(scriptHashHex, "1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");

    auto redeemScript = Script::buildPayToPublicKeyHash(scriptHash);
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[scriptHashHex] = scriptString;

    auto utxo0 = input.add_utxo();
    auto utxo0Script = parse_hex("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac");
    utxo0->set_script(utxo0Script.data(), utxo0Script.size());
    utxo0->set_amount(625'000'000);
    utxo0->mutable_out_point()->set_hash(hash0.data(), hash0.size());
    utxo0->mutable_out_point()->set_index(0);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);

    auto utxo1 = input.add_utxo();
    auto utxo1Script = parse_hex("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
    utxo1->set_script(utxo1Script.data(), utxo1Script.size());
    utxo1->set_amount(600'000'000);
    utxo1->mutable_out_point()->set_hash(hash1.data(), hash1.size());
    utxo1->mutable_out_point()->set_index(1);
    utxo1->mutable_out_point()->set_sequence(UINT32_MAX);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {625'000'000}, 335'790'000, 226));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_FALSE(result);
}

TEST(BitcoinSigning, SignLitecoinReal_a85f) {    
    auto coin = TWCoinTypeLitecoin;
    auto ownAddress = "ltc1qt36tu30tgk35tyzsve6jjq3dnhu2rm8l8v5q00";
    auto ownPrivateKey = "b820f41f96c8b7442f3260acd23b3897e1450b8c7c6580136a3c2d3a14e34674";

    // Setup input
    Proto::SigningInput input;
    input.set_coin_type(coin);
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_amount(3'899'774);
    input.set_use_max_amount(true);
    input.set_byte_fee(1);
    input.set_to_address("ltc1q0dvup9kzplv6yulzgzzxkge8d35axkq4n45hum");
    input.set_change_address(ownAddress);

    auto privKey = parse_hex(ownPrivateKey);
    input.add_private_key(privKey.data(), privKey.size());

    auto utxo0Script = Script::buildForAddress(ownAddress, coin);
    Data keyHash0;
    EXPECT_TRUE(utxo0Script.matchPayToWitnessPublicKeyHash(keyHash0));
    EXPECT_EQ(hex(keyHash0), "5c74be45eb45a3459050667529022d9df8a1ecff");

    auto redeemScript = Script::buildPayToPublicKeyHash(keyHash0);
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[std::string(keyHash0.begin(), keyHash0.end())] = scriptString;

    auto utxo0 = input.add_utxo();
    utxo0->set_script(utxo0Script.bytes.data(), utxo0Script.bytes.size());
    utxo0->set_amount(3'900'000);
    auto hash0 = parse_hex("7051cd18189401a844abf0f9c67e791315c4c154393870453f8ad98a818efdb5");
    std::reverse(hash0.begin(), hash0.end());
    utxo0->mutable_out_point()->set_hash(hash0.data(), hash0.size());
    utxo0->mutable_out_point()->set_index(9);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX - 1);

    // set plan, to match real tx
    input.mutable_plan()->set_available_amount(3'900'000);
    input.mutable_plan()->set_amount(3'899'774);
    input.mutable_plan()->set_fee(226);
    input.mutable_plan()->set_change(0);
    input.mutable_plan()->add_utxos();
    *input.mutable_plan()->mutable_utxos(0) = input.utxo(0);
    EXPECT_TRUE(verifyPlan(input.plan(), {3'900'000}, 3'899'774, 226));

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);

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

TEST(BitcoinSigning, SignLitecoinReal_8435) {
    auto coin = TWCoinTypeLitecoin;
    auto ownAddress = "ltc1q0dvup9kzplv6yulzgzzxkge8d35axkq4n45hum";
    auto ownPrivateKey = "690b34763f34e0226ad2a4d47098269322e0402f847c97166e8f39959fcaff5a";

    // Setup input
    Proto::SigningInput input;
    input.set_coin_type(coin);
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_amount(1'200'000);
    input.set_use_max_amount(false);
    input.set_byte_fee(1);
    input.set_to_address("ltc1qt36tu30tgk35tyzsve6jjq3dnhu2rm8l8v5q00");
    input.set_change_address(ownAddress);

    auto privKey = parse_hex(ownPrivateKey);
    input.add_private_key(privKey.data(), privKey.size());

    auto utxo0Script = Script::buildForAddress(ownAddress, coin);
    Data keyHash0;
    EXPECT_TRUE(utxo0Script.matchPayToWitnessPublicKeyHash(keyHash0));
    EXPECT_EQ(hex(keyHash0), "7b59c096c20fd9a273e240846b23276c69d35815");

    auto redeemScript = Script::buildPayToPublicKeyHash(keyHash0);
    auto scriptString = std::string(redeemScript.bytes.begin(), redeemScript.bytes.end());
    (*input.mutable_scripts())[std::string(keyHash0.begin(), keyHash0.end())] = scriptString;

    auto utxo0 = input.add_utxo();
    utxo0->set_script(utxo0Script.bytes.data(), utxo0Script.bytes.size());
    utxo0->set_amount(3'899'774);
    auto hash0 = parse_hex("a85fd6a9a7f2f54cacb57e83dfd408e51c0a5fc82885e3fa06be8692962bc407");
    std::reverse(hash0.begin(), hash0.end());
    utxo0->mutable_out_point()->set_hash(hash0.data(), hash0.size());
    utxo0->mutable_out_point()->set_index(0);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);

    {
        // test plan (but do not reuse plan result)
        auto plan = TransactionBuilder::plan(input);
        EXPECT_TRUE(verifyPlan(plan, {3'899'774}, 1'200'000, 141));
    }

    // Sign
    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(input)); 
    auto result = signer.sign();

    ASSERT_TRUE(result) << result.error();
    auto signedTx = result.payload();

    Data serialized;
    signer.encodeTx(signedTx, serialized);
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
