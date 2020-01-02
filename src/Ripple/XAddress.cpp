// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "XAddress.h"

#include "../Base58.h"
#include  "../BinaryCoding.h"
#include  "../Data.h"
#include <TrezorCrypto/ecdsa.h>

using namespace TW;
using namespace TW::Ripple;

const Data prefixMainnet = {0x05, 0x44};

bool XAddress::isValid(const std::string& string) {
    const auto decoded = Base58::ripple.decodeCheck(string);
    if (decoded.size() != XAddress::size) {
        return false;
    }
    if(!std::equal(decoded.begin(), decoded.begin() + 2, prefixMainnet.begin())) {
        return false;
    }
    if (!(decoded[22] == byte(TagFlag::none) || decoded[22] == byte(TagFlag::classic))) {
        return false;
    }
    return true;
}

XAddress::XAddress(const std::string& string) {
    if (!XAddress::isValid(string)) {
        throw std::invalid_argument("Invalid address string");
    }
    const auto decoded = Base58::ripple.decodeCheck(string);
    std::copy(decoded.begin() + prefixMainnet.size(), decoded.begin() + prefixMainnet.size() + XAddress::keyHashSize, bytes.begin());
    if (decoded[22] == byte(TagFlag::classic)) {
        tag = decode32LE(Data(decoded.end() - 8, decoded.end() - 4).data());
    } else if (decoded[22] == byte(TagFlag::none)) {
        flag = TagFlag::none;
    } else {
        throw std::invalid_argument("Invalid flag");
    }
}

XAddress::XAddress(const PublicKey& publicKey, const uint32_t destination): tag(destination) {
    ecdsa_get_pubkeyhash(publicKey.bytes.data(), HASHER_SHA2_RIPEMD, bytes.data());
}

std::string XAddress::string() const {
    /// see https://github.com/ripple/ripple-address-codec/blob/master/src/index.ts
    /// base58check(2 bytes prefix + 20 bytes keyhash + 1 byte flag + 4 bytes + 32bit tag + 4 bytes reserved)
    Data result;
    append(result, prefixMainnet);
    append(result, Data{bytes.begin(), bytes.end()});
    append(result, byte(flag));
    encode32LE(tag, result);
    append(result, Data{0x00, 0x00, 0x00, 0x00});
    return Base58::ripple.encodeCheck(result);
}
