// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "Base64.h"
#include "proto/Algorand.pb.h"
#include <TrustWalletCore/TWAlgorandSigner.h>

#include "TWTestUtilities.h"
#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Algorand;

TEST(TWAlgorandSigner, Sign) {
    auto privateKey = parse_hex("d5b43d706ef0cb641081d45a2ec213b5d8281f439f2425d1af54e2afdaabf55b");
    auto note = parse_hex("68656c6c6f");
    auto genesisHash = Base64::decode("wGHE2Pwdvd7S12BL5FaOP20EGYesN73ktiC1qzkkit8=");
    
    Proto::SigningInput input;
    auto &transaction = *input.mutable_transaction_pay();
    transaction.set_to_address("CRLADAHJZEW2GFY2UPEHENLOGCUOU74WYSTUXQLVLJUJFHEUZOHYZNWYR4");
    transaction.set_fee(263000ull);
    transaction.set_amount(1000000000000ull);
    transaction.set_first_round(1937767ull);
    transaction.set_last_round(1938767ull);
    input.set_genesis_id("mainnet-v1.0");
    input.set_genesis_hash(genesisHash.data(), genesisHash.size());
    input.set_note(note.data(), note.size());
    input.set_private_key(privateKey.data(), privateKey.size());

    auto inputData = input.SerializeAsString();
    auto inputTWData = TWDataCreateWithBytes((const byte *)inputData.data(), inputData.size());
    auto outputTWData = TWAlgorandSignerSign(inputTWData);

    auto output = Proto::SigningOutput();
    output.ParseFromArray(TWDataBytes(outputTWData), TWDataSize(outputTWData));

    ASSERT_EQ(hex(output.encoded()), "82a3736967c440baa00062adcdcb5875e4435cdc6885d26bfe5308ab17983c0fda790b7103051fcb111554e5badfc0ac7edf7e1223a434342a9eeed5cdb047690827325051560ba374786e8aa3616d74cf000000e8d4a51000a3666565ce00040358a26676ce001d9167a367656eac6d61696e6e65742d76312e30a26768c420c061c4d8fc1dbdded2d7604be4568e3f6d041987ac37bde4b620b5ab39248adfa26c76ce001d954fa46e6f7465c40568656c6c6fa3726376c42014560180e9c92da3171aa3c872356e30a8ea7f96c4a74bc1755a68929c94cb8fa3736e64c42061bf060efc02e2887dfffc8ed85268c8c091c013eedf315bc50794d02a8791ada474797065a3706179");
}
