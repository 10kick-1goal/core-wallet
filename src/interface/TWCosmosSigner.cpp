// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWCosmosSigner.h>

#include "../Cosmos/Signer.h"
#include "../proto/Cosmos.pb.h"

using namespace TW;
using namespace TW::Cosmos;

TW_Cosmos_Proto_SigningOutput TWCosmosSignerSign(TW_Cosmos_Proto_SigningInput data) {
    Proto::SigningInput input;
    input.ParseFromArray(TWDataBytes(data), static_cast<int>(TWDataSize(data)));

    auto signer = new TWCosmosSigner{ Signer(std::move(input)) };
    Proto::SigningOutput output = signer->impl.build();

    auto serialized = output.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()), serialized.size());
}
