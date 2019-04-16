// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "OngTxBuilder.h"

using namespace TW;
using namespace TW::Ontology;

TW_Ontology_Proto_SigningOutput OngTxBuilder::decimals(const Ontology::Proto::SigningInput &input) {
    auto transaction = Ong().decimals(input.nonce());
    auto encoded = transaction.serialize();
    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_encoded(encoded.data(), encoded.size());
    auto serialized = protoOutput.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()),
                                 serialized.size());
}

TW_Ontology_Proto_SigningOutput
OngTxBuilder::balanceOf(const Ontology::Proto::SigningInput &input) {
    auto queryAddress = Address(input.query_address());
    auto transaction = Ong().balanceOf(queryAddress, input.nonce());
    auto encoded = transaction.serialize();
    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_encoded(encoded.data(), encoded.size());
    auto serialized = protoOutput.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()),
                                 serialized.size());
}

TW_Ontology_Proto_SigningOutput OngTxBuilder::transfer(const Ontology::Proto::SigningInput &input) {
    auto payer = Signer(PrivateKey(input.payer_private_key()));
    auto owner = Signer(PrivateKey(input.owner_private_key()));
    auto toAddress = Address(input.to_address());
    auto transaction = Ong().transfer(owner, toAddress, input.amount(), payer, input.gas_price(),
                                      input.gas_limit(), input.nonce());
    auto encoded = transaction.serialize();
    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_encoded(encoded.data(), encoded.size());
    auto serialized = protoOutput.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()),
                                 serialized.size());
}

TW_Ontology_Proto_SigningOutput OngTxBuilder::withdraw(const Ontology::Proto::SigningInput &input) {
    auto payer = Signer(PrivateKey(input.payer_private_key()));
    auto owner = Signer(PrivateKey(input.owner_private_key()));
    auto toAddress = Address(input.to_address());
    auto transaction = Ong().withdraw(owner, toAddress, input.amount(), payer, input.gas_price(),
                                      input.gas_limit(), input.nonce());
    auto encoded = transaction.serialize();
    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_encoded(encoded.data(), encoded.size());
    auto serialized = protoOutput.SerializeAsString();
    return TWDataCreateWithBytes(reinterpret_cast<const uint8_t *>(serialized.data()),
                                 serialized.size());
}

TW_Ontology_Proto_SigningOutput OngTxBuilder::build(const Ontology::Proto::SigningInput &input) {
    auto method = std::string(input.method().begin(), input.method().end());
    if (method == "transfer") {
        return OngTxBuilder::transfer(input);
    }
    if (method == "balanceOf") {
        return OngTxBuilder::balanceOf(input);
    }
    if (method == "decimals") {
        return OngTxBuilder::decimals(input);
    }
    if (method == "withdraw") {
        return OngTxBuilder::withdraw(input);
    }
    std::vector<uint8_t> nullData;
    return TWDataCreateWithBytes(nullData.data(), nullData.size());
}