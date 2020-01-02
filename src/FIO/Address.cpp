// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Base58.h"
#include "../BinaryCoding.h"
#include "Address.h"

#include <TrezorCrypto/ripemd160.h>

#include <stdexcept>

using namespace TW;
using namespace TW::FIO;

bool Address::isValid(const std::string& string) {
    return decodeKeyData(string).has_value();
}

/// Determines whether the given byte vector is a valid keyBuffer
bool Address::isValid(const Data& bytes) {
    if (bytes.size() != size) {
        return false;
    }

    // last Address::ChecksumSize bytes are a checksum
    uint32_t checksum = decode32LE(bytes.data() + PublicKey::secp256k1Size);
    if (createChecksum(bytes) != checksum) {
        return false;
    }
    return true;
}

/// Creates a checksum of PublicKeyDataSize bytes at the buffer
uint32_t Address::createChecksum(const Data& bytes) {
    // create checksum and compare
    uint8_t hash[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160_CTX ctx;
    ripemd160_Init(&ctx);

    // add the bytes to the hash input
    ripemd160_Update(&ctx, bytes.data(), PublicKey::secp256k1Size);

    // finalize the hash
    ripemd160_Final(&ctx, hash);

    return decode32LE(hash);
}

/// Decode and verifies the key data from a base58 string.
std::optional<Data> Address::decodeKeyData(const std::string& string) {
    size_t prefixSize = prefix().size();
    if (string.substr(0, prefixSize) != prefix()) {
        return {};
    }

    const Data& decodedBytes = Base58::bitcoin.decode(string.substr(prefixSize));
    if (decodedBytes.size() != size) {
        return {};
    }

    if (!isValid(decodedBytes)) {
        return {};
    }

    return decodedBytes;
}

/// Initializes a FIO address from a string representation.
Address::Address(const std::string& string) {
    auto data = Address::decodeKeyData(string);
    if (!data.has_value()) {
        throw std::invalid_argument("Invalid address string!");
    }
    std::copy(std::begin(*data), std::end(*data), std::begin(bytes));
}

/// Initializes a FIO address from a public key.
Address::Address(const PublicKey& publicKey) {
    // copy the raw, compressed key data
    auto data = publicKey.compressed().bytes;

    // append the checksum
    uint32_t checksum = createChecksum(data);
    Data checksumBytes;
    encode32LE(checksum, checksumBytes);
    append(data, checksumBytes);

    std::copy(std::begin(data), std::end(data), std::begin(bytes));
}

/// Returns a string representation of the FIO address.
std::string Address::string() const {
    return prefix() + Base58::bitcoin.encode(bytes);
}
