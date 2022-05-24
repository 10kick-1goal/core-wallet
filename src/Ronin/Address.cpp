// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"
#include "Ethereum/Address.h"
#include "../Hash.h"
#include "../HexCoding.h"

const std::string prefix = "ronin:";
using namespace TW::Ronin;

bool Address::isValid(const std::string& string) {
    // check prefix
    if (string.compare(0, prefix.length(), prefix) == 0) {
        const auto suffix = string.substr(prefix.length());
        const auto data = parse_hex(suffix);
        return Ethereum::Address::isValid(data);
    }
    // accept Ethereum format as well
    if (Ethereum::Address::isValid(string)) {
        return true;
    }
    return false;
}

Address::Address(const std::string& string) {
    // check prefix
    if (string.compare(0, prefix.length(), prefix) == 0) {
        const auto suffix = string.substr(prefix.length());
        const auto data = parse_hex(suffix);
        std::copy(data.begin(), data.end(), bytes.begin());
    } else if (Ethereum::Address::isValid(string)) {
        // accept Ethereum format as well
        Ethereum::Address ethereumAddress(string);
        bytes = ethereumAddress.bytes;
    } else {
        throw std::invalid_argument("Invalid address data");
    }
}

std::string Address::string() const {
    return prefix + hex(bytes);
}
