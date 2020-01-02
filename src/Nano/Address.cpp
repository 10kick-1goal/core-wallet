// Copyright © 2019 Mart Roosmaa.
// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"
#include <TrezorCrypto/nano.h>

#include <string>

using namespace TW::Nano;

const std::string kPrefixNano{"nano_"};
const std::string kPrefixXrb{"xrb_"};

bool Address::isValid(const std::string& address) {
    bool valid = false;

    valid = nano_validate_address(
        kPrefixNano.c_str(), kPrefixNano.length(),
        address.c_str(), address.length(),
        NULL);
    if (!valid) {
        valid = nano_validate_address(
            kPrefixXrb.c_str(), kPrefixXrb.length(),
            address.c_str(), address.length(),
            NULL);
    }

    return valid;
}

Address::Address(const std::string& address) {
    bool valid = false;

    valid = nano_validate_address(
        kPrefixNano.c_str(), kPrefixNano.length(),
        address.c_str(), address.length(),
        bytes.data());

    if (!valid) {
        valid = nano_validate_address(
            kPrefixXrb.c_str(), kPrefixXrb.length(),
            address.c_str(), address.length(),
            bytes.data());
    }

    // Ensure address is valid
    if (!valid) {
        throw std::invalid_argument("Invalid address data");
    }
}

Address::Address(const PublicKey& publicKey) {
    if (publicKey.type != TWPublicKeyTypeED25519Blake2b) {
        throw std::invalid_argument("Invalid public key type");
    }

    auto keyBytes = publicKey.bytes;
    std::copy(keyBytes.begin(), keyBytes.begin() + 32, bytes.data());
}

std::string Address::string() const {
    std::array<char, 5 + 60 + 1> out = {0};

    size_t count = nano_get_address(
            bytes.data(),
            kPrefixNano.c_str(), kPrefixNano.length(),
            out.data(), out.size());
    out[count] = 0;
    return std::string(out.data());
}
