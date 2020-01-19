// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bech32Address.h"
#include "AddressChecksum.h"

#include <string>

namespace TW::Zilliqa {

/// Zilliqa address is a Bech32Address, with "zil" prefix and Sha2 hash.
class Address: public Bech32Address {
public:
    static const std::string hrp; // HRP_ZILLIQA

    static bool isValid(const std::string addr) { return Bech32Address::isValid(addr, hrp); }

    Address() : Bech32Address(hrp) {}

    /// Initializes an address with a key hash.
    Address(Data keyHash) : Bech32Address(hrp, keyHash) {}

    /// Initializes an address with a public key.
    Address(const PublicKey& publicKey) : Bech32Address(hrp, HASHER_SHA2, publicKey) {}

    static bool decode(const std::string& addr, Address& obj_out) {
        return Bech32Address::decode(addr, obj_out, hrp);
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

static std::string checkSum(const Data &keyHash) {
    return checksumed(keyHash);
}

} // namespace TW::Zilliqa

#pragma clang diagnostic pop
