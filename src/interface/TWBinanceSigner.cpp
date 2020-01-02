// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWBinanceSigner.h>

#include "../Binance/Signer.h"
#include "../proto/Binance.pb.h"

using namespace TW;
using namespace TW::Binance;

TW_Binance_Proto_SigningOutput TWBinanceSignerSign(TW_Binance_Proto_SigningInput data) {
    Proto::SigningInput input;
    input.ParseFromArray(TWDataBytes(data), static_cast<int>(TWDataSize(data)));

    auto signer = new TWBinanceSigner{ Signer(std::move(input)) };
    auto encoded = signer->impl.build();

    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_encoded(encoded.data(), encoded.size());

    auto serialized = protoOutput.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()), serialized.size());
}
