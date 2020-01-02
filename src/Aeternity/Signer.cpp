// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Address.h"
#include "Base58.h"
#include "Base64.h"
#include "HexCoding.h"
#include "Identifiers.h"
#include <Data.h>
#include <Ethereum/RLP.h>

using namespace TW;
using namespace TW::Aeternity;

/// implementation copied from
/// https://github.com/aeternity/aepp-sdk-go/blob/07aa8a77e5/aeternity/helpers.go#L367
Proto::SigningOutput Signer::sign(const TW::PrivateKey &privateKey, Transaction &transaction) {
    auto txRlp = transaction.encode();

    /// append networkId and txRaw
    auto msg = buildMessageToSign(txRlp);

    /// sign ed25519
    auto sigRaw = privateKey.sign(msg, TWCurveED25519);
    auto signature = Identifiers::prefixSignature + Base58::bitcoin.encodeCheck(sigRaw);

    /// encode the message using rlp
    auto rlpTxRaw = buildRlpTxRaw(txRlp, sigRaw);

    /// encode the rlp message with the prefix
    auto signedEncodedTx = encodeBase64WithChecksum(Identifiers::prefixTransaction, rlpTxRaw);

    return createProtoOutput(signature, signedEncodedTx);
}

Data Signer::buildRlpTxRaw(Data &txRaw, Data &sigRaw) {
    auto rlpTxRaw = Data();
    auto signaturesList = Data();
    append(signaturesList, Ethereum::RLP::encode(sigRaw));

    append(rlpTxRaw, Ethereum::RLP::encode(Identifiers::objectTagSignedTransaction));
    append(rlpTxRaw, Ethereum::RLP::encode(Identifiers::rlpMessageVersion));
    append(rlpTxRaw, Ethereum::RLP::encodeList(signaturesList));
    append(rlpTxRaw, Ethereum::RLP::encode(txRaw));

    return Ethereum::RLP::encodeList(rlpTxRaw);
}

Data Signer::buildMessageToSign(Data &txRaw) {
    auto data = Data();
    Data bytes(Identifiers::networkId.begin(), Identifiers::networkId.end());
    append(data, bytes);
    append(data, txRaw);
    return data;
}

Proto::SigningOutput Signer::createProtoOutput(std::string &signature, const std::string &signedTx) {
    auto output = Proto::SigningOutput();

    output.set_signature(signature);
    output.set_encoded(signedTx);
    return output;
}

std::string Signer::encodeBase64WithChecksum(const std::string &prefix, const TW::Data &rawTx) {
    auto checksum = Hash::sha256(Hash::sha256(rawTx));
    std::vector<unsigned char> checksumPart(checksum.begin(), checksum.begin() + checkSumSize);

    auto data = Data();
    append(data, rawTx);
    append(data, checksumPart);

    return prefix + TW::Base64::encode(data);
}