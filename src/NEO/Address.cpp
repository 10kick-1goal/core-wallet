// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"

#include "../Hash.h"
#include "Tezos/BinaryCoding.h"

#include <TrezorCrypto/base58.h>

using namespace TW::NEO;

bool Address::isValid(const std::string& string) {
    size_t capacity = 128;
    uint8_t buffer[capacity];

    int size = base58_decode_check(string.data(), HASHER_SHA2D, buffer, (int)capacity);
    if (size != Address::size || buffer[0] != version) {
        return false;
    }

    return true;
}

Address::Address(const std::string& string) {
    size_t capacity = 128;
    uint8_t buffer[capacity];

    int size = base58_decode_check(string.data(), HASHER_SHA2D, buffer, (int)capacity);
    if (size != Address::size)
        throw std::invalid_argument("Invalid address data");

    std::copy(buffer, buffer + Address::size, bytes.begin());
}

Address::Address(const std::vector<uint8_t>& data) {
    if (data.size() != size)
        throw std::invalid_argument("Invalid address data");

    std::copy(data.begin(), data.end(), bytes.begin());
}

Address::Address(const PublicKey& publicKey) {
    auto publicKeyData = publicKey.bytes;

    auto pkdata = Data(publicKeyData.begin(), publicKeyData.end());
    pkdata.insert(pkdata.begin(), 0x21);
    pkdata.push_back(0xAC);

    auto keyHash = Hash::ripemd(Hash::sha256(pkdata));
    keyHash.insert(keyHash.begin(), 0x17);

    if (keyHash.size() != Address::size)
        throw std::invalid_argument("Invalid address key data");

    std::copy(keyHash.data(), keyHash.data() + Address::size, bytes.begin());
}

std::string Address::string() const {
    return bytesToBase58(bytes.data(), Address::size);
}
