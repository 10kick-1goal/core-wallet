// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Base58.h"
#include "../Hash.h"
#include "../HexCoding.h"
#include "Transaction.h"

#include <TrezorCrypto/ripemd160.h>

#include <ctime>
#include <stdexcept>

using namespace TW;
using namespace TW::EOS;
using json = nlohmann::json;

Signature::Signature(Data sig, Type type) : data(sig), type(type) {
    if (sig.size() != DataSize) {
        throw std::invalid_argument("Invalid signature size!");
    }

    if (type == Type::Legacy) {
        throw std::invalid_argument("Legacy signatures are not supported.");
    }
}

void Signature::serialize(Data& os) const noexcept {
    // type should never be Legacy
    uint32_t typeId = type == Type::ModernK1 ? 0 : 1;
    Bravo::encodeVarInt32(typeId, os);
    os.insert(os.end(), data.begin(), data.end());
}

std::string Signature::string() const noexcept {
    const auto& prefix = sigPrefixForType(type);
    const auto& subPrefix = type == Type::ModernR1 ? Modern::R1::prefix : Modern::K1::prefix;

    Data buffer(data);

    // append the subPrefix to the buffer data and hash the buffer
    for (const char& c : subPrefix) {
        buffer.push_back(static_cast<uint8_t>(c));
    }

    Data hash;
    hash.resize(RIPEMD160_DIGEST_LENGTH);

    ripemd160(buffer.data(), static_cast<uint32_t>(buffer.size()), hash.data());

    // drop the subPrefix and append the checksum to the bufer
    buffer.resize(DataSize);

    for(size_t i = 0; i < ChecksumSize; i++) {
        buffer.push_back(hash[i]);
    }

    return prefix + TW::Base58::bitcoin.encode(buffer);
}

void Extension::serialize(Data& os) const noexcept {
    encode16LE(type, os);
    Bravo::encodeVarInt64(buffer.size(), os);
    append(os, buffer);
}

json Extension::serialize() const noexcept {
    return json::array({type, hex(buffer)});
}

Transaction::Transaction(const Data& referenceBlockId, int32_t referenceBlockTime) {
    setReferenceBlock(referenceBlockId);
    expiration = referenceBlockTime + Transaction::ExpirySeconds;
}

void Transaction::setReferenceBlock(const Data& refBlockId) {
    if (refBlockId.size() != Hash::sha256Size) {
        throw std::invalid_argument("Invalid Reference Block Id!");
    }

    refBlockNumber = decode16BE(refBlockId.data() + 2);
    refBlockPrefix = decode32LE(refBlockId.data() + 8);
}

void Transaction::serialize(Data& os) const noexcept{
    using namespace Bravo;

    encode32LE(expiration, os);
    encode16LE(refBlockNumber, os);
    encode32LE(refBlockPrefix, os);
    encodeVarInt32(maxNetUsageWords, os);
    os.push_back(maxCPUUsageInMS);
    encodeVarInt32(delaySeconds, os);

    encodeCollection(contextFreeActions, os);
    encodeCollection(actions, os);
    encodeCollection(transactionExtensions, os);
}

json Transaction::serialize() const {
    using namespace Bravo;

    // get a formatted date
    char formattedDate[20];
    time_t time = expiration;
    if (strftime(formattedDate, 19, "%FT%T", std::gmtime(&time)) != 19) {
        std::runtime_error("Error creating a formatted string!");
    }

    // create a json array of signatures
    json sigs = json::array();
    for (const auto& sig : signatures) {
        sigs.push_back(sig.string());
    }

    // add everything to the json object
    json obj;
    obj["ref_block_num"] = refBlockNumber;
    obj["ref_block_prefix"] = refBlockPrefix;
    obj["expiration"] = std::string(formattedDate, 19);
    obj["max_net_usage_words"] = maxNetUsageWords;
    obj["max_cpu_usage_ms"] = maxCPUUsageInMS;
    obj["delay_sec"] = delaySeconds;
    obj["context_free_actions"] = encodeCollection(contextFreeActions);
    obj["actions"] = encodeCollection(actions);
    obj["transaction_extensions"] = encodeCollection(transactionExtensions);
    obj["signatures"] = sigs;
    obj["context_free_data"] = hex(contextFreeData);

    return obj;
}
