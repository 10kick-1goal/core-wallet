// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWWanchainSigner.h>

#include "../Wanchain/Signer.h"
#include "../proto/Ethereum.pb.h"
#include "../uint256.h"

using namespace TW;
using namespace TW::Wanchain;

TW_Ethereum_Proto_SigningOutput TWWanchainSignerSign(TW_Ethereum_Proto_SigningInput data) {
    Ethereum::Proto::SigningInput input;
    input.ParseFromArray(TWDataBytes(data), static_cast<int>(TWDataSize(data)));

    auto key = PrivateKey(Data(input.private_key().begin(), input.private_key().end()));
    auto transaction = Ethereum::Transaction(
        /* nonce: */ load(input.nonce()),
        /* gasPrice: */ load(input.gas_price()),
        /* gasLimit: */ load(input.gas_limit()),
        /* to: */ Ethereum::Address(input.to_address()),
        /* amount: */ load(input.amount()),
        /* payload: */ Data(input.payload().begin(), input.payload().end())
    );

    auto signer = Signer(load(input.chain_id()));
    signer.sign(key, transaction);

    auto protoOutput = Ethereum::Proto::SigningOutput();

    auto encoded = signer.encode(transaction);
    protoOutput.set_encoded(encoded.data(), encoded.size());

    auto v = store(transaction.v);
    protoOutput.set_v(v.data(), v.size());

    auto r = store(transaction.r);
    protoOutput.set_r(r.data(), r.size());

    auto s = store(transaction.s);
    protoOutput.set_s(s.data(), s.size());

    auto serialized = protoOutput.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()), serialized.size());
}
