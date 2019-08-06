// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Base64.h"
#include "../HexCoding.h"

using namespace TW;
using namespace TW::Nebulas;

Proto::SigningOutput Signer::sign(Proto::SigningInput &input) const noexcept {
    Transaction tx(Address(input.from_address()),
        load(input.nonce()),
        load(input.gas_price()),
        load(input.gas_limit()),
        Address(input.to_address()),
        load(input.amount()),
        load(input.timestamp()),
        input.payload()
    );
    
    auto privateKey = PrivateKey(Data(input.private_key().begin(), input.private_key().end()));
    sign(privateKey, tx);

    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_algorithm(tx.algorithm);
    protoOutput.set_signature(reinterpret_cast<const char *>(tx.signature.data()), tx.signature.size());
    protoOutput.set_raw(TW::Base64::encode(tx.raw));
    return protoOutput;
}

void Signer::sign(const PrivateKey &privateKey, Transaction &transaction) const noexcept {
    transaction.hash = this->hash(transaction);
    transaction.chainID = chainID;
    transaction.algorithm = 1;
    transaction.signature = privateKey.sign(transaction.hash, TWCurveSECP256k1);
    transaction.serializeToRaw();
}

Data Signer::hash(const Transaction &transaction) const noexcept {
    auto encoded = Data();
    auto payload = Data();
    auto data = Transaction::newPayloadData(transaction.payload);
    payload.resize(data->ByteSize());
    data->SerializePartialToArray(payload.data(),(int)payload.size());
    delete data;

    encoded.insert(encoded.end(), transaction.from.bytes.begin(), transaction.from.bytes.end());
    encoded.insert(encoded.end(), transaction.to.bytes.begin(), transaction.to.bytes.end());
    encode256BE(encoded, transaction.amount, 128);
    encode256BE(encoded, transaction.nonce, 64);
    encode256BE(encoded, transaction.timestamp, 64);
    encoded.insert(encoded.end(), payload.begin(), payload.end());
    encode256BE(encoded, chainID, 32);
    encode256BE(encoded, transaction.gasPrice, 128);
    encode256BE(encoded, transaction.gasLimit, 128);
    return Hash::sha3_256(encoded);
}